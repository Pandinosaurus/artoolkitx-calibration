package org.artoolkitx.utilities.cameracalibration;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import org.libsdl.app.R;

public class CameraCalibrationSettingsActivity extends AppCompatActivity
{
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.cameracalibrationsettingsactivity);

        Toolbar settingsToolbar = (Toolbar) findViewById(R.id.settings_toolbar);
        setSupportActionBar(settingsToolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowHomeEnabled(true);

        getSupportFragmentManager().beginTransaction()
            .replace(R.id.settings_content, new CameraCalibrationSettingsFragment())
            .commit();
    }
}