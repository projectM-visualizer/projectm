/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualStageBox.cpp
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

#include "VisualStageBox.h"
#include "VisualGraphics.h"



using namespace VizKit;


VisualStageBox::VisualStageBox() {

	topCoord = 0.0f;
	leftCoord = 0.0f;
	bottomCoord = 0.0f;
	rightCoord = 0.0f;
	
	frontPosition = 0.0f;
	backPosition = 0.0f;
	
	coordWidth = 0.0;
	coordHeight = 0.0;
	coordDepth = 0.0;
	
	contentPixelWidth = 0;
	contentPixelHeight = 0;

	scalingBehaviour = kPreserveAspectRatio;
	scaleFactor = 1.0;
	
	hasLayout = false;
	
}


VisualStageBox::~VisualStageBox() {
	// NULL
}


VisualStageBox::VisualStageBox(const VisualStageBox& other) {
	copy(other);
}


VisualStageBox& VisualStageBox::operator=(const VisualStageBox& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualStageBox::copy(const VisualStageBox& other) {
	this->topCoord = other.topCoord;
	this->leftCoord = other.leftCoord;
	this->bottomCoord = other.bottomCoord;
	this->rightCoord = other.rightCoord;
	
	this->coordWidth = other.coordWidth;
	this->coordHeight = other.coordHeight;
	this->coordDepth = other.coordDepth;
	
	this->contentPixelWidth = other.contentPixelWidth;
	this->contentPixelHeight = other.contentPixelHeight;

	this->stagePosition = other.stagePosition;
	this->scalingBehaviour = other.scalingBehaviour;
	this->scaleFactor = other.scaleFactor;
	
	this->hasLayout = other.hasLayout;
}


void VisualStageBox::setContentPixelWidth(UInt32 pixelWidth) {
	if (pixelWidth != this->contentPixelWidth) {
		this->contentPixelWidth = pixelWidth;
		this->hasLayout = false;
	}
	if (this->hasLayout == false) {
		this->calcCoords();
	}
}


void VisualStageBox::setContentPixelHeight(UInt32 pixelHeight) {
	if (pixelHeight != this->contentPixelHeight) {
		this->contentPixelHeight = pixelHeight;
		this->hasLayout = false;
	}
	if (this->hasLayout == false) {
		this->calcCoords();
	}
}


void VisualStageBox::setFrontPosition(double aFrontPosition) {
	this->frontPosition = aFrontPosition;
}


double VisualStageBox::getFrontPosition() {
	return this->frontPosition;
}


void VisualStageBox::setBackPosition(double aBackPosition)  {
	this->backPosition = aBackPosition;
}


double VisualStageBox::getBackPosition(void) {
	return this->backPosition;
}


void VisualStageBox::setVisualStagePosition(const VisualStagePosition& aPosition) {
	this->stagePosition = aPosition;
	this->calcCoords();
}


void VisualStageBox::setScalingBehaviour(ScalingBehaviour aScalingBehaviour) {
	if (aScalingBehaviour != this->scalingBehaviour) {
		this->scalingBehaviour = aScalingBehaviour;
		this->hasLayout = false;
	}
	if (this->hasLayout == false) {
		this->calcCoords();
	}
}


void VisualStageBox::setScaleFactor(double aScaleFactor) {
	if (aScaleFactor != this->scaleFactor) {
		this->scaleFactor = aScaleFactor;
		this->hasLayout = false;
	}
	if (this->hasLayout == false) {
		this->calcCoords();
	}
}


void VisualStageBox::update() {
	this->calcCoords();
}


void VisualStageBox::calcCoords() {

	double top = 0.0;
	double left = 0.0;
	double bottom = 0.0;
	double right = 0.0;
	
	double marginLeftCoord = 0.0;
	double marginRightCoord = 0.0;
	double marginTopCoord = 0.0;
	double marginBottomCoord = 0.0;
	double minMarginLeftCoord = 0.0;
	double minMarginRightCoord = 0.0;
	double minMarginTopCoord = 0.0;
	double minMarginBottomCoord = 0.0;
	
	double finalMarginLeftCoord = 0.0;
	double finalMarginRightCoord = 0.0;
	double finalMarginTopCoord = 0.0;
	double finalMarginBottomCoord = 0.0;
	
	double prevCoordVal = 0.0;

	VisualGraphics* theVisualGraphics;
	theVisualGraphics = VisualGraphics::getInstance();

	this->coordWidth = theVisualGraphics->getCanvasCoordWidth() * ((double)this->contentPixelWidth / theVisualGraphics->getCanvasPixelWidth());
	this->coordHeight = theVisualGraphics->getCanvasCoordHeight() * ((double)this->contentPixelHeight / theVisualGraphics->getCanvasPixelHeight());

	// calc margin values
	if (this->stagePosition.marginBottom != 0.0) {
		if (this->stagePosition.marginBottomUnit == kPercent) {
			marginBottomCoord = theVisualGraphics->getCanvasCoordHeight() * (this->stagePosition.marginBottom / 100.0);
		} else if (this->stagePosition.marginBottomUnit == kPixel) {
			marginBottomCoord = theVisualGraphics->yPixelToCoord((UInt16)this->stagePosition.marginBottom);
		}
	}

	if (this->stagePosition.marginTop != 0.0) {
		if (this->stagePosition.marginBottomUnit == kPercent) {
			marginTopCoord = theVisualGraphics->getCanvasCoordHeight() * (this->stagePosition.marginBottom / 100.0);
		} else if (this->stagePosition.marginTopUnit == kPixel) {
			marginTopCoord = theVisualGraphics->yPixelToCoord((UInt16)this->stagePosition.marginTop);
		}
	}

	if (this->stagePosition.marginLeft != 0.0) {
		if (this->stagePosition.marginLeftUnit == kPercent) {
			marginLeftCoord = theVisualGraphics->getCanvasCoordWidth() * (this->stagePosition.marginLeft / 100.0);
		} else if (this->stagePosition.marginLeftUnit == kPixel) {
			marginLeftCoord = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.marginLeft);
		}
	}

	if (this->stagePosition.marginRight != 0.0) {
		if (this->stagePosition.marginRightUnit == kPercent) {
			marginRightCoord = theVisualGraphics->getCanvasCoordWidth() * (this->stagePosition.marginRight / 100.0);
		} else if (this->stagePosition.marginRightUnit == kPixel) {
			marginRightCoord = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.marginRight);
		}
	}

	if (this->stagePosition.minMarginBottom != 0.0) {
		if (this->stagePosition.minMarginBottomUnit == kPercent) {
			minMarginBottomCoord = theVisualGraphics->getCanvasCoordHeight() * (this->stagePosition.minMarginBottom / 100.0);
		} else if (this->stagePosition.minMarginBottomUnit == kPixel) {
			minMarginBottomCoord = theVisualGraphics->yPixelToCoord((UInt16)this->stagePosition.minMarginBottom);
		}
	}
	
	if (this->stagePosition.minMarginTop != 0.0) {
		if (this->stagePosition.minMarginTopUnit == kPercent) {
			minMarginTopCoord = theVisualGraphics->getCanvasCoordHeight() * (this->stagePosition.minMarginTop / 100.0);
		} else if (this->stagePosition.minMarginTopUnit == kPixel) {
			minMarginTopCoord = theVisualGraphics->yPixelToCoord((UInt16)this->stagePosition.minMarginTop);
		}
	}

	if (this->stagePosition.minMarginLeft != 0.0) {
		if (this->stagePosition.minMarginLeftUnit == kPercent) {
			minMarginLeftCoord = theVisualGraphics->getCanvasCoordWidth() * (this->stagePosition.minMarginLeft / 100.0);
		} else if (this->stagePosition.minMarginLeftUnit == kPixel) {
			minMarginLeftCoord = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.minMarginLeft);
		}
	}

	if (this->stagePosition.minMarginRight != 0.0) {
		if (this->stagePosition.minMarginRightUnit == kPercent) {
			minMarginRightCoord = theVisualGraphics->getCanvasCoordWidth() * (this->stagePosition.minMarginRight / 100.0);
		} else if (this->stagePosition.minMarginRightUnit == kPixel) {
			minMarginRightCoord = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.minMarginRight);
		}
	}
	
	// calc finalMarginValues
	if (marginTopCoord > 0.0) {
		finalMarginTopCoord = marginTopCoord;
	}
	if (marginLeftCoord > 0.0) {
		finalMarginLeftCoord = marginLeftCoord;
	}
	if (marginBottomCoord > 0.0) {
		finalMarginBottomCoord = marginBottomCoord;
	}
	if (marginRightCoord > 0.0) {
		finalMarginRightCoord = marginRightCoord;
	}
	if (coordWidth > (theVisualGraphics->getCanvasCoordWidth() - minMarginRightCoord - minMarginLeftCoord)) {
		prevCoordVal = coordWidth;
		coordWidth = theVisualGraphics->getCanvasCoordWidth() - minMarginRightCoord - minMarginLeftCoord;
		if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
			coordHeight = coordHeight * (coordWidth / prevCoordVal);
		}
		finalMarginLeftCoord = minMarginLeftCoord;
		finalMarginRightCoord = minMarginRightCoord;
	}
	if (coordHeight > (theVisualGraphics->getCanvasCoordHeight() - minMarginTopCoord - minMarginBottomCoord)) {
		prevCoordVal = coordHeight;
		coordHeight = theVisualGraphics->getCanvasCoordHeight() - minMarginTopCoord - minMarginBottomCoord;
		if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
			coordWidth = coordWidth * (coordHeight / prevCoordVal);
		}
		finalMarginTopCoord = minMarginTopCoord;
		finalMarginBottomCoord = minMarginBottomCoord;
	}

	// calc coordWidth and coordHeight
	if ((this->scalingBehaviour & kScalingAllowed) == kScalingAllowed) {

		// < minWidth
		if (this->stagePosition.minWidth > 0.0) {
			prevCoordVal = coordWidth;
			if ((this->stagePosition.minWidthUnit == kPercent) && ((coordWidth / theVisualGraphics->getCanvasCoordWidth() * 100.0) < this->stagePosition.minWidth)) {
				coordWidth = this->stagePosition.minWidth * theVisualGraphics->getCanvasCoordWidth() / 100.0;
				if (coordWidth > (theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord))) {
					coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
				} else {
					coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordHeight = coordHeight * (coordWidth / prevCoordVal);
				}
			} else if ((this->stagePosition.minWidthUnit == kPixel) && (static_cast<double>(theVisualGraphics->xCoordToPixel(coordWidth)) < this->stagePosition.minWidth)) {
				coordWidth = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.minWidth);
				if (coordWidth > (theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord))) {
					coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
				} else {
					coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordHeight = coordHeight * (coordWidth / prevCoordVal);
				}
			}
		}

		// < minHeight
		if (this->stagePosition.minHeight > 0.0) {
			prevCoordVal = coordHeight;
			if ((this->stagePosition.minHeightUnit == kPercent) && ((coordHeight / theVisualGraphics->getCanvasCoordHeight() * 100.0) < this->stagePosition.minHeight)) {
				coordHeight = this->stagePosition.minHeight * theVisualGraphics->getCanvasCoordHeight() / 100.0;
				if (coordHeight > (theVisualGraphics->getCanvasCoordHeight() - (minMarginTopCoord + minMarginBottomCoord))) {
					coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
				}
			} else if ((this->stagePosition.minHeightUnit == kPixel) && (static_cast<double>(theVisualGraphics->yCoordToPixel(coordHeight)) < this->stagePosition.minHeight)) {
				coordHeight = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.minHeight);
				if (coordHeight > (theVisualGraphics->getCanvasCoordHeight() - (minMarginTopCoord + minMarginBottomCoord))) {
					coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
				}
			}
		}

		// > maxWidth
		if (this->stagePosition.maxWidth > 0.0) {
			prevCoordVal = coordWidth;
			if ((this->stagePosition.maxWidthUnit == kPercent) && ((coordWidth / theVisualGraphics->getCanvasCoordWidth() * 100.0) > this->stagePosition.maxWidth)) {
				coordWidth = this->stagePosition.maxWidth * theVisualGraphics->getCanvasCoordWidth() / 100.0;
				if (coordWidth > (theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord))) {
					coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
				} else {
					coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordHeight = coordHeight * (coordWidth / prevCoordVal);
				}
			} else if ((this->stagePosition.maxWidthUnit == kPixel) && (static_cast<double>(theVisualGraphics->xCoordToPixel(coordWidth)) > this->stagePosition.maxWidth)) {
				coordWidth = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.maxWidth);
				if (coordWidth > (theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord))) {
					coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
				} else {
					coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordHeight = coordHeight * (coordWidth / prevCoordVal);
				}
			}
		}

		// > maxHeight
		if (this->stagePosition.maxHeight > 0.0) {
			prevCoordVal = coordHeight;
			if ((this->stagePosition.maxHeightUnit == kPercent) && ((coordHeight / theVisualGraphics->getCanvasCoordHeight() * 100.0) > this->stagePosition.maxHeight)) {
				coordHeight = this->stagePosition.maxHeight * theVisualGraphics->getCanvasCoordHeight() / 100.0;
				if (coordHeight > (theVisualGraphics->getCanvasCoordHeight() - (minMarginTopCoord + minMarginBottomCoord))) {
					coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
				}
			} else if ((this->stagePosition.maxHeightUnit == kPixel) && (static_cast<double>(theVisualGraphics->yCoordToPixel(coordHeight)) > this->stagePosition.maxHeight)) {
				coordHeight = theVisualGraphics->xPixelToCoord((UInt16)this->stagePosition.maxHeight);
				if (coordHeight > (theVisualGraphics->getCanvasCoordHeight() - (minMarginTopCoord + minMarginBottomCoord))) {
					coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					//coordWidth = coordWidth * (coordHeight / prevCoordVal * (theVisualGraphics->getCanvasCoordWidth() / theVisualGraphics->getCanvasCoordHeight()));
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
				}
			}
		}
		
		if (!((this->scalingBehaviour & kClippingAllowed) == kClippingAllowed)) {
			if (coordWidth > (theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord))) {
				prevCoordVal = coordWidth;
				coordWidth = theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord);
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordHeight = coordHeight * (coordWidth / prevCoordVal);
				}
			}
			if (coordHeight > (theVisualGraphics->getCanvasCoordHeight() - (minMarginTopCoord + minMarginBottomCoord))) {
				prevCoordVal = coordHeight;
				coordHeight = theVisualGraphics->getCanvasCoordHeight() - (minMarginTopCoord + minMarginBottomCoord);
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
				}
			}
		}
		
