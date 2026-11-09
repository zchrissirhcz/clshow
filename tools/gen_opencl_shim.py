#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
从 Khronos OpenCL 头文件 cl.h 生成整个 opencl_shim 独立库的全部源码。

以后升级 OpenCL 头文件版本时，只需重新运行本脚本即可整体重新生成。

用法:
    python gen_opencl_shim.py <path/to/cl.h> <output_dir>

在 <output_dir> 生成 4 个文件:
    opencl_shim.h         # Loader 库公共头文件（固定模板）
    opencl_shim.cpp       # Loader 库加载逻辑（固定模板）
    opencl_shim_apis.h    # 全部标准 cl API 的函数指针 extern 声明（动态生成）
    opencl_shim_apis.cpp  # 函数指针定义 + 名称->地址映射表 + opencl_shim_export()（动态生成）

其中固定模板（opencl_shim.h / .cpp）为手写逻辑，API 映射表随头文件版本自动更新。
"""
import os
import re
import sys


# ---------------------------------------------------------------------------
# 1. 从 cl.h 提取全部标准 cl API 签名
# ---------------------------------------------------------------------------
def extract_funcs(cl_h):
    """解析 cl.h，返回 [(fn_name, return_type, param_list), ...]"""
    text = open(cl_h, "r", encoding="utf-8").read()
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.DOTALL)  # 去块注释

    funcs = []
    pat = re.compile(
        r"extern\s+CL_API_ENTRY\s+(?P<ret>[^;]+?)\s+CL_API_CALL\s+"
        r"(?P<fn>cl[A-Za-z0-9]+)\s*"
        r"\((?P<params>[^()]*)\)\s*CL_API_SUFFIX__VERSION_[A-Za-z0-9_]+;",
        re.DOTALL,
    )
    for m in pat.finditer(text):
        fn = m.group("fn").strip()
        ret = m.group("ret").strip()
        # 剥离 deprecated 前缀宏（函数指针声明里不能出现属性宏）
        ret = re.sub(r"CL_API_PREFIX__VERSION_[A-Za-z0-9_]*_DEPRECATED\s+", "", ret)
        params = re.sub(r"\s+", " ", m.group("params")).strip()
        funcs.append((fn, ret, params))

    # 去重保序
    seen = set()
    out = []
    for f in funcs:
        if f[0] not in seen:
            seen.add(f[0])
            out.append(f)
    return out


def extract_version(cl_h_dir):
    """从 cl_version.h 提取最高 OpenCL 版本（如 3.1），用于注释标注。"""
    vp = os.path.join(cl_h_dir, "cl_version.h")
    if not os.path.exists(vp):
        return "?"
    text = open(vp, "r", encoding="utf-8").read()
    vers = re.findall(r"#define\s+CL_VERSION_(\d+)_(\d+)", text)
    if not vers:
        return "?"
    maj, min_ = max((int(a), int(b)) for a, b in vers)
    return f"{maj}.{min_}"


# ---------------------------------------------------------------------------
# 2. 固定模板：opencl_shim.h / opencl_shim.cpp
# ---------------------------------------------------------------------------
TEMPLATE_HEADER = """\
// opencl_shim.h —— 自写的跨平台 OpenCL Loader 库。
//
// 由 tools/gen_opencl_shim.py 生成，勿手改（如需改加载逻辑，改脚本里的模板）。
//
// 功能：把"加载 OpenCL 运行库 + 映射全部标准 cl API 函数指针"封装成独立库，
//       供上层（如 clshow 的查询逻辑）复用，与业务解耦。
//
// 加载策略（Android 上优先用 SP-HAL，这是 Android 12+ 加载 vendor 驱动的正规途径）：
//   [1] Android 12+：dlopen("libvndksupport.so") 解析 android_load_sphal_library()，
//       用它从 sphal namespace 加载厂商 OpenCL 驱动库（libOpenCL.so 等）。
//       这能绕过 app linker namespace 限制，跨厂商通用。
//   [2] 降级：android_load_sphal_library 不可用时，退化为 dlopen("libOpenCL.so")
//       （或按名称），厂商 loader 内部引导驱动。
//   [3] Windows：LoadLibrary("OpenCL.dll")。
//
// 加载成功后，所有 p_clXxx 函数指针指向对应符号，可直接调用。
#pragma once

