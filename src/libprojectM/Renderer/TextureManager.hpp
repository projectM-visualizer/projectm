#ifndef TextureManager_HPP
#define TextureManager_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "projectM-opengl.h"
#include "Texture.hpp"


class TextureManager
{
  std::string presetsURL;
  std::map<std::string, Texture*> textures;
  std::vector<Texture*> blurTextures;
  Texture * mainTexture;

  std::vector<std::string> random_textures;
  void loadTextureDir(const std::string & dirname);
  TextureSamplerDesc loadTexture(const std::string name, const std::string imageUrl);
  void ExtractTextureSettings(const std::string qualifiedName, GLint &_wrap_mode, GLint &_filter_mode, std::string & name);

public:
  TextureManager(std::string _presetsURL, const int texsizeX, const int texsizeY,
                 std::string datadir = "");
  ~TextureManager();

  // extensions are pushed into this vector when the texturemanager is initialized. This is public so when the shader engine fails at loading a texture, it can return a list of extensions it looked for.
  std::vector<std::string> extensions;
  void Clear();
  void Preload();
  TextureSamplerDesc tryLoadingTexture(const std::string name);
  TextureSamplerDesc getTexture(const std::string fullName, const GLenum defaultWrap, const GLenum defaultFilter);
  const Texture * getMainTexture() const;
  const std::vector<Texture *> & getBlurTextures() const;

  void updateMainTexture();

  TextureSamplerDesc getRandomTextureName(std::string rand_name);
  void clearRandomTextures();
};

#endif
