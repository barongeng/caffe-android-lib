Caffe-Android-Lib (OpenCL)
===============

## Goal
Porting [caffe](https://github.com/BVLC/caffe) **OpenCL** to android platform

### Support
* Up-to-date caffe 
* CPU & GPU 
* No support for hdf5

## Build

### On local machine
**Tested with XiaoMi 5, Android NDK r15c and cmake 3.5.1 on Ubuntu 16.06**

Install NDK r15c
```sh
wget https://dl.google.com/android/repository/android-ndk-r15c-linux-x86_64.zip
sudo unzip android-ndk-r15c-linux-x86_64.zip -d /etc
```
Download target libOpenCL.so from your Andriod device by adb. Normally under /vendor/lib64/libOpenCL.so
```sh
adb pull /vendor/lib64/libOpenCL.so  ~/
```

Start building
```sh
git clone --recursive https://github.com/quhezheng/caffe-android-lib.git
cd caffe-android-lib
export ANDROID_ABI=arm64-v8a
export NDK_ROOT=/etc/android-ndk-r15c  (or /your/path/to/ndk)
cp ~/libOpenCL.so android_lib/opencl-android/lib
./build.sh
```
There it will be \*.so \*.a libaray under caffe-android-lib/caffe/build/lib


### NOTE: OpenBLAS
OpenBLAS is the only supported BLAS choice now, and the supported ABIs are the following:

* `armeabi`
* `armeabi-v7a`
* `arm64-v8a` (default)
* `x86`
* `x86_64`

## Issues

Any comments, issues or PRs are welcomed.
Thanks.

## TODO
- [ ] Integrate using CMake's ExternalProject
- [ ] Support HDF5

Caffe-Android-Demo (App)
===============
## Goal
Demo Andriod app to run caffe on cell phone's GPU.
### Support
* App to load caffe demo model to recognize picture (Camera)
* CPU & GPU 
* Debug caffe C++ code on cell phone (PC as host)
* XiaoMi 5 (Tested by this repository)

## Setup & Build
### Link caffe native C++ code to Android project
```sh
cd caffe-android-lib/caffe-android-demo/app/caffe-opencl
./link.sh
```
### Android studio 2.3.3
```sh
cd ~/Downloads
wget https://dl.google.com/dl/android/studio/ide-zips/2.3.3.0/android-studio-ide-162.4069837-linux.zip
sudo unzip android-studio-ide-162.4069837-linux.zip -d /etc
```
* start Android studio
```sh
cd /etc/android-studio/bin
./studio.sh
```
* click 'import project(Eclipse ADT, Gradle, etc.)', brown to 'caffe-android-lib/caffe-android-demo', click 'OK' to open the project. Wait a few minutes to finish gradle building... 

* Build the project by menu 'Build -->Make Project'

* Collect \*.so files into target lib
```sh
cd YOUR/PATH/OF/caffe-android-lib/caffe-android-demo/app
cp .externalNativeBuild/cmake/release/arm64-v8a/lib/libcaffe.so src/main/jniLibs
cp .externalNativeBuild/cmake/release/arm64-v8a/lib/libcaffe_jni.so src/main/jniLibs
```

### Deploye Caffe Model on Cell Phone
Connect USB cable between your PC and Cell phone
```sh
# Download googlenet
cd YOUR/PATH/OF/caffe-android-lib/caffe
./scripts/download_model_binary.py models/bvlc_googlenet/
# push things to your device
adb shell mkdir -p /sdcard/caffe_mobile/
adb push models/bvlc_googlenet/ /sdcard/caffe_mobile/bvlc_googlenet/
```

### Debug C++
* Connect USB cable between your PC and Cell phone
* Setup a break point in a C++ code file from inside Android studio. Click 'Debug' buton in Android studio...
