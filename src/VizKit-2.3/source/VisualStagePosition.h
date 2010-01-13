/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStagePosition.h
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

#ifndef VisualStagePosition_h
#define VisualStagePosition_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"

 
namespace VizKit {

	/**
	 * Dimensionless position of asset on stage.
	 */
	class VisualStagePosition {

	public:

		/**
		 * The constructor.
		 */
		VisualStagePosition();

		/**
		 * The destructor.
		 */		
		~VisualStagePosition();

		/**
		 * Copy constructor.
		 * @param other Another VisualStagePosition.
		 */		
		VisualStagePosition(const VisualStagePosition& other);

		/**
		 * Assignment operator.
		 * @param other Another VisualStagePosition.
		 */
		VisualStagePosition& operator=(const VisualStagePosition& other);

		/**
		 * Equality operator.
		 * @param other Another VisualStagePosition.
		 */
		bool operator==(const VisualStagePosition& other) const;

		/**
		 * Inequality operator.
		 * @param other Another VisualStagePosition.
		 */
		bool operator!=(const VisualStagePosition& other);

		/**
		 * Resets the values of the stage position to default.
		 */			
		void reset(void);

		HorizontalAlignment horizontalAlignment; /**< The horizontal alignment of the box. */
		VerticalAlignment verticalAlignment; /**< The vertical alignment of the box. */
		DepthAlignment depthAlignment; /**< The depth alignment of the box (z-dimension). */
		
		double marginTop; /**< The top margin of the box. */
		double marginLeft; /**< The left margin of the box. */
		double marginBottom; /**< The bottom margin of the box. */
		double marginRight; /**< The right margin of the box. */

		double minMarginTop; /**< The minimum top margin of the box. */
		double minMarginLeft; /**< The minimum left margin of the box. */
		double minMarginBottom; /**< The minimum bottom margin of the box. */
		double minMarginRight; /**< The minimum right margin of the box. */
		
		double minWidth; /**< The minimum width of the box. */
		double maxWidth; /**< The maximum width of the box. */
		double minHeight; /**< The minimum height of the box. */
		double maxHeight; /**< The maximum height of the box. */
		
		Unit marginTopUnit; /**< The measure of the top margin value. */
		Unit marginLeftUnit; /**< The measure of the left margin value. */
		Unit marginRightUnit; /**< The measure of the right margin value. */
		Unit marginBottomUnit; /**< The measure of the bottom margin value. */

		Unit minMarginTopUnit; /**< The measure of the minimum top margin value. */
		Unit minMarginLeftUnit; /**< The measure of the minimum left margin value. */
		Unit minMarginRightUnit; /**< The measure of the minimum right margin value. */
		Unit minMarginBottomUnit; /**< The measure of the minimum bottom margin value. */
		
		Unit minWidthUnit; /**< The measure of the minimum width value. */
		Unit maxWidthUnit; /**< The measure of the maximum width value. */
		Unit minHeightUnit; /**< The measure of the minimum height value. */
		Unit maxHeightUnit; /**< The measure of the maximum height value. */
		
		double horizontalCoordOffset; /**< The horizontal offset (measured in coord values). */
		double verticalCoordOffset; /**< The vertical offset (measured in coord values). */
		
	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualStagePosition.
		 */
		void copy(const VisualStagePosition& other);
		
	};

}

#endif /* VisualStagePosition_h */
