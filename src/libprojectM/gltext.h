
// Repository: https://github.com/MrVallentin/glText
// License: https://github.com/MrVallentin/glText/blob/master/LICENSE.md
//
// Date Created: September 24, 2013
// Last Modified: November 03, 2019

// In one C or C++ file, define GLT_IMPLEMENTATION prior to inclusion to create the implementation.
//   #define GLT_IMPLEMENTATION
//   #include "gltext.h"

// Copyright (c) 2018-2019 Robert Pancoast
// Support OpenGLES
// 
// Copyright (c) 2013-2018 Christian Vallentin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.

// Refrain from using any exposed macros, functions
// or structs prefixed with an underscore. As these
// are only intended for internal purposes. Which
// additionally means they can be removed, renamed
// or changed between minor updates without notice.

#ifndef GL_TEXT_H
#define GL_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__gl_h_) && !defined(__glcorearb_h_) && !defined(USE_GLES)
#	error OpenGL header must be included prior to including glText header
#endif

#include <stdlib.h> /* malloc(), calloc(), free() */
#include <string.h> /* memset(), memcpy(), strlen() */
#include <stdint.h> /* uint8_t, uint16_t, uint32_t, uint64_t */

#if (defined(_DEBUG) || defined(DEBUG)) && !defined(GLT_DEBUG)
#	define GLT_DEBUG 1
#endif

#ifdef GLT_DEBUG
#	include <assert.h> /* assert() */
#	define _GLT_ASSERT(expression) assert(expression)
#else
#	define _GLT_ASSERT(expression)
#endif

#ifdef GLT_DEBUG_PRINT
#	include <stdio.h> /* printf */
#endif

#define _GLT_STRINGIFY(str) #str
#define _GLT_STRINGIFY_TOKEN(str) _GLT_STRINGIFY(str)

#define GLT_STRINGIFY_VERSION(major, minor, patch) _GLT_STRINGIFY(major) "." _GLT_STRINGIFY(minor) "." _GLT_STRINGIFY(patch)

#define GLT_NAME "glText"

#define GLT_VERSION_MAJOR 1
#define GLT_VERSION_MINOR 1
#define GLT_VERSION_PATCH 6

#define GLT_VERSION GLT_STRINGIFY_VERSION(GLT_VERSION_MAJOR, GLT_VERSION_MINOR, GLT_VERSION_PATCH)
#define GLT_NAME_VERSION GLT_NAME " " GLT_VERSION

#define GLT_NULL 0
#define GLT_NULL_HANDLE 0

#ifdef GLT_IMPORTS
#	define GLT_API extern
#else
#	ifndef GLT_STATIC
#		define GLT_STATIC
#	endif
#	define GLT_API static
#endif

#define GLT_LEFT 0
#define GLT_TOP 0

#define GLT_CENTER 1

#define GLT_RIGHT 2
#define GLT_BOTTOM 2

static GLboolean gltInitialized = GL_FALSE;

typedef struct GLTtext GLTtext;

GLT_API GLboolean gltInit(void);
GLT_API void gltTerminate(void);

GLT_API GLTtext* gltCreateText(void);
GLT_API void gltDeleteText(GLTtext *text);
#define gltDestroyText gltDeleteText

GLT_API GLboolean gltSetText(GLTtext *text, const char *string);
GLT_API const char* gltGetText(GLTtext *text);

GLT_API void gltViewport(GLsizei width, GLsizei height);

GLT_API void gltBeginDraw();
GLT_API void gltEndDraw();

GLT_API void gltDrawText(GLTtext *text, const GLfloat mvp[16]);

GLT_API void gltDrawText2D(GLTtext *text, GLfloat x, GLfloat y, GLfloat scale);
GLT_API void gltDrawText2DAligned(GLTtext *text, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment, int verticalAlignment);

GLT_API void gltDrawText3D(GLTtext *text, GLfloat x, GLfloat y, GLfloat z, GLfloat scale, GLfloat view[16], GLfloat projection[16]);

GLT_API void gltColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
GLT_API void gltGetColor(GLfloat *r, GLfloat *g, GLfloat *b, GLfloat *a);

GLT_API GLfloat gltGetLineHeight(GLfloat scale);

GLT_API GLfloat gltGetTextWidth(const GLTtext *text, GLfloat scale);
GLT_API GLfloat gltGetTextHeight(const GLTtext *text, GLfloat scale);

GLT_API GLboolean gltIsCharacterSupported(const char c);
GLT_API GLint gltCountSupportedCharacters(const char *str);

GLT_API GLboolean gltIsCharacterDrawable(const char c);
GLT_API GLint gltCountDrawableCharacters(const char *str);

GLT_API GLint gltCountNewLines(const char *str);

// After this point everything you'll see is the
// implementation and all the internal stuff.
// Use it at your own discretion, but be warned
// that everything can have changed in the next
// commit, so be careful relying on any of it.

#ifdef GLT_IMPLEMENTATION

#define _GLT_TEXT2D_POSITION_LOCATION 0
#define _GLT_TEXT2D_TEXCOORD_LOCATION 1

#define _GLT_TEXT2D_POSITION_SIZE 2
#define _GLT_TEXT2D_TEXCOORD_SIZE 2
#define _GLT_TEXT2D_VERTEX_SIZE (_GLT_TEXT2D_POSITION_SIZE + _GLT_TEXT2D_TEXCOORD_SIZE)

#define _GLT_TEXT2D_POSITION_OFFSET 0
#define _GLT_TEXT2D_TEXCOORD_OFFSET _GLT_TEXT2D_POSITION_SIZE

#define _GLT_MAT4_INDEX(row, column) ((row) + (column) * 4)

