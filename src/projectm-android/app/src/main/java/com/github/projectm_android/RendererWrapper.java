package com.github.projectm_android;

import android.content.res.Resources;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RendererWrapper implements Renderer {
    private String mAssetPath;

    private Boolean mNextPreset = false;

    public RendererWrapper(String assetPath) {
        mAssetPath = assetPath;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d("projectM","RenderWrapper onSurfaceCreated");
        int width = Resources.getSystem().getDisplayMetrics().widthPixels;
        int height = Resources.getSystem().getDisplayMetrics().heightPixels;
        libprojectMJNIWrapper.onSurfaceCreated(width, height, mAssetPath);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.d("projectM","RenderWrapper onSurfaceChanged");
        libprojectMJNIWrapper.onSurfaceChanged(width, height);
    }

    public void NextPreset() {
        mNextPreset = true;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (mNextPreset) {
            mNextPreset = false;
            libprojectMJNIWrapper.nextPreset();
        }
        libprojectMJNIWrapper.onDrawFrame();
    }
}
