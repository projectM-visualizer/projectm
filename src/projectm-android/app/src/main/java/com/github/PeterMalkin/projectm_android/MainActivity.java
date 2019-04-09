package com.github.PeterMalkin.projectm_android;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import java.io.File;


public class MainActivity extends AppCompatActivity {
    private GLSurfaceView glSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        String assetPath = new File(getCacheDir(), "projectM").toString();
        setContentView(R.layout.activity_main);
        glSurfaceView = new GLSurfaceView(this);
        glSurfaceView.setEGLContextClientVersion(2);
        glSurfaceView.setRenderer(new RendererWrapper(assetPath));
        setContentView(glSurfaceView);
    }

    @Override
    protected void onPause() {
        Log.d("MainActivty", "OnPause");
        super.onPause();
        glSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        Log.d("MainActivty", "OnResume");
        super.onResume();
        glSurfaceView.onResume();
    }

}
