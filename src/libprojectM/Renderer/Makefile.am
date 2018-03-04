noinst_LTLIBRARIES = libRenderer.la

libRenderer_la_SOURCES = \
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
	BeatDetect.hpp               PipelineContext.hpp          ShaderEngine.hpp\
	FBO.hpp                      RenderItemDistanceMetric.hpp TextureManager.hpp\
	Filters.hpp                  RenderItemMatcher.hpp        Transformation.hpp\
	MilkdropWaveform.hpp         RenderItemMergeFunction.hpp  UserTexture.hpp\
	PerPixelMesh.hpp             Renderable.hpp               VideoEcho.hpp\
	PerlinNoise.hpp              Renderer.hpp                 Waveform.hpp\
	Pipeline.hpp                 Shader.hpp\
	SOIL2/SOIL2.h           SOIL2/stbi_DDS.h\
	SOIL2/etc1_utils.h      SOIL2/stbi_DDS_c.h\
	SOIL2/image_DXT.h       SOIL2/stbi_ext.h\
	SOIL2/image_helper.h    SOIL2/stbi_ext_c.h\
	SOIL2/pkm_helper.h      SOIL2/stbi_pkm.h\
	SOIL2/pvr_helper.h      SOIL2/stbi_pkm_c.h\
	SOIL2/stb_image.h       SOIL2/stbi_pvr.h\
	SOIL2/stb_image_write.h SOIL2/stbi_pvr_c.h


libRenderer_la_CPPFLAGS = ${my_CFLAGS} \
-I$(top_srcdir)/src/libprojectM
