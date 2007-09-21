#ifndef TextureManager_HPP
#define TextureManager_HPP

#include "PresetFrameIO.hpp"
#ifdef LINUX
#include <GL/gl.h>
#endif
#ifdef WIN32
#include <glew.h>
#endif
#include "texture.h"
#include <iostream>
#include <string>

class TextureManager
{
  std::string presetURL;
  
public:

  TextureManager(std::string _presetURL);
  void unloadTextures(const PresetOutputs::cshape_container &shapes);
  GLuint getTexture(std::string imageUrl);
  unsigned int getTextureMemorySize();
};

#endif
