PROJECT(Renderer)
cmake_minimum_required(VERSION 2.4.0)

SET(SOIL_SOURCES SOIL/image_DXT.c SOIL/image_helper.c SOIL/SOIL.c SOIL/stb_image_aug.c)

SET(Renderer_SOURCES FBO.cpp MilkdropWaveform.cpp PerPixelMesh.cpp Pipeline.cpp Renderer.cpp  ShaderEngine.cpp UserTexture.cpp  Waveform.cpp Filters.cpp PerlinNoise.cpp PipelineContext.cpp  Renderable.cpp BeatDetect.cpp Shader.cpp TextureManager.cpp VideoEcho.cpp ${SOIL_SOURCES})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/.. "/usr/local/include")
ADD_LIBRARY(Renderer STATIC ${Renderer_SOURCES})
SET_TARGET_PROPERTIES(Renderer PROPERTIES VERSION 2.00 SOVERSION 2)
TARGET_LINK_LIBRARIES(Renderer m)
