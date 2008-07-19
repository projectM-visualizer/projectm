#ifndef TextureManager_HPP
#define TextureManager_HPP

#include <iostream>
#include <string>
#include <map>

class TextureManager
{
  std::string presetURL;
  std::map<std::string,unsigned int> textures;
  std::map<std::string,unsigned int> heights;
  std::map<std::string,unsigned int> widths;
public:
  ~TextureManager();
  TextureManager(std::string _presetURL);
  //void unloadTextures(const PresetOutputs::cshape_container &shapes);
  void Clear();
  void Preload();
  unsigned int getTexture(const std::string imageUrl);
  unsigned int getTextureMemorySize();
  int getTextureWidth(const std::string imageUrl);
  int getTextureHeight(const std::string imageUrl);
  void setTexture(const std::string name, const unsigned int texId, const int width, const int height);
};

#endif
