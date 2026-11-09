cmake_minimum_required(VERSION 3.16)

project(clshow)

# 自带的 Khronos OpenCL 头文件（jni/3rdparty，纯头文件，无需 Android SDK）
set(CLSHOW_OPENCL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/OpenCL-Headers/2026.05.29)
# opencl_shim 独立 loader 库目录
set(CLSHOW_OPENCL_SHIM ${CMAKE_CURRENT_SOURCE_DIR}/opencl_shim)
# Khronos ICD Loader 静态库（Android arm64，仅在 ICD Loader 模式使用）
set(CLSHOW_ICD_LOADER_LIB
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/OpenCL-ICD-Loader/2026.05.29/android-armv8-a-static/lib/libOpenCL-ICD-Loader.a)

# 编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 指定 OpenCL 目标版本，消除 cl_version.h 的“未定义默认 3.1”警告
add_compile_definitions(CL_TARGET_OPENCL_VERSION=300)

# MSVC 下把源文件按 UTF-8 解析（源码含中文注释，默认代码页 936 会解析错乱）
if(MSVC)
    add_compile_options("/source-charset:utf-8")
endif()

# 是否构建命令行可执行文件 clshow / clshow_cli
#  - OFF（默认）：Windows/桌面直接构建 clshow.exe；Android 下构建 JNI 库
#  - ON（deploy-android.py 传）：Android 交叉构建命令行可执行文件 clshow_cli
option(CLSHOW_BUILD_CLI "构建命令行可执行文件" OFF)

# 加载 OpenCL 的方式：
#  - OFF（默认）：用自写 opencl_shim（动态加载厂商 libOpenCL.so / Windows OpenCL.dll）
#  - ON：用 Khronos ICD Loader 静态库（仅 Android arm64 可用，直接调用标准 cl* 符号）
option(CLSHOW_USE_ICD_LOADER "使用 Khronos ICD Loader 静态库（而非 opencl_shim）" OFF)

if(CLSHOW_USE_ICD_LOADER)
    add_compile_definitions(CLSHOW_USE_ICD_LOADER=1)
endif()

# ============================================================
# opencl_shim：自写的跨平台 OpenCL Loader 库（仅非 ICD Loader 模式使用）。
# 负责加载 OpenCL（Android 优先 SP-HAL / 厂商 libOpenCL.so，Windows 用 OpenCL.dll）
# 并把全部标准 cl API 映射成 p_clXxx 函数指针。
# ============================================================
if(NOT CLSHOW_USE_ICD_LOADER)
    add_library(opencl_shim OBJECT
        ${CLSHOW_OPENCL_SHIM}/opencl_shim.cpp
        ${CLSHOW_OPENCL_SHIM}/opencl_shim_apis.cpp)
    target_include_directories(opencl_shim PUBLIC ${CLSHOW_OPENCL_HEADERS} ${CLSHOW_OPENCL_SHIM})
endif()

# 供两个目标共享的 OpenCL 库（根据开关切换）
if(CLSHOW_USE_ICD_LOADER)
    # ICD Loader 模式：静态链接 ICD Loader 库 + dl（运行时加载驱动）
    set(CLSHOW_OPENCL_LIBS dl ${CLSHOW_ICD_LOADER_LIB})
else()
    # opencl_shim 模式：链接 opencl_shim 对象 + dl
    set(CLSHOW_OPENCL_LIBS opencl_shim dl)
endif()

# ============================================================
# 1. Android app 的 JNI 库（供 APK 的 Java 层调用）。
# ============================================================
if(ANDROID AND NOT CLSHOW_BUILD_CLI)
    add_library(clshow SHARED clshow_jni.cpp clshow_core.cpp)
    target_include_directories(clshow PRIVATE ${CLSHOW_OPENCL_HEADERS})
    target_link_libraries(clshow PRIVATE ${CLSHOW_OPENCL_LIBS} log)
endif()

# ============================================================
# 2. 命令行可执行文件（与 APK 的 Java 层并列，便于直接运行查看）。
#    - Windows 原生（deploy-windows.ps1）：默认产出 clshow.exe
#    - Android 交叉（deploy-android.py 传 -DCLSHOW_BUILD_CLI=ON）：产出 clshow_cli
#    与 JNI 库共用同一套加载逻辑。
# ============================================================
if(NOT ANDROID OR CLSHOW_BUILD_CLI)
    set(CLSHOW_EXE_NAME clshow)
    if(CLSHOW_BUILD_CLI)
        set(CLSHOW_EXE_NAME clshow_cli)
    endif()

    add_executable(${CLSHOW_EXE_NAME} clshow_cli.cpp clshow_core.cpp)
    target_include_directories(${CLSHOW_EXE_NAME} PRIVATE ${CLSHOW_OPENCL_HEADERS})
    if(WIN32)
        # Windows 用 LoadLibrary("OpenCL.dll")，无 dl、无 ICD Loader 静态库
        if(NOT CLSHOW_USE_ICD_LOADER)
            target_link_libraries(${CLSHOW_EXE_NAME} PRIVATE opencl_shim)
        endif()
    else()
        # Android / 其他 POSIX：链接 dl + 所选 OpenCL 库
        target_link_libraries(${CLSHOW_EXE_NAME} PRIVATE ${CLSHOW_OPENCL_LIBS})
    endif()
endif()