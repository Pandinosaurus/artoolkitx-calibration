#! /bin/bash

#
# Build artoolkitX Camera Calibration utility for desktop platforms.
#
# Copyright 2018, Philip Lamb.
# Copyright 2018, Realmax, Inc.
# Copyright 2016-2017, DAQRI LLC.
#
# Author(s): Philip Lamb, Thorsten Bux, John Wolf, Dan Bell.
#

# -e = exit on errors; -x = debug
set -e

# -x = debug
set -x

# Get our location.
OURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


VERSION=`sed -En -e 's/.*VERSION_STRING[[:space:]]+"([0-9]+\.[0-9]+(\.[0-9]+)*)".*/\1/p' ${OURDIR}/version.h`
# If the tiny version number is 0, drop it.
VERSION=`echo -n "${VERSION}" | sed -E -e 's/([0-9]+\.[0-9]+)\.0/\1/'`
# Process for version number parts.
VERSION_MAJOR=$(echo ${VERSION} | sed -E -e 's/^([0-9]+).*/\1/')
VERSION_MINOR=$(echo ${VERSION} | sed -E -e 's/^[0-9]+\.([0-9]+).*/\1/')
# VERSION_TINY and its preceding dot can be absent, so allow for that in our regexp and set to 0 in that case.
VERSION_TINY=$(echo ${VERSION} | sed -E -e 's/^[0-9]+\.[0-9]+\.*([0-9]+)*.*/\1/')
VERSION_TINY=${VERSION_TINY:-0}
# VERSION_BUILD can be overridden in the environment, but defaults to 0.
VERSION_BUILD=${VERSION_BUILD:-0}
# Convert version to an integer (e.g. for use in incremental build numbering, as on Android).
VERSION_INT=$(printf "%d%02d%02d%02d" ${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_TINY} ${VERSION_BUILD})

echo "Build version ${VERSION_INT}"

function usage {
    echo "Usage: $(basename $0) [--debug] (macos | ios | windows | linux | linux-raspbian) "
    exit 1
}

if [ $# -eq 0 ]; then
    usage
fi

# Parse parameters
while test $# -gt 0
do
    case "$1" in
        osx) BUILD_MACOS=1
            ;;
        macos) BUILD_MACOS=1
            ;;
        ios) BUILD_IOS=1
            ;;
        linux) BUILD_LINUX=1
            ;;
        linux-raspbian) BUILD_LINUX_RASPBIAN=1
            ;;
        windows) BUILD_WINDOWS=1
            ;;
        android) BUILD_ANDROID=1
            ;;
        --debug) DEBUG=
            ;;
        --*) echo "bad option $1"
            usage
            ;;
        *) echo "bad argument $1"
            usage
            ;;
    esac
    shift
done

# Set OS-dependent variables.
OS=`uname -s`
ARCH=`uname -m`
TAR='/usr/bin/tar'
if [ "$OS" = "Linux" ]
then
    CPUS=`/usr/bin/nproc`
    TAR='/bin/tar'
    # Identify Linux OS. Sets useful variables: ID, ID_LIKE, VERSION, NAME, PRETTY_NAME.
    source /etc/os-release
    # Windows Subsystem for Linux identifies itself as 'Linux'. Additional test required.
    if grep -qE "(Microsoft|WSL)" /proc/version &> /dev/null ; then
        OS='Windows'
    fi
elif [ "$OS" = "Darwin" ]
then
    CPUS=`/usr/sbin/sysctl -n hw.ncpu`
elif [ "$OS" = "CYGWIN_NT-6.1" ]
then
    # bash on Cygwin.
    CPUS=`/usr/bin/nproc`
    OS='Windows'
elif [ "$OS" = "MINGW64_NT-10.0" ]
then
    # git-bash on Windows.
    CPUS=`/usr/bin/nproc`
    OS='Windows'
else
    CPUS=1
fi

ARTOOLKITX_VERSION=$(cat ${OURDIR}/artoolkitx-version.txt | tr -d '[:space:]')

