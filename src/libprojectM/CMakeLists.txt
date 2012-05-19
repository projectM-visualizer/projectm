PROJECT(projectM)
cmake_minimum_required(VERSION 2.8.0)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The projectM core library.")

INCLUDE(cmake/CPack-projectM.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

OPTION (USE_DEVIL "Use devIL for image loading rather than the builtin SOIL library" OFF)

OPTION (USE_FBO "Use Framebuffer Objects for increased rendering quality.  Disable this for OpenGL ES 1.x or if you are experiencing problems on older or poorly supported hardware." ON)

OPTION(USE_FTGL "Use FTGL for on-screen fonts (found on your system)" ON)

OPTION (USE_GLES1 "Use OpenGL ES 1.x" OFF)

OPTION (USE_THREADS "Use threads for parallelization" ON)

OPTION (USE_OPENMP "Use OpenMP and OMPTL for multi-core parallelization" ON)

OPTION (USE_NATIVE_GLEW "Use projectM's native implemention of GLEW." OFF)

OPTION (USE_CG "Use Cg for Pixel Shader support" OFF)

OPTION (BUILD_PROJECTM_STATIC "Build the projectM target library in the platform's native static (NOT shared) format." OFF)

OPTION (DISABLE_NATIVE_PRESETS "Turn off support for native (C++ style) presets" OFF)
OPTION (DISABLE_MILKDROP_PRESETS "Turn off support for Milkdrop (.milk / .prjm) presets"  OFF)

# projectM version block
set(PROJECTM_VERSION 2.1.0)
set(PROJECTM_ABI_LEVEL 2)
#

ADD_DEFINITIONS(-DCMAKE_INSTALL_PREFIX="\\\"${CMAKE_INSTALL_PREFIX}\\\"")

SET(LIB_INSTALL_DIR "\${exec_prefix}/lib"
  CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed (default is /lib)"
  FORCE)

if (USE_NATIVE_GLEW)
ADD_DEFINITIONS(-DUSE_NATIVE_GLEW)
 SET(GLEW_SOURCES glew.h glew.c)
 SET(GLEW_LIBRARY "")
else(USE_NATIVE_GLEW)
 set (GLEW_SOURCES "")
 find_package (GLEW)
endif(USE_NATIVE_GLEW)

SET(projectM_SOURCES projectM.cpp PCM.cpp Preset.cpp fftsg.cpp KeyHandler.cpp
timer.cpp wipemalloc.cpp PresetLoader.cpp  PresetChooser.cpp PipelineMerger.cpp ConfigFile.cpp  TimeKeeper.cpp PresetFactory.cpp PresetFactoryManager.cpp ${GLEW_SOURCES})

if (MSVC)
SET(projectM_SOURCES ${projectM_SOURCES} dlfcn.c win32-dirent.cpp)
endif(MSVC)

if (NOT DISABLE_NATIVE_PRESETS)
	add_subdirectory(NativePresetFactory)
	SET(PRESET_FACTORY_SOURCES ${PRESET_FACTORY_SOURCES} ${NativePresetFactory_SOURCE_DIR})
	SET(PRESET_FACTORY_BINARY_DIR ${PRESET_FACTORY_BINARY_DIR} ${NativePresetFactory_BINARY_DIR})
	SET(PRESET_FACTORY_LINK_TARGETS ${PRESET_FACTORY_LINK_TARGETS} NativePresetFactory)
endif(NOT DISABLE_NATIVE_PRESETS)

if (NOT DISABLE_MILKDROP_PRESETS)
	add_subdirectory(MilkdropPresetFactory)
	SET(PRESET_FACTORY_SOURCES ${PRESET_FACTORY_SOURCES} ${MilkdropPresetFactory_SOURCE_DIR})
	SET(PRESET_FACTORY_BINARY_DIR ${PRESET_FACTORY_BINARY_DIR} ${MilkdropPrmaesetFactory_BINARY_DIR})
	SET(PRESET_FACTORY_LINK_TARGETS ${PRESET_FACTORY_LINK_TARGETS} MilkdropPresetFactory)
endif(NOT DISABLE_MILKDROP_PRESETS)

if (USE_DEVIL)
	SET (projectM_SOURCES ${projectM_SOURCES})
	ADD_DEFINITIONS(-DUSE_DEVIL)
	SET (IMAGE_LINK_TARGETS IL ILU ILUT)
else (USE_DEVIL)
	SET (projectM_SOURCES ${projectM_SOURCES})
	SET (IMAGE_LINK_TARGETS )
endif (USE_DEVIL)

if (USE_CG)
	ADD_DEFINITIONS(-DUSE_CG)
	SET (CG_LINK_TARGETS Cg CgGL)
else (USE_CG)
	SET (CG_LINK_TARGETS)
endif(USE_CG)

include(CheckCXXCompilerFlag)

if(USE_GLES1)
	ADD_DEFINITIONS(-DUSE_GLES1)
endif(USE_GLES1)

if(USE_FBO)
	ADD_DEFINITIONS(-DUSE_FBO)
endif(USE_FBO)

if(USE_FTGL)
	ADD_DEFINITIONS(-DUSE_FTGL)

	find_package( Freetype REQUIRED)
	include_directories( ${FREETYPE_INCLUDE_DIRS} )

endif(USE_FTGL)

if (BUILD_PROJECTM_STATIC)
	ADD_LIBRARY(projectM STATIC ${projectM_SOURCES})
else(BUILD_PROJECTM_STATIC)
	ADD_LIBRARY(projectM SHARED ${projectM_SOURCES})
endif(BUILD_PROJECTM_STATIC)

set_property(TARGET projectM PROPERTY LINK_INTERFACE_LIBRARIES "")
set_property(TARGET projectM PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES "")
SET_TARGET_PROPERTIES(projectM PROPERTIES VERSION ${PROJECTM_VERSION} SOVERSION ${PROJECTM_ABI_LEVEL})

if (USE_THREADS)
ADD_DEFINITIONS(-DUSE_THREADS)
endif(USE_THREADS)

if (APPLE)
	ADD_DEFINITIONS(-DMACOS -DSTBI_NO_DDS)
	set(RESOURCE_PREFIX "Resources")
else (APPLE)
	if (WIN32)
		set (RESOURCE_PREFIX "Resources")
		ADD_DEFINITIONS(-DWIN32 -DSTBI_NO_DDS)
	else(WIN32)
		set (RESOURCE_PREFIX "share/projectM")
		ADD_DEFINITIONS(-DLINUX -DSTBI_NO_DDS)
	endif(WIN32)
endif(APPLE)

add_subdirectory(Renderer)

FIND_PACKAGE(OpenGL)

INCLUDE(FindPkgConfig.cmake)

pkg_search_module (FTGL ftgl)

#set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH};/opt/local/include;/Developer/SDKs/MACOSX10.5.sdk/usr/X11/include)
#set(CMAKE_LIBRARY_PATH /opt/local/lib;/Developer/SDKs/MACOSX10.5.sdk;/Developer/SDKs/MacOSX10.5.sdk/usr/X11/lib/)

IF(USE_FTGL)
	if (FTGL_FOUND)
		MESSAGE(STATUS "[projectM] FTGL font support was detected and enabled.")
	else (FTGL_FOUND)
		MESSAGE(STATUS
	"[projectM] Warning! FTGL has been enabled but not detected. Your compilation will probably break. Turn off FTGL support by setting USE_FTGL to false, or visit http://ftgl.wiki.sourceforge.net/ for help on installing FTGL.")
	endif (FTGL_FOUND)

	ADD_DEFINITIONS(-DUSE_FTGL)

	set (FTGL_INCUDE_DIRS ${FTGL_INCLUDE_DIRS} ${CMAKE_INCLUDE_PATH} ${OPENGL_INCLUDE_DIR})
	set (FTGL_LINK_DIRS ${FTGL_LIBRARY_DIRS} )
	set (FTGL_LINK_TARGETS ftgl freetype)

ELSE (USE_FTGL)
	IF (FTGL_FOUND)
		MESSAGE(STATUS "[projectM] Warning: FTGL font support was detected but NOT enabled. You can enable it by setting USE_FTGL=true")
	ELSE (FTGL_FOUND)
		MESSAGE(STATUS "[projectM] Warning: FTGL font support was not detected. Visit http://ftgl.wiki.sourceforge.net/ for help on installing FTGL.")
	ENDIF(FTGL_FOUND)

	# todo: change this because its obscure and probably not being used
	set (FTGL_INCLUDE_DIRS "/opt/local/include")
	set (FTGL_LINK_DIRS )
	set (FTGL_LINK_TARGETS )
ENDIF(USE_FTGL)

CHECK_CXX_COMPILER_FLAG("-fopenmp" SUPPORTS_OPENMP)

IF(USE_OPENMP AND SUPPORTS_OPENMP)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp ")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
ENDIF(USE_OPENMP AND SUPPORTS_OPENMP)

INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR}
		${FTGL_INCLUDE_DIRS}
		${Renderer_SOURCE_DIR}
		${PRESET_FACTORY_SOURCES}
)
LINK_DIRECTORIES(${FTGL_LIBRARY_DIRS}
		${Renderer_BINARY_DIR}
		${PRESET_FACTORY_BINARY_DIR}
)

