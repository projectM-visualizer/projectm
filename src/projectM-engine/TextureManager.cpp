#include "TextureManager.hpp"
#include "CustomShape.hpp"
#include "Common.hpp"




TextureManager::TextureManager(const std::string _presetURL): presetURL(_presetURL)
{
  ;
}

TextureManager::~TextureManager()
{
  // DeleteAllTextures();

  std::map<std::string, GLuint>::const_iterator iter;

  for(iter = textures.begin(); iter != textures.end(); iter++)
    {
      glDeleteTextures(1,&iter->second);
    }
  textures.clear();
}

void TextureManager::unloadTextures(const PresetOutputs::cshape_container &shapes)
{
  /*
   for (PresetOutputs::cshape_container::const_iterator pos = shapes.begin();
	pos != shapes.end(); ++pos) 
    {

      if( (*pos)->enabled==1)
	{
	 
	  if ( (*pos)->textured)
	    {
	      std::string imageUrl = (*pos)->getImageUrl();
	      if (imageUrl != "")
		{
		  std::string fullUrl = presetURL + "/" + imageUrl;
		  ReleaseTexture(LoadTexture(fullUrl.c_str()));		 
		}
	    }
	}
    }
  */
}


GLuint TextureManager::getTexture(const std::string imageURL)
{
 
   if (textures.find(imageURL)!= textures.end())
     {
       return textures[imageURL];
     }
   else
     {
       std::string fullURL = presetURL + PATH_SEPARATOR + imageURL;
       GLuint tex = SOIL_load_OGL_texture(
					  fullURL.c_str(),
					  SOIL_LOAD_AUTO,
					  SOIL_CREATE_NEW_ID,
					  SOIL_FLAG_POWER_OF_TWO
					  //| SOIL_FLAG_MIPMAPS
					  | SOIL_FLAG_MULTIPLY_ALPHA
					  | SOIL_FLAG_COMPRESS_TO_DXT
					  //| SOIL_FLAG_DDS_LOAD_DIRECT
					  );
       textures[imageURL]=tex;
       return tex;
       

     }
   // return LoadTexture(fullUrl.c_str(),false);
}

unsigned int TextureManager::getTextureMemorySize()
{
  return 0;//GetTotalTextureSize();
}
