package zz.clshow;

// ClShow —— OpenCL 信息探测的 native 封装。
public class ClShow
{
    // 返回所有 OpenCL 平台/设备信息的多行字符串。
    public native String GetOpenCLInfo();

    // 返回当前 OpenCL 加载方式：opencl_shim 或 icd_loader。
    public native String LoaderMode();

    static {
        System.loadLibrary("clshow");
    }
}