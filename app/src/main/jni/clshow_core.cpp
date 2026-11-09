// clshow_core.cpp —— OpenCL 信息查询核心库实现。
//
// 加载 OpenCL 有两种模式，由编译开关 CLSHOW_USE_ICD_LOADER 控制：
//   - opencl_shim（默认）：依赖独立库 opencl_shim，动态加载厂商 libOpenCL.so
//     （Android 走 SP-HAL / Windows 走 OpenCL.dll），映射全部 cl API 为 p_clXxx 指针。
//   - ICD Loader：静态链接 Khronos libOpenCL-ICD-Loader.a，直接调用标准 cl* 符号。
// 两种模式下 clshow_core 的 API 调用通过下方宏统一，业务逻辑一致。

#include "clshow_core.h"

#include <cstdio>

#include <CL/cl.h>

#ifdef CLSHOW_USE_ICD_LOADER
// ---- ICD Loader 模式：标准符号由静态链接的 libOpenCL-ICD-Loader.a 提供 ----
#  define CLSHIM_GetPlatformIDs    clGetPlatformIDs
#  define CLSHIM_GetPlatformInfo   clGetPlatformInfo
#  define CLSHIM_GetDeviceIDs      clGetDeviceIDs
#  define CLSHIM_GetDeviceInfo     clGetDeviceInfo
#  define CLSHIM_loaded()          true
#else
// ---- opencl_shim 模式：用 p_ 指针，需先 opencl_shim_load() ----
#  include "opencl_shim/opencl_shim.h"
#  define CLSHIM_GetPlatformIDs    p_clGetPlatformIDs
#  define CLSHIM_GetPlatformInfo   p_clGetPlatformInfo
#  define CLSHIM_GetDeviceIDs      p_clGetDeviceIDs
#  define CLSHIM_GetDeviceInfo     p_clGetDeviceInfo
#  define CLSHIM_loaded()          opencl_shim_load()
#endif

namespace clshow {

std::string loader_mode()
{
#ifdef CLSHOW_USE_ICD_LOADER
    return "icd_loader";
#else
    return "opencl_shim";
#endif
}

static void line(std::string& s, const char* label, const char* value)
{
    s += label;
    s += ": ";
    s += value;
    s += "\n";
}

static void pstr(std::string& s, cl_platform_id p, cl_platform_info name, const char* label)
{
    char buf[1024];
    size_t len = 0;
    if (CLSHIM_GetPlatformInfo(p, name, sizeof(buf), buf, &len) == CL_SUCCESS)
        line(s, label, buf);
}

static void dstr(std::string& s, cl_device_id d, cl_device_info name, const char* label)
{
    char buf[1024];
    size_t len = 0;
    if (CLSHIM_GetDeviceInfo(d, name, sizeof(buf), buf, &len) == CL_SUCCESS)
        line(s, label, buf);
}

static void dnum(std::string& s, cl_device_id d, cl_device_info name, const char* label)
{
    cl_ulong v = 0;
    if (CLSHIM_GetDeviceInfo(d, name, sizeof(v), &v, nullptr) == CL_SUCCESS)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%llu", (unsigned long long)v);
        line(s, label, buf);
    }
}

std::string get_opencl_info()
{
    if (!CLSHIM_loaded())
        return "Failed to load OpenCL\n";

    std::string s;

    cl_uint n = 0;
    cl_int err = CLSHIM_GetPlatformIDs(0, nullptr, &n);
    if (err != CL_SUCCESS)
    {
        s = "clGetPlatformIDs failed";
        if (err == -1001) // CL_PLATFORM_NOT_FOUND_KHR
        {
            s += " (CL_PLATFORM_NOT_FOUND_KHR, -1001)\n";
            s += "OpenCL 库已加载但未引导到驱动。\n";
        }
        else
        {
            char num[32];
            snprintf(num, sizeof(num), " (%d)\n", (int)err);
            s += num;
        }
        return s;
    }

    char num[64];
    snprintf(num, sizeof(num), "Number of platforms: %u\n", n);
    s = num;
    if (n == 0)
    {
        s += "No OpenCL platform found\n";
        return s;
    }

    auto* plats = new cl_platform_id[n];
    CLSHIM_GetPlatformIDs(n, plats, nullptr);
    for (cl_uint i = 0; i < n; i++)
    {
        snprintf(num, sizeof(num), "\n== Platform %u ==\n", i);
        s += num;
        pstr(s, plats[i], CL_PLATFORM_NAME, "Name");
        pstr(s, plats[i], CL_PLATFORM_VERSION, "Version");

        cl_uint nd = 0;
        CLSHIM_GetDeviceIDs(plats[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &nd);
        snprintf(num, sizeof(num), "devices: %u\n", nd);
        s += num;

        auto* devs = new cl_device_id[nd ? nd : 1];
        if (nd)
            CLSHIM_GetDeviceIDs(plats[i], CL_DEVICE_TYPE_ALL, nd, devs, nullptr);
        for (cl_uint j = 0; j < nd; j++)
        {
            snprintf(num, sizeof(num), "--- Device %u ---\n", j);
            s += num;
            dstr(s, devs[j], CL_DEVICE_NAME, "  Name");
            dstr(s, devs[j], CL_DEVICE_VENDOR, "  Vendor");
            dstr(s, devs[j], CL_DEVICE_VERSION, "  OpenCL");
            dstr(s, devs[j], CL_DRIVER_VERSION, "  Driver");
            dstr(s, devs[j], CL_DEVICE_OPENCL_C_VERSION, "  OpenCL C");
            dnum(s, devs[j], CL_DEVICE_MAX_COMPUTE_UNITS, "  Max compute units");
            dnum(s, devs[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, "  Max work group size");
            dnum(s, devs[j], CL_DEVICE_GLOBAL_MEM_SIZE, "  Global mem size");
            s += "\n";
        }
        delete[] devs;
    }
    delete[] plats;
    return s;
}

} // namespace clshow