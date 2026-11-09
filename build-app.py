#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 简介: 一键构建 Android app 并部署到设备。
#
# 功能:
#   - 用 Gradle 编译 APK（默认 opencl_shim，可切到 Khronos ICD Loader）
#   - 输出生成的 APK 路径
#   - 安装到已连接的 Android 设备
#   - 启动应用
#
# 用法:
#   python build-app.py                     # 默认 opencl_shim 模式
#   python build-app.py --use-icd-loader    # 用 Khronos ICD Loader
#   python build-app.py --sid <序列号>      # 指定设备（多台时）
#   python build-app.py --no-install        # 只编译，不安装不启动

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent

# 构建完成后 APK 所在目录（Gradle debug 输出）
APK_DIR = SCRIPT_ROOT / "app" / "build" / "outputs" / "apk" / "debug"


# ---------------------------------------------------------------------------
# 参数解析
# ---------------------------------------------------------------------------
ap = argparse.ArgumentParser(description="一键构建 clshow Android app 并部署")
ap.add_argument("--use-icd-loader", dest="use_icd_loader", action="store_true",
                help="用 Khronos ICD Loader 静态库（而非默认的 opencl_shim）")
ap.add_argument("--sid", default="", help="设备序列号；空则自动探测")
ap.add_argument("--no-install", dest="install", action="store_false",
                help="只编译，不安装不启动")
args = ap.parse_args()

use_icd_loader = args.use_icd_loader
sid = args.sid
do_install = args.install


# ---------------------------------------------------------------------------
# 工具
# ---------------------------------------------------------------------------
def run(cmd, *, desc="", env=None):
    """运行命令，实时回显；非零退出码抛错。cmd 为参数列表。"""
    print(f"\n>>> {' '.join(cmd)}")
    r = subprocess.run(cmd, text=True, encoding="utf-8", errors="replace", env=env)
    if r.returncode != 0:
        raise RuntimeError(f"{desc or '命令'}失败 (exit {r.returncode})")


def detect_sid():
    """若用户未传 --sid，探测唯一连接的设备；多台则提示用 --sid"""
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
        raise RuntimeError(f"检测到多台设备 {lines}，请用 --sid 指定序列号")
    # 0 台则交给 adb 默认


def adb(*cmd_args):
    """adb 命令（带 -s sid）"""
    exe = ["adb"]
    if sid:
        exe += ["-s", sid]
    exe += list(cmd_args)
    subprocess.run(exe, check=True)


# ---------------------------------------------------------------------------
# 构建
# ---------------------------------------------------------------------------
def gradlew():
    """找到 gradlew 可执行文件（Windows 用 .bat）"""
    bat = SCRIPT_ROOT / "gradlew.bat"
    if bat.exists():
        return str(bat)
    return str(SCRIPT_ROOT / "gradlew")


def build_apk():
    # 关键：JAVA_HOME 需指向 JDK21（Gradle 8.12 不支持 JDK25）
    env = os.environ.copy()
    if not env.get("JAVA_HOME") and Path(r"C:\soft\jdk-21").exists():
        env["JAVA_HOME"] = r"C:\soft\jdk-21"

    cmd = [gradlew(), "assembleDebug"]
    if use_icd_loader:
        cmd.append("-Pclshow.useIcdLoader=true")
    run(cmd, desc="Gradle 构建", env=env)

    # 找到生成的 APK
    apks = sorted(APK_DIR.glob("*.apk"), key=lambda p: p.stat().st_mtime, reverse=True)
    if not apks:
        raise RuntimeError(f"未找到生成的 APK：{APK_DIR}")
    apk = apks[0]
    print(f"\nAPK 生成: {apk}")
    return apk


# ---------------------------------------------------------------------------
# 主流程
# ---------------------------------------------------------------------------
def main():
    print("==== 一键构建 clshow app ====")
    print(f"加载方式: {'ICD Loader' if use_icd_loader else 'opencl_shim'}")

    apk = build_apk()

    if not do_install:
        print("\n（--no-install，跳过安装/启动）")
        return

    detect_sid()
    print(f"\n==== 安装到设备 {sid or '(adb 默认)'} ====")
    adb("install", "-r", str(apk))
    adb("shell", "am start -n zz.clshow/.MainActivity")
    print("\n完成：app 已安装并启动。请在手机上查看 OpenCL 信息。")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n中止: {e}", file=sys.stderr)
        sys.exit(1)