
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "bmpLoader.h"
#include "pcxLoader.h"
#include "tgaLoader.h"
#include <iostream>
#include <string>
#include <map>


// openGL extensions if not present, define them
#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
	#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT			0x83F0
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT		0x83F1
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT		0x83F2
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT		0x83F3
#endif

#ifndef GL_TEXTURE_COMPRESSED_IMAGE_SIZE
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
#endif

//--------------------------------------------------------------------------
/// \brief	This function takes raw image data and converts it to an openGL
///			texture. 
/// \param	Format		-	GL_RGB,GL_RGBA,GL_ALPHA
/// \param	pixels		-	the pixel data
/// \param	w			-	the image width
/// \param	h			-	the image height
///	\param	compressed	-	true for DXT compression
///

unsigned int MakeGlTexture(GLenum Format,const unsigned char *pixels,unsigned int w,unsigned int h,bool compressed)
{
	unsigned int texObject;

	glGenTextures(1,&texObject);

	glPixelStorei (GL_UNPACK_ALIGNMENT, 0);
	glBindTexture (GL_TEXTURE_2D, texObject);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if(compressed) {
		switch(Format) {
		//
		case GL_RGB:
  			glTexImage2D (GL_TEXTURE_2D,0,GL_COMPRESSED_RGB_S3TC_DXT1_EXT,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,pixels);
			break;

		//
		case GL_RGBA:
  			glTexImage2D (GL_TEXTURE_2D,0,GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
			break;

		//
		case GL_ALPHA:
			glTexImage2D (GL_TEXTURE_2D,0,GL_ALPHA,w,h,0,GL_ALPHA,GL_UNSIGNED_BYTE,pixels);
			break;

		default: ;
		}
	}
	else {
		glTexImage2D (GL_TEXTURE_2D,0,Format,w,h,0,Format,GL_UNSIGNED_BYTE,pixels);
	}

	return texObject;

}

/// this structure is used to reference count OpenGL textures
struct TexRef {

	/// the openGL texture object
	unsigned int idx;

	/// the reference count
	unsigned int ref;

	/// the size of the loaded texture
	unsigned int size;

	/// ctor
	TexRef(unsigned id_,unsigned sz) : idx(id_),ref(1),size(sz) {}
	/// ctor
	TexRef() : idx(0),ref(0),size(0) {}
	/// ctor
	TexRef(const TexRef& tr) : idx(tr.idx),ref(tr.ref),size(tr.size) {}
};

/// a global map of all currently loaded textures
std::map<std::string,TexRef> g_Textures;

//--------------------------------------------------------------------------
/// \brief	This function loads the specified image file and returns the 
///			openGL texture object for it. Only tga, bmp and pcx images are
///			supported.
/// \param	filename	-	the image file to use for the texture
/// \return	compressed	-	if true DXT compression will be used.
///
unsigned int LoadTexture(const char* filename,bool compressed) {

	// check to see if file is already loaded.
	std::map<std::string,TexRef>::iterator it = g_Textures.find(filename);
	if ( it!=g_Textures.end()) {
		it->second.ref++;
		return it->second.idx;
	}

	// if not, try to load the file.
	unsigned int w=0,h=0,bpp=0;
	unsigned char* pixels=0;

	int len = static_cast<int>(strlen(filename));

	// load a bmp
	if( (filename[len-3] == 'b' || filename[len-3] == 'B') &&
		(filename[len-2] == 'm' || filename[len-2] == 'M') &&
		(filename[len-1] == 'p' || filename[len-1] == 'P') ) {
		if(!LoadBmpImage(filename,&pixels,&w,&h,&bpp))
			return 0;
	}
	else
	// load a pcx
	if( (filename[len-3] == 'p' || filename[len-3] == 'P') &&
		(filename[len-2] == 'c' || filename[len-2] == 'C') &&
		(filename[len-1] == 'x' || filename[len-1] == 'X') ) {
		if(!LoadPcxImage(filename,&pixels,&w,&h,&bpp))
			return 0;
	}
	else
	// load a tga 
	if( (filename[len-3] == 't' || filename[len-3] == 'T') &&
		(filename[len-2] == 'g' || filename[len-2] == 'G') &&
		(filename[len-1] == 'a' || filename[len-1] == 'A') ) {
		if(!LoadTgaImage(filename,&pixels,&w,&h,&bpp))
			return 0;
	}
	else {
		std::cerr << "Unsupported image format\n";
		return 0;
	}

	// generat the correct texture type for the image
	unsigned int tex_object;
	switch(bpp) {
	case 1:
		tex_object = MakeGlTexture(GL_ALPHA,pixels,w,h,compressed);
		break;
	case 2:
		break;
	case 3:
		tex_object = MakeGlTexture(GL_RGB,pixels,w,h,compressed);
		break;
	case 4:
		tex_object = MakeGlTexture(GL_RGBA,pixels,w,h,compressed);
		break;
	default:
		break;
	}

	int data_size=0;
	if(compressed) {
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE,&data_size);
	}
	else {
		data_size = bpp*w*h;
	}


	// delete the pixel data because we no longer need it
	free(pixels);
	
	// insert the texture into a map to keep track of it
	g_Textures.insert( std::make_pair( std::string(filename), TexRef(tex_object,data_size) ) );

	// return GL texture object
	return tex_object;

}

//--------------------------------------------------------------------------
/// \brief	This function will release the specified texture (as long as 
///			it's not used elsewhere.
/// \param	the OpenGL texture object
///
void ReleaseTexture(unsigned int idx) {

	// iterate through the textures to find the requested texture
	std::map<std::string,TexRef>::iterator it=g_Textures.begin();
	for( ; it != g_Textures.end(); ++it ) {

		// when found...
		if(idx == it->second.idx) {

			// if ref count is zero ...
			if(--it->second.ref == 0) {

				// delete texture
				glDeleteTextures(1,&(it->second.idx));
				g_Textures.erase(it);
			}
			return;
		}
	}
}

//--------------------------------------------------------------------------
/// \brief	This function deletes all currently loaded textures
///
void DeleteAllTextures() {

	std::map<std::string,TexRef>::iterator it=g_Textures.begin();
	for( ; it != g_Textures.end(); ++it ) {
		glDeleteTextures(1,&it->second.idx);
	}
	g_Textures.clear();
}


//--------------------------------------------------------------------------
/// \brief	This function returns the amount of memory used to store the 
///			specified openGL texture. 
/// \param	idx	-	the openGL texture object
/// \return	the amount of memory used (in bytes)
///
unsigned int GetTextureSize(unsigned int idx) {

	// iterate through the textures to find the requested texture
	std::map<std::string,TexRef>::iterator it=g_Textures.begin();
	for( ; it != g_Textures.end(); ++it ) {

		// when found...
		if(idx == it->second.idx) {

			return it->second.size;
		}
	}
	return 0;
}

//--------------------------------------------------------------------------
/// \brief	This returns the amount of memory used to store all textures 
///			currently loaded. The size returned is in bytes
/// \return	the total amount of memory used (in bytes)
///
unsigned int GetTotalTextureSize() {
	unsigned int sz=0;
	// iterate through the textures to find the requested texture
	std::map<std::string,TexRef>::iterator it=g_Textures.begin();
	for( ; it != g_Textures.end(); ++it ) {
		sz += it->second.size;		
	}
	return sz;
}

