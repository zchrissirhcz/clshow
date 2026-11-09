#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 简介: 编译 clshow_cli 并部署到 Android 设备运行的脚本
#
# 由 opencl-notes/clshow/deploy-android.py 改写:
#   - 仅使用 adb 作为设备桥接
#   - 默认 NDK 路径改为本机 r27c: C:\soft\toolchains\android-ndk\r27c
#   - 构建统一走 CMake（编译选项集中在 jni/CMakeLists.txt，只改一处）
#
# 加载方案: clshow_cli 与 app 使用同一套代码（clshow_core.cpp + opencl_shim），
# 默认用 opencl_shim（动态加载厂商 libOpenCL.so）；传 --use-icd-loader 用
# Khronos ICD Loader 静态库（此时需 OCL_ICD_FILENAMES 指定驱动库，脚本自动探测）。
#
# 用法:
#   python build-android-cli.py                       # 默认设备（opencl_shim）
#   python build-android-cli.py --use-icd-loader      # 用 Khronos ICD Loader
#   python build-android-cli.py --sid <序列号>        # 指定设备
#   python build-android-cli.py --no-log              # 不写日志，仅控制台

import argparse
import re
import subprocess
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# 默认参数
# ---------------------------------------------------------------------------
SCRIPT_ROOT = Path(__file__).resolve().parent
SCRIPT_STEM = Path(__file__).stem  # 脚本名（不含扩展名）

JNI_DIR = SCRIPT_ROOT / "app" / "src" / "main" / "jni"

DEFAULTS = {
    "build_dir": "build-android",
    "ndk_root": r"C:\soft\toolchains\android-ndk\r27c",
    "test_name": "clshow_cli",
    "api": "33",               # Android API 级别（ANDROID_PLATFORM=android-<api>）
    "abi": "arm64-v8a",
    "sid": "",                 # 空则自动探测唯一设备（或由 adb 默认）
    "remote_dir": "/data/local/tmp/clshow",
    "use_icd_loader": False, # True 用 Khronos ICD Loader 静态库；False（默认）用 opencl_shim
    "logfile": "",             # 不传则自动按 build_dir 生成日志
    "log_enabled": True,       # 是否写入日志文件
}

# 解析命令行参数
ap = argparse.ArgumentParser(description="编译 clshow_cli 并部署到 Android 设备")
ap.add_argument("-B", "--build-dir", default=DEFAULTS["build_dir"])
ap.add_argument("-r", "--ndk-root", default=DEFAULTS["ndk_root"])
ap.add_argument("-t", "--test-name", default=DEFAULTS["test_name"])
ap.add_argument("--api", default=DEFAULTS["api"])
ap.add_argument("--abi", default=DEFAULTS["abi"])
ap.add_argument("--sid", default=DEFAULTS["sid"])
ap.add_argument("--remote-dir", default=DEFAULTS["remote_dir"])
ap.add_argument("--use-icd-loader", dest="use_icd_loader", action="store_true",
                help="用 Khronos ICD Loader 静态库（而非默认的 opencl_shim）")
ap.add_argument("--logfile", default=DEFAULTS["logfile"])
ap.add_argument("--no-log", dest="log_enabled", action="store_false",
                help="关闭日志文件，仅控制台输出")
args = ap.parse_args()

build_dir = args.build_dir
ndk_root = Path(args.ndk_root)
test_name = args.test_name
api = args.api
abi = args.abi
sid = args.sid
remote_dir = args.remote_dir
use_icd_loader = args.use_icd_loader
logfile = args.logfile
log_enabled = args.log_enabled


# ---------------------------------------------------------------------------
# 工具函数
# ---------------------------------------------------------------------------
def safe_name(s):
    """把路径里可能非法/不宜入日志名的字符替换成 -"""
    return re.sub(r'[\\/:*?"<>|]', '-', s)


def get_log_path(suffix):
    """生成 <脚本名>-<build_dir>-<后缀>.log 完整路径；显式传 --logfile 则原样返回"""
    if logfile:
        return logfile
    return SCRIPT_ROOT / f"{SCRIPT_STEM}-{safe_name(build_dir)}-{suffix}.log"


