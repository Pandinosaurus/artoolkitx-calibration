/*
 *  loc_strings.cpp
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


#include "loc_strings.hpp"

lang_t loc = lang_en;

const std::unordered_map<const loc_string, const char*> loc_strings[] =
{
    // en
    {
        {loc_string::Intro, "Welcome to artoolkitX Camera Calibrator\n(c)2023 artoolkitX Contributors.\n\nPress 'space' to begin a calibration run.\n\nPress 'p' for settings and help."},
        {loc_string::VideoOpenError, "Welcome to artoolkitX Camera Calibrator\n(c)2023 artoolkitX Contributors.\n\nUnable to open video source.\n\nPress 'p' for settings and help."},
        {loc_string::Reintro, "Press 'space' to begin a calibration run.\n\nPress 'p' for settings and help."},
        {loc_string::IntroTouchscreen, "Welcome to artoolkitX Camera Calibrator\n(c)2023 artoolkitX Contributors.\n\nTap '+' to begin a calibration run.\n\nTap the menu button for settings and help."},
        {loc_string::VideoOpenErrorTouchscreen, "Welcome to artoolkitX Camera Calibrator\n(c)2023 artoolkitX Contributors.\n\nUnable to open video source.\n\nTap the menu button for settings and help."},
        {loc_string::ReintroTouchscreen, "Tap '+' to begin a calibration run.\n\nTap the menu button for settings and help."},
        {loc_string::CalibCapturing, "Capturing image %d/%d"},
        {loc_string::CalibCanceled, "Calibration canceled"},
        {loc_string::CalibCalculating, "Calculating camera parameters..."},
        {loc_string::CalibResults,  "Camera parameters calculated (error min=%.3f, avg=%.3f, max=%.3f)"}
    }
};
    

