project(NativePresetFactory)

SET(NativePresetFactory_SOURCES NativePresetFactory.cpp)

IF(NOT MSVC)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(NOT MSVC)
find_library(DL_LIBRARIES dl)

if(MSVC)
	SET (MATH_LIBRARIES )
else(MSVC)
	SET (MATH_LIBRARIES m)
endif(MSVC)

INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR} ${Renderer_SOURCE_DIR})
ADD_LIBRARY(NativePresetFactory STATIC ${NativePresetFactory_SOURCES})
TARGET_LINK_LIBRARIES(NativePresetFactory Renderer ${MATH_LIBRARIES} ${DL_LIBRARIES})
