BUILD=../../../build/

SOURCES= \
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


CONFIG=libftgl libGL libfreetype

CPPFLAGS+= -I. -I.. -ISOIL2
LDFLAGS+=-lftgl -lgl -lfreetype -lprojectM

#LIBRARIES=../libprojectM
PRODUCTS=Renderer.lib

include $(BUILD)rules.mk

$(BUILD)rules.mk:
	cd .. && git submodule update --init --recursive
