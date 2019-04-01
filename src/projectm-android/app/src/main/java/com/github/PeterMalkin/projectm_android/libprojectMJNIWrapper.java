package com.github.PeterMalkin.projectm_android;

import static java.lang.System.loadLibrary;

public class libprojectMJNIWrapper {
    static {
        System.loadLibrary("c++_shared");
        loadLibrary("projectM");
    }

    public static native void onSurfaceCreated(int window_width, int window_height);

    public static native void onSurfaceChanged(int width, int height);

    public static native void onDrawFrame();

    public static native void addPCM(short pcm_data[], short nsamples);
}