/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualGraphicTypes.cpp
 *
 */

/***************************************************************************
 
 Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)
 
 
 This software is provided 'as-is', without any expressed or implied warranty. 
 In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; 
 you must not claim that you wrote the original software. 
 If you use this software in a product, an acknowledgment 
 in the product documentation would be appreciated 
 but is not required.
 
 2. Altered source versions must be plainly marked as such, 
 and must not be misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source distribution.
 
 ***************************************************************************/

#include "VisualGraphicTypes.h"

#if TARGET_OS_MAC
#include <OpenGL/gl.h>
#endif

#if TARGET_OS_WIN
#include <windows.h> // OpenGL types
#include <GL/gl.h>
#include <GL/glu.h>
#include "win/wglext.h"
#include "win/glext.h"
#endif



const int VizKit::kGL_POINTS = GL_POINTS;
const int VizKit::kGL_LINES = GL_LINES;
const int VizKit::kGL_LINE_STRIP = GL_LINE_STRIP;
const int VizKit::kGL_LINE_LOOP = GL_LINE_LOOP;
const int VizKit::kGL_TRIANGLES = GL_TRIANGLES;
const int VizKit::kGL_TRIANGLE_STRIP = GL_TRIANGLE_STRIP;
const int VizKit::kGL_TRIANGLE_FAN = GL_TRIANGLE_FAN;
const int VizKit::kGL_QUADS = GL_QUADS;
const int VizKit::kGL_QUAD_STRIP = GL_QUAD_STRIP;
const int VizKit::kGL_POLYGON = GL_POLYGON;

const int VizKit::kGL_RGBA = GL_RGBA;
const int VizKit::kGL_BGRA = GL_BGRA_EXT;
const int VizKit::kGL_RGB = GL_RGB;
const int VizKit::kGL_LUMINANCE = GL_LUMINANCE;
const int VizKit::kGL_LUMINANCE_WITH_ALPHA = GL_LUMINANCE_ALPHA;
const int VizKit::kGL_INTENSITY = GL_INTENSITY;

const int VizKit::kGL_UNSIGNED_BYTE = GL_UNSIGNED_BYTE;
const int VizKit::kGL_FLOAT = GL_FLOAT;
const int VizKit::kGL_UNSIGNED_INT_8_8_8_8_REV = GL_UNSIGNED_INT_8_8_8_8_REV;
const int VizKit::kGL_UNSIGNED_INT_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8;

const int VizKit::kGL_FRONT_COLOR_BUFFER = GL_FRONT;
const int VizKit::kGL_BACK_COLOR_BUFFER = GL_BACK;

const VizKit::VertexColor VizKit::black = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
const VizKit::VertexColor VizKit::white = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
const VizKit::VertexColor VizKit::red = {1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0};
const VizKit::VertexColor VizKit::green = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
const VizKit::VertexColor VizKit::blue = {0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0};
const VizKit::VertexColor VizKit::transparent = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

const VizKit::Coord VizKit::zeroCoord = {0.0, 0.0, 0.0};
