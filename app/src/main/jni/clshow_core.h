// clshow_core.h —— OpenCL 信息查询核心库。
// 依赖独立库 opencl_shim（或 Khronos ICD Loader）加载 OpenCL 并映射全部 cl API。
// 该库同时被 JNI 层（供 Android app 的 Java/Kotlin 层调用）和 CLI 可执行文件复用。
#pragma once

#include <string>

namespace clshow {

// 返回所有 OpenCL 平台/设备信息的多行字符串。
std::string get_opencl_info();

// 返回当前加载方式描述：默认 "opencl_shim"，ICD Loader 模式为 "icd_loader"。
std::string loader_mode();

} // namespace clshow