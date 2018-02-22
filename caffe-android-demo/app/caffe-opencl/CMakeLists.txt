cmake_minimum_required(VERSION 2.8.7)

# ---[ Caffe project
project(Caffe C CXX)
set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})
# ---[ Caffe version
set(CAFFE_TARGET_VERSION "1.0.0" CACHE STRING "Caffe logical version")
set(CAFFE_TARGET_SOVERSION "1.0.0" CACHE STRING "Caffe soname version")
add_definitions(-DCAFFE_VERSION=${CAFFE_TARGET_VERSION})

# This code is taken from https://github.com/sh1r0/caffe-android-lib
# Search packages for host system instead of packages for target system
# in case of cross compilation these macro should be defined by toolchain file
set(INSTALL_DIR "/home/hezheng/caffe-android-lib/android_lib")
set(ANDROID_LIB_ROOT ${INSTALL_DIR})
set(BOOST_HOME ${INSTALL_DIR}/boost)
set(GFLAGS_HOME ${INSTALL_DIR}/gflags)
set(GLOG_ROOT ${INSTALL_DIR}/glog)
set(OPENCV_ROOT ${INSTALL_DIR}/opencv/sdk/native/jni)
set(PROTOBUF_ROOT ${INSTALL_DIR}/protobuf)
set(SNAPPY_ROOT_DIR ${INSTALL_DIR}/snappy)
set(LEVELDB_ROOT ${INSTALL_DIR}/leveldb)
set(LMDB_DIR ${INSTALL_DIR}/lmdb)
set(OpenBLAS_HOME ${INSTALL_DIR}/openblas)

set(BOOST_ROOT ${BOOST_HOME})
set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include")
set(Boost_LIBRARIES "${BOOST_ROOT}/lib/libboost_thread.a;${BOOST_ROOT}/lib/libboost_system.a;${BOOST_ROOT}/lib/libboost_filesystem.a;${BOOST_ROOT}/lib/libboost_date_time.a;${BOOST_ROOT}/lib/libboost_atomic.a")
set(GFLAGS_INCLUDE_DIR ${GFLAGS_HOME}/include)
set(GFLAGS_LIBRARY ${GFLAGS_HOME}/lib/libgflags.a)
set(OPENCL_INCLUDE_DIRS ${ANDROID_LIB_ROOT}/opencl-android/include/1.x)
set(OPENCL_LIBRARIES ${ANDROID_LIB_ROOT}/opencl-android/lib/libOpenCL.so)
set(GLOG_INCLUDE_DIR ${GLOG_ROOT}/include)
set(GLOG_LIBRARY ${GLOG_ROOT}/lib/libglog.a)
set(OpenCV_DIR ${OPENCV_ROOT})
set(PROTOBUF_PROTOC_EXECUTABLE ${ANDROID_LIB_ROOT}/protobuf_host/bin/protoc)
set(PROTOBUF_INCLUDE_DIR ${PROTOBUF_ROOT}/include)
set(PROTOBUF_LIBRARY ${PROTOBUF_ROOT}/lib/libprotobuf.a)
set(VIENNACL_HOME ${INSTALL_DIR}/ViennaCL-1.7.1)
set(BLAS Open CACHE STRING "Selected BLAS library")

if(NOT COMMAND find_host_package)
  macro(find_host_package)
    find_package(${ARGN})
  endmacro()
endif()
if(NOT COMMAND find_host_program)
  macro(find_host_program)
    find_program(${ARGN})
  endmacro()
endif()

# ---[ Using cmake scripts and modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

include(ExternalProject)
include(GNUInstallDirs)

include(cmake/Utils.cmake)
include(cmake/Targets.cmake)
include(cmake/Misc.cmake)
include(cmake/Summary.cmake)
include(cmake/ConfigGen.cmake)
include(cmake/WindowsCreateLinkHeader.cmake)
include(cmake/TargetResolvePrerequesites.cmake)

# ---[ Options
caffe_option(CPU_ONLY "Build Caffe without CUDA and OpenCL support" OFF)
caffe_option(USE_INDEX_64 "Build Caffe with 64 bit indexing" OFF)
caffe_option(USE_CUDA "Build Caffe with CUDA support" OFF)
caffe_option(USE_GREENTEA "Build Caffe with OpenCL support" ON)
caffe_option(DISABLE_DEVICE_HOST_UNIFIED_MEMORY "Disable host/device shared memory" ON)
caffe_option(USE_LIBDNN "Build Caffe with LibDNN library support" ON)
caffe_option(USE_CLBLAS "Build Caffe with clBLAS support (instead of using ViennaClBLAS)" OFF)
caffe_option(USE_CLBLAST "Build Caffe with CLBlast support (instead of using ViennaClBLAS)" OFF)
caffe_option(USE_ISAAC "Build Caffe with ISAAC support (instead of using ViennaClBLAS)" OFF)
caffe_option(USE_CUDNN "Build Caffe with cuDNN library support" OFF)
caffe_option(USE_NCCL "Build Caffe with NCCL library support" OFF)
caffe_option(BUILD_SHARED_LIBS "Build shared libraries" ON)


