package com.github.projectm_android;

import android.app.Application;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class ProjectMApplication extends Application {
    private static final String TAG = MainActivity.class.getSimpleName();
    @Override
    public void onCreate() {
        super.onCreate();

        Log.d(TAG, "Extracting assets...");

        File cacheDir = new File(getCacheDir(), "projectM");
        if (cacheDir.exists()) {
            deleteFile(cacheDir);
        }

        AssetManager assetManager = getAssets();
        String[] dirNames = {"fonts", "presets"};

        try {
            for (String dirName : dirNames) {
                File dir = new File(cacheDir, dirName);
                dir.mkdirs();
                String assetDir = String.format("projectM%s%s", File.separator, dirName);
                for(String fileName : assetManager.list(assetDir)) {
                    String assetName = String.format("projectM/%s/%s", dirName, fileName);
                    writeStreamToFile(assetManager.open(assetName), new File(dir, fileName));
                }
            }
            Log.d(TAG, "Assets extracted");
        } catch (IOException e) {
            Log.e(TAG, "error extracting assets", e);
        }

    }

    private static void deleteFile(File entry) {
        if (entry.isDirectory()) {
            for (File sub : entry.listFiles()) {
                deleteFile(sub);
            }
        }
        entry.delete();
    }

    private static void writeStreamToFile(InputStream is, File file) throws IOException {
        OutputStream os = null;
        try {
            os = new FileOutputStream(file);
            int read = 0;
            byte[] buf = new byte[1024];
            while ((read = is.read(buf)) != -1) {
                os.write(buf, 0, read);
            }
        }
        finally {
            if (os != null) {
                os.close();
            }
        }
    }
}
