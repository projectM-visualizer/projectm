PROJECT(Renderer)
cmake_minimum_required(VERSION 2.4.0)

ADD_DEFINITIONS(-DLINUX)
ADD_DEFINITIONS(-DCMAKE_INSTALL_PREFIX="\\\"${CMAKE_INSTALL_PREFIX}\\\"") 
SET(Renderer_SOURCES FBO.cpp MilkdropWaveform.cpp PerPixelMesh.cpp Pipeline.cpp Renderer.cpp  ShaderEngine.cpp UserTexture.cpp  Waveform.cpp Filters.cpp PerlinNoise.cpp PipelineContext.cpp  Renderable.cpp Shader.cpp TextureManager.cpp VideoEcho.cpp)



INCLUDE_DIRECTORIES("../")
ADD_LIBRARY(Renderer STATIC ${Renderer_SOURCES})
SET_TARGET_PROPERTIES(Renderer PROPERTIES VERSION 2.00 SOVERSION 2)
TARGET_LINK_LIBRARIES(Renderer m)
