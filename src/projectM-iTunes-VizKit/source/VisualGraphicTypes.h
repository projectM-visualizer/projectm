/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualGraphicTypes.h
 *
 */

/***************************************************************************

Copyright (c) 2004-2007 Heiko Wichmann (http://www.imagomat.de/vizkit)


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

#ifndef VisualGraphicTypes_h
#define VisualGraphicTypes_h

#include <vector>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


/* The common graphics related types used by VisualGraphics and VisualActorGraphics. */

namespace VizKit {

	/**
	 * A pixel contains ARGB values (alpha, red, green, blue with 8 bits for each color).
	 */
	typedef UInt32 Pixel;

	/**
	 * A coordPoint is a point relative to logical coordinate system.
	 */
	typedef struct {
		double x; /**< Horizontal position. */
		double y; /**< Vertical position. */
	} CoordPoint;

	/**
	 * A pixelPoint is a point relative to physical coordinate system.
	 */
	typedef struct {
		double x; /**< Horizontal position. */
		double y; /**< Vertical position. */
	} PixelPoint;

	/**
	 * A size relative to logical coordinate system.
	 */
	typedef struct {
		double width; /**< Horizontal size. */
		double height; /**< Vertical size. */
		double depth; /**< Size in z-dimension. */
	} CoordSize;

	/**
	 * A size relative to physical coordinate system.
	 */
	typedef struct {
		double width; /**< Horizontal size. */
		double height; /**< Vertical size. */
		double depth; /**< Size in z-dimension. */
	} PixelSize;

	/**
	 * A rectangle.
	 */
	typedef struct {
		PixelSize pixelSize; /**< The size of the rectangle in pixels. */
		CoordSize coordSize; /**< The size of the rectangle in coord dimensions. */
	} Rect;

	/**
	 * A positioned rectangle.
	 */
	typedef struct {
		PixelSize pixelSize; /**< The size of the rectangle in pixels. */
		CoordSize coordSize; /**< The size of the rectangle in coord dimensions. */
		PixelPoint pixelPoint; /**< A point measured in pixel. */
		CoordPoint coordPoint; /**< A point measured in coords. */
	} PositionedRect;

	/**
	 * ScalingAttributes specify the scaling behaviour of the texture in combination with margins.
	 */
	typedef enum {
		kNoScaling = 0x1, // 00000001
		kScalingAllowed = 0x2, // 00000010
		kPreserveAspectRatio = 0x6, // 00000110
		kNonproportional = 0xA, // 00001010
		kClippingAllowed = 0x10 // 00010000
	} ScalingBehaviour;	
	
	/**
	 * Visibility specifies the visibility.\ Two states are possible: visible and invisible.
	 */
	typedef enum {
		kVisible = 0,
		kInvisible
	} Visibility;

	/**
	 * Symbolic names for arbitrary positions in 3-dimensional space.
	 */
	typedef enum {
		kTopLeft = 0,
		kBottomLeft,
		kBottomRight,
		kTopRight		
	} SpacePosition;

	/**
	 * Stores the values of a composite color of red, green, blue and alpha.
	 */
	typedef struct {
		float r; /**< Red value. 0.0f .. 1.0f. */
		float g; /**< Green value. 0.0f .. 1.0f. */
		float b; /**< Blue value. 0.0f .. 1.0f. */
		float a; /**< Alpha value. 0.0f .. 1.0f. */
	} RGBAColor;

	/**
	 * Stores the color values used by a vertex.
	 */
	typedef RGBAColor VertexColor;
		
	/**
	 * Stores the coord position values used by a vertex.
	 */
	typedef struct {
		double xPos; /**< X-Position of vertex. */
		double yPos; /**< Y-Position of vertex. */
		double zPos; /**< Z-Position of vertex. */
	} VertexPosition;

	/** Chain of vertex positions. */
	typedef std::vector<VertexPosition*> VertexPositionChain;
	/** Pointer to chain of vertex positions. */
	typedef VertexPositionChain* VertexPositionChainRef;
	/** Constant chain of vertex positions. */
	typedef const VertexPositionChain ConstVertexPositionChain;
	/** Pointer to constant chain of vertex positions. */
	typedef const VertexPositionChainRef ConstVertexPositionChainRef;

	/** Iterator of chain of vertex positions. */
	typedef VertexPositionChain::iterator VertexPositionChainIterator;
	/** Iterator of constant chain of vertex positions. */
	typedef ConstVertexPositionChain::iterator ConstVertexPositionChainIterator;
	/** Constant iterator of chain of vertex positions. */
	typedef VertexPositionChain::const_iterator VertexPositionChainConstIterator;
	/** Constant iterator of constant chain of vertex positions. */
	typedef ConstVertexPositionChain::const_iterator ConstVertexPositionChainConstIterator;

	/**
	 * Stores the s and t coordinate values of texture positions. 
	 * Texture coordinates are referred to as s-, t-, r-, and q-coordinates to distinguish
	 * them from object coordinates (x, y, z, and w) and from elevator coordinates (u and v).
	 */
	typedef struct {
		double sPos; /**< Horizontal position of texture. */
		double tPos; /**< Vertical position of texture. */
	} TexCoordPosition;

