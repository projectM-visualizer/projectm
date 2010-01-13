/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualVertex.h
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

#ifndef VisualVertex_h
#define VisualVertex_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"


namespace VizKit {

	/**
	 * A vertex is coord position information, incl. color information and coord position.
	 * Texture position belongs to vertex data, too.
	 */
	class VisualVertex {

	public:

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param coordPoint Coord position.
		 */
		VisualVertex(Coord coordPoint);

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param coordPoint Coord position.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 */
		VisualVertex(Coord coordPoint, VertexColor anRGBAColor);

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param coordPoint Coord position.
		 * @param relativeCoordPoint Relative coord position. x: 0.0 means: left, 1.0 means: right; y: 0.0 means: bottom, 1.0 means: top; z: 0.0 means: front, 1.0 means: back.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 */
		VisualVertex(Coord coordPoint, Coord relativeCoordPoint, VertexColor anRGBAColor);

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param coordPoint Coord position.
		 * @param texCoordPoint A position of a texture.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 */
		VisualVertex(Coord coordPoint, TexCoord texCoordPoint, VertexColor anRGBAColor);

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param coordPoint Coord position.
		 * @param relativeCoordPoint Relative coord position. x: 0.0 means: left, 1.0 means: right; y: 0.0 means: bottom, 1.0 means: top; z: 0.0 means: front, 1.0 means: back.
		 * @param texCoordPoint A position of a texture.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 */
		VisualVertex(Coord coordPoint, Coord relativeCoordPoint, TexCoord texCoordPoint, VertexColor anRGBAColor);

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param coordPoint Coord position.
		 * @param relativeCoordPoint Relative coord position. x: 0.0 means: left, 1.0 means: right; y: 0.0 means: bottom, 1.0 means: top; z: 0.0 means: front, 1.0 means: back.
		 * @param texCoordPoint A position of a texture.
		 * @param relTexCoordPoint A relative position of a texture.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 */
		VisualVertex(Coord coordPoint, Coord relativeCoordPoint, TexCoord texCoordPoint, TexCoord relTexCoordPoint, VertexColor anRGBAColor);

		/**
		 * Constructor that creates and returns a vertex at the specified position.
		 * @param aVertexPosition A vertex position.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 */		
		VisualVertex(VertexPosition aVertexPosition, VertexColor anRGBAColor);

		/**
		 * The destructor.
		 */
		~VisualVertex();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualVertex.
		 */
		VisualVertex(const VisualVertex& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualVertex.
		 */
		VisualVertex& operator=(const VisualVertex& other);

		/**
		 * Updates the relative position values of the vertex.
		 * @param coordPoint The requested relative position values of the vertex.
		 */
		void setRelativeVertexPosition(Coord coordPoint);

		/**
		 * Updates the relative tex coord position of the vertex.
		 * @param aTexCoord The requested relative tex coord position of the vertex.
		 */
		void setRelativeTexCoordPosition(TexCoord aTexCoord);

		/**
		 * True if vertex contains a valid color.
		 * @return True if vertex contains a valid color.
		 */
		bool colorHasBeenSet(void);

		VertexColor vertexColor; /**< The color of the vertex. */
		
		VertexPosition vertexPosition; /**< The coord position of the vertex. */
		
		TexCoordPosition texCoordPosition; /**< Texture position at the vertex. */

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualVertex.
		 */
		void copy(const VisualVertex& other);

		/**
		 * True if color of vertex is set.
		 */
		bool colorIsSet;
		
	};

}

#endif /* VisualVertex_h */
