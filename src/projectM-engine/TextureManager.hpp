#ifndef TextureManager_HPP
#define TextureManager_HPP

#include "PresetFrameIO.hpp"
#include <GL/gl.h>
#include "texture.h"
#include <iostream>
#include <string>

class TextureManager
{
  std::string presetURL;
  
public:

  TextureManager(std::string _presetURL);
  void loadTextures(PresetOutputs::cshape_container &shapes);
  GLuint getTexture(std::string imageUrl);

};

#endif
