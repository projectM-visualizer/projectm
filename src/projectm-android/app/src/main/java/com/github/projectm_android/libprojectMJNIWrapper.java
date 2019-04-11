package com.github.projectm_android;


public class libprojectMJNIWrapper {
    static {
        System.loadLibrary("jniwrapper");
    }

    public static native void onSurfaceCreated(int window_width, int window_height, String assetPath);

    public static native void onSurfaceChanged(int width, int height);

    public static native void onDrawFrame();

    public static native void addPCM(short pcm_data[], short nsamples);

    public static native void nextPreset();
}