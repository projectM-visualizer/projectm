
noinst_LTLIBRARIES = libRenderer.la

libRenderer_la_SOURCES = \
  SOIL2/image_DXT.c \
  SOIL2/image_helper.c \
  SOIL2/SOIL2.c \
  SOIL2/etc1_utils.c \
  MilkdropWaveform.cpp \
  PerPixelMesh.cpp \
  Pipeline.cpp \
  Renderer.cpp \
  ShaderEngine.cpp \
  StaticGlShaders.cpp \
  Texture.cpp \
  Waveform.cpp \
  Filters.cpp \
  PerlinNoise.cpp \
  PerlinNoiseWithAlpha.cpp \
  PipelineContext.cpp \
  Renderable.cpp \
  BeatDetect.cpp \
  Shader.cpp \
  StaticGlShaders.cpp \
  TextureManager.cpp \
  VideoEcho.cpp \
  RenderItemDistanceMetric.cpp \
  RenderItemMatcher.cpp \
	BeatDetect.hpp               PipelineContext.hpp          ShaderEngine.hpp\
	RenderItemDistanceMetric.hpp TextureManager.hpp\
	Filters.hpp                  RenderItemMatcher.hpp        Transformation.hpp\
	MilkdropWaveform.hpp         RenderItemMergeFunction.hpp  Texture.hpp\
	PerPixelMesh.hpp             Renderable.hpp               VideoEcho.hpp\
	PerlinNoise.hpp  PerlinNoiseWithAlpha.hpp            Renderer.hpp                 Waveform.hpp\
	Pipeline.hpp                 Shader.hpp\
	SOIL2/SOIL2.h           SOIL2/stbi_DDS.h\
	SOIL2/etc1_utils.h      SOIL2/stbi_DDS_c.h\
	SOIL2/image_DXT.h       SOIL2/stbi_ext.h\
	SOIL2/image_helper.h    SOIL2/stbi_ext_c.h\
	SOIL2/pkm_helper.h      SOIL2/stbi_pkm.h\
	SOIL2/pvr_helper.h      SOIL2/stbi_pkm_c.h\
	SOIL2/stb_image.h       SOIL2/stbi_pvr.h\
	SOIL2/stb_image_write.h SOIL2/stbi_pvr_c.h\
	StaticGlShaders.h\
	hlslparser/src/CodeWriter.cpp hlslparser/src/Engine.h \
	hlslparser/src/HLSLParser.h hlslparser/src/HLSLTree.cpp \
	hlslparser/src/CodeWriter.h hlslparser/src/GLSLGenerator.cpp \
	hlslparser/src/HLSLTokenizer.cpp hlslparser/src/HLSLTree.h \
	hlslparser/src/Engine.cpp hlslparser/src/GLSLGenerator.h hlslparser/src/HLSLParser.cpp \
	hlslparser/src/HLSLTokenizer.h

libRenderer_la_CPPFLAGS = ${my_CFLAGS} \
	-include $(top_builddir)/config.h \
	-I$(top_srcdir)/src/libprojectM/Renderer/hlslparser/src \
	-I$(top_srcdir)/src/libprojectM

libRenderer_la_LDFLAGS = \
  ${FTGL_LIBS}
