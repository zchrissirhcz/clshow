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

// 生成的全部函数指针声明（104 个标准 cl API）
#include "opencl_shim_apis.h"
