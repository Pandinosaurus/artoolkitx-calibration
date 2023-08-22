package org.artoolkitx.utilities.cameracalibration;

import android.graphics.Color;
import android.os.Bundle;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.preference.EditTextPreference;
import androidx.preference.ListPreference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.SwitchPreferenceCompat;

import org.artoolkitx.arx.arxj.ARX_jni;
import org.libsdl.app.R;

import java.util.ArrayList;
import java.util.List;

public class CameraCalibrationSettingsFragment extends PreferenceFragmentCompat {
    @Override
    @SuppressWarnings("ConstantConditions")
    public void onCreatePreferences(Bundle savedInstanceState, @Nullable String rootKey) {
        setPreferencesFromResource(R.xml.cameracalibrationsettings, rootKey);

        final ListPreference camera = (ListPreference)findPreference("camera");
        setCameraInputList(camera);

        // Toggle fields on or off.
        final SwitchPreferenceCompat upload_canonical = (SwitchPreferenceCompat)findPreference("upload_canonical");
        final SwitchPreferenceCompat upload_user = (SwitchPreferenceCompat)findPreference("upload_user");
        final EditTextPreference upload_user_csuu = (EditTextPreference)findPreference("upload_user_csuu");
        final EditTextPreference upload_user_csat= (EditTextPreference)findPreference("upload_user_csat");
        if (Config.ARTOOLKITX_CSAT.isEmpty() || Config.ARTOOLKITX_CSUU.isEmpty()) {
            upload_canonical.setVisible(false);
            upload_user.setVisible(true);
            upload_user_csuu.setVisible(true);
            upload_user_csat.setVisible(true);
        } else {
            upload_canonical.setVisible(true);
            upload_user.setVisible(false);
            upload_user_csuu.setVisible(false);
            upload_user_csat.setVisible(false);
        }

        // Set numeric fields.
        EditTextPreference camera_width = findPreference("camera_width");
        EditTextPreference camera_height = findPreference("camera_height");
        EditTextPreference pattern_width = findPreference("pattern_width");
        EditTextPreference pattern_height = findPreference("pattern_height");
        EditTextPreference pattern_spacing = findPreference("pattern_spacing");
        EditTextPreference.OnBindEditTextListener numbersOnly = new EditTextPreference.OnBindEditTextListener() {
            @Override
            public void onBindEditText(@NonNull EditText editText) {
                editText.setInputType(InputType.TYPE_CLASS_NUMBER);
            }
        };
        EditTextPreference.OnBindEditTextListener numbersDecimalOnly = new EditTextPreference.OnBindEditTextListener() {
            @Override
            public void onBindEditText(@NonNull EditText editText) {
                editText.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL);
            }
        };
        camera_width.setOnBindEditTextListener(numbersOnly);
        camera_height.setOnBindEditTextListener(numbersOnly);
        pattern_width.setOnBindEditTextListener(numbersOnly);
        pattern_height.setOnBindEditTextListener(numbersOnly);
        pattern_spacing.setOnBindEditTextListener(numbersDecimalOnly);
    }

    @NonNull
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = super.onCreateView(inflater, container, savedInstanceState);
        view.setBackgroundColor(Color.WHITE);
        return view;
    }

    protected static void setCameraInputList(ListPreference lp) {
        List<String> entriesList = new ArrayList<>();
        List<String> entryValuesList = new ArrayList<>();
        int cameraCount = ARX_jni.arwCreateVideoSourceInfoList("-module=Android");
        for (int i = 0; i < cameraCount; i++) {
            String[] name = new String[1];
            String[] model = new String[1];
            String[] UID = new String[1];
            int[] flags = new int[1];
            String[] openToken = new String[1];
            if (ARX_jni.arwGetVideoSourceInfoListEntry(i, name, model, UID, flags, openToken)) {
                entriesList.add(name[0]);
                entryValuesList.add(openToken[0]);
            }
        }
        lp.setEntries(entriesList.toArray(new String[0]));
        lp.setEntryValues(entryValuesList.toArray(new String[0]));
    }
}
