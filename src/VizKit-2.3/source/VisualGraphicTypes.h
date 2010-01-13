/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualGraphicTypes.h
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

#ifndef VisualGraphicTypes_h
#define VisualGraphicTypes_h


#include "VisualTypes.h"
#include <vector>


/* The common graphics related types used by VisualGraphics and VisualActorGraphics. */

namespace VizKit {
	
	class VisualVertex; // Forward declaration (to avoid include of header file).
	
	/**
	 * A pixel color contains ARGB values (alpha, red, green, blue with 8 bits for each color channel).
	 */
	typedef uint32 PixelColor;
	
	/**
	 * A Point2D is a position in 2-dimensional planar space.
	 */
	typedef struct {
		double x; /**< Horizontal position. */
		double y; /**< Vertical position. */
	} Point2D;
	
	/**
	 * A pixel is a two-dimensional point relative to physical coordinate system.
	 */	
	typedef struct {
		uint32 x; /**< Horizontal position. */
		uint32 y; /**< Vertical position. */
	} Pixel;
	
	/**
	 * A colored pixel is a pixel with a color value.
	 */	
	typedef struct {
		PixelColor color; /**< Color value of the pixel. */
		uint32 x; /**< Horizontal position. */
		uint32 y; /**< Vertical position. */
	} ColoredPixel;
	
	/**
	 * A Point3D is a position in 3-dimensional space.
	 */
	typedef struct {
		double x; /**< Horizontal position. */
		double y; /**< Vertical position. */
		double z; /**< Depth position. */
	} Point3D;
	
	/**
	 * A Coord is a three-dimensional point relative to logical coordinate system.
	 */
	typedef Point3D Coord;
	
	/**
	 * A RelCoord is a relative three-dimensional point in coordinate system.
	 * x: 0.0 means: left, 1.0 means: right; y: 0.0 means: bottom, 1.0 means: top; z: 0.0 means: front, 1.0 means: back.
	 */
	typedef Point3D RelCoord;
	
	/**
	 * Predefined zero coord (x = 0.0 and y = 0.0).
	 */
	extern const Coord zeroCoord;
	
	/**
	 * A Vector points in a direction.
	 */
	typedef Point3D Vector;
	
	/**
	 * A TexCoord is a coord position of a texture.
	 */
	typedef struct {
		double s; /**< Horizontal position. */
		double t; /**< Vertical position. */
	} TexCoord;
	
	/**
	 * A RelTexCoord is a coord position of a texture.
	 * s: 0.0 means: left, 1.0 means: right; t: 0.0 means: bottom, 1.0 means: top.
	 */
	typedef TexCoord RelTexCoord;
	
	/**
	 * A two-dimensional size relative to logical coordinate system.
	 */
	typedef struct {
		double width; /**< Horizontal size. */
		double height; /**< Vertical size. */
	} CoordSize2D;
	
	/**
	 * A three-dimensional size relative to logical coordinate system.
	 */
	typedef struct {
		double width; /**< Horizontal size. */
		double height; /**< Vertical size. */
		double depth; /**< Size in z-dimension. */
	} CoordSize3D;
	
	/**
	 * A two-dimensional size relative to physical coordinate system.
	 */
	typedef struct {
		uint32 width; /**< Horizontal size. */
		uint32 height; /**< Vertical size. */
	} PixelRect;
	
	/**
	 * A rectangle.
	 */
	typedef struct {
		PixelRect pixelRect; /**< The size of the rectangle in pixels. */
		CoordSize2D coordSize; /**< The size of the rectangle in coord dimensions. */
	} Rect;
	
	/**
	 * A positioned rectangle, measured in pixels. Origin at top-left.
	 */
	typedef struct {
		PixelRect pixelRect; /**< The size of the rectangle in pixels. */
		Pixel topLeftPixel; /**< The top-left pixel of the positioned rect. */
	} TopLeftPositionedPixelRect;
	
	/**
	 * A positioned rectangle, measured in pixels. Origin at bottom-left.
	 */
	typedef struct {
		PixelRect pixelRect; /**< The size of the rectangle in pixels. */
		Pixel bottomLeftPixel; /**< The bottom-left pixel of the positioned rect. */
	} BottomLeftPositionedPixelRect;
	
	/**
	 * A positioned rectangle, measured in coords. Origin at bottom-left.
	 */
	typedef struct {
		CoordSize2D coordSize; /**< The size of the rectangle in coords. */
		Coord bottomLeftCoord; /**< The top-left coord of the positioned rect. */
	} PositionedCoordRect;
	