	/**
	 * A definition of a vertex contains color information and coord position.
	 * Texture position belongs to vertex data, too.
	 */
	typedef struct {
		VertexColor vertexColor; /**< The color of the vertex. */
		VertexPosition vertexPosition; /**< The coord position of the vertex. */
		TexCoordPosition texCoordPosition; /**< Texture position at the vertex. */
	} Vertex;

	/** Chain of vertices. */
	typedef std::vector<Vertex*> VertexChain;
	/** Pointer to chain of vertices. */
	typedef VertexChain* VertexChainRef;
	/** Constant chain of vertices. */
	typedef const VertexChain ConstVertexChain;
	/** Pointer to constant chain of vertices. */
	typedef const VertexChainRef ConstVertexChainRef;

	/** Iterator of chain of vertices. */
	typedef VertexChain::iterator VertexChainIterator;
	/** Iterator of constant chain of vertices. */
	typedef ConstVertexChain::iterator ConstVertexChainIterator;
	/** Constant iterator of chain of vertices. */
	typedef VertexChain::const_iterator VertexChainConstIterator;
	/** Constant iterator of constant chain of vertices. */
	typedef ConstVertexChain::const_iterator ConstVertexChainConstIterator;

	extern const int kGL_POINTS; /**< Primitive GL type GL_POINTS (mode). */
	extern const int kGL_LINES; /**< Primitive GL type GL_LINES (mode). */
	extern const int kGL_LINE_STRIP; /**< Primitive GL type GL_LINE_STRIP (mode). */
	extern const int kGL_LINE_LOOP; /**< Primitive GL type GL_LINE_LOOP (mode). */
	extern const int kGL_TRIANGLES; /**< Primitive GL type GL_TRIANGLES (mode). */
	extern const int kGL_TRIANGLE_STRIP; /**< Primitive GL type GL_TRIANGLE_STRIP (mode). */
	extern const int kGL_TRIANGLE_FAN; /**< Primitive GL type GL_TRIANGLE_FAN (mode). */
	extern const int kGL_QUADS; /**< Primitive GL type GL_QUADS (mode). */
	extern const int kGL_QUAD_STRIP; /**< Primitive GL type GL_QUAD_STRIP (mode). */
	extern const int kGL_POLYGON; /**< Primitive GL type GL_POLYGON (mode). */
	
	extern const int kGL_RGBA; /**< Primitive GL type GL_RGBA (format). */
	extern const int kGL_BGRA; /**< Primitive GL type GL_BGRA (format). */
	extern const int kGL_RGB; /**< Primitive GL type GL_RGB (format). */
	extern const int kGL_LUMINANCE; /**< Primitive GL type GL_LUMINANCE (format). */
	extern const int kGL_LUMINANCE_WITH_ALPHA; /**< Primitive GL type GL_LUMINANCE_ALPHA (format). */
	extern const int kGL_INTENSITY; /**< Primitive GL type GL_INTENSITY (format). */
	
	extern const int kGL_UNSIGNED_BYTE; /**< Primitive GL type GL_UNSIGNED_BYTE (data type). */
	extern const int kGL_FLOAT; /**< Primitive GL type GL_FLOAT (data type). */
	extern const int kGL_UNSIGNED_INT_8_8_8_8_REV; /**< Primitive GL type GL_UNSIGNED_INT_8_8_8_8_REV (data type). */
	extern const int kGL_UNSIGNED_INT_8_8_8_8; /**< Primitive GL type GL_UNSIGNED_INT_8_8_8_8 (data type). */
	
	extern const int kGL_FRONT_COLOR_BUFFER; /**< GL enum GL_FRONT (color buffer specification for reading or drawing pixels). */
	extern const int kGL_BACK_COLOR_BUFFER; /**< GL enum GL_BACK (color buffer specification for reading or drawing pixels). */

	/**
	 * The measure of box values.
	 */
	typedef enum {
		kPixel = 0, /**< Box attribute is measured in pixels. */
		kPercent /**< Box attribute is measured in percent (of enclosing/surrounding VisualStageBox). */
	} Unit;

	/**
	 * Types of horizontal alignment.
	 */
	typedef enum {
		kLeftAligned = 0, /**< Left alignment. */
		kCenterAligned, /**< Centered alignment. */
		kRightAligned /**< Right alignment. */
	} HorizontalAlignment;

	/**
	 * Types of vertical alignment.
	 */
	typedef enum {
		kTopAligned = 0, /**< Top alignment. */
		kMiddleAligned, /**< Middle alignment. */
		kBottomAligned /**< Bottom alignment. */
	} VerticalAlignment;

	/**
	 * Types of alignment in z-dimension.
	 */
	typedef enum {
		kFrontAligned = 0, /**< Front alignment. */
		kDepthCenterAligned, /**< Centered alignment. */
		kBackAligned /**< Back alignment. */
	} DepthAlignment;

	/**
	 * Whether the Nurbs is displayed in outlined mode or filled.
	 */
	typedef enum {
		kFill = 0, /**< Fill Nurbs. */
		kOutlinePolygon /**< Draw outline. */
	} NurbsDisplayMode;
	
	/**
	 * How to blend texture data with the framebuffer data.
	 */
	typedef enum {
		kBlend = 0, /**< Blend with framebuffer data (glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA) and . */
		kReplace /**< Replace framebuffer data. */
	} BlendMode;

}

#endif /* VisualGraphicTypes_h */
