/*
 *  calib_camera_jni.cpp
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

//
// Presents a handful of entry points to allow the Java code to call into the
// native side.
// The Java side of this interface is in the class
// org.artoolkitx.utilities.cameracalibration.CameraCalibrationJNI.
//

#ifdef ANDROID

#include "flow.hpp"
#include "calib_camera.h"

#include <jni.h>

#define JNIFUNCTION_CC(sig) Java_org_artoolkitx_utilities_cameracalibration_CameraCalibrationJNI_##sig

extern "C" {

JNIEXPORT void JNICALL JNIFUNCTION_CC(handleBackButton(void))
{
    flowHandleEvent(EVENT_BACK_BUTTON);
}

JNIEXPORT void JNICALL JNIFUNCTION_CC(handleAddButton(void))
{
    flowHandleEvent(EVENT_TOUCH);
}

JNIEXPORT void JNICALL JNIFUNCTION_CC(sendPreferencesChangedEvent(void))
{
    SDL_Event event;
    SDL_zero(event);
    event.type = gSDLEventPreferencesChanged;
    event.user.code = (Sint32)0;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
}

}
#endif

