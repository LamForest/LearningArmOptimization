#!/usr/bin/env bash

set -e
set -x

basepath=$(cd `dirname $0`/; pwd)

BUILD_DIR=${basepath}/build

BUILD_ANDROID_NDK_HOME=/d/gao_tianlin/Android/AndroidSDK/ndk/21.1.6352462
NINJA_EXE=/d/gao_tianlin/Android/AndroidSDK/cmake/3.10.2.4988404/bin/ninja.exe
#BUILD_ANDROID_NDK_HOME=/d/gao_tianlin/Android/android-ndk-r15c-windows-x86_64/ndk-bundle

DEPLOY_DIR=/data/local/tmp/ldp
CMAKE=/d/gao_tianlin/Android/AndroidSDK/cmake/3.10.2.4988404/bin/cmake

#set CXX=D:/gao_tianlin/Android/AndroidSDK/ndk/21.1.6352462/toolchains/llvm/prebuilt/windows-x86_64/lib/gcc/arm-linux-androideabi/4.9.x

# rm -rf ${BUILD_DIR}
if [[ ! -d ${BUILD_DIR} ]]; then
    mkdir -p ${BUILD_DIR}
fi

cd ${BUILD_DIR}
$CMAKE \
-DCMAKE_TOOLCHAIN_FILE=${BUILD_ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
-DANDROID_NDK=${BUILD_ANDROID_NDK_HOME} \
-DANDROID_ABI="armeabi-v7a with NEON" \
-DANDROID_NATIVE_API_LEVEL=23 \
-DANDROID_ARM_NEON=TRUE \
-DCMAKE_MAKE_PROGRAM=${NINJA_EXE} \
-G Ninja \
../
#-DANDROID_TOOLCHAIN=gcc \
#-DANDROID_TOOLCHAIN=clang 

${NINJA_EXE}

#adb shell "mkdir -p ${DEPLOY_DIR}/lib"
#adb push ${basepath}/build/unit_test/runUnitTests ${DEPLOY_DIR}
#adb push ${basepath}/build/src/libboxfilter.so ${DEPLOY_DIR}/lib

#adb shell "cd ${DEPLOY_DIR}; export LD_LIBRARY_PATH=./lib ; ./runUnitTests"
