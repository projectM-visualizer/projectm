//--------------------------------------------------------------------------
/// \file	texture.h
/// \author	Rob Bateman [robthebloke@hotmail.com]
/// \date	7-3-2005
/// \brief	This header defines some functions 
/// 
//--------------------------------------------------------------------------

#ifndef __GL_TEXTURE__H__
#define __GL_TEXTURE__H__

//--------------------------------------------------------------------------
/// \brief	This function loads the specified image file and returns the 
///			openGL texture object for it. Only tga, bmp and pcx images are
///			supported.
/// \param	filename	-	the image file to use for the texture
/// \return	compressed	-	if true DXT compression will be used.
///
unsigned int LoadTexture(const char* filename,bool compressed=true);

//--------------------------------------------------------------------------
/// \brief	This function will release the specified texture (as long as 
///			it's not used elsewhere.
/// \param	the OpenGL texture object
///
void ReleaseTexture(unsigned int);

//--------------------------------------------------------------------------
/// \brief	This function deletes all currently loaded textures
///
void DeleteAllTextures();

//--------------------------------------------------------------------------
/// \brief	This function returns the amount of memory used to store the 
///			specified openGL texture. 
/// \param	idx	-	the openGL texture object
/// \return	the amount of memory used (in bytes)
///
unsigned int GetTextureSize(unsigned int);

//--------------------------------------------------------------------------
/// \brief	This returns the amount of memory used to store all textures 
///			currently loaded. The size returned is in bytes
/// \return	the total amount of memory used (in bytes)
///
unsigned int GetTotalTextureSize();

#endif