def assert_ok(desc, returncode):
    """检查命令退出码，非 0 则抛错"""
    if returncode != 0:
        raise RuntimeError(f"{desc} 失败 (exit {returncode})")


def write_line(log_path, line, append=True):
    """向日志写入一行"""
    if not log_path:
        return
    with open(log_path, "a" if append else "w", encoding="utf-8") as f:
        f.write(line)
        if not line.endswith("\n"):
            f.write("\n")


def run(cmd, *, log_path=None, desc="", append=True, shell=False):
    """运行命令：实时回显，可选写日志，非零退出码即抛错。

    cmd 为字符串时走 shell（可含管道/通配符）；为列表时直接执行（更安全）。
    """
    if log_enabled and log_path:
        if not append:
            with open(log_path, "w", encoding="utf-8"):
                pass
        p = subprocess.Popen(cmd, text=True, encoding="utf-8",
                             errors="replace", shell=shell,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        assert p.stdout is not None
        for line in p.stdout:
            line = line.rstrip("\n")
            print(line)
            write_line(log_path, line)
        p.wait()
        assert_ok(desc, p.returncode)
    else:
        r = subprocess.run(cmd, shell=shell)
        assert_ok(desc, r.returncode)


# ---------------------------------------------------------------------------
# adb 封装
# ---------------------------------------------------------------------------
def adb(*cmd_args):
    """执行 adb 命令：-s sid + 记日志。cmd_args 为参数列表。"""
    log_path = get_log_path("adb")

    if log_enabled and log_path:
        log_dir = Path(log_path).parent
        if log_dir and not log_dir.exists():
            log_dir.mkdir(parents=True, exist_ok=True)
        sid_part = f" -s {sid}" if sid else ""
        write_line(log_path, f"\n>>> adb{sid_part} {' '.join(cmd_args)}\n")

    exe = ["adb"]
    if sid:
        exe += ["-s", sid]
    exe += list(cmd_args)
    run(exe, log_path=log_path if log_enabled else None, desc="adb")


def detect_sid():
    """若用户未传 --sid，尝试探测唯一连接的设备；多台则提示用 --sid"""
    global sid
    if sid:
        return
    r = subprocess.run(["adb", "devices"], capture_output=True, text=True)
    lines = [l.strip().split("\t")[0] for l in r.stdout.splitlines()
             if l.strip() and "\tdevice" in l]
    if len(lines) == 1:
        sid = lines[0]
        print(f"自动选择设备: {sid}")
    elif len(lines) > 1:
        raise RuntimeError(
            f"检测到多台设备 {lines}，请用 --sid 指定序列号")
    # 0 台则交给 adb 默认（可能报错提示）


# ---------------------------------------------------------------------------
# 编译（统一走 CMake，编译选项集中在 CMakeLists.txt）
# ---------------------------------------------------------------------------
def find_toolchain():
    """NDK 自带的 Android CMake toolchain 文件"""
    return ndk_root / "build" / "cmake" / "android.toolchain.cmake"


def build():
    toolchain = find_toolchain()
    if not toolchain.exists():
        raise RuntimeError(f"未找到 NDK toolchain: {toolchain}；请检查 --ndk-root")

    # 两种加载模式用不同 build 目录，避免 CMake 缓存串扰
    bdir = build_dir
    if use_icd_loader:
        bdir = f"{build_dir}-icd"

    cmake_args = ["cmake", "-S", str(JNI_DIR), "-B", str(bdir),
                  "-G", "Ninja",
                  f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
                  f"-DANDROID_ABI={abi}",
                  f"-DANDROID_PLATFORM=android-{api}",
                  "-DCLSHOW_BUILD_CLI=ON"]
    if use_icd_loader:
        cmake_args.append("-DCLSHOW_USE_ICD_LOADER=ON")
    # 抑制 NDK 自带 toolchain 文件内部的 deprecation 警告
    cmake_args.append("-Wno-deprecated")

    run(cmake_args,
        log_path=get_log_path("cmake-configure"), desc="cmake configure",
        append=False)
    run(["cmake", "--build", str(bdir), "--target", test_name],
        log_path=get_log_path("cmake-build"), desc="cmake build",
        append=False)

    exe_out = Path(bdir) / test_name
    if not exe_out.exists():
        raise RuntimeError(f"构建未产出 {exe_out}")
    print(f"编译完成: {exe_out}")
    return exe_out


# ---------------------------------------------------------------------------
# 部署并运行
# ---------------------------------------------------------------------------
def deploy_and_run(exe_path):
    adb("shell", f"mkdir -p '{remote_dir}' && echo mkdir ok")
    adb("push", str(exe_path), f"{remote_dir}/{test_name}")
    adb("shell", f"chmod +x {remote_dir}/{test_name}")
    if use_icd_loader:
        # ICD Loader 模式：需 OCL_ICD_FILENAMES 指定驱动库才能枚举平台
        driver = probe_ocl_driver()
        adb("shell", f"cd {remote_dir}; OCL_ICD_FILENAMES={driver} ./{test_name}")
    else:
        adb("shell", f"{remote_dir}/{test_name}")


def invoke_shell_capture(cmd):
    """执行 adb shell 命令并返回远程输出（用于探测，不打印/不记日志）"""
    exe = ["adb"]
    if sid:
        exe += ["-s", sid]
    exe += ["shell", cmd]
    r = subprocess.run(exe, capture_output=True, text=True, encoding="utf-8",
                       errors="replace")
    return r.stdout + r.stderr


def probe_ocl_driver():
    """ICD Loader 模式需要 OCL_ICD_FILENAMES 指定驱动库，按 GPU 厂商探测。

    参考 opencl-notes/4-定位android设备上的opencl库.md 的已知路径表（仅 adb）。
    """
    driver_paths = {
        "samsung_exynos": ["/vendor/lib64/libOpenCL_samsung.so",
                           "/vendor/lib64/libSGPUOpenCL.so"],
        "qualcomm_adreno": ["/vendor/lib64/libOpenCL_adreno.so"],
        "arm_mali": ["/vendor/lib64/libGLES_mali.so"],
        "powervr": ["/vendor/lib64/libPVROCL.so"],
    }
    gpu_keywords = [
        ("samsung_exynos", ["exynos", "erd9", "erd8", "s5e", "samsungexynos"]),
        ("qualcomm_adreno", ["adreno", "qualcomm", "sm8", "sm8650", "sm8550",
                             "kalama", "sa8295", "sa8397", "sdm8"]),
        ("arm_mali", ["mali", "mt6", "mtk", "rockchip", "rk35", "rk3588",
                      "unisoc", "t61", "t620", "t760", "g720"]),
        ("powervr", ["powervr", "sgx", "rogue"]),
    ]
    print("==== 探测 OpenCL 驱动库（ICD Loader 模式） ====")
    blob = " ".join(invoke_shell_capture(f"getprop {k}").strip().lower()
                    for k in ["ro.product.brand", "ro.board.platform",
                              "ro.soc.model", "ro.hardware", "ro.hardware.egl"])
    for key, kws in gpu_keywords:
        if any(kw in blob for kw in kws):
            for p in driver_paths[key]:
                if "__YES__" in invoke_shell_capture(f"[ -e {p} ] && echo __YES__"):
                    print(f"  识别到 {key}，驱动库: {p}")
                    return p
    raise RuntimeError("无法自动探测 OpenCL 驱动库；ICD Loader 模式需手动设置 OCL_ICD_FILENAMES")


# ---------------------------------------------------------------------------
# 打印参数
# ---------------------------------------------------------------------------
def show_params():
    print("==== 参数 ====")
    vals = {
        "build_dir": build_dir, "ndk_root": ndk_root, "test_name": test_name,
        "api": api, "abi": abi, "sid": sid or "<自动>",
        "remote_dir": remote_dir,
        "use_icd_loader": use_icd_loader, "log_enabled": log_enabled,
    }
    for k, v in vals.items():
        print(f"{k} = {v}")
    print("==============")


# ---------------------------------------------------------------------------
# 主流程
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    detect_sid()
    show_params()
    try:
        exe = build()
        deploy_and_run(exe)
    except Exception as e:
        print(f"中止: {e}", file=sys.stderr)
        sys.exit(1)