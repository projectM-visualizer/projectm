# Shorthand for
# "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"

set(CMAKE_OSX_SYSROOT "iphoneos")

find_library(OPENGL_gles3_LIBRARY
        NAMES GLESv3
        PATHS
)

add_library(OpenGL::GLES3 INTERFACE IMPORTED)

set_target_properties(OpenGL::GLES3 PROPERTIES
        IMPORTED_LIBNAME "${OPENGL_gles3_LIBRARY}")