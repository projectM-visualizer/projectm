PROJECT(NativePresetFactory)
cmake_minimum_required(VERSION 2.4.0)

    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

SET(NativePresetFactory_SOURCES NativePresetFactory.cpp)

IF(NOT MSVC)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(NOT MSVC)

if(MSVC)
	SET (MATH_LIBRARIES )
else(MSVC)
	SET (MATH_LIBRARIES m)
endif(MSVC)

INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR} ${Renderer_SOURCE_DIR})
ADD_LIBRARY(NativePresetFactory STATIC ${NativePresetFactory_SOURCES})
SET_TARGET_PROPERTIES(NativePresetFactory PROPERTIES VERSION 2.00 SOVERSION 2)
TARGET_LINK_LIBRARIES(NativePresetFactory Renderer ${MATH_LIBRARIES} dl)