# Set default CMake generator for Windows.
echo "$CMAKE_GENERATOR"
if [ $OS = "Windows" ]  && test -z "$CMAKE_GENERATOR"; then
    CMAKE_GENERATOR="Visual Studio 16 2019"
    CMAKE_ARCH="x64"
fi

# Function to allow check for required packages.
function check_package {
	# Variant for distros that use debian packaging.
	if (type dpkg-query >/dev/null 2>&1) ; then
		if ! $(dpkg-query -W -f='${Status}' $1 | grep -q '^install ok installed$') ; then
			echo "Warning: required package '$1' does not appear to be installed. To install it use 'sudo apt-get install $1'."
		fi
	# Variant for distros that use rpm packaging.
	elif (type rpm >/dev/null 2>&1) ; then
		if ! $(rpm -qa | grep -q $1) ; then
			echo "Warning: required package '$1' does not appear to be installed. To install it use 'sudo dnf install $1'."
		fi
	fi
}

find_or_fetch_artoolkitx() {
    if [ ! -f "${1}" ] ; then
        echo "Downloading ${1}..."
        curl --location "https://github.com/artoolkitx/artoolkitx/releases/download/${ARTOOLKITX_VERSION}/${1}" -O
    fi
}

if [ "$OS" = "Darwin" ] ; then
# ======================================================================
#  Build platforms hosted by macOS
# ======================================================================

# macOS
if [ $BUILD_MACOS ] ; then
    
    cd "${OURDIR}"
    # Fetch the ARX.framework from latest build into a location where Xcode will find it.
    MOUNTPOINT=mnt$$
    IMAGE="artoolkitX.for.macOS.v${ARTOOLKITX_VERSION}.dmg"
    find_or_fetch_artoolkitx "${IMAGE}"
    mkdir -p "${MOUNTPOINT}"
    hdiutil attach "${IMAGE}" -noautoopen -quiet -mountpoint "${MOUNTPOINT}"
    rm -rf depends/macOS/Frameworks/ARX.framework
    ditto "${MOUNTPOINT}/artoolkitX/SDK/Frameworks/ARX.framework" depends/macOS/Frameworks/ARX.framework
    hdiutil detach "${MOUNTPOINT}" -quiet -force
    rmdir "${MOUNTPOINT}"

    # Make the version number available to Xcode.
    cp macOS/user-config-in.xcconfig macOS/user-config.xcconfig
    sed -E -i "" -e "s/@VERSION@/${VERSION}/" macOS/user-config.xcconfig
    
    (cd macOS
    xcodebuild -target "artoolkitX Camera Calibration Utility" -configuration Release
    )
fi
# /BUILD_MACOS

# iOS
if [ $BUILD_IOS ] ; then
    
    cd "${OURDIR}"
    # Fetch libARX from latest build into a location where Xcode will find it.
    MOUNTPOINT=mnt$$
    IMAGE="artoolkitX.for.iOS.v${ARTOOLKITX_VERSION}.dmg"
    find_or_fetch_artoolkitx "${IMAGE}"
    mkdir -p "${MOUNTPOINT}"
    hdiutil attach "${IMAGE}" -noautoopen -quiet -mountpoint "${MOUNTPOINT}"
    rm -rf depends/iOS/include/ARX/
    cp -af "${MOUNTPOINT}/artoolkitX/SDK/include/ARX" depends/iOS/include
    rm -f depends/iOS/lib/libARX.a
    cp -af "${MOUNTPOINT}/artoolkitX/SDK/lib/libARX.a" depends/iOS/lib
    hdiutil detach "${MOUNTPOINT}" -quiet -force
    rmdir "${MOUNTPOINT}"
    
    # Make the version number available to Xcode.
    cp iOS/user-config-in.xcconfig iOS/user-config.xcconfig
    sed -E -i "" -e "s/@VERSION@/${VERSION}/" iOS/user-config.xcconfig
    
    (cd iOS
    xcodebuild -target "artoolkitX Camera Calibration Utility" -configuration Release -destination generic/platform=iOS
    )
