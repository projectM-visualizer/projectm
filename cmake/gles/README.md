# GLES3 Find Script

The OpenGL CMake find script in this directory contains additional code to find GLES versions 1 to 3. It was taken from
CMake 3.22 and patched accordingly. A merge request was accepted upstream, it is part of CMake 3.27.

We use this script for CMake lower than 3.27 as a local copy if GLES3 support is requested to provide compatibility. A version check 
compares the current CMake version so this file will only be used if the CMake version used to build projectM is too low.