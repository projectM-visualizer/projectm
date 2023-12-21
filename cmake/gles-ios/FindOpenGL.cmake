# Shorthand for
# "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"

set(CMAKE_OSX_SYSROOT "iphoneos")

find_library(OPENGL_gles2_LIBRARY
        NAMES GLESv2
        PATHS
)

add_library(OpenGL::GLES2 INTERFACE IMPORTED)

set_target_properties(OpenGL::GLES2 PROPERTIES
        IMPORTED_LIBNAME "${OPENGL_gles2_LIBRARY}")