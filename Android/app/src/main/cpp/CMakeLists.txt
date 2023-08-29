#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)


project("artoolkitx_calib_camera"
        VERSION 1.0.0
        LANGUAGES CXX C
)

set(ARTOOLKITX_CAMERA_CALIBRATION_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../../../../..")
set(ARTOOLKITX_HOME "${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/depends/android/artoolkitx")
list(APPEND CMAKE_MODULE_PATH "${ARTOOLKITX_HOME}/Source/cmake")

# armeabi-v7a requires cpufeatures library?
# include(AndroidNdkModules)
# android_ndk_import_module_cpufeatures()

add_library(main SHARED)

# Enable C++11 for GCC/Clang compilers.
set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${ARTOOLKITX_CAMERA_CALIBRATION_HOME})

set(CMAKE_LIBRARY_ARCHITECTURE "${ANDROID_ABI}")
list(APPEND CMAKE_FIND_ROOT_PATH "${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/depends/android")
list(APPEND CMAKE_FIND_ROOT_PATH "${ARTOOLKITX_HOME}/Source/depends/android")

# SDL
find_path(
    SDL3_INCLUDE_DIR
    NAMES SDL3/SDL.h
    REQUIRED
)
find_library(SDL3_LIBRARY
    NAMES SDL3
    PATHS lib/${ANDROID_ABI}
    REQUIRED
)
message("-- Found SDL3: ${SDL3_INCLUDE_DIR} ${SDL3_LIBRARY}")
target_include_directories(main PRIVATE ${SDL3_INCLUDE_DIR})
add_library(SDL3 SHARED IMPORTED)
set_property(TARGET SDL3 PROPERTY
             IMPORTED_LOCATION "${SDL3_LIBRARY}")
target_link_libraries(main SDL3)

# OpenGL ES
find_package(OpenGLES2 REQUIRED)
if(OPENGLES2_FOUND)
    target_include_directories(main PRIVATE ${OPENGLES2_INCLUDE_DIR})
    target_link_libraries(main ${OPENGLES2_LIBRARIES})
    add_definitions("-DHAVE_GLES2=1")
endif()

find_package(JPEG REQUIRED)
target_include_directories(main PRIVATE ${JPEG_INCLUDE_DIR})
target_link_libraries(main ${JPEG_LIBRARIES})

# OpenCV
set(OpenCV_PATH "${ARTOOLKITX_HOME}/Source/depends/android/lib/${ANDROID_ABI}")
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIR})
    target_link_libraries(main ${OpenCV_LIBS})
endif()

# CURL
find_package(CURL REQUIRED)
message(${CURL_LIBRARIES})
target_include_directories(main PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(main ${CURL_LIBRARIES} z)

#find_package(OpenSSL REQUIRED)
#target_include_directories(main PRIVATE ${OPENSSL_INCLUDE_DIRS})
#target_link_libraries(main ${OPENSSL_LIBRARIES})

#find_package(PkgConfig)
#pkg_check_modules(LIBCONFIG REQUIRED libconfig)
#include_directories(${LIBCONFIG_INCLUDE_DIRS})

# ARX
add_definitions("-DARX_TARGET_PLATFORM_ANDROID=1")
target_include_directories(main PRIVATE ${ARTOOLKITX_HOME}/SDK/include)
add_library(ARX SHARED IMPORTED)
set_property(TARGET ARX PROPERTY
             IMPORTED_LOCATION "${ARTOOLKITX_HOME}/SDK/lib/${ANDROID_ABI}/libARX.so")
target_link_libraries(main ARX)

set(SOURCE
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/version.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/calib_camera.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/calib_camera.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/calib_camera_jni.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Calibration.hpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Calibration.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/calc.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/calc.hpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/fileUploader.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/fileUploader.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/flow.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/flow.hpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/loc_strings.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/loc_strings.hpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/prefs.hpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/prefsNull.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/prefsAndroid.cpp
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/Eden.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenError.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenGLFont.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenGLFont.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenMath.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenMessage.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenMessage.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenSurfaces.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenSurfaces.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenTime.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenTime.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenUtil.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenUtil.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenUIInput.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/EdenUIInput.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/glStateCache.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/glStateCache.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/readtex.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/readtex.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_8x13.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_9x15.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_bitmap.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_bwidth.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_hel10.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_hel12.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_hel18.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_mroman.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_roman.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_stroke.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_swidth.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_tr10.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glut_tr24.c
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glutbitmap.h
    ${ARTOOLKITX_CAMERA_CALIBRATION_HOME}/Eden/gluttext/glutstroke.h
)
target_sources(main PRIVATE ${SOURCE})

target_link_libraries(main
    android
    log
)

get_directory_property(ARXCC_DEFINES DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)
foreach(d ${ARXCC_DEFINES})
    message(STATUS "Defined: " ${d})
endforeach()