/*
		if (!((this->scalingBehaviour & kClippingAllowed) == kClippingAllowed)) {
			double aspectRatioCanvas = theVisualGraphics->getCanvasCoordWidth() / theVisualGraphics->getCanvasCoordHeight();
			double aspectRatioCover = coverArtTextureContainer->getImageWidth() / coverArtTextureContainer->getImageHeight();
			coordWidthUnscaled = (double)theVisualGraphics->getCanvasCoordWidth() * ((double)coverArtTextureContainer->getImageWidth() / (double)theVisualGraphics->getCanvasPixelWidth());
			coordHeightUnscaled = (double)theVisualGraphics->getCanvasCoordHeight() * ((double)coverArtTextureContainer->getImageHeight() / (double)theVisualGraphics->getCanvasPixelHeight());
			if (aspectRatioCanvas > aspectRatioCover) {
				prevCoordVal = coordWidth;
				coordWidth = (double)theVisualGraphics->getCanvasCoordWidth();
				coordHeight = coordHeightUnscaled * (coordWidth / coordWidthUnscaled);
			} else {
				coordHeight = (double)theVisualGraphics->getCanvasCoordHeight();
				coordWidth = coordWidthUnscaled * (coordHeight / coordHeightUnscaled);
			}
		}
*/
	}
	
	//printf("coordHeight: %f, coordWidth: %f\n", coordHeight, coordWidth);

	// recalc finalMarginCoords
	if (this->stagePosition.verticalAlignment == kTopAligned) {
		if (minMarginTopCoord > marginTopCoord) {
			finalMarginTopCoord = minMarginTopCoord;
		} else {
			finalMarginTopCoord = marginTopCoord;
		}
		if (minMarginBottomCoord > marginBottomCoord) {
			finalMarginBottomCoord = minMarginBottomCoord;
		} else {
			finalMarginBottomCoord = marginBottomCoord;
		}
		if (coordHeight < (theVisualGraphics->getCanvasCoordHeight() - (finalMarginTopCoord + finalMarginBottomCoord))) {
			finalMarginBottomCoord = 0.0;
		}
	} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
		finalMarginTopCoord = (theVisualGraphics->getCanvasCoordHeight() / 2.0) - (coordHeight / 2.0) - marginTopCoord;
		finalMarginBottomCoord = (theVisualGraphics->getCanvasCoordHeight() / 2.0) - (coordHeight / 2.0) - marginBottomCoord;
		if (finalMarginTopCoord < minMarginTopCoord) {
			finalMarginTopCoord = minMarginTopCoord;
		}
		if (finalMarginBottomCoord < minMarginBottomCoord) {
			finalMarginBottomCoord = minMarginBottomCoord;
		}
		if ((coordHeight / 2.0) >= (theVisualGraphics->getCanvasCoordHeight() / 2.0) - minMarginBottomCoord) {
			finalMarginBottomCoord = minMarginBottomCoord;
			finalMarginTopCoord = theVisualGraphics->getCanvasCoordHeight() - (finalMarginBottomCoord + coordHeight);
			if ((finalMarginTopCoord + coordHeight + finalMarginBottomCoord) > theVisualGraphics->getCanvasCoordHeight()) {
				finalMarginTopCoord = minMarginTopCoord;
				prevCoordVal = coordHeight;
				coordHeight = theVisualGraphics->getCanvasCoordHeight() - finalMarginTopCoord - finalMarginBottomCoord;
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
				}
			}
		}
	} else if (this->stagePosition.verticalAlignment == kBottomAligned) {
		if (minMarginBottomCoord > marginBottomCoord) {
			finalMarginBottomCoord = minMarginBottomCoord;
		} else {
			finalMarginBottomCoord = marginBottomCoord;
		}
		if (minMarginTopCoord > marginTopCoord) {
			finalMarginTopCoord = minMarginTopCoord;
		} else {
			finalMarginTopCoord = marginTopCoord;
		}
		if (coordHeight < (theVisualGraphics->getCanvasCoordHeight() - (finalMarginTopCoord + finalMarginBottomCoord))) {
			finalMarginTopCoord = 0.0;
		}
	}

	if (this->stagePosition.horizontalAlignment == kLeftAligned) {
		if (minMarginLeftCoord > marginLeftCoord) {
			finalMarginLeftCoord = minMarginLeftCoord;
		} else {
			finalMarginLeftCoord = marginLeftCoord;
		}
		if (minMarginRightCoord > marginRightCoord) {
			finalMarginRightCoord = minMarginRightCoord;
		} else {
			finalMarginRightCoord = marginRightCoord;
		}
		if (coordWidth < (theVisualGraphics->getCanvasCoordWidth() - (finalMarginLeftCoord + finalMarginRightCoord))) {
			finalMarginRightCoord = 0.0;
		}
	} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
