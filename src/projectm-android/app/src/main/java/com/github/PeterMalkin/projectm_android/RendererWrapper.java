package com.github.PeterMalkin.projectm_android;

import android.content.res.Resources;
import android.opengl.GLSurfaceView.Renderer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RendererWrapper implements Renderer {
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        int width = Resources.getSystem().getDisplayMetrics().widthPixels;
        int height = Resources.getSystem().getDisplayMetrics().heightPixels;
        libprojectMJNIWrapper.onSurfaceCreated(width, height);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        libprojectMJNIWrapper.onSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        libprojectMJNIWrapper.onDrawFrame();
    }
}
