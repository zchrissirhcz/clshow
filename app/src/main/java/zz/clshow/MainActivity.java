package zz.clshow;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity
{
    private ClShow clshow = new ClShow();

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setTitle("OpenCL Info");

        TextView textviewInfo = new TextView(this);
        textviewInfo.setTextSize(14);
        textviewInfo.setPadding(16, 16, 16, 16);
        textviewInfo.setTextIsSelectable(true);

        // 在后台线程调用 native（首次加载/枚举可能耗时），完成后回主线程刷新
        final TextView tv = textviewInfo;
        tv.setText("Loading OpenCL info...");

        ScrollView scroll = new ScrollView(this);
        scroll.addView(tv);
        setContentView(scroll);

        new Thread(new Runnable() {
            public void run() {
                final String info = buildInfo();
                tv.post(new Runnable() {
                    public void run() {
                        tv.setText(info);
                    }
                });
            }
        }).start();
    }

    private String buildInfo()
    {
        StringBuilder sb = new StringBuilder();
        sb.append("Device: ").append(Build.MODEL).append("\n");
        sb.append("Android: ").append(Build.VERSION.RELEASE).append("\n");
        sb.append("ABI: ").append(Build.SUPPORTED_ABIS != null && Build.SUPPORTED_ABIS.length > 0 ? Build.SUPPORTED_ABIS[0] : "?").append("\n");
        sb.append("Loader: ").append(clshow.LoaderMode()).append("\n");
        sb.append("----------------------------------------\n");
        sb.append(clshow.GetOpenCLInfo());
        return sb.toString();
    }
}