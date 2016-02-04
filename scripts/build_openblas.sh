#!/usr/bin/env sh
set -e

if [ -z "$NDK_ROOT" ] && [ "$#" -eq 0 ]; then
    echo 'Either $NDK_ROOT should be set or provided as argument'
    echo "e.g., 'export NDK_ROOT=/path/to/ndk' or"
    echo "      '${0} /path/to/ndk'"exit 1
else
    NDK_ROOT="${1:-${NDK_ROOT}}"
fi

if [ "$(uname)" = "Darwin" ]; then
    OS=darwin
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    OS=linux
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ||
       "$(expr substr $(uname -s) 1 9)" = "CYGWIN_NT" ]; then
    OS=windows
else
    echo "Unknown OS"
    exit 1
fi

if [ "$(uname -m)" = "x86_64"  ]; then
    BIT=x86_64
else
    BIT=x86
fi

WD=$(readlink -f "`dirname $0`/..")
OPENBLAS_ROOT=${WD}/OpenBLAS
INSTALL_DIR=${WD}/android_lib
N_JOBS=${N_JOBS:-4}

cd "${OPENBLAS_ROOT}"

make clean
if [ "${ANDROID_ABI}" = "armeabi-v7a-hard-softfp with NEON" ]; then
    TOOLCHAIN_DIR=$NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/${OS}-${BIT}/bin
    make -j${N_JOBS} \
         CC="$TOOLCHAIN_DIR/arm-linux-androideabi-gcc --sysroot=$NDK_ROOT/platforms/android-21/arch-arm" \
         CROSS_SUFFIX=$TOOLCHAIN_DIR/arm-linux-androideabi- \
         HOSTCC=gcc NO_LAPACK=1 TARGET=ARMV7 \
         USE_THREAD=1 NUM_THREADS=8 USE_OPENMP=1
elif [ "${ANDROID_ABI}" = "arm64-v8a"  ]; then
    TOOLCHAIN_DIR=$NDK_ROOT/toolchains/aarch64-linux-android-4.9/prebuilt/${OS}-${BIT}/bin
    make -j${N_JOBS} \
         CC="$TOOLCHAIN_DIR/aarch64-linux-android-gcc --sysroot=$NDK_ROOT/platforms/android-21/arch-arm64" \
         CROSS_SUFFIX=$TOOLCHAIN_DIR/aarch64-linux-android- \
         HOSTCC=gcc NO_LAPACK=1 TARGET=ARMV8 \
         USE_THREAD=1 NUM_THREADS=8 USE_OPENMP=1
elif [ "${ANDROID_ABI}" = "armeabi"  ]; then
    TOOLCHAIN_DIR=$NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/${OS}-${BIT}/bin
    make -j${N_JOBS} \
         CC="$TOOLCHAIN_DIR/arm-linux-androideabi-gcc --sysroot=$NDK_ROOT/platforms/android-21/arch-arm" \
         CROSS_SUFFIX=$TOOLCHAIN_DIR/arm-linux-androideabi- \
         HOSTCC=gcc NO_LAPACK=1 TARGET=ARMV5 \
         USE_THREAD=1 NUM_THREADS=8 USE_OPENMP=1
elif [ "${ANDROID_ABI}" = "x86_64"  ]; then
    TOOLCHAIN_DIR=$NDK_ROOT/toolchains/x86_64-4.9/prebuilt/${OS}-${BIT}/bin
    make -j${N_JOBS} \
         CC="$TOOLCHAIN_DIR/x86_64-linux-android-gcc --sysroot=$NDK_ROOT/platforms/android-21/arch-x86_64" \
         CROSS_SUFFIX=$TOOLCHAIN_DIR/x86_64-linux-android- \
         HOSTCC=gcc NO_LAPACK=1 TARGET=ATOM BINARY=64 \
         USE_THREAD=1 NUM_THREADS=8 USE_OPENMP=1
else
    echo "Error: not support OpenBLAS for ABI: ${ANDROID_ABI}"
    exit 1
fi

rm -rf "$INSTALL_DIR/openblas"
make PREFIX="$INSTALL_DIR/openblas" install

cd "${WD}"