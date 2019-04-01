#include <jni.h>
#include <string>
#include "../../../../../libprojectM/projectM.hpp"
#include "../../../../../libprojectM/PCM.hpp"

projectM *instance = NULL;

extern "C" JNIEXPORT void JNICALL
Java_com_github_PeterMalkin_projectm_1android_libprojectMJNIWrapper_onSurfaceCreated(
        JNIEnv *env,
        jobject obj,
        jint window_width,
        jint window_height) {

    projectM::Settings settings;
    settings.windowHeight = window_height;
    settings.windowWidth = window_width;
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