/*
		if (coordWidth >= (theVisualGraphics->getCanvasCoordWidth() - (minMarginLeftCoord + minMarginRightCoord))) {
			finalMarginLeftCoord = minMarginLeftCoord;
			finalMarginRightCoord = minMarginRightCoord;
		} else {
			finalMarginLeftCoord = (theVisualGraphics->getCanvasCoordWidth() / 2.0) - (coordWidth / 2.0) - marginLeftCoord;
			finalMarginRightCoord = (theVisualGraphics->getCanvasCoordWidth() / 2.0) - (coordWidth / 2.0) - marginRightCoord;
		}
*/
		finalMarginLeftCoord = (theVisualGraphics->getCanvasCoordWidth() / 2.0) - (coordWidth / 2.0) - marginLeftCoord;
		finalMarginRightCoord = (theVisualGraphics->getCanvasCoordWidth() / 2.0) - (coordWidth / 2.0) - marginRightCoord;
		if (finalMarginLeftCoord < minMarginLeftCoord) {
			finalMarginLeftCoord = minMarginLeftCoord;
		}
		if (finalMarginRightCoord < minMarginRightCoord) {
			finalMarginRightCoord = minMarginRightCoord;
		}
		if ((coordWidth / 2.0) >= (theVisualGraphics->getCanvasCoordWidth() / 2.0) - minMarginRightCoord) {
			finalMarginRightCoord = minMarginRightCoord;
			finalMarginLeftCoord = theVisualGraphics->getCanvasCoordWidth() - (finalMarginRightCoord - coordWidth);
			if ((finalMarginLeftCoord + coordWidth + finalMarginRightCoord) > theVisualGraphics->getCanvasCoordWidth()) {
				finalMarginLeftCoord = minMarginLeftCoord;
				prevCoordVal = coordWidth;
				coordWidth = theVisualGraphics->getCanvasCoordWidth() - finalMarginLeftCoord - finalMarginRightCoord;
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordWidth / prevCoordVal);
				}
			}
		}
	} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
		if (minMarginRightCoord > marginRightCoord) {
			finalMarginRightCoord = minMarginRightCoord;
		} else {
			finalMarginRightCoord = marginRightCoord;
		}
		if (minMarginLeftCoord > marginLeftCoord) {
			finalMarginLeftCoord = minMarginLeftCoord;
		} else {
			finalMarginLeftCoord = marginLeftCoord;
		}
		if (coordWidth < (theVisualGraphics->getCanvasCoordHeight() - (finalMarginLeftCoord + finalMarginRightCoord))) {
			finalMarginLeftCoord = 0.0;
		}
	}

	// position
	if (this->stagePosition.horizontalAlignment == kLeftAligned) {
		left = theVisualGraphics->getMaxLeftCoordOfCanvas() + finalMarginLeftCoord;
		right = left + coordWidth;
	} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
		left = theVisualGraphics->getMaxLeftCoordOfCanvas() + finalMarginLeftCoord;
		right = theVisualGraphics->getMaxRightCoordOfCanvas() - finalMarginRightCoord;
	} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
		right = theVisualGraphics->getMaxRightCoordOfCanvas() - finalMarginRightCoord;
		left = right - coordWidth;
	}
	
	if (this->stagePosition.verticalAlignment == kTopAligned) {
		top = theVisualGraphics->getMaxTopCoordOfCanvas() - finalMarginTopCoord;
		bottom = top - coordHeight;
	} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
		top = theVisualGraphics->getMaxTopCoordOfCanvas() - finalMarginTopCoord;
		bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + finalMarginBottomCoord;
	} else if (this->stagePosition.verticalAlignment == kBottomAligned) {
		bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + finalMarginBottomCoord;
		top = bottom + coordHeight;
	}
	