	/**
	 * Orientation.
	 */
	typedef enum {
		kPortrait = 0,
		kLandscape,
		kSquare
	} Orientation;
	
	/**
	 * A relational rectangle is determined by aspect ratio of width and height and orientation. No absolute are given.
	 */
	typedef struct {
		Orientation orientation; /**< The orientation of the rectangle. */
		double aspectRatio; /**< The aspect ratio (being > 1). */
	} RelationalRect;
	
	/**
	 * A box.
	 */
	typedef struct {
		CoordSize3D coordSize; /**< The size of the box in coord dimensions. */
	} Box;
	
	/**
	 * A positioned box. Origin at bottom-left-front.
	 */
	typedef struct {
		CoordSize3D coordSize; /**< The size of the rectangle in coord dimensions. */
		Coord bottomLeftFrontCoord; /**< The top-left-front coord of the positioned box. */
	} PositionedBox;
	
	/**
	 * ScalingBehaviourEnumValues enumerate the possible bit flag values of a ScalingBehaviour.
	 */
	enum ScalingBehaviourEnumValues {
		kNoScaling = 0x1, // 00000001
		kScalingAllowed = 0x2, // 00000010
		kPreserveAspectRatio = 0x6, // 00000110
		kNonproportional = 0xA, // 00001010
		kClippingAllowed = 0x10 // 00010000
	};
	
	/**
	 * ScalingBehaviour is the data type for specifying the scaling behaviour of a texture in combination with margins
	 */
	typedef uint32 ScalingBehaviour;
	
	/**
	 * Visibility specifies the visibility. Two states are possible: visible and invisible.
	 */
	typedef enum {
		kVisible = 0,
		kInvisible
	} Visibility;
	
	/**
	 * ProjectionMode specifies orthographic or perspective projection.
	 */
	typedef enum {
		kOrthographic = 0,
		kPerspective
	} ProjectionMode;
	
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
		double red; /**< Red value. Backing store. Value does not change except if it is set explicitely exernally. 0.0f .. 1.0f. */
		double green; /**< Green value. Backing store. Value does not change except if it is set explicitely exernally. 0.0f .. 1.0f. */
		double blue; /**< Blue value. Backing store. Value does not change except if it is set explicitely exernally. 0.0f .. 1.0f. */
		double alpha; /**< Alpha value. Backing store. Value does not change except if it is set explicitely exernally. 0.0f .. 1.0f. */
		double r; /**< Red value. 0.0f .. 1.0f. */
		double g; /**< Green value. 0.0f .. 1.0f. */
		double b; /**< Blue value. 0.0f .. 1.0f. */
		double a; /**< Alpha value. 0.0f .. 1.0f. */
	} RGBAColor;
	
	/**
	 * Stores the color values used by a vertex.
	 */
	typedef RGBAColor VertexColor;
	
	/**
	 * Predefined black color.
	 */
	extern const VertexColor black;
	
	/**
	 * Predefined white color.
	 */
	extern const VertexColor white;
	
	/**
	 * Predefined red color.
	 */
	extern const VertexColor red;
	
	/**
	 * Predefined green color.
	 */
	extern const VertexColor green;
	
	/**
	 * Predefined blue color.
	 */
	extern const VertexColor blue;
	
	/**
	 * Predefined transparent color.
	 */
	extern const VertexColor transparent;
	
	/**
	 * Stores the coord position values used by a vertex.
	 */
	typedef struct {
		Coord coord; /**< Coord position of vertex. */
		RelCoord relCoord; /**< Relative coord position of vertex. x: 0.0 means: left, 1.0 means: right; y: 0.0 means: bottom, 1.0 means: top; z: 0.0 means: front, 1.0 means: back. */
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
	/*
	 typedef struct {
	 double sPos;
	 double tPos;
	 double sRelativePos;
	 double tRelativePos;
	 } TexCoordPosition;
	 */
	typedef struct {
		TexCoord coord; /**< Position of texture coord. */
		RelTexCoord relCoord; /**< Relative position of texture coord. */
	} TexCoordPosition;
	
	/** Chain of vertices. */
	typedef std::vector<VisualVertex*> VertexChain;
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
		kBlend = 0, /**< Blend with framebuffer data (glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA). */
		kReplace /**< Replace framebuffer data. */
	} BlendMode;
	
}

#endif /* VisualGraphicTypes_h */
