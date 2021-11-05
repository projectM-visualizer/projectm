# GLES3 Find Script

The OpenGL CMake find script in this directory contains additional code to find GLES versions 1 to 3. It was taken from
CMake 3.22 and patched accordingly. A merge request is underway upstream, so ideally, this will be part of CMake 3.23+.

In the meantime, we'll use this script as a local copy if GLES3 support is requested. Will add a version check once it's
in upstream CMake so this file will only be used if the CMake version used to build projectM is too low.