/*
 *  prefsAndroid.cpp
 *  artoolkitX
 *
 *  This file is part of artoolkitX.
 *
 *  artoolkitX is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  artoolkitX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with artoolkitX.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2023 Philip Lamb.
 *
 *  Author(s): Philip Lamb
 *
 */


#include <stdio.h>
#include <ARX/AR/config.h>
#include "prefs.hpp"

#if ARX_TARGET_PLATFORM_ANDROID
#  include <jni.h>
#  include <SDL3/SDL.h>
#  include <stdlib.h>
#  define MODE_PRIVATE 0

// These constants must match the values defined in camera_calibration_settings.xml.
static const char *k_pref_camera = "camera"; // String
static const char *k_pref_camera_position = "camera_position"; // String
static const char *k_pref_camera_size_strategy = "camera_size_strategy"; // String
static const char *k_pref_camera_width = "camera_width" ;// int
static const char *k_pref_camera_height = "camera_height"; // int
static const char *k_pref_print_paper_size = "print_paper_size"; // String
static const char *k_pref_save = "save"; // boolean
static const char *k_pref_upload_canonical = "upload_canonical"; // boolean
static const char *k_pref_upload_user = "upload_user"; // boolean
static const char *k_pref_upload_user_csuu = "upload_user_csuu"; // String
static const char *k_pref_upload_user_csat = "upload_user_csat"; // String
static const char *k_pref_pattern = "pattern"; // String
static const char *k_pref_pattern_width = "pattern_width"; // int
static const char *k_pref_pattern_height = "pattern_height"; // int
static const char *k_pref_pattern_spacing = "pattern_spacing"; // float

// These constants must match the values defined in strings.xml.
static const char *k_pattern_chessboard = "chessboard";
static const char *k_pattern_circles = "circles";
static const char *k_pattern_asymmetriccircles = "asymmetriccircles";

// Fallback defaults. While these shouldn't be needed, they should match the
// values defined in camera_calibration_settings.xml and strings.xml to avoid problems.
static const char *camera_default = ""; // Any valid open() token.
static const int camera_width_default = 640;
static const int camera_height_default = 480;
static const char *size_strategy_default = "closestpixelcount";
static const bool save_default = false;
static const bool upload_canonical_default = true;
static const bool upload_user_default = false;
static const char *upload_user_csuu_default = "";
static const char *upload_user_csat_default = "";

static char *prefsFileName = NULL;

void *initPreferences(void)
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joActivity = (jobject)SDL_AndroidGetActivity();
    jclass jcActivity = env->GetObjectClass(joActivity);
    jmethodID jmGetPackageName = env->GetMethodID(jcActivity, "getPackageName", "()Ljava/lang/String;");
    jstring packageName = (jstring)env->CallObjectMethod(joActivity, jmGetPackageName);
    const char *packageNameC = env->GetStringUTFChars(packageName, NULL);
    if (packageNameC) {
        free(prefsFileName);
        prefsFileName = NULL;
        if (asprintf(&prefsFileName, "%s_preferences", packageNameC) < 0) {
            ARLOGe("initPreferences error\n");
        }
    }
    env->ReleaseStringUTFChars(packageName, packageNameC);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(packageName);
    env->DeleteLocalRef(jcActivity);
    env->DeleteLocalRef(joActivity);   
    return (NULL);
}

void preferencesFinal(void **preferences_p)
{
    free(prefsFileName);
    prefsFileName = NULL;
}

void showPreferences(void *preferences)
{
}

static char *newCStringFromJString(JNIEnv* env, jstring js)
{
    const char *s = env->GetStringUTFChars(js, NULL);
    if (!s) return NULL;
    char *sc = strdup(s);
    env->ReleaseStringUTFChars(js, s);
    return sc;
}