caffe_option(BUILD_python "Build Python wrapper" OFF)
set(python_version "2" CACHE STRING "Specify which Python version to use")
caffe_option(BUILD_matlab "Build Matlab wrapper" OFF)
caffe_option(BUILD_docs   "Build documentation" OFF)
caffe_option(BUILD_python_layer "Build the Caffe Python layer" OFF)
caffe_option(USE_OPENCV "Build with OpenCV support" ON)
caffe_option(USE_LEVELDB "Build with levelDB" OFF)
caffe_option(USE_LMDB "Build with lmdb" OFF)
caffe_option(ALLOW_LMDB_NOLOCK "Allow MDB_NOLOCK when reading LMDB files (only if necessary)" OFF)
caffe_option(USE_OPENMP "Link with OpenMP (when your BLAS wants OpenMP and you get linker errors)" ON)
caffe_option(USE_FFT "Build with fftw3 or/and clFFT" OFF)

# ---[ Flag consistency check
if(CPU_ONLY)
  set(USE_CUDA OFF)
  set(USE_GREENTEA OFF)
  set(USE_CUDNN OFF)
  set(USE_LIBDNN OFF)
  set(USE_CLBLAS OFF)
  set(USE_CLBLAST OFF)
endif()

if(USE_ISAAC)
  set(USE_CLBLAS ON)
endif()

caffe_option(protobuf_MODULE_COMPATIBLE "Make the protobuf-config.cmake compatible with the module mode" ON IF MSVC)
caffe_option(COPY_PREREQUISITES "Copy the prerequisites next to each executable or shared library directory" ON IF MSVC)
caffe_option(INSTALL_PREREQUISITES "Install the prerequisites next to each executable or shared library directory" ON IF MSVC)

# ---[ Prebuild dependencies on windows
include(cmake/WindowsDownloadPrebuiltDependencies.cmake)

# This code is taken from https://github.com/sh1r0/caffe-android-lib
caffe_option(USE_HDF5 "Build with hdf5" OFF)
if(ANDROID)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")
  caffe_enable_cpp11_support()
endif()


# ---[ Dependencies
include(cmake/Dependencies.cmake)

# ---[ Flags
if(DISABLE_DEVICE_HOST_UNIFIED_MEMORY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISABLE_DEVICE_HOST_UNIFIED_MEMORY")
endif()

if(UNIX OR APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -std=c++11 -DCMAKE_BUILD")
endif()

if(DISABLE_DOUBLE_SUPPORT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISABLE_DOUBLE_SUPPORT")
endif()


caffe_set_caffe_link()

if(USE_libstdcpp)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -std=c++11")
  message("-- Warning: forcing libstdc++ (controlled by USE_libstdcpp option in cmake)")
endif()

if(USE_INDEX_64)
  list(APPEND Caffe_DEFINITIONS PUBLIC -DUSE_INDEX_64)
endif()

# ---[ Warnings
caffe_warnings_disable(CMAKE_CXX_FLAGS -Wno-sign-compare -Wno-uninitialized)

# ---[ Config generation
configure_file(cmake/Templates/caffe_config.h.in "${PROJECT_BINARY_DIR}/caffe_config.h")

# ---[ Includes
set(Caffe_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${Caffe_INCLUDE_DIR} ${PROJECT_BINARY_DIR} ${Caffe_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
set(Caffe_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})
include_directories(BEFORE src) # This is needed for gtest.

# ---[ Includes & defines for CUDA

# cuda_compile() does not have per-call dependencies or include paths
# (cuda_compile() has per-call flags, but we set them here too for clarity)
#
# list(REMOVE_ITEM ...) invocations remove PRIVATE and PUBLIC keywords from collected definitions and include paths

set(Caffe_ALL_INCLUDE_DIRS ${Caffe_INCLUDE_DIRS})
list(REMOVE_ITEM Caffe_ALL_INCLUDE_DIRS PRIVATE PUBLIC)
set(Caffe_ALL_DEFINITIONS ${Caffe_DEFINITIONS})
list(REMOVE_ITEM Caffe_ALL_DEFINITIONS PRIVATE PUBLIC)

if(HAVE_CUDA)
  # pass include paths to cuda_include_directories()
  cuda_include_directories(${Caffe_INCLUDE_DIR} ${Caffe_SRC_DIR} ${Caffe_ALL_INCLUDE_DIRS})
  # add definitions to nvcc flags directly
  list(APPEND CUDA_NVCC_FLAGS ${Caffe_ALL_DEFINITIONS})
endif()

# ---[ Subdirectories
#add_subdirectory(src/gtest)
add_subdirectory(src/caffe)
#add_subdirectory(tools)
#add_subdirectory(examples)
# This code is taken from https://github.com/sh1r0/caffe-android-lib
add_subdirectory(android)
#add_subdirectory(python)
#add_subdirectory(matlab)
#add_subdirectory(docs)

# ---[ Linter target
add_custom_target(lint COMMAND ${CMAKE_COMMAND} -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE} -P ${PROJECT_SOURCE_DIR}/cmake/lint.cmake)

# ---[ pytest target
if(BUILD_python)
  if(UNIX)
    set(python_executable python${python_version})
  else()
    set(python_executable ${PYTHON_EXECUTABLE})
  endif()
  add_custom_target(pytest COMMAND ${python_executable} -m unittest discover -s caffe/test WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python )
  add_dependencies(pytest pycaffe)
endif()

# ---[ uninstall target
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/Uninstall.cmake
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/Uninstall.cmake)

# ---[ Configuration summary
caffe_print_configuration_summary()

# ---[ Export configs generation
caffe_generate_export_configs()

