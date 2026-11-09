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