if(MSVC)
	SET (MATH_LIBRARIES )
else(MSVC)
	SET (MATH_LIBRARIES m)
endif(MSVC)

if(BUILD_PROJECTM_STATIC)
	TARGET_LINK_LIBRARIES(projectM Renderer
				${IMAGE_LINK_TARGETS}
				${CG_LINK_TARGETS}
				${PRESET_FACTORY_LINK_TARGETS}
				${GLEW_LIBRARY}
				${FTGL_LINK_TARGETS}
				${MATH_LIBRARIES}
				dl
				${OPENGL_LIBRARIES}
	)
else(BUILD_PROJECTM_STATIC)
	TARGET_LINK_LIBRARIES(projectM Renderer
				${IMAGE_LINK_TARGETS}
				${CG_LINK_TARGETS}
				${PRESET_FACTORY_LINK_TARGETS}
				${GLEW_LIBRARY}
				${FTGL_LINK_TARGETS}
				${MATH_LIBRARIES}
				dl
				${OPENGL_LIBRARIES}
	)
endif(BUILD_PROJECTM_STATIC)

SET(projectM_FONT_MENU "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/VeraMono.ttf" CACHE FILEPATH "Path to the default menu TrueType font")
SET(projectM_FONT_TITLE "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/Vera.ttf" CACHE FILEPATH "Path to the default title TrueType font")
ADD_DEFINITIONS(-DprojectM_FONT_MENU="\\\"${projectM_FONT_MENU}\\\"")
ADD_DEFINITIONS(-DprojectM_FONT_TITLE="\\\"${projectM_FONT_TITLE}\\\"")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libprojectM.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libprojectM.pc" @ONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config.inp.in" "${CMAKE_CURRENT_BINARY_DIR}/config.inp" @ONLY)

