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

Tested with Android NDK r11c and cmake 3.5.2 on Ubuntu 14.04

```sh
git clone --recursive https://github.com/sh1r0/caffe-android-lib.git
cd caffe-android-lib
export ANDROID_ABI=x86_64
export NDK_ROOT=/path/to/ndk
./build.sh
```

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
- [ ] OpenCL support
- [ ] CUDA suuport

## Optional
`.envrc` files are for [direnv](http://direnv.net/)
> direnv is an environment variable manager for your shell. It knows how to hook into bash, zsh and fish shell to load or unload environment variables depending on your current directory. This allows to have project-specific environment variables and not clutter the "~/.profile" file.
