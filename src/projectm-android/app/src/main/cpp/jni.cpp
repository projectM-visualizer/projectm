#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include "libprojectM/projectM.hpp"
#include "libprojectM/PCM.hpp"

#define TAG "ProjectM"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

projectM *instance = NULL;

void next_preset(bool hard_cut) {
    if (!instance) {
        ALOGE("libprojectM not initialized");
        return;
    }
    srand((unsigned) time(NULL));
    int preset_list_size = instance->getPlaylistSize();
    if (preset_list_size <= 0) {
        ALOGE("Could not load any presets");
    }
    int preset_number = (int)(rand() % (preset_list_size-1));
    ALOGD("Switching to preset %d", preset_number);
    instance->selectPreset(preset_number, hard_cut);
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_projectm_1android_libprojectMJNIWrapper_onSurfaceCreated(
        JNIEnv *env,
        jobject obj,
        jint window_width,
        jint window_height,
        jstring jasset_path) {
    if (instance) {
        ALOGD("Destroy existing instance");
        delete instance;
    }
    const char* asset_path_chars = env->GetStringUTFChars(jasset_path, NULL);
    std::string asset_path(asset_path_chars);
    projectM::Settings settings;
    settings.windowHeight = window_height;
    settings.windowWidth = window_width;
    settings.presetURL = asset_path + "/presets";
    settings.smoothPresetDuration = 7;
    settings.presetDuration = 3;
    settings.shuffleEnabled = true;
    ALOGD("presetURL: %s", settings.presetURL.c_str());
    env->ReleaseStringUTFChars(jasset_path, asset_path_chars);
    ALOGD("Creating new instance");
    instance = new projectM(settings);
    srand(time(0));
    next_preset(true);
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_projectm_1android_libprojectMJNIWrapper_onSurfaceChanged(
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
Java_com_github_projectm_1android_libprojectMJNIWrapper_onDrawFrame(
        JNIEnv *env,
        jobject obj) {
    if (!instance) {
        return;
    }
    instance->renderFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_projectm_1android_libprojectMJNIWrapper_addPCM(
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

extern "C" JNIEXPORT void JNICALL
Java_com_github_projectm_1android_libprojectMJNIWrapper_nextPreset(
        JNIEnv *env,
        jobject obj) {
    next_preset(true);
}
