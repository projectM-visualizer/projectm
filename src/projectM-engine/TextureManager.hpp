#ifndef TextureManager_HPP
#define TextureManager_HPP

#include <iostream>
#include <string>
#include <map>

class TextureManager
{
  std::string presetURL;
  std::map<std::string,unsigned int> textures;
public:
  ~TextureManager();
  TextureManager(std::string _presetURL);
  //void unloadTextures(const PresetOutputs::cshape_container &shapes);
  void Clear();
  void Preload();
  unsigned int getTexture(std::string imageUrl);
  unsigned int getTextureMemorySize();
};

#endif