static const char *_gltFontGlyphCharacters = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&`*#=[]\"";
#define _gltFontGlyphCount 83

#define _gltFontGlyphMinChar ' '
#define _gltFontGlyphMaxChar 'z'

static const int _gltFontGlyphHeight = 17; // Line height

typedef struct _GLTglyph {
	char c;

	GLint x, y;
	GLint w, h;

	GLfloat u1, v1;
	GLfloat u2, v2;

	GLboolean drawable;
} _GLTglyph;

typedef struct _GLTglyphdata {
	uint32_t x, y;
	uint32_t w, h;

	uint32_t marginLeft, marginTop;
	uint32_t marginRight, marginBottom;

	uint16_t dataWidth, dataHeight;
} _GLTglyphdata;

static _GLTglyph _gltFontGlyphs[_gltFontGlyphCount];

#define _gltFontGlyphLength (_gltFontGlyphMaxChar - _gltFontGlyphMinChar + 1)
static _GLTglyph _gltFontGlyphs2[_gltFontGlyphLength];

static GLuint _gltText2DShader = GLT_NULL_HANDLE;
static GLuint _gltText2DFontTexture = GLT_NULL_HANDLE;

static GLint _gltText2DShaderMVPUniformLocation = -1;
static GLint _gltText2DShaderColorUniformLocation = -1;

static GLfloat _gltText2DProjectionMatrix[16];

struct GLTtext {
	char *_text;
	GLsizei _textLength;

	GLboolean _dirty;

	GLsizei vertexCount;
	GLfloat *_vertices;

	GLuint _vao;
	GLuint _vbo;
};

GLT_API void _gltGetViewportSize(GLint *width, GLint *height);

GLT_API void _gltMat4Mult(const GLfloat lhs[16], const GLfloat rhs[16], GLfloat result[16]);

GLT_API void _gltUpdateBuffers(GLTtext *text);

GLT_API GLboolean _gltCreateText2DShader(void);
GLT_API GLboolean _gltCreateText2DFontTexture(void);

GLT_API GLTtext* gltCreateText(void)
{
	GLTtext *text = (GLTtext*)calloc(1, sizeof(GLTtext));

	_GLT_ASSERT(text);

	if (!text)
		return GLT_NULL;

	glGenVertexArrays(1, &text->_vao);
	glGenBuffers(1, &text->_vbo);

	_GLT_ASSERT(text->_vao);
	_GLT_ASSERT(text->_vbo);

	if (!text->_vao || !text->_vbo)
	{
		gltDeleteText(text);
		return GLT_NULL;
	}

	glBindVertexArray(text->_vao);

	glBindBuffer(GL_ARRAY_BUFFER, text->_vbo);

	glEnableVertexAttribArray(_GLT_TEXT2D_POSITION_LOCATION);
	glVertexAttribPointer(_GLT_TEXT2D_POSITION_LOCATION, _GLT_TEXT2D_POSITION_SIZE, GL_FLOAT, GL_FALSE, (_GLT_TEXT2D_VERTEX_SIZE * sizeof(GLfloat)), (const void*)(_GLT_TEXT2D_POSITION_OFFSET * sizeof(GLfloat)));

	glEnableVertexAttribArray(_GLT_TEXT2D_TEXCOORD_LOCATION);
	glVertexAttribPointer(_GLT_TEXT2D_TEXCOORD_LOCATION, _GLT_TEXT2D_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, (_GLT_TEXT2D_VERTEX_SIZE * sizeof(GLfloat)), (const void*)(_GLT_TEXT2D_TEXCOORD_OFFSET * sizeof(GLfloat)));

	glBindVertexArray(0);

	return text;
}

GLT_API void gltDeleteText(GLTtext *text)
{
	if (!text)
		return;

	if (text->_vao)
	{
		glDeleteVertexArrays(1, &text->_vao);
		text->_vao = GLT_NULL_HANDLE;
	}

	if (text->_vbo)
	{
		glDeleteBuffers(1, &text->_vbo);
		text->_vbo = GLT_NULL_HANDLE;
	}

	if (text->_text)
		free(text->_text);

	if (text->_vertices)
		free(text->_vertices);

	free(text);
}

GLT_API GLboolean gltSetText(GLTtext *text, const char *string)
{
	if (!text)
		return GL_FALSE;

	int strLength = 0;

	if (string)
		strLength = strlen(string);

	if (strLength)
	{
		if (text->_text)
		{
			if (strcmp(string, text->_text) == 0)
				return GL_TRUE;

			free(text->_text);
			text->_text = GLT_NULL;
		}

		text->_text = (char*)malloc((strLength + 1) * sizeof(char));

		if (text->_text)
		{
			memcpy(text->_text, string, (strLength + 1) * sizeof(char));

			text->_textLength = strLength;
			text->_dirty = GL_TRUE;

			return GL_TRUE;
		}
	}
	else
	{
		if (text->_text)
		{
			free(text->_text);
			text->_text = GLT_NULL;
		}
		else
		{
			return GL_TRUE;
		}

		text->_textLength = 0;
		text->_dirty = GL_TRUE;

		return GL_TRUE;
	}

	return GL_FALSE;
}

GLT_API const char* gltGetText(GLTtext *text)
{
	if (text && text->_text)
		return text->_text;

	return "\0";
}

GLT_API void gltViewport(GLsizei width, GLsizei height)
{
	_GLT_ASSERT(width > 0);
	_GLT_ASSERT(height > 0);

	const GLfloat left = 0.0f;
	const GLfloat right = (GLfloat)width;
	const GLfloat bottom = (GLfloat)height;
	const GLfloat top = 0.0f;
	const GLfloat zNear = -1.0f;
	const GLfloat zFar = 1.0f;

	const GLfloat projection[16] = {
		(2.0f / (right - left)), 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f / (top - bottom)), 0.0f, 0.0f,
		0.0f, 0.0f, (-2.0f / (zFar - zNear)), 0.0f,

		-((right + left) / (right - left)),
		-((top + bottom) / (top - bottom)),
		-((zFar + zNear) / (zFar - zNear)),
		1.0f,
	};

	memcpy(_gltText2DProjectionMatrix, projection, 16 * sizeof(GLfloat));
}

GLT_API void gltBeginDraw()
{
	glUseProgram(_gltText2DShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _gltText2DFontTexture);
}

GLT_API void gltEndDraw()
{

}

#define _gltDrawText() \
	glUniformMatrix4fv(_gltText2DShaderMVPUniformLocation, 1, GL_FALSE, mvp); \
	\
	glBindVertexArray(text->_vao); \
	glDrawArrays(GL_TRIANGLES, 0, text->vertexCount);

GLT_API void gltDrawText(GLTtext *text, const GLfloat mvp[16])
{
	if (!text)
		return;

	if (text->_dirty)
		_gltUpdateBuffers(text);

	if (!text->vertexCount)
		return;

	_gltDrawText();
}

GLT_API void gltDrawText2D(GLTtext *text, GLfloat x, GLfloat y, GLfloat scale)
{
	if (!text)
		return;

	if (text->_dirty)
		_gltUpdateBuffers(text);

	if (!text->vertexCount)
		return;

#ifndef GLT_MANUAL_VIEWPORT
	GLint viewportWidth, viewportHeight;
	_gltGetViewportSize(&viewportWidth, &viewportHeight);
	gltViewport(viewportWidth, viewportHeight);
#endif

	const GLfloat model[16] = {
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		x, y, 0.0f, 1.0f,
	};

	GLfloat mvp[16];
	_gltMat4Mult(_gltText2DProjectionMatrix, model, mvp);

	_gltDrawText();
}

GLT_API void gltDrawText2DAligned(GLTtext *text, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment, int verticalAlignment)
{
	if (!text)
		return;

	if (text->_dirty)
		_gltUpdateBuffers(text);

	if (!text->vertexCount)
		return;

	if (horizontalAlignment == GLT_CENTER)
		x -= gltGetTextWidth(text, scale) * 0.5f;
	else if (horizontalAlignment == GLT_RIGHT)
		x -= gltGetTextWidth(text, scale);

	if (verticalAlignment == GLT_CENTER)
		y -= gltGetTextHeight(text, scale) * 0.5f;
	else if (verticalAlignment == GLT_RIGHT)
		y -= gltGetTextHeight(text, scale);

	gltDrawText2D(text, x, y, scale);
}

GLT_API void gltDrawText3D(GLTtext *text, GLfloat x, GLfloat y, GLfloat z, GLfloat scale, GLfloat view[16], GLfloat projection[16])
{
	if (!text)
		return;

	if (text->_dirty)
		_gltUpdateBuffers(text);

	if (!text->vertexCount)
		return;

	const GLfloat model[16] = {
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, -scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		x, y + (GLfloat)_gltFontGlyphHeight * scale, z, 1.0f,
	};

	GLfloat mvp[16];
	GLfloat vp[16];

	_gltMat4Mult(projection, view, vp);
	_gltMat4Mult(vp, model, mvp);

	_gltDrawText();
}

GLT_API void gltColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glUniform4f(_gltText2DShaderColorUniformLocation, r, g, b, a);
}

GLT_API void gltGetColor(GLfloat *r, GLfloat *g, GLfloat *b, GLfloat *a)
{
	GLfloat color[4];
	glGetUniformfv(_gltText2DShader, _gltText2DShaderColorUniformLocation, color);

	if (r) (*r) = color[0];
	if (g) (*g) = color[1];
	if (b) (*b) = color[2];
	if (a) (*a) = color[3];
}

GLT_API GLfloat gltGetLineHeight(GLfloat scale)
{
	return (GLfloat)_gltFontGlyphHeight * scale;
}

GLT_API GLfloat gltGetTextWidth(const GLTtext *text, GLfloat scale)
{
	if (!text || !text->_text)
		return 0.0f;

	GLfloat maxWidth = 0.0f;
	GLfloat width = 0.0f;

	_GLTglyph glyph;

	char c;
	int i;
	for (i = 0; i < text->_textLength; i++)
	{
		c = text->_text[i];

		if ((c == '\n') || (c == '\r'))
		{
			if (width > maxWidth)
				maxWidth = width;

			width = 0.0f;

			continue;
		}

		if (!gltIsCharacterSupported(c))
		{
#ifdef GLT_UNKNOWN_CHARACTER
			c = GLT_UNKNOWN_CHARACTER;
			if (!gltIsCharacterSupported(c))
				continue;
#else
			continue;
#endif
		}

		glyph = _gltFontGlyphs2[c - _gltFontGlyphMinChar];

		width += (GLfloat)glyph.w;
	}

	if (width > maxWidth)
		maxWidth = width;

	return maxWidth * scale;
}

GLT_API GLfloat gltGetTextHeight(const GLTtext *text, GLfloat scale)
{
	if (!text || !text->_text)
		return 0.0f;

	return (GLfloat)(gltCountNewLines(text->_text) + 1) * gltGetLineHeight(scale);
}

GLT_API GLboolean gltIsCharacterSupported(const char c)
{
	if (c == '\t') return GL_TRUE;
	if (c == '\n') return GL_TRUE;
	if (c == '\r') return GL_TRUE;

	int i;
	for (i = 0; i < _gltFontGlyphCount; i++)
	{
		if (_gltFontGlyphCharacters[i] == c)
			return GL_TRUE;
	}

	return GL_FALSE;
}

GLT_API GLint gltCountSupportedCharacters(const char *str)
{
	if (!str)
		return 0;

	GLint count = 0;

	while ((*str) != '\0')
	{
		if (gltIsCharacterSupported(*str))
			count++;

		str++;
	}

	return count;
}

GLT_API GLboolean gltIsCharacterDrawable(const char c)
{
	if (c < _gltFontGlyphMinChar) return GL_FALSE;
	if (c > _gltFontGlyphMaxChar) return GL_FALSE;

	if (_gltFontGlyphs2[c - _gltFontGlyphMinChar].drawable)
		return GL_TRUE;

	return GL_FALSE;
}

GLT_API GLint gltCountDrawableCharacters(const char *str)
{
	if (!str)
		return 0;

	GLint count = 0;

	while ((*str) != '\0')
	{
		if (gltIsCharacterDrawable(*str))
			count++;

		str++;
	}

	return count;
}

GLT_API GLint gltCountNewLines(const char *str)
{
	GLint count = 0;

	while ((str = strchr(str, '\n')) != NULL)
	{
		count++;
		str++;
	}

	return count;
}

GLT_API void _gltGetViewportSize(GLint *width, GLint *height)
{
	GLint dimensions[4];
	glGetIntegerv(GL_VIEWPORT, dimensions);

	if (width) (*width) = dimensions[2];
	if (height) (*height) = dimensions[3];
}

GLT_API void _gltMat4Mult(const GLfloat lhs[16], const GLfloat rhs[16], GLfloat result[16])
{
	int c, r, i;

	for (c = 0; c < 4; c++)
	{
		for (r = 0; r < 4; r++)
		{
			result[_GLT_MAT4_INDEX(r, c)] = 0.0f;

			for (i = 0; i < 4; i++)
				result[_GLT_MAT4_INDEX(r, c)] += lhs[_GLT_MAT4_INDEX(r, i)] * rhs[_GLT_MAT4_INDEX(i, c)];
		}
	}
}

GLT_API void _gltUpdateBuffers(GLTtext *text)
{
	if (!text || !text->_dirty)
		return;

	if (text->_vertices)
	{
		text->vertexCount = 0;

		free(text->_vertices);
		text->_vertices = GLT_NULL;
	}

	if (!text->_text || !text->_textLength)
	{
		text->_dirty = GL_FALSE;
		return;
	}

	const GLsizei countDrawable = gltCountDrawableCharacters(text->_text);

	if (!countDrawable)
	{
		text->_dirty = GL_FALSE;
		return;
	}

	const GLsizei vertexCount = countDrawable * 2 * 3; // 3 vertices in a triangle, 2 triangles in a quad

	const GLsizei vertexSize = _GLT_TEXT2D_VERTEX_SIZE;
	GLfloat *vertices = (GLfloat*)malloc(vertexCount * vertexSize * sizeof(GLfloat));

	if (!vertices)
		return;

	GLsizei vertexElementIndex = 0;

	GLfloat glyphX = 0.0f;
	GLfloat glyphY = 0.0f;

	GLfloat glyphWidth;
	const GLfloat glyphHeight = (GLfloat)_gltFontGlyphHeight;

	const GLfloat glyphAdvanceX = 0.0f;
	const GLfloat glyphAdvanceY = 0.0f;

	_GLTglyph glyph;

	char c;
	int i;
	for (i = 0; i < text->_textLength; i++)
	{
		c = text->_text[i];

		if (c == '\n')
		{
			glyphX = 0.0f;
			glyphY += glyphHeight + glyphAdvanceY;

			continue;
		}
		else if (c == '\r')
		{
			glyphX = 0.0f;

			continue;
		}

		if (!gltIsCharacterSupported(c))
		{
#ifdef GLT_UNKNOWN_CHARACTER
			c = GLT_UNKNOWN_CHARACTER;
			if (!gltIsCharacterSupported(c))
				continue;
#else
			continue;
#endif
		}

		glyph = _gltFontGlyphs2[c - _gltFontGlyphMinChar];

		glyphWidth = (GLfloat)glyph.w;

		if (glyph.drawable)
		{
			vertices[vertexElementIndex++] = glyphX;
			vertices[vertexElementIndex++] = glyphY;
			vertices[vertexElementIndex++] = glyph.u1;
			vertices[vertexElementIndex++] = glyph.v1;

			vertices[vertexElementIndex++] = glyphX + glyphWidth;
			vertices[vertexElementIndex++] = glyphY + glyphHeight;
			vertices[vertexElementIndex++] = glyph.u2;
			vertices[vertexElementIndex++] = glyph.v2;

			vertices[vertexElementIndex++] = glyphX + glyphWidth;
			vertices[vertexElementIndex++] = glyphY;
			vertices[vertexElementIndex++] = glyph.u2;
			vertices[vertexElementIndex++] = glyph.v1;

			vertices[vertexElementIndex++] = glyphX;
			vertices[vertexElementIndex++] = glyphY;
			vertices[vertexElementIndex++] = glyph.u1;
			vertices[vertexElementIndex++] = glyph.v1;

			vertices[vertexElementIndex++] = glyphX;
			vertices[vertexElementIndex++] = glyphY + glyphHeight;
			vertices[vertexElementIndex++] = glyph.u1;
			vertices[vertexElementIndex++] = glyph.v2;

			vertices[vertexElementIndex++] = glyphX + glyphWidth;
			vertices[vertexElementIndex++] = glyphY + glyphHeight;
			vertices[vertexElementIndex++] = glyph.u2;
			vertices[vertexElementIndex++] = glyph.v2;
		}

		glyphX += glyphWidth + glyphAdvanceX;
	}

	text->vertexCount = vertexCount;
	text->_vertices = vertices;

	glBindBuffer(GL_ARRAY_BUFFER, text->_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * _GLT_TEXT2D_VERTEX_SIZE * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);

	text->_dirty = GL_FALSE;
}

GLT_API GLboolean gltInit(void)
{
	if (gltInitialized)
		return GL_TRUE;

	if (!_gltCreateText2DShader())
		return GL_FALSE;

	if (!_gltCreateText2DFontTexture())
		return GL_FALSE;

	gltInitialized = GL_TRUE;
	return GL_TRUE;
}

GLT_API void gltTerminate(void)
{
	if (_gltText2DShader != GLT_NULL_HANDLE)
	{
		glDeleteProgram(_gltText2DShader);
		_gltText2DShader = GLT_NULL_HANDLE;
	}

	if (_gltText2DFontTexture != GLT_NULL_HANDLE)
	{
		glDeleteTextures(1, &_gltText2DFontTexture);
		_gltText2DFontTexture = GLT_NULL_HANDLE;
	}

	gltInitialized = GL_FALSE;
}
	
#if USE_GLES
#define GLSL_VERSION "300 es"
#else
#define GLSL_VERSION "330"
#endif
	
static const GLchar* _gltText2DVertexShaderSource =
"#version "
GLSL_VERSION
"\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"\n"
"uniform mat4 mvp;\n"
"\n"
"out vec2 fTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"	fTexCoord = texCoord;\n"
"	\n"
"	gl_Position = mvp * vec4(position, 0.0, 1.0);\n"
"}\n";

static const GLchar* _gltText2DFragmentShaderSource =
"#version "
GLSL_VERSION
"\n"
#if USE_GLES
"precision mediump float;\n"
"\n"
#endif
"out vec4 fragColor;\n"
"\n"
"in vec2 fTexCoord;\n"
"\n"
"uniform sampler2D diffuse;\n"
"\n"
"uniform vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"	fragColor = texture(diffuse, fTexCoord) * color;\n"
"}\n";

GLT_API GLboolean _gltCreateText2DShader(void)
{
	GLuint vertexShader, fragmentShader;
	GLint compileStatus, linkStatus;

#ifdef GLT_DEBUG_PRINT
	GLint infoLogLength;
	GLsizei infoLogSize;
	GLchar *infoLog;
#endif

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &_gltText2DVertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
#ifdef GLT_DEBUG_PRINT
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);

		// If no information log exists 0 is returned or 1 for a
		// string only containing the null termination char.
		if (infoLogLength > 1)
		{
			infoLogSize = infoLogLength * sizeof(GLchar);
			infoLog = (GLchar*)malloc(infoLogSize);

			glGetShaderInfoLog(vertexShader, infoLogSize, NULL, infoLog);

			printf("Vertex Shader #%u <Info Log>:\n%s\n", vertexShader, infoLog);

			free(infoLog);
		}
#endif

		glDeleteShader(vertexShader);
		gltTerminate();

#ifdef GLT_DEBUG
		_GLT_ASSERT(compileStatus == GL_TRUE);
		return GL_FALSE;
#else
		return GL_FALSE;
#endif
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &_gltText2DFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
#ifdef GLT_DEBUG_PRINT
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);

		// If no information log exists 0 is returned or 1 for a
		// string only containing the null termination char.
		if (infoLogLength > 1)
		{
			infoLogSize = infoLogLength * sizeof(GLchar);
			infoLog = (GLchar*)malloc(infoLogSize);

			glGetShaderInfoLog(fragmentShader, infoLogSize, NULL, infoLog);

			printf("Fragment Shader #%u <Info Log>:\n%s\n", fragmentShader, infoLog);

			free(infoLog);
		}
#endif

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		gltTerminate();

#ifdef GLT_DEBUG
		_GLT_ASSERT(compileStatus == GL_TRUE);
		return GL_FALSE;
#else
		return GL_FALSE;
#endif
	}

	_gltText2DShader = glCreateProgram();

	glAttachShader(_gltText2DShader, vertexShader);
	glAttachShader(_gltText2DShader, fragmentShader);

	glBindAttribLocation(_gltText2DShader, _GLT_TEXT2D_POSITION_LOCATION, "position");
	glBindAttribLocation(_gltText2DShader, _GLT_TEXT2D_TEXCOORD_LOCATION, "texCoord");

#if !defined(USE_GLES)
	glBindFragDataLocation(_gltText2DShader, 0, "fragColor");
#endif

	glLinkProgram(_gltText2DShader);

	glDetachShader(_gltText2DShader, vertexShader);
	glDeleteShader(vertexShader);

	glDetachShader(_gltText2DShader, fragmentShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(_gltText2DShader, GL_LINK_STATUS, &linkStatus);

	if (linkStatus != GL_TRUE)
	{
#ifdef GLT_DEBUG_PRINT
		glGetProgramiv(_gltText2DShader, GL_INFO_LOG_LENGTH, &infoLogLength);

		// If no information log exists 0 is returned or 1 for a
		// string only containing the null termination char.
		if (infoLogLength > 1)
		{
			infoLogSize = infoLogLength * sizeof(GLchar);
			infoLog = (GLchar*)malloc(infoLogSize);

			glGetProgramInfoLog(_gltText2DShader, infoLogSize, NULL, infoLog);

			printf("Program #%u <Info Log>:\n%s\n", _gltText2DShader, infoLog);

			free(infoLog);
		}
#endif

		gltTerminate();

#ifdef GLT_DEBUG
		_GLT_ASSERT(linkStatus == GL_TRUE);
		return GL_FALSE;
#else
		return GL_FALSE;
#endif
	}

	glUseProgram(_gltText2DShader);

	_gltText2DShaderMVPUniformLocation = glGetUniformLocation(_gltText2DShader, "mvp");
	_gltText2DShaderColorUniformLocation = glGetUniformLocation(_gltText2DShader, "color");

	glUniform1i(glGetUniformLocation(_gltText2DShader, "diffuse"), 0);

	glUseProgram(0);

	return GL_TRUE;
}

static const uint64_t _gltFontGlyphRects[_gltFontGlyphCount] = {
	0x1100040000, 0x304090004, 0x30209000D, 0x304090016, 0x30209001F, 0x304090028, 0x302090031, 0x409003A,
	0x302090043, 0x30109004C, 0x1080055, 0x30209005D, 0x302090066, 0x3040A006F, 0x304090079, 0x304090082,
	0x409008B, 0x4090094, 0x30409009D, 0x3040900A6, 0x3020900AF, 0x3040900B8, 0x3040900C1, 0x3040A00CA,
	0x3040900D4, 0x40A00DD, 0x3040900E7, 0x3020900F0, 0x3020900F9, 0x302090102, 0x30209010B, 0x302090114,
	0x30209011D, 0x302090126, 0x30209012F, 0x302070138, 0x30209013F, 0x302090148, 0x302090151, 0x3020A015A,
	0x3020A0164, 0x30209016E, 0x302090177, 0x102090180, 0x302090189, 0x302090192, 0x30209019B, 0x3020901A4,
	0x3020901AD, 0x3020A01B6, 0x3020901C0, 0x3020901C9, 0x3020901D2, 0x3020901DB, 0x3020901E4, 0x3020901ED,
	0x3020901F6, 0x3020A01FF, 0x302090209, 0x302090212, 0x30209021B, 0x302090224, 0x30209022D, 0x309060236,
	0x10906023C, 0x302070242, 0x302090249, 0x706090252, 0x50409025B, 0x202090264, 0x10207026D, 0x102070274,
	0x30406027B, 0x104060281, 0x2010B0287, 0x3020A0292, 0xB0007029C, 0x5040A02AA, 0x3020A02B4, 0x6050902BE,
	0x20702C7, 0x20702CE, 0xB010902D5,
};

#define _GLT_FONT_GLYPH_DATA_TYPE uint64_t
#define _GLT_FONT_PIXEL_SIZE_BITS 2

#define _gltFontGlyphDataCount 387

static const _GLT_FONT_GLYPH_DATA_TYPE _gltFontGlyphData[_gltFontGlyphDataCount] = {
	0x551695416A901554, 0x569695A5A56AA55A, 0x555554155545AA9, 0x916AA41569005A40, 0xA5A569695A5A5696, 0x51555556AA569695, 0x696916A941554155, 0x69155A55569555A5,
	0x15541555456A9569, 0xA9569545A4005500, 0x569695A5A569695A, 0x5545AA9569695A5A, 0x916A941554555415, 0x55A56AA95A5A5696, 0x40555416A9555695, 0x55A45AA505550155,
	0xA55AAA455A555691, 0x169005A45569155, 0xA945554015400554, 0x569695A5A569695A, 0x9545AA9569695A5A, 0x4555555AA95A5556, 0x55A4016900154555, 0xA569695A5A45AA90,
	0x69695A5A569695A5, 0x9001541555455555, 0x5AA4155505A4016, 0xA40169005A501695, 0x155555AAA4569505, 0x5A405A4015405555, 0x5A505A545AA45554, 0x5A405A405A405A40,
	0x555556A95A555A40, 0x569005A400551554, 0x9569A569695A5A45, 0xA5A56969169A556A, 0xA405555555155555, 0x169005A50169505A, 0x9505A40169005A40, 0x5555155555AAA456,
	0x95A66916AA905555, 0x695A6695A6695A66, 0x154555555A5695A6, 0x5696916AA4155555, 0x9695A5A569695A5A, 0x45555155555A5A56, 0xA5A5696916A94155, 0x9569695A5A569695,
	0x155515541555456A, 0x695A5A5696916AA4, 0x56AA569695A5A569, 0x5540169155A55569, 0x56AA515550015400, 0x9695A5A569695A5A, 0x5A5516AA55A5A56, 0x500155005A401695,
	0xA56A695A5A455555, 0x169015A55569556, 0x54005500155005A4, 0x555A555695AA9455, 0xAA569555A5505AA5, 0x15415551555556, 0x5A55AAA455A50169, 0x40169005A4556915,
	0x550155505AA5055A, 0xA569695A5A455555, 0x69695A5A569695A5, 0x555554155545AA95, 0x5A5A569695A5A455, 0xA515AA55A5A56969, 0x1545505500555055, 0x95A6695A6695A569,
	0xA4569A55A6695A66, 0x5551555015554569, 0x456A9569695A5A45, 0xA5A5696916A95569, 0x4155545555155555, 0xA45A5A45A5A45A5A, 0xA945A5A45A5A45A5, 0x56A915A555695056,
	0x4555501554055551, 0x6945695169555AAA, 0x55AAA45569156955, 0x5A50055055551555, 0xA569695A5A45AA50, 0x69695A5A56AA95A5, 0x555555155555A5A5, 0x5A5A5696916AA415,
	0x5A5696956AA56969, 0x1555556AA569695A, 0x96916A9415541555, 0x9555A555695A5A56, 0x6A9569695A5A4556, 0xA405551554155545, 0x69695A5A45A6905A, 0x695A5A569695A5A5,
	0x5550555555AA55A, 0x5A555695AAA45555, 0x4556916AA4156955, 0x5555AAA45569155A, 0x95AAA45555555515, 0x6AA41569555A5556, 0xA40169155A455691, 0x1554005500155005,
	0x695A5A5696916A94, 0x5A5A56A69555A555, 0x54155545AA956969, 0x569695A5A4555555, 0x9695AAA569695A5A, 0x55A5A569695A5A56, 0x55AA455555551555, 0x5A416905A456915A,
	0x515555AA45A51690, 0x169005A400550055, 0x9555A40169005A40, 0x456A9569695A5A56, 0xA5A4555515541555, 0xA55A69569A569695, 0x6969169A45A6915A, 0x555555155555A5A5,
	0x5A40169005A400, 0x5A40169005A40169, 0x155555AAA4556900, 0x695A569154555555, 0x6695A6695A9A95A5, 0xA5695A5695A6695A, 0x55154555555A5695, 0x95A5695A56915455,
	0x695AA695A6A95A5A, 0x5695A5695A5695A9, 0x155455154555555A, 0x695A5A5696916A94, 0x5A5A569695A5A569, 0x541555456A956969, 0x5696916AA4155515, 0x56956AA569695A5A,
	0x5005A40169155A55, 0x6A94155400550015, 0xA569695A5A569691, 0x69695A5A569695A5, 0x5A5415A5456A95, 0x16AA415555500155, 0xAA569695A5A56969, 0x569695A5A55A6956,
	0x5545555155555A5A, 0x5555A5696916A941, 0xA5545A5005A5155A, 0x41555456A9569695, 0x56955AAA45555155, 0x9005A40169055A51, 0x5A40169005A4016, 0x5A45555055001550,
	0x569695A5A569695A, 0x9695A5A569695A5A, 0x515541555456A956, 0xA5A569695A5A4555, 0xA569695A5A569695, 0x555055A515AA55A5, 0x95A5691545505500, 0x695A6695A5695A56,
	0x9A4569A55A6695A6, 0x555015554169A456, 0x9569695A5A455551, 0x5A6515A515694566, 0x555A5A569695A5A4, 0x5A5A455555555155, 0xA9569695A5A56969, 0x169015A41569456,
	0x55505500155005A4, 0x5A55169555AAA45, 0x55A455A555A515A5, 0x5155555AAA455690, 0x696916A941554555, 0xA95A5A56A695A9A5, 0x56A9569695A6A569, 0x9401540155415554,
	0x5A5516AA45A9516, 0xA40169005A401695, 0x4154005540169005, 0xA5A5696916A94155, 0x9556945695169555, 0x55555AAA45569156, 0x6916A94155455551, 0x56A5169555A5A569,
	0xA9569695A5A56955, 0x15415541555456, 0x4169A4055A4005A4, 0xA916969169A5169A, 0x50056954569555AA, 0x5AAA455551540015, 0xAA41569555A55569, 0x55A555A551695516,
	0x55005550555555AA, 0x915694569416A401, 0xA5A569695A5A45AA, 0x41555456A9569695, 0x69555AAA45555155, 0x9415A415A5056951, 0x169015A40569056, 0xA941554015400554,
	0x569A95A5A5696916, 0x9695A5A56A6956A9, 0x415541555456A956, 0xA5A5696916A94155, 0x516AA55A5A569695, 0x155415A915694569, 0x555A95A915505540, 0x5A55A95A91555545,
	0x1694154154555569, 0xA456956A95AA56A9, 0x55416905A415515, 0x696916A941554154, 0x9055A515A555A5A5, 0x5A4016900554056, 0xAA45555055001550, 0x5505555155555A,
	0x6955AAA4569505A4, 0x5500155055A515, 0x690169405A400550, 0x90569415A415A505, 0x569015A415A5056, 0x6941540015400554, 0xA456915A55A55691, 0x16905A505A416905,
	0x6901555405541694, 0x16905A505A416941, 0x6955A45A516905A4, 0xA455415415545695, 0x6A45555515556A56, 0x56A45555515556A5, 0xA56A45555515556A, 0x5505515555A56956,
	0x690569A4016A5001, 0x4056954169A9459A, 0x416A690156941569, 0x15A9505A695169A6, 0x4015505540055540, 0x94169A4169A405A9, 0x5A56A9A4555A415A, 0x555169A955A5A55A,
	0x6945A90555555415, 0x1555154055416941, 0x56AAA456A545A690, 0x40555515A69156A5, 0x6945A69015550555, 0xA6915A6956AAA45A, 0x5A6956AAA45A6955, 0x455540555515A691,
	0xAA9555556AA91555, 0xA915555554555556, 0x416905A556955A56, 0x5A416905A416905A, 0x555515555AA45690, 0x6905A516955AA455, 0x416905A416905A41, 0x55556A95A556905A,
	0xA5A5696915555554, 0x5555155555,
};

GLT_API GLboolean _gltCreateText2DFontTexture(void)
{
	if (gltInitialized)
		return GL_TRUE;

	memset(_gltFontGlyphs, 0, _gltFontGlyphCount * sizeof(_GLTglyph));
	memset(_gltFontGlyphs2, 0, _gltFontGlyphLength * sizeof(_GLTglyph));

	GLsizei texWidth = 0;
	GLsizei texHeight = 0;

	GLsizei drawableGlyphCount = 0;

	_GLTglyphdata *glyphsData = (_GLTglyphdata*)calloc(_gltFontGlyphCount, sizeof(_GLTglyphdata));

	uint64_t glyphPacked;
	uint32_t glyphMarginPacked;

	uint16_t glyphX, glyphY, glyphWidth, glyphHeight;
	uint16_t glyphMarginLeft, glyphMarginTop, glyphMarginRight, glyphMarginBottom;

	uint16_t glyphDataWidth, glyphDataHeight;

	glyphMarginLeft = 0;
	glyphMarginRight = 0;

	_GLTglyph *glyph;
	_GLTglyphdata *glyphData;

	char c;
	int i;
	int x, y;

	for (i = 0; i < _gltFontGlyphCount; i++)
	{
		c = _gltFontGlyphCharacters[i];

		glyphPacked = _gltFontGlyphRects[i];

		glyphX = (glyphPacked >> (uint64_t)(8 * 0)) & 0xFFFF;
		glyphWidth = (glyphPacked >> (uint64_t)(8 * 2)) & 0xFF;

		glyphY = 0;
		glyphHeight = _gltFontGlyphHeight;

		glyphMarginPacked = (glyphPacked >> (uint64_t)(8 * 3)) & 0xFFFF;

		glyphMarginTop = (glyphMarginPacked >> (uint16_t)(0)) & 0xFF;
		glyphMarginBottom = (glyphMarginPacked >> (uint16_t)(8)) & 0xFF;

		glyphDataWidth = glyphWidth;
		glyphDataHeight = glyphHeight - (glyphMarginTop + glyphMarginBottom);

		glyph = &_gltFontGlyphs[i];

		glyph->c = c;

		glyph->x = glyphX;
		glyph->y = glyphY;
		glyph->w = glyphWidth;
		glyph->h = glyphHeight;

		glyphData = &glyphsData[i];

		glyphData->x = glyphX;
		glyphData->y = glyphY;
		glyphData->w = glyphWidth;
		glyphData->h = glyphHeight;

		glyphData->marginLeft = glyphMarginLeft;
		glyphData->marginTop = glyphMarginTop;
		glyphData->marginRight = glyphMarginRight;
		glyphData->marginBottom = glyphMarginBottom;

		glyphData->dataWidth = glyphDataWidth;
		glyphData->dataHeight = glyphDataHeight;

		glyph->drawable = GL_FALSE;

		if ((glyphDataWidth > 0) && (glyphDataHeight > 0))
			glyph->drawable = GL_TRUE;

		if (glyph->drawable)
		{
			drawableGlyphCount++;

			texWidth += (GLsizei)glyphWidth;

			if (texHeight < glyphHeight)
				texHeight = glyphHeight;
		}
	}

	const GLsizei textureGlyphPadding = 1; // amount of pixels added around the whole bitmap texture
	const GLsizei textureGlyphSpacing = 1; // amount of pixels added between each glyph on the final bitmap texture

	texWidth += textureGlyphSpacing * (drawableGlyphCount - 1);

	texWidth += textureGlyphPadding * 2;
	texHeight += textureGlyphPadding * 2;

	const GLsizei texAreaSize = texWidth * texHeight;

	const GLsizei texPixelComponents = 4; // R, G, B, A
	GLubyte *texData = (GLubyte*)malloc(texAreaSize * texPixelComponents * sizeof(GLubyte));

	GLsizei texPixelIndex;

	for (texPixelIndex = 0; texPixelIndex < (texAreaSize * texPixelComponents); texPixelIndex++)
		texData[texPixelIndex] = 0;

#define _GLT_TEX_PIXEL_INDEX(x, y) ((y) * texWidth * texPixelComponents + (x) * texPixelComponents)

#define _GLT_TEX_SET_PIXEL(x, y, r, g, b, a) { \
		texPixelIndex = _GLT_TEX_PIXEL_INDEX(x, y); \
		texData[texPixelIndex + 0] = r; \
		texData[texPixelIndex + 1] = g; \
		texData[texPixelIndex + 2] = b; \
		texData[texPixelIndex + 3] = a; \
	}

	const int glyphDataTypeSizeBits = sizeof(_GLT_FONT_GLYPH_DATA_TYPE) * 8; // 8 bits in a byte

	int data0Index = 0;
	int data1Index = 0;

	int bit0Index = 0;
	int bit1Index = 1;

	char c0 = '0';
	char c1 = '0';

	GLuint r, g, b, a;

	GLfloat u1, v1;
	GLfloat u2, v2;

	GLsizei texX = 0;
	GLsizei texY = 0;

	texX += textureGlyphPadding;

	for (i = 0; i < _gltFontGlyphCount; i++)
	{
		glyph = &_gltFontGlyphs[i];
		glyphData = &glyphsData[i];

		if (!glyph->drawable)
			continue;

		c = glyph->c;

		glyphX = glyph->x;
		glyphY = glyph->y;
		glyphWidth = glyph->w;
		glyphHeight = glyph->h;

		glyphMarginLeft = glyphData->marginLeft;
		glyphMarginTop = glyphData->marginTop;
		glyphMarginRight = glyphData->marginRight;
		glyphMarginBottom = glyphData->marginBottom;

		glyphDataWidth = glyphData->dataWidth;
		glyphDataHeight = glyphData->dataHeight;

		texY = textureGlyphPadding;

		u1 = (GLfloat)texX / (GLfloat)texWidth;
		v1 = (GLfloat)texY / (GLfloat)texHeight;

		u2 = (GLfloat)glyphWidth / (GLfloat)texWidth;
		v2 = (GLfloat)glyphHeight / (GLfloat)texHeight;

		glyph->u1 = u1;
		glyph->v1 = v1;

		glyph->u2 = u1 + u2;
		glyph->v2 = v1 + v2;

		texX += glyphMarginLeft;
		texY += glyphMarginTop;

		for (y = 0; y < glyphDataHeight; y++)
		{
			for (x = 0; x < glyphDataWidth; x++)
			{
				c0 = (_gltFontGlyphData[data0Index] >> bit0Index) & 1;
				c1 = (_gltFontGlyphData[data1Index] >> bit1Index) & 1;

				if ((c0 == 0) && (c1 == 0))
				{
					r = 0;
					g = 0;
					b = 0;
					a = 0;
				}
				else if ((c0 == 0) && (c1 == 1))
				{
					r = 255;
					g = 255;
					b = 255;
					a = 255;
				}
				else if ((c0 == 1) && (c1 == 0))
				{
					r = 0;
					g = 0;
					b = 0;
					a = 255;
				}

				_GLT_TEX_SET_PIXEL(texX + x, texY + y, r, g, b, a);

				bit0Index += _GLT_FONT_PIXEL_SIZE_BITS;
				bit1Index += _GLT_FONT_PIXEL_SIZE_BITS;

				if (bit0Index >= glyphDataTypeSizeBits)
				{
					bit0Index = bit0Index % glyphDataTypeSizeBits;
					data0Index++;
				}

				if (bit1Index >= glyphDataTypeSizeBits)
				{
					bit1Index = bit1Index % glyphDataTypeSizeBits;
					data1Index++;
				}
			}
		}

		texX += glyphDataWidth;
		texY += glyphDataHeight;

		texX += glyphMarginRight;
		texX += textureGlyphSpacing;
	}

	for (i = 0; i < _gltFontGlyphCount; i++)
	{
		glyph = &_gltFontGlyphs[i];

		_gltFontGlyphs2[glyph->c - _gltFontGlyphMinChar] = *glyph;
	}

#undef _GLT_TEX_PIXEL_INDEX
#undef _GLT_TEX_SET_PIXEL

	glGenTextures(1, &_gltText2DFontTexture);
	glBindTexture(GL_TEXTURE_2D, _gltText2DFontTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	free(texData);

	free(glyphsData);

	return GL_TRUE;
}

#endif

#ifdef __cplusplus
}
#endif

#endif