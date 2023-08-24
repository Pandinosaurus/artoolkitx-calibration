package org.artoolkitx.utilities.cameracalibration;

import android.content.Intent;
import android.os.Bundle;
import android.net.Uri;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import androidx.annotation.NonNull;

import org.libsdl.app.R;
import org.libsdl.app.SDLActivity;

public class CameraCalibrationActivity extends SDLActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        findViewById(R.id.btn_cancel).setOnClickListener(view -> {
            CameraCalibrationActivity.handleBackButton();
        });
        findViewById(R.id.btn_add).setOnClickListener(view -> {
            CameraCalibrationActivity.handleAddButton();
        });
    }

    public static native void handleBackButton();
    public static native void handleAddButton();

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                this.finish();
                return true;
            case R.id.menu_help:
                startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://github.com/artoolkitx/artoolkitx-calibration/wiki")));
                return true;
            case R.id.menu_print:
                return true;
            case R.id.menu_settings:
                startActivity(new Intent(this, CameraCalibrationSettingsActivity.class));
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main_menu, menu);
        return true;
    }

}
