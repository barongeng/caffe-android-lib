#!/usr/bin/env bash

set -eu

# shellcheck source=/dev/null
. "$(dirname "$0")/../config.sh"

CAFFE_ROOT=${PROJECT_DIR}/caffe
BUILD_DIR=${CAFFE_ROOT}/build
ANDROID_LIB_ROOT=${PROJECT_DIR}/android_lib
BOOST_HOME=${INSTALL_DIR}/boost
GFLAGS_HOME=${INSTALL_DIR}/gflags
GLOG_ROOT=${INSTALL_DIR}/glog
OPENCV_ROOT=${INSTALL_DIR}/opencv/sdk/native/jni
PROTOBUF_ROOT=${INSTALL_DIR}/protobuf
SNAPPY_ROOT_DIR=${INSTALL_DIR}/snappy
export LEVELDB_ROOT=${INSTALL_DIR}/leveldb
export LMDB_DIR=${INSTALL_DIR}/lmdb
export OpenBLAS_HOME="${INSTALL_DIR}/openblas"

. "$(dirname "$0")/get_viennacl.sh"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
pushd "${BUILD_DIR}"

cmake -DCMAKE_TOOLCHAIN_FILE="${PROJECT_DIR}/android-cmake/android.toolchain.cmake" \
      -DANDROID_NDK="${NDK_ROOT}" \
      -DCMAKE_BUILD_TYPE=Release \
      -DANDROID_ABI="${ANDROID_ABI}" \
      -DANDROID_NATIVE_API_LEVEL=21 \
      -DANDROID_USE_OPENMP=ON \
      -DADDITIONAL_FIND_PATH="${ANDROID_LIB_ROOT}" \
      -DBUILD_matlab=OFF \
      -DBUILD_python=OFF \
      -DBUILD_python_layer=OFF \
      -DBUILD_docs=OFF \
      -DCPU_ONLY=OFF \
      -DUSE_CUDA=OFF \
      -DUSE_GREENTEA=ON \
	  -DDISABLE_DEVICE_HOST_UNIFIED_MEMORY=ON \
      -DUSE_LMDB=OFF \
      -DUSE_LEVELDB=OFF \
      -DUSE_HDF5=OFF \
      -DBLAS=open \
      -DBOOST_ROOT="${BOOST_HOME}" \
      -DGFLAGS_INCLUDE_DIR="${GFLAGS_HOME}/include" \
      -DGFLAGS_LIBRARY="${GFLAGS_HOME}/lib/libgflags.a" \
      -DOPENCL_INCLUDE_DIRS="${ANDROID_LIB_ROOT}/opencl-android/include/1.x" \
      -DOPENCL_LIBRARIES="${ANDROID_LIB_ROOT}/opencl-android/lib/libOpenCL.so" \
      -DGLOG_INCLUDE_DIR="${GLOG_ROOT}/include" \
      -DGLOG_LIBRARY="${GLOG_ROOT}/lib/libglog.a" \
      -DOpenCV_DIR="${OPENCV_ROOT}" \
      -DPROTOBUF_PROTOC_EXECUTABLE="${ANDROID_LIB_ROOT}/protobuf_host/bin/protoc" \
      -DPROTOBUF_INCLUDE_DIR="${PROTOBUF_ROOT}/include" \
      -DPROTOBUF_LIBRARY="${PROTOBUF_ROOT}/lib/libprotobuf.a" \
      -DCMAKE_INSTALL_PREFIX="${ANDROID_LIB_ROOT}/caffe" \
..

make -j"${N_JOBS}"
#rm -rf "${INSTALL_DIR}/caffe"
#make install

popd
