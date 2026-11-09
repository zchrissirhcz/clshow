# setup-android-sdk.ps1 —— 一键配置 Android Studio 的 SDK / NDK / CMake 环境
#
# 说明：
#   - SDK 组件从腾讯云镜像下载（官方 dl.google.com 在国内被墙，无法直连）
#   - NDK 使用本机已有的 r27c（C:\soft\toolchains\android-ndk\r27c），不重新下载
#   - CMake / Ninja 使用 PATH 中已有的，不重新下载
#   - 生成 local.properties 供 Android Studio / Gradle 使用
#
# 用法：
#   powershell -ExecutionPolicy Bypass -File setup-android-sdk.ps1

$ErrorActionPreference = "Stop"

# ============================================================
# 可配置项
# ============================================================
$SdkRoot     = "C:\soft\AndroidSdk"                  # SDK 安装目录
$NdkRoot     = "C:\soft\toolchains\android-ndk\r27c" # 已有 NDK 路径
$NdkVersion  = "27.2.12479018"                        # NDK 版本（对应 r27c）
$CmakeRoot   = "C:\soft\cmake\4.2.1"                  # 已有 CMake 路径
$ProjectRoot = "C:\work\clshow"                       # 工程根目录（写 local.properties）
$Mirror      = "https://mirrors.cloud.tencent.com/AndroidSDK"  # SDK 下载镜像
$CmdlineToolsZip = "commandlinetools-win-11076708_latest.zip"  # 版本可调

# 需要下载的 SDK 组件（镜像上的文件名）
$Components = @(
    @{ Zip="platform-tools-latest-windows.zip";   Dest="$SdkRoot\platform-tools" }
    @{ Zip="platform-33_r02.zip";                 Dest="$SdkRoot\platforms\android-33";  RenameFrom="android-13" }
    @{ Zip="build-tools_r34-windows.zip";         Dest="$SdkRoot\build-tools\34.0.0";     RenameFrom="android-14" }
)

Write-Host "==== 配置 Android SDK 环境 ====" -ForegroundColor Cyan
Write-Host "SDK 目录: $SdkRoot"
Write-Host "NDK 使用: $NdkRoot"
Write-Host "CMake 使用: $CmakeRoot"
Write-Host "镜像: $Mirror"

# ============================================================
# 1. 下载并安装 cmdline-tools（提供 sdkmanager）
# ============================================================
$tmp = Join-Path $env:TEMP "clshow-sdk-setup"
if (Test-Path $tmp) { Remove-Item -Recurse -Force $tmp }
New-Item -ItemType Directory -Force -Path $tmp | Out-Null

if (-not (Test-Path "$SdkRoot\cmdline-tools\latest\bin\sdkmanager.bat")) {
    Write-Host "`n[1/4] 下载 cmdline-tools ..." -ForegroundColor Yellow
    Invoke-WebRequest -Uri "$Mirror/$CmdlineToolsZip" -OutFile "$tmp\clt.zip" -UseBasicParsing
    New-Item -ItemType Directory -Force -Path "$tmp\clt_x" | Out-Null
    Expand-Archive -Path "$tmp\clt.zip" -DestinationPath "$tmp\clt_x" -Force
    New-Item -ItemType Directory -Force -Path "$SdkRoot\cmdline-tools\latest" | Out-Null
    Copy-Item -Recurse -Force "$tmp\clt_x\cmdline-tools\*" "$SdkRoot\cmdline-tools\latest\"
    Write-Host "  cmdline-tools 已安装"
} else {
    Write-Host "`n[1/4] cmdline-tools 已存在，跳过"
}

# ============================================================
# 2. 下载 SDK 组件（platform-tools / platform / build-tools）
# ============================================================
Write-Host "`n[2/4] 下载 SDK 组件 ..." -ForegroundColor Yellow
foreach ($c in $Components) {
    $zipPath = "$tmp\$(Split-Path $c.Zip -Leaf)"
    if (-not (Test-Path $c.Dest)) {
        Write-Host "  下载 $($c.Zip) ..."
        Invoke-WebRequest -Uri "$Mirror/$($c.Zip)" -OutFile $zipPath -UseBasicParsing
        $exDir = "$tmp\ex_$([guid]::NewGuid().ToString('N'))"
        Expand-Archive -Path $zipPath -DestinationPath $exDir -Force
        # 找到解压后唯一的顶层目录
        $top = Get-ChildItem $exDir | Select-Object -First 1
        $src = if ($c.RenameFrom) { Join-Path $exDir $c.RenameFrom } else { $top.FullName }
        New-Item -ItemType Directory -Force -Path (Split-Path $c.Dest -Parent) | Out-Null
        Move-Item $src $c.Dest -Force
        Remove-Item -Recurse -Force $exDir -ErrorAction SilentlyContinue
        Write-Host "  已安装: $($c.Dest)"
    } else {
        Write-Host "  已存在: $($c.Dest)"
    }
}

# ============================================================
# 3. 在 SDK/ndk 下建 junction，让 AGP 使用本机已有 NDK（不下载）
# ============================================================
Write-Host "`n[3/4] 关联本机 NDK ..." -ForegroundColor Yellow
if (-not (Test-Path "$SdkRoot\ndk\$NdkVersion")) {
    New-Item -ItemType Directory -Force -Path "$SdkRoot\ndk" | Out-Null
    cmd /c mklink /J "$SdkRoot\ndk\$NdkVersion" "$NdkRoot" | Out-Null
    Write-Host "  已创建 junction: $SdkRoot\ndk\$NdkVersion -> $NdkRoot"
} else {
    Write-Host "  NDK junction 已存在"
}

# ============================================================
# 4. 生成 local.properties
# ============================================================
Write-Host "`n[4/4] 生成 local.properties ..." -ForegroundColor Yellow
$lp = @"
# Android SDK 位置（腾讯云镜像手动下载）
sdk.dir=$($SdkRoot.Replace('\','\\'))

# 使用本机已有 NDK，避免 Android Studio 重新下载
ndk.dir=$($NdkRoot.Replace('\','\\'))

# 使用 PATH 中的 CMake，避免 Android Studio 自行下载
cmake.dir=$($CmakeRoot.Replace('\','\\'))
"@
$lpPath = "$ProjectRoot\local.properties"
$lp | Set-Content -Path $lpPath -Encoding UTF8
Write-Host "  已写入: $lpPath"

Write-Host "`n==== 配置完成 ====" -ForegroundColor Green
Write-Host "提示：在 Android Studio 中打开 $ProjectRoot，"
Write-Host "  若提示下载 NDK/CMake，可在 File -> Project Structure -> SDK Location 确认指向上述路径。"
Write-Host "  依赖仓库镜像（阿里云）已在 settings.gradle 中配置。"

# 清理临时目录
Remove-Item -Recurse -Force $tmp -ErrorAction SilentlyContinue