IF(projectM_FONT_MENU MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/Vera.ttf" OR projectM_FONT_TITLE MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/Vera.ttf")
	INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fonts/VeraMono.ttf DESTINATION ${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts)
ENDIF(projectM_FONT_MENU MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/Vera.ttf" OR projectM_FONT_TITLE MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/Vera.ttf")

IF(projectM_FONT_MENU MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/VeraMono.ttf" OR projectM_FONT_TITLE MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/VeraMono.ttf")
	INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fonts/VeraMono.ttf DESTINATION ${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts)
ENDIF(projectM_FONT_MENU MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/VeraMono.ttf" OR projectM_FONT_TITLE MATCHES "${CMAKE_INSTALL_PREFIX}/${RESOURCE_PREFIX}/fonts/VeraMono.ttf")

FILE(GLOB presets "presets/*.milk" "presets/*.prjm" "presets/*.tga")
INSTALL(FILES ${presets} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/presets)
INSTALL(FILES ${Renderer_SOURCE_DIR}/projectM.cg ${Renderer_SOURCE_DIR}/blur.cg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/shaders)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libprojectM.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.inp DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM)
INSTALL(FILES projectM.hpp event.h dlldefs.h fatal.h PCM.hpp Common.hpp DESTINATION include/libprojectM)
INSTALL(TARGETS projectM DESTINATION lib)