/*
	if (this->stagePosition.verticalAlignment == kTop) {
		bottom = theVisualGraphics->getMaxTopCoordOfCanvas() - coordHeight - finalMarginTopCoord;
	} else if (this->stagePosition.verticalAlignment == kMiddle) {
		bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + finalMarginBottomCoord + (((theVisualGraphics->getCanvasCoordHeight() - finalMarginBottomCoord) / 2.0) - (coordHeight / 2.0));
		if ((minMarginBottomCoord > marginBottomCoord) && ((theVisualGraphics->getMaxBottomCoordOfCanvas() - bottom) < minMarginBottomCoord)) {
			//bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + minMarginBottomCoord;
		}
	} else if (this->stagePosition.verticalAlignment == kBottom) {
		bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + finalMarginBottomCoord;
	}
	top = bottom + coordHeight;
*/	
/*
	if (this->stagePosition.horizontalAlignment == kLeft) {
		left = theVisualGraphics->getMaxLeftCoordOfCanvas() + finalMarginLeftCoord;
	} else if (this->stagePosition.horizontalAlignment == kCenter) {
		left = (theVisualGraphics->getMaxLeftCoordOfCanvas() + finalMarginLeftCoord + (theVisualGraphics->getCanvasCoordWidth() / 2.0)) - (coordWidth / 2.0);
	} else if (this->stagePosition.horizontalAlignment == kRight) {
		left = theVisualGraphics->getMaxRightCoordOfCanvas() - coordWidth - finalMarginRightCoord;
	}
	right = left + coordWidth;
*/
	// adjust bounds to current bounding rect
	if ((this->scalingBehaviour & kScalingAllowed) == kScalingAllowed) {

		// positioning
/*
		if (this->stagePosition.verticalAlignment == kTop) {
			bottom = theVisualGraphics->getMaxTopCoordOfCanvas() - coordHeight - finalMarginTopCoord;
		} else if (this->stagePosition.verticalAlignment == kMiddle) {
			bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + ((theVisualGraphics->getCanvasCoordHeight() / 2.0) - (coordHeight / 2.0));
		} else if (this->stagePosition.verticalAlignment == kBottom) {
			bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + finalMarginBottomCoord;
		}
		top = bottom + coordHeight;
*/
/*
		if (this->stagePosition.horizontalAlignment == kLeft) {
			left = theVisualGraphics->getMaxLeftCoordOfCanvas() + finalMarginLeftCoord;
		} else if (this->stagePosition.horizontalAlignment == kCenter) {
			left = theVisualGraphics->getMaxLeftCoordOfCanvas() + ((theVisualGraphics->getCanvasCoordWidth() / 2.0) - (coordWidth / 2.0));
		} else if (this->stagePosition.horizontalAlignment == kRight) {
			left = theVisualGraphics->getMaxRightCoordOfCanvas() - coordWidth - finalMarginRightCoord;
		}
		right = left + coordWidth;
*/
		// calc distances
		double distanceBottom = 0.0;
		if (bottom > 0.0) {
			distanceBottom = (theVisualGraphics->getCanvasCoordHeight() / 2.0) + bottom;
		} else {
			distanceBottom = (theVisualGraphics->getMaxBottomCoordOfCanvas() - bottom) * -1.0;
		}
		double distanceTop = 0.0;
		if (top > 0.0) {
			distanceTop = theVisualGraphics->getMaxTopCoordOfCanvas() - top;
		} else {
			distanceTop = (theVisualGraphics->getCanvasCoordHeight() / 2.0) + (top * -1.0);
		}
		double distanceLeft = 0.0;
		if (left > 0.0) {
			distanceLeft = (theVisualGraphics->getCanvasCoordWidth() / 2.0) + left;
		} else {
			distanceLeft = (theVisualGraphics->getMaxLeftCoordOfCanvas() - left) * -1.0;
		}
		double distanceRight = 0.0;
		if (right > 0.0) {
			distanceRight = theVisualGraphics->getMaxRightCoordOfCanvas() - right;
		} else {
			distanceRight = (theVisualGraphics->getCanvasCoordWidth() / 2.0) + (right * -1.0);
		}
		// rescale because minMargin is exceeded?
		//if ((distanceBottom < minMarginBottomCoord) || (distanceTop < minMarginTopCoord) || (distanceLeft < minMarginLeftCoord) || (distanceRight < minMarginRightCoord)) {
		if (1 == 2) {
			prevCoordVal = bottom;
			if (this->stagePosition.verticalAlignment == kTopAligned) {
				bottom = theVisualGraphics->getMaxTopCoordOfCanvas() - coordHeight - marginTopCoord;
			} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
				bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + marginBottomCoord + (((theVisualGraphics->getCanvasCoordHeight() - marginBottomCoord) / 2.0) - (coordHeight / 2.0));
				if ((minMarginBottomCoord > marginBottomCoord) && ((theVisualGraphics->getMaxBottomCoordOfCanvas() - bottom) < minMarginBottomCoord)) {
					bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + minMarginBottomCoord;
				}
			} else if (this->stagePosition.verticalAlignment == kBottomAligned) {
				bottom = theVisualGraphics->getMaxBottomCoordOfCanvas() + marginBottomCoord;
			}
			top -= (prevCoordVal - bottom);
			
			prevCoordVal = left;
			if (this->stagePosition.horizontalAlignment == kLeftAligned) {
				left = theVisualGraphics->getMaxLeftCoordOfCanvas() + minMarginLeftCoord;
			} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
				left = theVisualGraphics->getMaxLeftCoordOfCanvas() + marginLeftCoord + (((theVisualGraphics->getCanvasCoordWidth() - marginLeftCoord) / 2.0) - (coordWidth / 2.0));
				if ((minMarginLeftCoord > marginLeftCoord) && ((theVisualGraphics->getMaxLeftCoordOfCanvas() - left) < minMarginLeftCoord)) {
					left = theVisualGraphics->getMaxLeftCoordOfCanvas() + minMarginLeftCoord;
				}
			} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
				left = theVisualGraphics->getMaxRightCoordOfCanvas() - coordWidth - marginRightCoord;
			}
			right -= (prevCoordVal - left);
			
			// scale?
			if (top > (theVisualGraphics->getMaxTopCoordOfCanvas() - minMarginTopCoord)) {
				prevCoordVal = coordHeight;
				coordHeight = theVisualGraphics->getCanvasCoordHeight() - minMarginTopCoord - minMarginBottomCoord;
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordWidth = coordWidth * (coordHeight / prevCoordVal);
					if (this->stagePosition.horizontalAlignment == kLeftAligned) {
						left = theVisualGraphics->getMaxLeftCoordOfCanvas() + marginLeftCoord;
					} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
						left = (theVisualGraphics->getMaxLeftCoordOfCanvas() + marginLeftCoord + (theVisualGraphics->getCanvasCoordWidth() / 2.0)) - (coordWidth / 2.0);
						//left = theVisualGraphics->getMaxLeftCoordOfCanvas() + marginLeftCoord + (coordWidth / 2.0);
					} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
						left = theVisualGraphics->getMaxRightCoordOfCanvas() - coordWidth - marginRightCoord;
					}
					right = left + coordWidth;
				}
				top = bottom + coordHeight;
			}
			if (right < (theVisualGraphics->getMaxRightCoordOfCanvas() - minMarginRightCoord)) {
				prevCoordVal = coordWidth;
				//coordWidth = theVisualGraphics->getCanvasCoordWidth() - minMarginLeftCoord - minMarginRightCoord;
				/*
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					coordHeight = coordHeight * (coordWidth / prevCoordVal);
					if (this->stagePosition.verticalAlignment == kTop) {
						top = theVisualGraphics->getMaxTopCoordOfCanvas() - marginTopCoord;
					} else if (this->stagePosition.verticalAlignment == kMiddle) {
						top = (theVisualGraphics->getMaxTopCoordOfCanvas() - marginTopCoord - (theVisualGraphics->getCanvasCoordHeight() / 2.0)) + (coordHeight / 2.0);
					} else if (this->stagePosition.verticalAlignment == kBottom) {
						top = theVisualGraphics->getMaxBottomCoordOfCanvas() + coordHeight + marginBottomCoord;
					}
					bottom = top - coordHeight;
				}
				*/
				//left = theVisualGraphics->getMaxLeftCoordOfCanvas() + minMarginLeftCoord;
				//right = left + coordWidth;
			}
		}
	}

	this->topCoord = top;
	this->leftCoord = left;
	this->bottomCoord = bottom;
	this->rightCoord = right;

}


ScalingBehaviour VisualStageBox::getScalingBehaviour() {
	return this->scalingBehaviour;
}


double VisualStageBox::getScaleFactor() {
	return this->scaleFactor;
}


double VisualStageBox::getCoordWidth() {
	this->coordWidth = this->rightCoord - this->leftCoord;
	return this->coordWidth;
}


double VisualStageBox::getCoordHeight() {
	this->coordHeight = this->topCoord - this->bottomCoord;
	return this->coordHeight;
}


double VisualStageBox::getCoordDepth() {
	return this->coordDepth;
}


double VisualStageBox::getTopCoord() {
	return this->topCoord * this->scaleFactor;
}


double VisualStageBox::getLeftCoord() {
	return this->leftCoord * this->scaleFactor;
}


double VisualStageBox::getBottomCoord() {
	return this->bottomCoord * this->scaleFactor;
}


double VisualStageBox::getRightCoord() {
	return this->rightCoord * this->scaleFactor;
}


VisualStagePosition VisualStageBox::getVisualStagePosition() {
	return this->stagePosition;
}

