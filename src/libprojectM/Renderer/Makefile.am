lib_LTLIBRARIES=libRenderer.la

libRenderer_la_SOURCES= \
  SOIL2/image_DXT.c \
  SOIL2/image_helper.c \
  SOIL2/SOIL2.c \
  SOIL2/etc1_utils.c \
  FBO.cpp \
  MilkdropWaveform.cpp \
  PerPixelMesh.cpp \
  Pipeline.cpp \
  Renderer.cpp \
  ShaderEngine.cpp \
  UserTexture.cpp \
  Waveform.cpp \
  Filters.cpp \
  PerlinNoise.cpp \
  PipelineContext.cpp \
  Renderable.cpp \
  BeatDetect.cpp \
  Shader.cpp \
  TextureManager.cpp \
  VideoEcho.cpp \
  RenderItemDistanceMetric.cpp \
  RenderItemMatcher.cpp \
  ../KeyHandler.cpp

libRenderer_la_CPPFLAGS = ${my_CFLAGS} \
-fvisibility=hidden \
-ffunction-sections \
-fdata-sections \
-I$(top_srcdir)/src/libprojectM

libRenderer_la_LDFLAGS = -static
