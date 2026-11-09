// clshow_jni.cpp —— Android app 的 JNI 层。
// 复用核心库 clshow::get_opencl_info()，返回给 Java 层显示。
#include <android/log.h>
#include <jni.h>

#include <string>

#include "clshow_core.h"

#define LOG_TAG "clshow"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_zz_clshow_ClShow_GetOpenCLInfo(JNIEnv* env, jobject)
{
    std::string info = clshow::get_opencl_info();
    LOGI("GetOpenCLInfo: %s", info.c_str());
    return env->NewStringUTF(info.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_zz_clshow_ClShow_LoaderMode(JNIEnv* env, jobject)
{
    std::string mode = clshow::loader_mode();
    return env->NewStringUTF(mode.c_str());
}