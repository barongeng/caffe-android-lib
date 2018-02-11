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

## Optional
`.envrc` files are for [direnv](http://direnv.net/)
> direnv is an environment variable manager for your shell. It knows how to hook into bash, zsh and fish shell to load or unload environment variables depending on your current directory. This allows to have project-specific environment variables and not clutter the "~/.profile" file.