#include <CL/cl.h>

// 是否已成功加载并解析 OpenCL
bool opencl_shim_loaded();

// 加载 OpenCL 并解析全部 cl API 符号到 p_clXxx 指针。
// 返回 true 表示成功。可重复调用（内部缓存）。
bool opencl_shim_load();

// 卸载（释放句柄、清空指针）。一般不主动调用。
void opencl_shim_unload();

// 生成的全部函数指针声明（__API_COUNT__ 个标准 cl API）
#include "opencl_shim_apis.h"
"""

TEMPLATE_SOURCE = """\
// opencl_shim.cpp —— 自写的跨平台 OpenCL Loader 库实现。
//
// 由 tools/gen_opencl_shim.py 生成，勿手改（如需改加载逻辑，改脚本里的模板）。
//
// Android 加载策略（优先级）：
//   [1] 优先用 SP-HAL：dlopen("libvndksupport.so") 解析 android_load_sphal_library()，
//       从 sphal namespace 加载厂商 OpenCL 驱动库（如 libOpenCL.so）。
//       这是 Android 12+ 上加载 vendor 驱动的正规途径（参考 flutter_gemma），
//       能绕过 app linker namespace 限制，跨厂商通用。
//   [2] 降级：android_load_sphal_library 不可用时，退化为 dlopen("libOpenCL.so")。
//   [3] Windows：LoadLibrary("OpenCL.dll")。
//
// 加载成功后用 opencl_shim_export() 解析全部 cl API 符号。

#include "opencl_shim.h"

#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {

// 已加载的 OpenCL 库句柄（进程生命周期内持有）
void* g_handle = nullptr;
bool  g_tried  = false;

// 解析 android_load_sphal_library（Android 专用，非 NDK 公共 API，需动态解析）
void* load_via_sphal(const char* name)
{
#ifndef _WIN32
    // libvndksupport.so 提供 android_load_sphal_library / android_unload_sphal_library
    void* vndk = dlopen("libvndksupport.so", RTLD_NOW | RTLD_LOCAL);
    if (!vndk)
        return nullptr;
    // AOSP 签名: void* android_load_sphal_library(const char* name, int flags);
    typedef void* (*load_sphal_fn)(const char*, int);
    auto load_sphal = (load_sphal_fn)dlsym(vndk, "android_load_sphal_library");
    if (!load_sphal)
    {
        dlclose(vndk);
        return nullptr;
    }
    void* h = load_sphal(name, RTLD_NOW | RTLD_LOCAL);
    // 保留 vndk 句柄不关，避免符号失效（小代价）
    return h;
#else
    (void)name;
    return nullptr;
#endif
}

} // namespace

bool opencl_shim_loaded()
{
    return g_handle != nullptr;
}

bool opencl_shim_load()
{
    if (g_handle)
        return true;
    if (g_tried)
        return false;
    g_tried = true;

#ifdef _WIN32
    HMODULE h = LoadLibraryA("OpenCL.dll");
    g_handle = (void*)h;
    if (h && opencl_shim_export(h))
        return true;
#else
    // [1] 优先 SP-HAL 加载厂商 libOpenCL.so（Android 12+ 正规途径）
    void* h = load_via_sphal("libOpenCL.so");
    if (!h)
        // [2] 降级：直接 dlopen libOpenCL.so
        h = dlopen("libOpenCL.so", RTLD_NOW | RTLD_LOCAL);
    if (h)
    {
        // 用解析到的符号数量判断：核心符号必须解析到
        unsigned n = opencl_shim_export(h);
        if (n > 0 && p_clGetPlatformIDs && p_clGetDeviceIDs)
        {
            g_handle = h;
            return true;
        }
        // 加载成功但没解析出核心符号，按失败处理
#ifdef _WIN32
        FreeLibrary((HMODULE)h);
#else
        dlclose(h);
#endif
        return false;
    }
#endif

    return false;
}

