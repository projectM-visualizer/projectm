/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNurbs.h
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

#ifndef VisualNurbs_h
#define VisualNurbs_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

#if TARGET_OS_MAC
#include <OpenGL/glu.h>
#endif

#if TARGET_OS_WIN
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif


namespace VizKit {


	/**
	 * Nurbs (non-uniform rational b-spline) functionality.
	 */
	class VisualNurbs {
	
	public:

		/**
		 * The constructor with all necessary arguments to create a NURBS.
		 * @param sKnotsCount The number of knots in horizontal direction.
		 * @param sKnotsPointer A pointer to the array of horizontal knot values. The knot values are copied.
		 * @param tKnotsCount The number of knots in vertical direction.
		 * @param tKnotsPointer A pointer to the array of vertical knot values. The knot values are copied.
		 * @param s_Stride Specifies the offset (as a number of single-precision floating point values) between successive control points in the parametric u direction in control.
		 * @param t_Stride Specifies the offset (as a number of single-precision floating point values) between successive control points in the parametric v direction in control.
		 * @param s_Order Specifies the order of the NURBS surface in the parametric u direction. The order is one more than the degree, hence a surface that is cubic in u has a u order of 4.
		 * @param t_Order Specifies the order of the NURBS surface in the parametric v direction. The order is one more than the degree, hence a surface that is cubic in v has a v order of 4.
		 */
		VisualNurbs(uint8 sKnotsCount, float* sKnotsPointer, uint8 tKnotsCount, float* tKnotsPointer, uint16 s_Stride, uint16 t_Stride, uint16 s_Order, uint16 t_Order);
		
		/**
		 * The destructor.
		 */
		~VisualNurbs();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualNurbs.
		 */
		VisualNurbs(const VisualNurbs& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualNurbs.
		 */
		VisualNurbs& operator=(const VisualNurbs& other);

		/**
		 * Stores the control points of the NURBS.
		 * @param numberOfControlPoints The number of control points.
		 * @param someControlPoints A pointer to an array of control points. The control points are copied.
		 */
		void setControlPoints(uint16 numberOfControlPoints, double* someControlPoints);

		/**
		 * Associates the NURBS with a texture.
		 * @param sKnotsCount The number of knots in horizontal direction.
		 * @param sKnotsPointer A pointer to the array of horizontal knot values. The knot values are copied.
		 * @param tKnotsCount The number of knots in vertical direction.
		 * @param tKnotsPointer A pointer to the array of vertical knot values. The knot values are copied.
		 * @param s_Stride Specifies the offset (as a number of single-precision floating point values) between successive control points in the parametric u direction in control.
		 * @param t_Stride Specifies the offset (as a number of single-precision floating point values) between successive control points in the parametric v direction in control.
		 * @param s_Order Specifies the order of the NURBS surface in the parametric u direction. The order is one more than the degree, hence a surface that is cubic in u has a u order of 4.
		 * @param t_Order Specifies the order of the NURBS surface in the parametric v direction. The order is one more than the degree, hence a surface that is cubic in v has a v order of 4.
		 * @param aTextureName The name/number/id of the texture.
		 * @param canUseRectExtensionBool True if graphics system does support rectangular texture extension. False if only textures with power-of-2-dimensions are allowed.
		 */
		void setTexture(uint8 sKnotsCount, float* sKnotsPointer, uint8 tKnotsCount, float* tKnotsPointer, uint16 s_Stride, uint16 t_Stride, uint16 s_Order, uint16 t_Order, uint32 aTextureName, bool canUseRectExtensionBool);

		/**
		 * Stores the control points of the texture.
		 * @param numberOfTextureControlPoints The number of control points.
		 * @param someTextureControlPoints A pointer to an array of control points. The control points are copied.
		 */
		void setTextureControlPoints(uint16 numberOfTextureControlPoints, float* someTextureControlPoints);
		
		/**
		 * Draws the NURBS on screen.
		 */
		void draw(void);
		
		/**
		 * Sets the display mode of the NURBS to filled or outlined polygon.
		 * @param requestedDisplayMode The requested display mode.
		 */
		void setDisplayMode(NurbsDisplayMode requestedDisplayMode);

		/**
		 * Sets the sampling tolerance of the NURBS.
		 * @param samplingTolerance The requested sampling tolerance.
		 */
		void setSamplingTolerance(double samplingTolerance);
		
		/**
		 * Stores whether the control points of the NURBS should be drawn with the NURBS (or stay invisible).
		 * @param visibility The requested visibility of the control points.
		 */		
		void setControlPointsVisibility(Visibility visibility);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualNurbs.
		 */
		void copy(const VisualNurbs& other);
		
		GLUnurbsObj* nurbsObject; /**< The NURBS object. */
		
		uint8 sNumKnots; /**< The number of knots in horizontal direction. */
		
		float* sKnots; /**< An array of horizontal knot values. */
		
		uint8 tNumKnots; /**< The number of knots in vertical direction. */
		
		float* tKnots; /**< An array of vertical knot values. */
		
		uint16 sStride; /**< The offset between successive control points in u direction. */
		
		uint16 tStride; /**< The offset between successive control points in v direction. */
		
		uint16 sOrder; /**< The order of the NURBS surface in u direction. */
		
		uint16 tOrder; /**< The order of the NURBS surface in v direction. */
		
		float* controlPoints; /**< An array of control points. */
		
		Visibility controlPointsVisibility; /**< The visibility of the control points. */
		
		void setNurbsProperty(uint32 property, float value); /**< Sets an OpenGL attribute of the NURBS object. */
		
		/** 
		 * A texture of a NURBS.
		 */
		class NurbsTexture {
		public:
			uint8 sNumKnots; /**< The number of knots in horizontal direction. */
			float* sKnots; /**< An array of horizontal knot values. */
			uint8 tNumKnots; /**< The number of knots in vertical direction. */
			float* tKnots; /**< An array of vertical knot values. */
			uint16 sStride; /**< The offset between successive control points in u direction. */
			uint16 tStride; /**< The offset between successive control points in v direction. */
			uint16 sOrder; /**< The order of the texture in u direction. */
			uint16 tOrder; /**< The order of the texture in v direction. */
			uint32 textureName; /**< The name/number/id of the texture. */
			bool canUseRectExtension; /**< True if graphics system does support rectangular texture extension. False if only textures with power-of-2-dimensions are allowed. */
			float* texturePoints; /**< An array of control points. */
			NurbsTexture(); /**< The constructor. */
			~NurbsTexture(); /**< The destructor. */
			/**
			 * Copy constructor of NurbsTexture.
			 * @param other Reference to another NurbsTexture.
			 */
			NurbsTexture(const NurbsTexture& other);
			/**
			 * Assignment operator of NurbsTexture.
			 * @param other Reference to another NurbsTexture.
			 */
			NurbsTexture& operator=(const NurbsTexture& other);
		private:
			/**
			 * Copy method for assignment operator and copy constructor.
			 * @param other Another NurbsTexture.
			 */
			void copy(const NurbsTexture& other);
		};
		
		NurbsTexture* nurbsTexture; /**< The texture of the NURBS. */
		
	};

}

#endif /* VisualNurbs_h */
