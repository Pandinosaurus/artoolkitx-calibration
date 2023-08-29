/*
 *  loc_strings.hpp
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
 *  Copyright 2023 Philip Lamb
 *
 *  Author(s): Philip Lamb
 *
 */

#pragma once
#include <unordered_map>

enum lang_t {
    lang_en = 0
};

enum class loc_string {
    Intro,
    VideoOpenError,
    Reintro,
    IntroTouchscreen,
    VideoOpenErrorTouchscreen,
    ReintroTouchscreen,
    CalibCapturing,
    CalibCanceled,
    CalibCalculating,
    CalibResults,
};

extern lang_t loc;
extern const std::unordered_map<const loc_string, const char*> loc_strings[];

// Return a localized string n in the current language.
// Throws std::out_of_range exception if the string is not defined for the current language.
#define LOC_STRING(n) (loc_strings[loc].at(n))

#if (__cplusplus >= 201703L)
// Return a localized string n in the current language.
// If not defined for this language, falls back to the string in English.
// Throws std::out_of_range exception if the string is not defined for English.
#define LOC_STRING0(n) ((auto s = loc_strings[loc].find(n); s != loc_strings[loc].end()) ? s->second : loc_strings[0].at(n))
#endif
