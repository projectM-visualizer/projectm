/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualCamera.h
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

#ifndef VisualCamera_h
#define VisualCamera_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"


namespace VizKit {

	/**
	 * A VisualCamera.
	 */
	class VisualCamera {

	public:

		/**
		 * Constructor of VisualCamera.
		 */
		VisualCamera();

		/**
		 * The destructor.
		 */
		~VisualCamera();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualCamera.
		 */
		VisualCamera(const VisualCamera& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualCamera.
		 */
		VisualCamera& operator=(const VisualCamera& other);

		/**
		 * Sets up an orthographic projection matrix of GL.
		 */
		void setOrthographicProjection();

		/**
		 * Sets up the perspective projection matrix of GL.
		 * @param nearClippingPlanePos The maximum near position. With the maxNearCoord the position of the near clipping plane of the viewing volume is set. By setting maxNearCoord the perspective projection (the shape of the viewing volume) can be adjusted. The value of maxNearCoord must be positive.
		 * @remarks Default value for maxNearCoord is 1.0. Default value for zoomFactor is 1.0.
		 */
		void setPerspectiveProjection(double nearClippingPlanePos = 1.0);

		/**
		 * Performs a viewing transformation.
		 * @param eye The position of the eye point.
		 * @param center The position of the reference point.
		 * @param up the direction of the up vector.
		 */	
		void lookAt(Point3D eye, Point3D center, Vector up);

		/**
		 * Activates the camera with its current settings.
		 * @remarks The camera sets up the remembered projection mode.
		 */	
		void activate(void);

		/**
		 * Static function which creates a camera with default orientation (orthographic projection).
		 * @return A camera with default orientation (orthographic projection).
		 * @remarks Caller has to delete the camera instance by calling delete.
		 */	
		static VisualCamera* createDefaultCamera(void);

		/**
		 * Returns the maximum top position.
		 * @return The maximum top position.
		 */
		double getMaxTopCoord(void) const;

		/**
		 * Returns the maximum left position.
		 * @return The maximum left position.
		 */
		double getMaxLeftCoord(void) const;

		/**
		 * Returns the maximum bottom position.
		 * @return The maximum bottom position.
		 */
		double getMaxBottomCoord(void) const;

		/**
		 * Returns the maximum right position.
		 * @return The maximum right position.
		 */
		double getMaxRightCoord(void) const;

		/**
		 * Returns the maximum near position.
		 * @return The maximum near position.
		 */
		double getMaxNearPos(void) const;

		/**
		 * Returns the maximum far position.
		 * @return The maximum far position.
		 */
		double getMaxFarPos(void) const;

		/**
		 * Returns the 3D dimensions of the camera view.
		 * @return The 3D dimensions of the camera view.
		 */
		CoordSize3D getSize(void) const;

		/**
		 * Returns the height of the camera view.
		 * @return The height of the camera view.
		 */
		double getCoordHeight(void) const;

		/**
		 * Returns the width of the camera view.
		 * @return The width of the camera view.
		 */
		double getCoordWidth(void) const;

		/**
		 * Returns the depth of the camera view.
		 * @return The depth of the camera view.
		 */
		double getDepth(void) const;

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualCamera.
		 */
		void copy(const VisualCamera& other);

		/** The maximum top position. */
		double maxTopCoord;
		
		/** The maximum left position. */
		double maxLeftCoord;
		
		/** The maximum bottom position. */
		double maxBottomCoord;
		
		/** The maximum right position. */
		double maxRightCoord;
		
		/** The maximum near position. */
		double maxNearPos;
		
		/** The maximum far position. */
		double maxFarPos;

		/** The zoom factor. */
		double zoomFactor;
		
		/** The mode of projection. */
		ProjectionMode projectionMode;
	};

}

#endif /* VisualCamera_h */
