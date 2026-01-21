C++ Interface Test Suite
========================

This is a small, standalone test build suite made to test if the C++ API links properly.

It must not be included in the main libprojectM build process, as the application requires the final installation layout
instead of the build dir.

For a successful test, libprojectM must be built with the `ENABLE_CXX_INTERFACE` option enabled.