static void getSharedPreferences(JNIEnv* env, jobject *jo, jclass *jc)
{
    jobject joActivity = (jobject)SDL_AndroidGetActivity();
    jclass jcActivity = env->GetObjectClass(joActivity);
    jmethodID jmGetSharedPreferences = env->GetMethodID(jcActivity, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;"); // SharedPreferences Context.getSharedPreferences(String name, int mode);
    *jo = env->CallObjectMethod(joActivity, jmGetSharedPreferences, env->NewStringUTF(prefsFileName), MODE_PRIVATE);
    *jc = env->FindClass("android/content/SharedPreferences");
    env->DeleteLocalRef(jcActivity);
    env->DeleteLocalRef(joActivity);
}

// result must be free()d.
static char *getStringPref(const char *key, const char *defaultValue)
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joSharedPreferences;
    jclass jcSharedPreferences;
    getSharedPreferences(env, &joSharedPreferences, &jcSharedPreferences);
    jmethodID jmGetString = env->GetMethodID(jcSharedPreferences, "getString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"); // String SharedPreferences.getString (String key, String defValue);
    jstring joString = (jstring)env->CallObjectMethod(joSharedPreferences, jmGetString, env->NewStringUTF(key), env->NewStringUTF(defaultValue));
    char *ret = newCStringFromJString(env, joString);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(joString);
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    return ret;
}

static bool getBoolPref(const char *key, const bool defaultValue)
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joSharedPreferences;
    jclass jcSharedPreferences;
    getSharedPreferences(env, &joSharedPreferences, &jcSharedPreferences);
    jmethodID jmGetBoolean = env->GetMethodID(jcSharedPreferences, "getBoolean", "(Ljava/lang/String;Z)Z"); // boolean SharedPreferences.getBoolean (String key, boolean defValue);
    bool ret = (bool)env->CallBooleanMethod(joSharedPreferences, jmGetBoolean, env->NewStringUTF(key), (jboolean)defaultValue);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    return ret;
}

static int getIntPref(const char *key, const int defaultValue)
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joSharedPreferences;
    jclass jcSharedPreferences;
    getSharedPreferences(env, &joSharedPreferences, &jcSharedPreferences);
    jmethodID jmGetInt = env->GetMethodID(jcSharedPreferences, "getInt", "(Ljava/lang/String;I)I"); // int SharedPreferences.getInt (String key, int defValue);
    int ret = (int)env->CallIntMethod(joSharedPreferences, jmGetInt, env->NewStringUTF(key), (jint)defaultValue);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    return ret;
}

static float getFloatPref(const char *key, const float defaultValue)
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joSharedPreferences;
    jclass jcSharedPreferences;
    getSharedPreferences(env, &joSharedPreferences, &jcSharedPreferences);
    jmethodID jmGetFloat = env->GetMethodID(jcSharedPreferences, "getFloat", "(Ljava/lang/String;F)F"); // float SharedPreferences.getFloat (String key, float defValue);
    float ret = (float)env->CallFloatMethod(joSharedPreferences, jmGetFloat, env->NewStringUTF(key), (jfloat)defaultValue);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    return ret;
}

char *getPreferenceCameraOpenToken(void *preferences)
{
    return getStringPref(k_pref_camera, camera_default);
}

char *getPreferenceCameraResolutionToken(void *preferences)
{
    char *ret = NULL;
    int w = camera_width_default;
    int h = camera_height_default;
    char *ws = getStringPref(k_pref_camera_width, "");
    char *hs = getStringPref(k_pref_camera_height, "");
    if (ws) {
        w = atoi(ws);
        free(ws);
    }
    if (hs) {
        h = atoi(hs);
        free(hs);
    }
    char *size_strategy = getStringPref(k_pref_camera_size_strategy, size_strategy_default);
    if (!size_strategy || !*size_strategy) {
        size_strategy = strdup(size_strategy_default);
    }
    asprintf(&ret, "-prefer=%s -width=%d -height=%d", size_strategy, w, h);
    free(size_strategy);
    return ret;
}

