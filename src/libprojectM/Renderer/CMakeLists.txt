project(Renderer)

SET(SOIL_SOURCES
	SOIL2/image_DXT.c
	SOIL2/image_helper.c
	SOIL2/SOIL2.c
)

SET(Renderer_SOURCES
	FBO.cpp
	MilkdropWaveform.cpp
	PerPixelMesh.cpp
	Pipeline.cpp
	Renderer.cpp
	ShaderEngine.cpp
	UserTexture.cpp
	Waveform.cpp
	Filters.cpp
	PerlinNoise.cpp
	PipelineContext.cpp
	Renderable.cpp
	BeatDetect.cpp
	Shader.cpp
	TextureManager.cpp
	VideoEcho.cpp
	RenderItemDistanceMetric.cpp
	RenderItemMatcher.cpp
	${SOIL_SOURCES}
)

IF(NOT MSVC)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
ENDIF(NOT MSVC)

if(MSVC)
	SET (MATH_LIBRARIES )
else(MSVC)
	SET (MATH_LIBRARIES m)
endif(MSVC)

if (APPLE)
	FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
	INCLUDE_DIRECTORIES( /usr/include )
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include")
else (APPLE)
	set(COREFOUNDATION_LIBRARY )
endif(APPLE)

INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR})

ADD_LIBRARY(Renderer STATIC ${Renderer_SOURCES})
TARGET_LINK_LIBRARIES(Renderer ${MATH_LIBRARIES}
			${GLEW_LINK_TARGETS}
			${FTGL_LINK_TARGETS}
			${OPENGL_LIBRARIES}
			${IMAGE_LINK_TARGETS}
			${CG_LINK_TARGETS}
			${FREETYPE2_LIBRARIES}
			${COREFOUNDATION_LIBRARY}
)
set_target_properties(Renderer PROPERTIES COMPILE_FLAGS "-Wno-parentheses")
