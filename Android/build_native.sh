#! /bin/bash

# Get our location.
OURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

DEBUG=1

mkdir -p build && cd build
rm -f CMakeCache.txt
cmake ../app/src/main/cpp \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$ANDROID_HOME/ndk-bundle/build/cmake/android.toolchain.cmake \
    -DANDROID_PLATFORM=android-24 \
    -DANDROID_ABI=$abi \
    -DANDROID_ARM_MODE=arm \
    -DANDROID_ARM_NEON=TRUE \
    -DANDROID_STL=c++_shared \
    -DCMAKE_BUILD_TYPE=${DEBUG+Debug}${DEBUG-Release}
cmake --build .