bool getPreferenceCalibrationSave(void *preferences)
{
#if defined(ARTOOLKITX_CSUU) && defined(ARTOOLKITX_CSAT)
    bool uploadOn = getBoolPref(k_pref_upload_canonical, upload_canonical_default);
#else
    bool uploadOn = getBoolPref(k_pref_upload_user, upload_user_default);
#endif
    return (uploadOn ? getBoolPref(k_pref_save, save_default) : true);
}

char *getPreferenceCalibrationServerUploadURL(void *preferences)
{

#if defined(ARTOOLKITX_CSUU) && defined(ARTOOLKITX_CSAT)
    if (!getBoolPref(k_pref_upload_canonical, upload_canonical_default)) return (NULL);
    return (strdup(ARTOOLKITX_CSUU));
#else
    if (!getBoolPref(k_pref_upload_user, upload_user_default)) return (NULL);
    char *ret = getStringPref(k_pref_upload_user_csuu, upload_user_csuu_default);
    if (ret && strlen(ret) == 0) {
        free(ret);
        ret = NULL;
    }
    return ret;
#endif
}

char *getPreferenceCalibrationServerAuthenticationToken(void *preferences)
{
#if defined(ARTOOLKITX_CSUU) && defined(ARTOOLKITX_CSAT)
    if (!getBoolPref(k_pref_upload_canonical, upload_canonical_default)) return (NULL);
    return (strdup(ARTOOLKITX_CSAT));
#else
    if (!getBoolPref(k_pref_upload_user, upload_user_default)) return (NULL);
    char *ret = getStringPref(k_pref_upload_user_csat, upload_user_csat_default);
    if (ret && strlen(ret) == 0) {
        free(ret);
        ret = NULL;
    }
    return ret;
#endif
}

Calibration::CalibrationPatternType getPreferencesCalibrationPatternType(void *preferences)
{
    Calibration::CalibrationPatternType patternType = CALIBRATION_PATTERN_TYPE_DEFAULT;
    char *pattern = getStringPref(k_pref_pattern, "");
    if (pattern) {
        if (strcmp(pattern, k_pattern_chessboard) == 0) patternType = Calibration::CalibrationPatternType::CHESSBOARD;
        else if (strcmp(pattern, k_pattern_circles) == 0) patternType = Calibration::CalibrationPatternType::CIRCLES_GRID;
        else if (strcmp(pattern, k_pattern_asymmetriccircles) == 0) patternType = Calibration::CalibrationPatternType::ASYMMETRIC_CIRCLES_GRID;
        free(pattern);
    }
    return patternType;
}

cv::Size getPreferencesCalibrationPatternSize(void *preferences)
{
    int w = 0;
    int h = 0;
    char *ws = getStringPref(k_pref_pattern_width, "");
    char *hs = getStringPref(k_pref_pattern_height, "");
    if (ws) {
        w = atoi(ws);
        free(ws);
    }
    if (hs) {
        h = atoi(hs);
        free(hs);
    }
    if (w > 0 && h > 0) return cv::Size(w, h);
    return Calibration::CalibrationPatternSizes[getPreferencesCalibrationPatternType(preferences)];
}

float getPreferencesCalibrationPatternSpacing(void *preferences)
{
    float f = 0.0f;
    char *fs = getStringPref(k_pref_pattern_spacing, "");
    if (fs) {
        f = atof(fs);
        free(fs);
    }
    if (f > 0.0f) return f;
    return Calibration::CalibrationPatternSpacings[getPreferencesCalibrationPatternType(preferences)];
}

char *getPreferenceCalibSaveDir(void *preferences)
{
    return arUtilGetResourcesDirectoryPath(AR_UTIL_RESOURCES_DIRECTORY_BEHAVIOR_USE_APP_EXTERNAL_DATA_DIR, NULL);
}
#endif
