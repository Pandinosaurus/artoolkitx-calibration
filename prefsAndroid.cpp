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
#  define MODE_PRIVATE 0

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
        if (asprintf(&prefsFileName, "%s_preferences", packageNameC) != -1) {
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

// result must be free()d.
static char *getStringPref(const char *key, const char *defaultValue)
{
    char *ret = NULL;
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joActivity = (jobject)SDL_AndroidGetActivity();
    jclass jcActivity = env->GetObjectClass(joActivity);
    jmethodID jmGetSharedPreferences = env->GetMethodID(jcActivity, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;"); // SharedPreferences Context.getSharedPreferences(String name, int mode);
    jobject joSharedPreferences = env->CallObjectMethod(joActivity, jmGetSharedPreferences, env->NewStringUTF(prefsFileName), MODE_PRIVATE);
    jclass jcSharedPreferences = env->FindClass("android/content/SharedPreferences");
    jmethodID jmGetString = env->GetMethodID(jcSharedPreferences, "getString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"); // String SharedPreferences.getString (String key, String defValue);
    jstring joString = (jstring)env->CallObjectMethod(joSharedPreferences, jmGetString, env->NewStringUTF(key), env->NewStringUTF(defaultValue));
    ret = newCStringFromJString(env, joString);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(joString);
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    env->DeleteLocalRef(jcActivity);
    env->DeleteLocalRef(joActivity);
    return ret;
}

static bool getBoolPref(const char *key, const bool defaultValue)
{
    bool ret = defaultValue;
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joActivity = (jobject)SDL_AndroidGetActivity();
    jclass jcActivity = env->GetObjectClass(joActivity);
    jmethodID jmGetSharedPreferences = env->GetMethodID(jcActivity, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;"); // SharedPreferences Context.getSharedPreferences(String name, int mode);
    jobject joSharedPreferences = env->CallObjectMethod(joActivity, jmGetSharedPreferences, env->NewStringUTF(prefsFileName), MODE_PRIVATE);
    jclass jcSharedPreferences = env->FindClass("android/content/SharedPreferences");
    jmethodID jmGetBoolean = env->GetMethodID(jcSharedPreferences, "getBoolean", "(Ljava/lang/String;Z)Z"); // boolean SharedPreferences.getBoolean (String key, boolean defValue);
    ret = (bool)env->CallBooleanMethod(joSharedPreferences, jmGetBoolean, env->NewStringUTF(key), (jboolean)defaultValue);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    env->DeleteLocalRef(jcActivity);
    env->DeleteLocalRef(joActivity);
    return ret;
}

static int getIntPref(const char *key, const int defaultValue)
{
    int ret = defaultValue;
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joActivity = (jobject)SDL_AndroidGetActivity();
    jclass jcActivity = env->GetObjectClass(joActivity);
    jmethodID jmGetSharedPreferences = env->GetMethodID(jcActivity, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;"); // SharedPreferences Context.getSharedPreferences(String name, int mode);
    jobject joSharedPreferences = env->CallObjectMethod(joActivity, jmGetSharedPreferences, env->NewStringUTF(prefsFileName), MODE_PRIVATE);
    jclass jcSharedPreferences = env->FindClass("android/content/SharedPreferences");
    jmethodID jmGetInt = env->GetMethodID(jcSharedPreferences, "getInt", "(Ljava/lang/String;I)I"); // int SharedPreferences.getInt (String key, int defValue);
    ret = (int)env->CallIntMethod(joSharedPreferences, jmGetInt, env->NewStringUTF(key), (jint)defaultValue);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    env->DeleteLocalRef(jcActivity);
    env->DeleteLocalRef(joActivity);
    return ret;
}

static float getFloatPref(const char *key, const float defaultValue)
{
    float ret = defaultValue;
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject joActivity = (jobject)SDL_AndroidGetActivity();
    jclass jcActivity = env->GetObjectClass(joActivity);
    jmethodID jmGetSharedPreferences = env->GetMethodID(jcActivity, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;"); // SharedPreferences Context.getSharedPreferences(String name, int mode);
    jobject joSharedPreferences = env->CallObjectMethod(joActivity, jmGetSharedPreferences, env->NewStringUTF(prefsFileName), MODE_PRIVATE);
    jclass jcSharedPreferences = env->FindClass("android/content/SharedPreferences");
    jmethodID jmGetFloat = env->GetMethodID(jcSharedPreferences, "getFloat", "(Ljava/lang/String;Z)Z"); // float SharedPreferences.getFloat (String key, float defValue);
    ret = (float)env->CallFloatMethod(joSharedPreferences, jmGetFloat, env->NewStringUTF(key), (jfloat)defaultValue);
    // Since we might be called from main() which can be very long-lived in SDL, we should clean up the local references.
    env->DeleteLocalRef(jcSharedPreferences);
    env->DeleteLocalRef(joSharedPreferences);
    env->DeleteLocalRef(jcActivity);
    env->DeleteLocalRef(joActivity);
    return ret;
}

char *getPreferenceCameraOpenToken(void *preferences)
{
    return getStringPref(k_pref_camera, "");
}

char *getPreferenceCameraResolutionToken(void *preferences)
{
    char *ret = NULL;
    
    if (!ret) {
        ret = strdup("-prefer=any");
    }
    return ret;
}

bool getPreferenceCalibrationSave(void *preferences)
{
    return false;
}

char *getPreferenceCalibrationServerUploadURL(void *preferences)
{
    return NULL;
}

char *getPreferenceCalibrationServerAuthenticationToken(void *preferences)
{
    return NULL;
}

Calibration::CalibrationPatternType getPreferencesCalibrationPatternType(void *preferences)
{
    return CALIBRATION_PATTERN_TYPE_DEFAULT;
}

cv::Size getPreferencesCalibrationPatternSize(void *preferences)
{
    return Calibration::CalibrationPatternSizes[CALIBRATION_PATTERN_TYPE_DEFAULT];
}

float getPreferencesCalibrationPatternSpacing(void *preferences)
{
    return Calibration::CalibrationPatternSpacings[CALIBRATION_PATTERN_TYPE_DEFAULT];
}
char *getPreferenceCalibSaveDir(void *preferences)
{
    return arUtilGetResourcesDirectoryPath(AR_UTIL_RESOURCES_DIRECTORY_BEHAVIOR_USE_USER_ROOT, NULL);
}
#endif
