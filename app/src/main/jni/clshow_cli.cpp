// clshow_cli.cpp —— 命令行可执行文件入口。
// 复用核心库 clshow::get_opencl_info()，与 Android app 的 Java 层并列。
// 用于在设备/桌面上直接以命令行方式查看 OpenCL 信息。
#include <cstdio>

#include "clshow_core.h"

int main()
{
    std::printf("%s", clshow::get_opencl_info().c_str());
    return 0;
}