void opencl_shim_unload()
{
    if (g_handle)
    {
#ifndef _WIN32
        dlclose(g_handle);
#else
        FreeLibrary((HMODULE)g_handle);
#endif
        g_handle = nullptr;
    }
}
"""


# ---------------------------------------------------------------------------
# 3. 动态生成：opencl_shim_apis.h / opencl_shim_apis.cpp
# ---------------------------------------------------------------------------
def gen_apis_header(funcs, out_path, version):
    lines = [
        "// opencl_shim_apis.h —— 由 tools/gen_opencl_shim.py 自动生成，勿手改",
        f"// 基于 OpenCL {version} 头文件，共 {len(funcs)} 个标准 cl API",
        "#pragma once",
        "#include <CL/cl.h>",
        "",
        'extern "C" {',
        "",
        "// 每个标准 cl API 的函数指针",
    ]
    for fn, ret, params in funcs:
        p = "void" if params == "void" else params
        lines.append(f"extern {ret} (*p_{fn})({p});")
    lines += [
        "",
        "// 解析全部 cl API 符号到 p_* 指针（从已加载的 OpenCL 库）。",
        "// 返回解析成功的个数。",
        "unsigned opencl_shim_export(void* handle);",
        "",
        '} // extern "C"',
        "",
    ]
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print(f"wrote {out_path} ({len(funcs)} functions)")


def gen_apis_source(funcs, out_path, version):
    lines = [
        "// opencl_shim_apis.cpp —— 由 tools/gen_opencl_shim.py 自动生成，勿手改",
        f"// 基于 OpenCL {version} 头文件，共 {len(funcs)} 个标准 cl API",
        '#include "opencl_shim_apis.h"',
        "",
        "#ifdef _WIN32",
        "#include <windows.h>",
        "#define SHIM_DLSYM(h, s) ((void*)GetProcAddress((HMODULE)(h), (s)))",
        "#else",
        "#include <dlfcn.h>",
        "#define SHIM_DLSYM(h, s) dlsym((h), (s))",
        "#endif",
        "",
        "// 定义函数指针",
    ]
    for fn, ret, params in funcs:
        p = "void" if params == "void" else params
        lines.append(f"{ret} (*p_{fn})({p}) = nullptr;")
    lines += [
        "",
        "unsigned opencl_shim_export(void* handle)",
        "{",
        "    if (!handle) return 0;",
        "    unsigned n = 0;",
        "    struct { const char* name; void** p; } tab[] = {",
    ]
    for fn, ret, params in funcs:
        lines.append(f'        {{ "{fn}", (void**)&p_{fn} }},')
    lines += [
        "    };",
        "    for (unsigned i = 0; i < sizeof(tab)/sizeof(tab[0]); i++)",
        "    {",
        "        void* sym = SHIM_DLSYM(handle, tab[i].name);",
        "        if (sym) { *tab[i].p = sym; n++; }",
        "    }",
        "    return n;",
        "}",
        "",
    ]
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print(f"wrote {out_path}")


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    cl_h = sys.argv[1]
    out_dir = sys.argv[2]
    os.makedirs(out_dir, exist_ok=True)

    version = extract_version(os.path.dirname(cl_h))
    funcs = extract_funcs(cl_h)
    if not funcs:
        print("ERROR: 未提取到任何函数，请检查 cl.h 路径", file=sys.stderr)
        sys.exit(1)
    print(f"OpenCL {version}：提取到 {len(funcs)} 个函数")

    # 固定模板
    header = TEMPLATE_HEADER.replace("__API_COUNT__", str(len(funcs)))
    with open(os.path.join(out_dir, "opencl_shim.h"), "w", encoding="utf-8") as f:
        f.write(header)
    with open(os.path.join(out_dir, "opencl_shim.cpp"), "w", encoding="utf-8") as f:
        f.write(TEMPLATE_SOURCE)
    print(f"wrote {out_dir}/opencl_shim.h, opencl_shim.cpp")

    # 动态 API 表
    gen_apis_header(funcs, os.path.join(out_dir, "opencl_shim_apis.h"), version)
    gen_apis_source(funcs, os.path.join(out_dir, "opencl_shim_apis.cpp"), version)


if __name__ == "__main__":
    main()