# - Try to find GLEW
# Once done this will define
#
#  GLEW_FOUND        - system has GLEW
#  GLEW_INCLUDE_DIR  - the GLEW include directory
#  GLEW_LIBRARY      - Link these to use GLEW
#

   FIND_PATH( GLEW_INCLUDE_DIR glew.h wglew.h
              PATHS /usr/local /usr c:/glew "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK" "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK"
              PATH_SUFFIXES include include/gl include/GL Include Include/gl Include/GL )
   FIND_LIBRARY( GLEW_LIBRARY
                 NAMES glew32 glew GLEW
                 PATHS /usr /usr/local c:/glew "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK"  "C:/Program Files/Microsoft Visual Studio 8/VC/PlatformSDK"
                 PATH_SUFFIXES lib Lib
               )

IF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
   SET(GLEW_FOUND TRUE)
   SET( GLEW_LIBRARY_DIR ${GLEW_LIBRARY} )
ELSE (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
   SET( GLEW_FOUND FALSE )
ENDIF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
