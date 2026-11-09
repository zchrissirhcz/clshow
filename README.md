# clshow —— 显示 OpenCL 信息的 Android app

基于 [ncnn-android-vkpeak](https://github.com/nihui/ncnn-android-vkpeak) 工程骨架改造。
**去掉了 Vulkan 峰值测试与 ncnn 依赖**，改为显示设备的 OpenCL 信息（平台 / 设备的基本信息），
枚举逻辑参考 `clshow.cpp`。

本工程位于 `C:\work\clshow`，与原始仓库相互独立，不修改原版 `ncnn-android-vkpeak`。

## 架构

```
C:\work\clshow/
├── app/
│   └── src/main/
│       ├── java/zz/clshow/
│       │   ├── ClShow.java         #   native 封装（System.loadLibrary("clshow")）
│       │   └── MainActivity.java   #   简单 UI，后台线程调 native 后显示多行文本
│       ├── jni/                    #   C++ 层（CMake 工程）
│       │   ├── 3rdparty/           #   自带的第三方依赖（无需 SDK / submodule）
│       │   │   └── OpenCL-Headers/     #     Khronos OpenCL 头文件（纯头文件）
│       │   ├── opencl_shim/        #   【独立 Loader 库】跨平台加载 OpenCL + 映射全部 cl API（由脚本生成）
│       │   │   ├── opencl_shim.h/.cpp    #    加载逻辑（Android SP-HAL / Windows OpenCL.dll）
│       │   │   └── opencl_shim_apis.*    #    全部标准 cl API 映射表（自动生成）
│       │   ├── clshow_core.h/.cpp  #   【核心库】依赖 opencl_shim，枚举平台/设备信息
│       │   ├── clshow_jni.cpp      #   JNI 层，供 app 的 Java 层调用
│       │   ├── clshow_cli.cpp      #   【CLI 可执行】main() 入口，与 Java 层并列
│       │   └── CMakeLists.txt
│       ├── AndroidManifest.xml
│       └── res/
├── tools/gen_opencl_shim.py        # 从 cl.h 生成整个 opencl_shim 库源码（升级头文件时重跑）
├── build-android-cli.py            # 一键脚本：编译 clshow_cli 并部署到设备运行
├── .github/workflows/release-apk.yml
├── build.gradle / settings.gradle / gradlew
└── README.md
```

`clshow_core` 是核心业务逻辑，被两个使用方复用；`opencl_shim` 是独立的跨平台
OpenCL Loader 库，负责加载运行库并映射全部 API，与业务解耦：

| 库 | 用途 |
|----|------|
| `opencl_shim` | 跨平台加载 OpenCL（Android 优先 SP-HAL，Windows 用 OpenCL.dll），映射全部标准 cl API 为 `p_clXxx` 函数指针 |
| `clshow_core` | 依赖 `opencl_shim`，枚举平台/设备信息成多行字符串 |
| `libclshow.so`（JNI 库） | 打包进 APK，供 Java 层显示 |
| `clshow_cli`（可执行） | 命令行直接查看，与 JNI 共用同一套 `opencl_shim` 加载逻辑 |

### opencl_shim 是脚本生成的

`opencl_shim/` 下的全部 4 个源文件都由 `tools/gen_opencl_shim.py` 生成：
- `opencl_shim.h` / `opencl_shim.cpp`：加载逻辑（固定模板，改它需改脚本模板）
- `opencl_shim_apis.h` / `.cpp`：从 `cl.h` 自动提取的全部标准 cl API 映射表

**升级 OpenCL 头文件版本时**，只需换掉 `3rdparty/OpenCL-Headers/` 里的头文件，
然后重新跑一次：

```bash
python tools/gen_opencl_shim.py \
    app/src/main/jni/3rdparty/OpenCL-Headers/<新版本>/CL/cl.h \
    app/src/main/jni/opencl_shim
```

脚本会自动标注检测到的 OpenCL 版本和 API 数量，并重新生成全部文件。

## 关键实现

- **OpenCL 头文件**：来自 `app/src/main/jni/3rdparty/OpenCL-Headers/2026.05.29`（Khronos 纯头文件，无需 Android SDK）。
- **自写 OpenCL Loader（opencl_shim）**：核心库不直接加载 OpenCL，而是依赖独立库 `opencl_shim`。
  它跨平台加载运行库并把全部标准 `cl*` API 映射成 `p_clXxx` 函数指针：
  - **Android**：优先用 SP-HAL——`dlopen("libvndksupport.so")` 解析 `android_load_sphal_library()`
    从 sphal namespace 加载厂商 `libOpenCL.so`（Android 12+ 加载 vendor 驱动的正规途径，
    参考 flutter_gemma）；不可用时降级 `dlopen("libOpenCL.so")`。
  - **Windows**：`LoadLibrary("OpenCL.dll")`。
  - app 与 CLI 完全共用同一套 `opencl_shim`，实现真正统一。
- **加载方式可切换**：提供编译开关 `CLSHOW_USE_ICD_LOADER`，可在 `opencl_shim` 与
  Khronos ICD Loader 静态库之间切换，便于对比验证（见下文"加载方式开关"）。
- **本地构建不需要 Android SDK**：用本机 `android-ndk` 的交叉编译器即可编译 native（见下）。

## 加载方式开关

默认用自写 `opencl_shim`（动态加载厂商 `libOpenCL.so`，任意平台通用）。也可切到
Khronos ICD Loader 静态库（仅 Android arm64），用于对比两种方案的差异。

**CLI**（`build-android-cli.py`）：
```bash
python build-android-cli.py                        # 默认 opencl_shim
python build-android-cli.py --use-icd-loader       # 用 Khronos ICD Loader
```

**APK**（gradle 属性）：
```bash
./gradlew assembleDebug                                  # 默认 opencl_shim
./gradlew assembleDebug -Pclshow.useIcdLoader=true        # 用 Khronos ICD Loader
```

> 说明：ICD Loader 模式在 CLI（shell 进程）下可用（脚本会自动探测驱动库并注入
> `OCL_ICD_FILENAMES`）；但在 **app 进程里受 linker namespace 限制**，通常返回
> `-1001`，这正是两种方案的差异所在，可用此开关对比验证。

## 在 Android 上构建并运行控制台程序 clshow_cli

`clshow_cli` 是独立的本机可执行程序（`app/src/main/jni/clshow_cli.cpp`），
不需要 APK，直接推送到手机即可在终端查看 OpenCL 信息。用仓库根目录的
`build-android-cli.py` 一键完成：**检测设备 → 用 NDK 交叉编译 → 推送 → 运行**。
（`opencl_shim` 自动加载厂商 `libOpenCL.so`，无需任何手动配置。）

前提：手机通过 USB 连接电脑并已开启调试（`adb devices` 能看到设备），
且已安装 Python 3。

```powershell
cd C:\work\clshow
python build-android-cli.py                        # 自动探测唯一连接的设备
python build-android-cli.py --sid <序列号>        # 指定设备（多台设备时）
python build-android-cli.py --no-log              # 关闭日志，仅控制台输出
```

脚本默认参数：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `-r/--ndk-root` | `C:\soft\toolchains\android-ndk\r27c` | 本机 NDK 路径 |
| `--api` | `33` | 编译目标 Android API 级别 |
| `-B/--build-dir` | `build-android` | 编译产物目录 |
| `--remote-dir` | `/data/local/tmp/clshow` | 设备上的运行目录 |
| `--sid` | 空 | 设备序列号；空则自动探测 |

脚本运行后会直接打印手机上的 OpenCL 信息；日志默认写入
`build-android-cli-build-android-*.log`（UTF-8）。

### 手动编译（等价命令，可自行调用）

> 注意：clshow 依赖 `opencl_shim` 独立 loader 库，手动编译时需一起编译。
> 下面是等价于 `build-android-cli.py` 的手动命令（arm64-v8a）：

```powershell
$ndk = "C:\soft\toolchains\android-ndk\r27c"
$bin = "$ndk\toolchains\llvm\prebuilt\windows-x86_64\bin"
$jni = "app\src\main\jni"
$inc = "app\src\main\jni\3rdparty\OpenCL-Headers\2026.05.29"
$shim = "app\src\main\jni\opencl_shim"

& "$bin\aarch64-linux-android33-clang++.cmd" -std=c++11 -DCL_TARGET_OPENCL_VERSION=300 -fPIE -pie `
    -I"$inc" -I"$jni" -o clshow_cli `
    "$jni\clshow_core.cpp" "$jni\clshow_cli.cpp" `
    "$shim\opencl_shim.cpp" "$shim\opencl_shim_apis.cpp" -ldl
```

```powershell
adb push clshow_cli /data/local/tmp/clshow_cli
adb shell chmod +x /data/local/tmp/clshow_cli
# opencl_shim 会自动加载厂商 libOpenCL.so（Android 12+ 走 SP-HAL），无需设环境变量
adb shell /data/local/tmp/clshow_cli
```

### 编译 JNI 共享库 libclshow.so（APK 用，非控制台）

```powershell
& "$bin\aarch64-linux-android33-clang++.cmd" -std=c++11 -DCL_TARGET_OPENCL_VERSION=300 -shared -fPIC `
    -I"$inc" -I"$jni" -I"$ndk\toolchains\llvm\prebuilt\windows-x86_64\sysroot\usr\include" `
    -o libclshow.so "$jni\clshow_core.cpp" "$jni\clshow_jni.cpp" `
    "$shim\opencl_shim.cpp" "$shim\opencl_shim_apis.cpp" -llog -ldl
```

> 本机（Qualcomm Adreno 830）实测 `clshow_cli` 输出示例：
> ```
> Number of platforms: 1
> == Platform 0 ==
> Name: QUALCOMM Snapdragon(TM)
> Version: OpenCL 3.0 QUALCOMM build: 0800.71
> devices: 1
> --- Device 0 ---
>   Name: QUALCOMM Adreno(TM) 830
>   Vendor: QUALCOMM
>   OpenCL: OpenCL 3.0 Adreno(TM) 830
>   Driver: OpenCL 3.0 QUALCOMM build: 0800.71 Compiler E031.47.18.49
>   OpenCL C: OpenCL C 3.0 Adreno(TM) 830
>   Max compute units: 12
>   Max work group size: 1024
>   Global mem size: 7923818496
> ```

## 踩过的坑

以下是本项目从"静态链接 ICD Loader"→"OCL_ICD_FILENAMES"→最终"自写 opencl_shim +
SP-HAL"过程中实测遇到的坑与结论，供日后维护参考。

### 1. 静态链接 ICD Loader 返回 -1001 —— 不是 loader 的错，是没指定驱动库

最初把 Khronos ICD Loader 静态链进程序，运行 `clGetPlatformIDs` 返回 `-1001`，曾一度误判成
"这台设备用不了 ICD Loader"。实际上 `-1001` 就是 `CL_PLATFORM_NOT_FOUND_KHR`：
ICD Loader 本身只做转发，**它得先找到驱动库才能枚举平台**。

- Linux/PC 上，ICD Loader 默认扫描 `/etc/OpenCL/vendors/` 下的 `.icd` 文件找到驱动；
- **Android 上没有这个目录**，厂商驱动（如 `/vendor/lib64/libOpenCL_adreno.so`）
  不会自动被发现，所以 ICD Loader 一个平台都枚举不出来。

**结论**：Khronos ICD Loader 静态链接方案在 Android 上不可行（既缺 ICD 文件，
又受 namespace 限制），最终被自写 `opencl_shim` 取代。

### 2. Android 上 OpenCL 库有三种，别混为一谈

参考 `opencl-notes/4-定位android设备上的opencl库.md`：

| 类型 | 特征 | 能否被 ICD Loader 用 |
|------|------|----------------------|
| 实现库（如 `/vendor/lib64/libOpenCL.so`） | 导出全部标准 `cl*` API | 应用可直接链接 |
| 驱动库（如 `/vendor/lib64/libOpenCL_adreno.so`） | 只导出 `clIcdGetPlatformIDsKHR` 等 ICD 接口 | 被 ICD Loader 运行时发现并分派 |
| Loader 库 | 导出全部 `cl*` API 但只转发 | 应用链接它 |

**判断驱动库的关键符号是 `clIcdGetPlatformIDsKHR`**。关键观察：**`libOpenCL.so`
几乎每个设备都有，是厂商的标准对外入口**（壳或完整实现）。这决定了最终方案：
统一加载 `libOpenCL.so` 即可，无需按厂商精确指定驱动库。

### 3. Android app 进程的 linker namespace 限制（SP-HAL 的由来）

CLI（shell 进程）能 `dlopen` 任意库，但 **app 进程运行在隔离的 linker namespace**：
- `dlopen("/vendor/lib64/libOpenCL.so")` = OK（manifest `uses-native-library` 声明后可见）
- `dlopen("/vendor/lib64/libOpenCL_adreno.so")` = **失败**，报
  `not accessible for the namespace "clns-10"`

因此 app 里 Khronos ICD Loader 无法加载厂商驱动库（`libOpenCL_adreno.so`）。

**解法**：Android 12+ 用 **SP-HAL**——`dlopen("libvndksupport.so")` 解析
`android_load_sphal_library()`，从 sphal namespace 加载 vendor 库，绕过 namespace 限制。
这是加载 vendor OpenCL 驱动的正规途径（参考 flutter_gemma）。这也是 `opencl_shim`
在 Android 上优先走 SP-HAL 的原因。注意 `android_load_sphal_library` **不是 NDK 公共 API**，
头文件里没有声明，必须运行时动态解析。

### 4. 别只取 4 个 API 绕开 loader

早期版本用 `dlopen` + `dlsym` 只解析 `clGetPlatformIDs` 等 4 个函数指针，违背了
"映射全部 OpenCL API"的设计意图。**正确做法**是像 `opencl_shim` 那样，用脚本从 `cl.h`
自动提取全部标准 cl API 并映射成 `p_clXxx` 指针，天然覆盖全部 API，且头文件升级可重生成。

### 5. Windows 没有可静态链接的 ICD Loader

Windows 上 OpenCL 是 `OpenCL.dll`（NVIDIA/AMD 驱动自带的完整实现库），无 `.lib` 可静态链接。
故 `opencl_shim` 在 `_WIN32` 下用 `LoadLibrary("OpenCL.dll")` + `GetProcAddress` 动态解析。

### 6. 其他小坑

- **`CL_PLATFORM_NOT_FOUND_KHR` 未声明**：它定义在 `cl_ext.h`，而 `cl.h` 不保证自动包含它，
  故代码里直接比较数值 `-1001` 而非符号名。
- **MSVC 中文注释乱码**：源码含中文注释，Windows 默认代码页 936 会解析错乱（`C1019` 等），
  CMake 里加 `/source-charset:utf-8` 解决。
- **`cl_version.h` 警告**：用 `add_compile_definitions(CL_TARGET_OPENCL_VERSION=300)`
  指定目标版本消除。
- **API 生成脚本的坑**：cl.h 里 deprecated 函数的返回类型前带
  `CL_API_PREFIX__VERSION_*_DEPRECATED` 宏，在函数指针声明里会展开成属性导致编译失败，
  生成脚本需剥离该前缀（见 `tools/gen_opencl_shim.py`）。

## 一键构建并安装 app

用仓库根目录的 `build-app.py` 一键完成：**用 Gradle 编译 APK → 输出路径 → 安装到设备 → 启动**。

```bash
python build-app.py                        # 默认 opencl_shim 模式
python build-app.py --use-icd-loader       # 用 Khronos ICD Loader
python build-app.py --sid <序列号>         # 指定设备（多台时）
python build-app.py --no-install           # 只编译，不安装不启动
```

脚本会自动：
- 设置 `JAVA_HOME`（本机为 `C:\soft\jdk-21`，Gradle 8.12 需 JDK21）
- 按模式传入 gradle 属性（`-Pclshow.useIcdLoader=true`）
- 打印生成的 APK 路径
- `adb install -r` 安装并 `am start` 启动

app 界面顶部会显示当前加载方式：

```
Device: 2410DPN6CC
Android: 16
ABI: arm64-v8a
Loader: opencl_shim        ← 当前加载方式（opencl_shim 或 icd_loader）
----------------------------------------
Number of platforms: 1
...
```

便于直观对比两种方案。

## 构建 APK（GitHub Actions）

完整 APK 由 GitHub Actions 的 workflow 构建（`.github/workflows/release-apk.yml`），
它会在 CI 环境自动安装 Android SDK 组件（platform 33 / build-tools / NDK 27），
然后 `./gradlew assembleRelease` 并签名发布。

把本目录推送到 GitHub 仓库后，在仓库页面 **Actions → release-apk → Run workflow** 触发，产出已签名的 APK。