fi
# /BUILD_MACOS

if [ $BUILD_ANDROID ] ; then
    
    cd "${OURDIR}"
    # If artoolkitx folder is not a symlink, fetch artoolkitx from latest build into a location where the build can find it.
    if [[ ! -L "${OURDIR}/depends/android/artoolkitx" ]] ; then
        IMAGE="artoolkitx-${ARTOOLKITX_VERSION}-Android.zip"
        find_or_fetch_artoolkitx "${IMAGE}"
        rm -rf "${OURDIR}/depends/android/artoolkitx"
        unzip "${OURDIR}/${SDK_FILENAME}" -d "${OURDIR}/depends/android/artoolkitx"
    fi
    
    # Make the version number available to Gradle.
    sed -E -i "" -e "s/versionCode [0-9]+/versionCode ${VERSION_INT}/" -e "s/versionName \"[0-9\.]+\"/versionName \"${VERSION}\"/" Android/app/build.gradle

    (cd "${OURDIR}/Android"
    echo "Building Android project"
    ./gradlew assembleRelease
    )
fi
# /BUILD_ANDROID

fi
# /Darwin

if [ "$OS" = "Linux" ] ; then
# ======================================================================
#  Build platforms hosted by Linux
# ======================================================================

# Linux
if [ $BUILD_LINUX ] ; then
    #Before we can install the artoolkitx-dev package we need to install the -lib. As -dev depends on -lib
    #SDK_FILENAME="artoolkitx-lib_${SDK_VERSION}_amd64.deb"
    #curl -f -o "${SDK_FILENAME}" --location "${SDK_URL_DIR}$(rawurlencode "${SDK_FILENAME}")"
    #sudo dpkg -i "${SDK_FILENAME}"

    # Fetch the artoolkitx-dev package and install it.
    #SDK_FILENAME="artoolkitx-dev_${SDK_VERSION}_amd64.deb"
    #curl -f -o "${SDK_FILENAME}" --location "${SDK_URL_DIR}$(rawurlencode "${SDK_FILENAME}")"
    #sudo dpkg -i "${SDK_FILENAME}"

    (cd Linux
	mkdir -p build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Release "-DVERSION=${VERSION}"
    make
	make install
    )

fi
# /BUILD_LINUX

# Linux
if [ $BUILD_LINUX_RASPBIAN ] ; then
    (cd Linux
	mkdir -p build-raspbian
	cd build-raspbian
	cmake .. -DCMAKE_BUILD_TYPE=Release -DARX_TARGET_PLATFORM_VARIANT="raspbian" -DVERSION="${VERSION}"
    make
	make install
    )

fi
# /BUILD_LINUX_RASPBIAN

fi
# /Linux

if [ "$OS" = "Windows" ] ; then
# ======================================================================
#  Build platforms hosted by Windows
# ======================================================================

# Windows
if [ $BUILD_WINDOWS ] ; then

    cd "${OURDIR}"
    MOUNTPOINT=mnt$$
    IMAGE="artoolkitX-${ARTOOLKITX_VERSION}-Windows.zip"
    find_or_fetch_artoolkitx "${IMAGE}"
    unzip -q -o "${IMAGE}" -d "${MOUNTPOINT}"
    # TODO COPY FROM "${MOUNTPOINT}/artoolkitX"
    rm -rf "${MOUNTPOINT}"

    if [ ! -d "build-windows" ] ; then
        mkdir build-windows
    fi

    (cd Windows
    mkdir -p build
    cd build
    cmake.exe .. -DCMAKE_CONFIGURATION_TYPES=${DEBUG+Debug}${DEBUG-Release} -G "$CMAKE_GENERATOR" ${CMAKE_ARCH+-A ${CMAKE_ARCH}} -D"VERSION=${VERSION}"
    cmake.exe --build . --config ${DEBUG+Debug}${DEBUG-Release}  --target install
    )
fi
# /BUILD_WINDOWS

fi
# /Windows
