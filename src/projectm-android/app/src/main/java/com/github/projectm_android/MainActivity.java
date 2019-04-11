package com.github.projectm_android;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.WindowManager;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    private AudioThread audioThread;

    public class projectMGLView extends GLSurfaceView {
        private RendererWrapper mRenderer;

        public projectMGLView(Context context, String assetPath) {
            super(context);
            this.setEGLContextClientVersion(2);
            mRenderer = new RendererWrapper(assetPath);
            this.setRenderer(mRenderer);
        }

        @Override
        public boolean onTouchEvent(MotionEvent e) {
            mRenderer.NextPreset();
            return true;
        }

        @Override
        public void onPause() {
            super.onPause();
        }

        @Override
        public void onResume() {
            super.onResume();
        }

    }

    private projectMGLView glSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        String assetPath = new File(getCacheDir(), "projectM").toString();
        glSurfaceView = new projectMGLView(this, assetPath);
        setContentView(glSurfaceView);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onPause() {
        super.onPause();
        glSurfaceView.onPause();
        audioThread.stop_recording();
        try {
            audioThread.join();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        glSurfaceView.onResume();
        audioThread = new AudioThread();
        audioThread.start();
    }
}
