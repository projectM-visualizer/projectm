#include <android/log.h>
#include <jni.h>
#include <string>
#include "libprojectM/projectM.hpp"
#include "libprojectM/PCM.hpp"

#define TAG "ProjectM"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

projectM *instance = NULL;

extern "C" JNIEXPORT void JNICALL
Java_com_github_PeterMalkin_projectm_1android_libprojectMJNIWrapper_onSurfaceCreated(
        JNIEnv *env,
        jobject obj,
        jint window_width,
        jint window_height,
        jstring jasset_path) {
    const char* asset_path_chars = env->GetStringUTFChars(jasset_path, NULL);
    std::string asset_path(asset_path_chars);
    projectM::Settings settings;
    settings.windowHeight = window_height;
    settings.windowWidth = window_width;
    settings.presetURL = asset_path + "/presets";
    ALOGD("presetURL: %s", settings.presetURL.c_str());
    env->ReleaseStringUTFChars(jasset_path, asset_path_chars);
    instance = new projectM(settings);
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_PeterMalkin_projectm_1android_libprojectMJNIWrapper_onSurfaceChanged(
        JNIEnv *env,
        jobject obj,
        jint window_width,
        jint window_height) {
    if (!instance) {
        return;
    }

    instance->projectM_resetGL(window_width, window_height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_PeterMalkin_projectm_1android_libprojectMJNIWrapper_onDrawFrame(
        JNIEnv *env,
        jobject obj) {
    if (!instance) {
        return;
    }
    instance->renderFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_PeterMalkin_projectm_1android_libprojectMJNIWrapper_addPCM(
        JNIEnv *env,
        jobject obj,
        jshortArray pcm_data,
        jshort nsamples) {
    if (!instance) {
        return;
    }
    jshort *data = env->GetShortArrayElements(pcm_data, NULL);
    instance->pcm()->addPCM16Data(data, nsamples);
    env->ReleaseShortArrayElements(pcm_data, data, 0);

}
