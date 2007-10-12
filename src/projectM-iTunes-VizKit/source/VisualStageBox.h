/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualStageBox.h
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

#ifndef VisualStageBox_h
#define VisualStageBox_h

#include "VisualGraphicTypes.h"
#include "VisualStagePosition.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	/**
	 * Position and dimensions of a box on stage.
	 * Used by VisualAsset as a positioning container.\ It's the bounding box of the visual asset.
	 */
	class VisualStageBox {

	public:

		/**
		 * The constructor.
		 */
		VisualStageBox();
		
		/**
		 * The destructor.
		 */
		~VisualStageBox();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualStageBox.
		 */
		VisualStageBox(const VisualStageBox& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualStageBox.
		 */
		VisualStageBox& operator=(const VisualStageBox& other);

		/**
		 * Sets the pixel width of the image data.
		 * @param pixelWidth The pixel width of the image data.
		 */
		void setContentPixelWidth(UInt32 pixelWidth);
		
		/**
		 * Sets the pixel height of the image data.
		 * @param pixelHeight The pixel height of the image data.
		 */
		void setContentPixelHeight(UInt32 pixelHeight);

		/**
		 * Sets value of the front position.
		 * @param aFrontPosition The value of the front position.
		 */
		void setFrontPosition(double aFrontPosition);

		/**
		 * Returns the value of the front position.
		 * @return The value of the front position.
		 */
		double getFrontPosition(void);

		/**
		 * Sets value of the back position.
		 * @param aBackPosition The value of the back position.
		 */
		void setBackPosition(double aBackPosition);

		/**
		 * Returns the value of the back position.
		 * @return The value of the back position.
		 */
		double getBackPosition(void);

		/**
		 * Sets the stage position of the box.
		 * @param aPosition The position that is supposed to become the current position.
		 */		
		void setVisualStagePosition(const VisualStagePosition& aPosition);

		/**
		 * Specifies the scaling behaviour.
		 * @param aScalingBehaviour The requested scaling behaviour.
		 */
		void setScalingBehaviour(ScalingBehaviour aScalingBehaviour);
		
		/**
		 * Specifies the magnification/minification of the stage box when drawn.
		 * @param aScaleFactor The factor by which the stage box should be multiplied when rendered (1.0 means no magnification/minification).
		 */
		void setScaleFactor(double aScaleFactor);

		/**
		 * Updates the coord values (e.g.\ after canvas reshape).
		 */
		void update(void);

		/**
		 * Returns the scaling attributes.
		 * @return The scaling attributes.
		 */
		ScalingBehaviour getScalingBehaviour(void);

		/**
		 * Returns the magnification/minification of the stage box when drawn.
		 * @return The factor by which the stage box is multiplied when rendered (1.0 means no magnification/minification).
		 */
		double getScaleFactor(void);
	
		/**
		 * Returns the width of the box.
		 * @return The width of the box.
		 */
		double getCoordWidth(void);

		/**
		 * Returns the height of the box.
		 * @return The height of the box.
		 */		
		double getCoordHeight(void);

		/**
		 * Returns the depth of the box.
		 * @return The depth of the box.
		 */		
		double getCoordDepth(void);
		
		/**
		 * Returns the top coord of the box.
		 * @return The top coord of the box.
		 */			
		double getTopCoord(void);

		/**
		 * Returns the left coord of the box.
		 * @return The left coord of the box.
		 */		
		double getLeftCoord(void);

		/**
		 * Returns the bottom coord of the box.
		 * @return The bottom coord of the box.
		 */
		double getBottomCoord(void);

		/**
		 * Returns the right coord of the box.
		 * @return The right coord of the box.
		 */
		double getRightCoord(void);

		/**
		 * Returns a copy of the stage position of the box.
		 * @return A copy of the stage position of the box.
		 */
		VisualStagePosition getVisualStagePosition(void);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualStageBox.
		 */
		void copy(const VisualStageBox& other);
		
		/**
		 * Calculates the coord dimension of the box according to alignment and margin settings.
		 */
		void calcCoords(void);
		
		bool hasLayout; /**< True if calcCoords() was executed at least once, initial state is false. */
		
		double topCoord; /**< The top coord value of the positioned box. */
		
		double leftCoord; /**< The left coord value of the positioned box. */
		
		double bottomCoord; /**< The bottom coord value of the positioned box. */
		
		double rightCoord; /**< The right coord value of the positioned box. */
		
		double frontPosition; /**< The value of the front position. */
		
		double backPosition; /**< The value of the back position. */
		
		double coordWidth; /**< The width of the box measured in coord values. */
		
		double coordHeight; /**< The height of the box measured in coord values. */
		
		double coordDepth; /**< The depth of the box measured in coord values. */
		
		UInt32 contentPixelWidth; /**< The width of the box measured in pixels. */
		
		UInt32 contentPixelHeight; /**< The height of the box measured in pixels. */
		
		VisualStagePosition stagePosition; /**< The dimensionless position of the box. */
		
		ScalingBehaviour scalingBehaviour; /**< Specifies the scaling behaviour. */
		
		double scaleFactor; /**< The factor by which the stage box should be multiplied when rendered (1.0 means no magnification/minification). */
		
	};

}

#endif /* VisualStageBox_h */
