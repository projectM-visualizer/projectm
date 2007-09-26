#include "TextureManager.hpp"
#include "CustomShape.hpp"
#include "Common.hpp"
#include <map>

TextureManager::TextureManager(const std::string _presetURL): presetURL(_presetURL)
{
  ;
}


void TextureManager::unloadTextures(const PresetOutputs::cshape_container &shapes)
{
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
}


GLuint TextureManager::getTexture(const std::string imageUrl)
{
   std::string fullUrl = presetURL + PATH_SEPARATOR + imageUrl;
   return LoadTexture(fullUrl.c_str());
}

unsigned int TextureManager::getTextureMemorySize()
{
  return GetTotalTextureSize();
}
