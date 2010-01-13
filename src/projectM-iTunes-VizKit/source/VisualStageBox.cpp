/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStageBox.cpp
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

#include "VisualStageBox.h"
#include "VisualGraphics.h"
#include "VisualErrorHandling.h"
#include "VisualGraphicTypes.h"
#include "VisualVertex.h"
#include "VisualAsset.h"
#include "VisualImage.h"
#include "VisualCamera.h"

using namespace VizKit;


VisualStageBox::VisualStageBox(VisualAsset* anAssetRef) {

	topCoord = 0.0;
	leftCoord = 0.0;
	bottomCoord = 0.0;
	rightCoord = 0.0;
	
	this->coordWidth = 0.0;
	unscaledCoordWidth = 0.0;
	this->coordHeight = 0.0;
	unscaledCoordHeight = 0.0;
	coordDepth = 0.0;
	unscaledCoordDepth = 0.0;
	
	contentPixelWidth = 0;
	contentPixelHeight = 0;

	scalingBehaviour = kPreserveAspectRatio;
	scaleFactor = 1.0;
	
	hasLayout = false;
	
	assetRef = anAssetRef;

	if (anAssetRef == NULL) {
		char errLog[64];
		sprintf(errLog, "ERR: assetRef == NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}

	debugMode = false;
	
	// initially 2-D
	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	frontPosition = aCamera.getMaxNearPos();
	backPosition = aCamera.getMaxNearPos();
	
}


VisualStageBox::~VisualStageBox() {
	removeAllVertexChains();
}


VisualStageBox::VisualStageBox(const VisualStageBox& other) {
	copy(other);
}


VisualStageBox& VisualStageBox::operator=(const VisualStageBox& other) {
	
	if (this == &other) return *this;
	
	this->removeAllVertexChains();
	this->copy(other);

	return *this;
}


void VisualStageBox::copy(const VisualStageBox& other) {
	this->topCoord = other.topCoord;
	this->leftCoord = other.leftCoord;
	this->bottomCoord = other.bottomCoord;
	this->rightCoord = other.rightCoord;
	
	this->coordWidth = other.coordWidth;
	this->unscaledCoordWidth = other.unscaledCoordWidth;
	this->coordHeight = other.coordHeight;
	this->unscaledCoordHeight = other.unscaledCoordHeight;
	this->coordDepth = other.coordDepth;
	this->unscaledCoordDepth = other.unscaledCoordDepth;
	
	this->contentPixelWidth = other.contentPixelWidth;
	this->contentPixelHeight = other.contentPixelHeight;

	this->stagePosition = other.stagePosition;
	this->scalingBehaviour = other.scalingBehaviour;
	this->scaleFactor = other.scaleFactor;
	
	this->hasLayout = other.hasLayout;
	
	for (ConstVertexChainMapIterator mapIt = other.vertexChainMap.begin(); mapIt != other.vertexChainMap.end(); mapIt++) {
		VisualItemIdentifier mapId = mapIt->first;
		this->vertexChainMap[mapId] = new VertexChain;
		for (ConstVertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			this->vertexChainMap[mapId]->push_back(new VisualVertex(**chainIt));
		}
	}
	
	this->assetRef = other.assetRef;
	
	this->debugMode = other.debugMode;
}


void VisualStageBox::setContentPixelWidth(uint32 pixelWidth) {
	if (this->contentPixelWidth != pixelWidth) {
		this->contentPixelWidth = pixelWidth;
		this->hasLayout = false;
	}
}


void VisualStageBox::setContentPixelHeight(uint32 pixelHeight) {
	if (this->contentPixelHeight != pixelHeight) {
		this->contentPixelHeight = pixelHeight;
		this->hasLayout = false;
	}
}


void VisualStageBox::setCoordDepth(double aCoordDepth) {
	if (this->coordDepth != aCoordDepth) {
		this->coordDepth = aCoordDepth;
		this->unscaledCoordDepth = aCoordDepth;
		this->hasLayout = false;
	}
}


void VisualStageBox::setFrontPosition(double aFrontPosition) {
	if (this->frontPosition != aFrontPosition) {
		this->frontPosition = aFrontPosition;
		this->hasLayout = false;
	}
}


double VisualStageBox::getFrontPosition() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->frontPosition;
}


void VisualStageBox::setBackPosition(double aBackPosition)  {
	if (this->backPosition != aBackPosition) {
		this->backPosition = aBackPosition;
		this->hasLayout = false;
	}
}


double VisualStageBox::getBackPosition(void) {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->backPosition;
}


void VisualStageBox::setVisualStagePosition(const VisualStagePosition& aPosition) {
	if (this->stagePosition != aPosition) {
		this->stagePosition = aPosition;
		this->hasLayout = false;
	}
}


void VisualStageBox::setScalingBehaviour(ScalingBehaviour aScalingBehaviour) {
	if (aScalingBehaviour != this->scalingBehaviour) {
		this->scalingBehaviour = aScalingBehaviour;
		this->hasLayout = false;
	}
}


void VisualStageBox::setScaleFactor(double aScaleFactor) {
	if (aScaleFactor != this->scaleFactor) {
		this->scaleFactor = aScaleFactor;
		this->hasLayout = false;
	}
}


void VisualStageBox::setDirty() {
	this->hasLayout = false;
}


void VisualStageBox::update() {
	this->hasLayout = false;
	this->calcCoords(this->assetRef->getCamera());
}


void VisualStageBox::updateIfNeeded() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
}


void VisualStageBox::calcCoords(const VisualCamera& aCamera) {

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
	
	//printf("hwtest finalMarginLeftCoord1: %f\n", finalMarginLeftCoord);

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	//printf("hwtest contentPixelWidth: %f, getCanvasPixelWidth: %f, getCanvasCoordWidth: %f\n", (double)this->contentPixelWidth, (double)theVisualGraphics->getCanvasPixelWidth(), size.width);

	//printf("hwtest coordWidth1: %f\n", this->coordWidth);
	//this->contentPixelWidth = 0;
	//printf("hwtesthwtest: %ld, %f\n", this->contentPixelWidth, (double)this->contentPixelWidth);

	CoordSize3D size = aCamera.getSize();
	this->coordWidth = size.width * ((double)this->contentPixelWidth / (double)theVisualGraphics->getCanvasPixelWidth());
	//printf("hwtest coordWidth2: %f\n", this->coordWidth);
	this->coordHeight = size.height * ((double)this->contentPixelHeight / (double)theVisualGraphics->getCanvasPixelHeight());

	// calc margin values
	if (this->stagePosition.marginBottom != 0.0) {
		if (this->stagePosition.marginBottomUnit == kPercent) {
			marginBottomCoord = size.height * (this->stagePosition.marginBottom / 100.0);
		} else if (this->stagePosition.marginBottomUnit == kPixel) {
			marginBottomCoord = theVisualGraphics->yPixelToCoord((uint16)this->stagePosition.marginBottom, aCamera);
		}
	}

	if (this->stagePosition.marginTop != 0.0) {
		if (this->stagePosition.marginBottomUnit == kPercent) {
			marginTopCoord = size.height * (this->stagePosition.marginBottom / 100.0);
		} else if (this->stagePosition.marginTopUnit == kPixel) {
			marginTopCoord = theVisualGraphics->yPixelToCoord((uint16)this->stagePosition.marginTop, aCamera);
		}
	}

	if (this->stagePosition.marginLeft != 0.0) {
		if (this->stagePosition.marginLeftUnit == kPercent) {
			marginLeftCoord = size.width * (this->stagePosition.marginLeft / 100.0);
		} else if (this->stagePosition.marginLeftUnit == kPixel) {
			marginLeftCoord = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.marginLeft, aCamera);
		}
	}

	if (this->stagePosition.marginRight != 0.0) {
		if (this->stagePosition.marginRightUnit == kPercent) {
			marginRightCoord = size.width * (this->stagePosition.marginRight / 100.0);
		} else if (this->stagePosition.marginRightUnit == kPixel) {
			marginRightCoord = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.marginRight, aCamera);
		}
	}

	if (this->stagePosition.minMarginBottom != 0.0) {
		if (this->stagePosition.minMarginBottomUnit == kPercent) {
			minMarginBottomCoord = size.height * (this->stagePosition.minMarginBottom / 100.0);
		} else if (this->stagePosition.minMarginBottomUnit == kPixel) {
			minMarginBottomCoord = theVisualGraphics->yPixelToCoord((uint16)this->stagePosition.minMarginBottom, aCamera);
		}
	}
	
	if (this->stagePosition.minMarginTop != 0.0) {
		if (this->stagePosition.minMarginTopUnit == kPercent) {
			minMarginTopCoord = size.height * (this->stagePosition.minMarginTop / 100.0);
		} else if (this->stagePosition.minMarginTopUnit == kPixel) {
			minMarginTopCoord = theVisualGraphics->yPixelToCoord((uint16)this->stagePosition.minMarginTop, aCamera);
		}
	}

	if (this->stagePosition.minMarginLeft != 0.0) {
		if (this->stagePosition.minMarginLeftUnit == kPercent) {
			minMarginLeftCoord = size.width * (this->stagePosition.minMarginLeft / 100.0);
		} else if (this->stagePosition.minMarginLeftUnit == kPixel) {
			minMarginLeftCoord = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.minMarginLeft, aCamera);
		}
	}

	if (this->stagePosition.minMarginRight != 0.0) {
		if (this->stagePosition.minMarginRightUnit == kPercent) {
			minMarginRightCoord = size.width * (this->stagePosition.minMarginRight / 100.0);
		} else if (this->stagePosition.minMarginRightUnit == kPixel) {
			minMarginRightCoord = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.minMarginRight, aCamera);
		}
	}
	
	//printf("hwtest finalMarginLeftCoord2: %f\n", finalMarginLeftCoord);
	//printf("hwtest coordWidth102: %f\n", this->coordWidth);
	
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
	// wider than canvas
	if (this->coordWidth > (size.width - minMarginRightCoord - minMarginLeftCoord)) {
		prevCoordVal = this->coordWidth;
		this->coordWidth = size.width - minMarginRightCoord - minMarginLeftCoord;
		if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
			if (prevCoordVal > 0.0) {
				this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
			}
		}
		finalMarginLeftCoord = minMarginLeftCoord;
		finalMarginRightCoord = minMarginRightCoord;
	}
	// higher than canvas
	if (this->coordHeight > (size.height - minMarginTopCoord - minMarginBottomCoord)) {
		prevCoordVal = this->coordHeight;
		this->coordHeight = size.height - minMarginTopCoord - minMarginBottomCoord;
		if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
			if (prevCoordVal > 0.0) {
				this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
			}
		}
		finalMarginTopCoord = minMarginTopCoord;
		finalMarginBottomCoord = minMarginBottomCoord;
	}
	
	if (this->coordWidth == 0.0) {
		this->coordWidth = size.width - finalMarginLeftCoord - finalMarginRightCoord;
	}
	if (this->coordHeight == 0.0) {
		this->coordHeight = size.height - finalMarginTopCoord - finalMarginBottomCoord;
	}

	// calc coordWidth and coordHeight
	if ((this->scalingBehaviour & kScalingAllowed) == kScalingAllowed) {

		// < minWidth
		if (this->stagePosition.minWidth > 0.0) {
			prevCoordVal = this->coordWidth;
			if ((this->stagePosition.minWidthUnit == kPercent) && ((this->coordWidth / size.width * 100.0) < this->stagePosition.minWidth)) {
				this->coordWidth = this->stagePosition.minWidth * size.width / 100.0;
				if (this->coordWidth > (size.width - (minMarginLeftCoord + minMarginRightCoord))) {
					this->coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
				} else {
					this->coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
					}
				}
			} else if ((this->stagePosition.minWidthUnit == kPixel) && (static_cast<double>(theVisualGraphics->xCoordToPixel(this->coordWidth, aCamera)) < this->stagePosition.minWidth)) {
				this->coordWidth = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.minWidth, aCamera);
				if (this->coordWidth > (size.width - (minMarginLeftCoord + minMarginRightCoord))) {
					this->coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
				} else {
					this->coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
					}
				}
			}
		}
		
		//printf("hwtest coordWidth103: %f (%f)\n", this->coordWidth, this->stagePosition.minHeight);

		// < minHeight
		if (this->stagePosition.minHeight > 0.0) {
			prevCoordVal = this->coordHeight;
			if ((this->stagePosition.minHeightUnit == kPercent) && ((this->coordHeight / size.height * 100.0) < this->stagePosition.minHeight)) {
				this->coordHeight = this->stagePosition.minHeight * size.height / 100.0;
				if (this->coordHeight > (size.height - (minMarginTopCoord + minMarginBottomCoord))) {
					this->coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					this->coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
						//printf("hwtest coordWidth103a: %f\n", this->coordWidth);
					}
				}
			} else if ((this->stagePosition.minHeightUnit == kPixel) && (static_cast<double>(theVisualGraphics->yCoordToPixel(this->coordHeight, aCamera)) < this->stagePosition.minHeight)) {
				this->coordHeight = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.minHeight, aCamera);
				if (this->coordHeight > (size.height - (minMarginTopCoord + minMarginBottomCoord))) {
					this->coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					this->coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
						//printf("hwtest coordWidth103b: %f\n", this->coordWidth);
					}
				}
			}
		}

		// > maxWidth
		if (this->stagePosition.maxWidth > 0.0) {
			prevCoordVal = this->coordWidth;
			if ((this->stagePosition.maxWidthUnit == kPercent) && ((this->coordWidth / size.width * 100.0) > this->stagePosition.maxWidth)) {
				this->coordWidth = this->stagePosition.maxWidth * size.width / 100.0;
				//printf("hwtest coordWidth103c: %f\n", this->coordWidth);
				if (this->coordWidth > (size.width - (minMarginLeftCoord + minMarginRightCoord))) {
					this->coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
					//printf("hwtest coordWidth103d: %f\n", this->coordWidth);
				} else {
					this->coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
					//printf("hwtest coordWidth103e: %f\n", this->coordWidth);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
					}
				}
			} else if ((this->stagePosition.maxWidthUnit == kPixel) && (static_cast<double>(theVisualGraphics->xCoordToPixel(this->coordWidth, aCamera)) > this->stagePosition.maxWidth)) {
				this->coordWidth = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.maxWidth, aCamera);
				if (this->coordWidth > (size.width - (minMarginLeftCoord + minMarginRightCoord))) {
					this->coordWidth -= (minMarginLeftCoord + minMarginRightCoord);
					//printf("hwtest coordWidth103f: %f\n", this->coordWidth);
				} else {
					this->coordWidth -= (finalMarginLeftCoord + finalMarginRightCoord);
					//printf("hwtest coordWidth103g: %f\n", this->coordWidth);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
					}
				}
			}
		}
		
		//printf("hwtest coordWidth104: %f\n", this->coordWidth);

		// > maxHeight
		if (this->stagePosition.maxHeight > 0.0) {
			prevCoordVal = this->coordHeight;
			if ((this->stagePosition.maxHeightUnit == kPercent) && ((this->coordHeight / size.height * 100.0) > this->stagePosition.maxHeight)) {
				this->coordHeight = this->stagePosition.maxHeight * size.height / 100.0;
				if (this->coordHeight > (size.height - (minMarginTopCoord + minMarginBottomCoord))) {
					this->coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					this->coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
					}
				}
			} else if ((this->stagePosition.maxHeightUnit == kPixel) && (static_cast<double>(theVisualGraphics->yCoordToPixel(this->coordHeight, aCamera)) > this->stagePosition.maxHeight)) {
				this->coordHeight = theVisualGraphics->xPixelToCoord((uint16)this->stagePosition.maxHeight, aCamera);
				if (this->coordHeight > (size.height - (minMarginTopCoord + minMarginBottomCoord))) {
					this->coordHeight -= (minMarginTopCoord + minMarginBottomCoord);
				} else {
					this->coordHeight -= (finalMarginTopCoord + finalMarginBottomCoord);
				}
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						//this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal * (size.width / size.height));
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
					}
				}
			}
		}
		
		//printf("hwtest coordWidth105: %f\n", this->coordWidth);
		
		//printf("hwtest finalMarginLeftCoord3: %f\n", finalMarginLeftCoord);
		
		if (!((this->scalingBehaviour & kClippingAllowed) == kClippingAllowed)) {
			if (this->coordWidth > (size.width - (minMarginLeftCoord + minMarginRightCoord))) {
				prevCoordVal = this->coordWidth;
				this->coordWidth = size.width - (minMarginLeftCoord + minMarginRightCoord);
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
					}
				}
			}
			if (this->coordHeight > (size.height - (minMarginTopCoord + minMarginBottomCoord))) {
				prevCoordVal = this->coordHeight;
				this->coordHeight = size.height - (minMarginTopCoord + minMarginBottomCoord);
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
					}
				}
			}
		}
		
/*
		if (!((this->scalingBehaviour & kClippingAllowed) == kClippingAllowed)) {
			double aspectRatioCanvas = size.width / size.height;
			double aspectRatioCover = coverArtTextureContainer->getImageWidth() / coverArtTextureContainer->getImageHeight();
			coordWidthUnscaled = (double)size.width * ((double)coverArtTextureContainer->getImageWidth() / (double)theVisualGraphics->getCanvasPixelWidth());
			coordHeightUnscaled = (double)size.height * ((double)coverArtTextureContainer->getImageHeight() / (double)theVisualGraphics->getCanvasPixelHeight());
			if (aspectRatioCanvas > aspectRatioCover) {
				prevCoordVal = this->coordWidth;
				this->coordWidth = (double)size.width;
				this->coordHeight = coordHeightUnscaled * (this->coordWidth / coordWidthUnscaled);
			} else {
				this->coordHeight = (double)size.height;
				this->coordWidth = coordWidthUnscaled * (this->coordHeight / coordHeightUnscaled);
			}
		}
*/
	}
	
	//printf("hwtest finalMarginLeftCoord4: %f\n", finalMarginLeftCoord);
	
	//printf("this->coordHeight: %f, this->coordWidth: %f\n", this->coordHeight, this->coordWidth);

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
		if (this->coordHeight < (size.height - (finalMarginTopCoord + finalMarginBottomCoord))) {
			finalMarginBottomCoord = 0.0;
		}
	} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
		finalMarginTopCoord = (size.height / 2.0) - (this->coordHeight / 2.0) - marginTopCoord;
		finalMarginBottomCoord = (size.height / 2.0) - (this->coordHeight / 2.0) - marginBottomCoord;
		if (finalMarginTopCoord < minMarginTopCoord) {
			finalMarginTopCoord = minMarginTopCoord;
		}
		if (finalMarginBottomCoord < minMarginBottomCoord) {
			finalMarginBottomCoord = minMarginBottomCoord;
		}
		if ((this->coordHeight / 2.0) >= (size.height / 2.0) - minMarginBottomCoord) {
			finalMarginBottomCoord = minMarginBottomCoord;
			finalMarginTopCoord = size.height - (finalMarginBottomCoord + this->coordHeight);
			if ((finalMarginTopCoord + this->coordHeight + finalMarginBottomCoord) > size.height) {
				finalMarginTopCoord = minMarginTopCoord;
				prevCoordVal = this->coordHeight;
				this->coordHeight = size.height - finalMarginTopCoord - finalMarginBottomCoord;
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
					}
				}
			}
		}
		//printf("hwtest coordWidth106: %f\n", this->coordWidth);
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
		if (this->coordHeight < (size.height - (finalMarginTopCoord + finalMarginBottomCoord))) {
			finalMarginTopCoord = 0.0;
		}
	}

	if (this->stagePosition.horizontalAlignment == kLeftAligned) {
		//printf("hwtest finalMarginLeftCoord4a: %f\n", finalMarginLeftCoord);
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
		if (this->coordWidth < (size.width - (finalMarginLeftCoord + finalMarginRightCoord))) {
			finalMarginRightCoord = 0.0;
		}
		//printf("hwtest finalMarginLeftCoord4b: %f\n", finalMarginLeftCoord);
	} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
/*
		if (this->coordWidth >= (size.width - (minMarginLeftCoord + minMarginRightCoord))) {
			finalMarginLeftCoord = minMarginLeftCoord;
			finalMarginRightCoord = minMarginRightCoord;
		} else {
			finalMarginLeftCoord = (size.width / 2.0) - (this->coordWidth / 2.0) - marginLeftCoord;
			finalMarginRightCoord = (size.width / 2.0) - (this->coordWidth / 2.0) - marginRightCoord;
		}
*/
		//printf("hwtest finalMarginLeftCoord4c1: %f , %f , %f\n", finalMarginLeftCoord, this->coordWidth, marginLeftCoord);
		finalMarginLeftCoord = (size.width / 2.0) - (this->coordWidth / 2.0) - marginLeftCoord;
		//printf("hwtest finalMarginLeftCoord4c2: %f\n", finalMarginLeftCoord);
		finalMarginRightCoord = (size.width / 2.0) - (this->coordWidth / 2.0) - marginRightCoord;
		if (finalMarginLeftCoord < minMarginLeftCoord) {
			finalMarginLeftCoord = minMarginLeftCoord;
			//printf("hwtest finalMarginLeftCoord4c3: %f\n", finalMarginLeftCoord);
		}
		if (finalMarginRightCoord < minMarginRightCoord) {
			finalMarginRightCoord = minMarginRightCoord;
		}
		//printf("hwtest finalMarginLeftCoord4d: %f\n", finalMarginLeftCoord);
		if ((this->coordWidth / 2.0) >= (size.width / 2.0) - minMarginRightCoord) {
			finalMarginRightCoord = minMarginRightCoord;
			finalMarginLeftCoord = size.width - (finalMarginRightCoord - this->coordWidth);
			if ((finalMarginLeftCoord + this->coordWidth + finalMarginRightCoord) > size.width) {
				finalMarginLeftCoord = minMarginLeftCoord;
				prevCoordVal = this->coordWidth;
				this->coordWidth = size.width - finalMarginLeftCoord - finalMarginRightCoord;
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordWidth / prevCoordVal);
					}
				}
			}
		}
		//printf("hwtest finalMarginLeftCoord4e: %f\n", finalMarginLeftCoord);
	} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
		//printf("hwtest finalMarginLeftCoord4f: %f\n", finalMarginLeftCoord);
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
		if (this->coordWidth < (size.height - (finalMarginLeftCoord + finalMarginRightCoord))) {
			finalMarginLeftCoord = 0.0;
		}
		//printf("hwtest finalMarginLeftCoord4g: %f\n", finalMarginLeftCoord);
	}
	
	//printf("hwtest finalMarginLeftCoord5: %f\n", finalMarginLeftCoord);
	
	
	//printf("hwtest left1: %f\n", left);
	//printf("hwtest a: %f, b: %f\n", aCamera.getMaxLeftCoord(), finalMarginLeftCoord);

	// position
	if (this->stagePosition.horizontalAlignment == kLeftAligned) {
		left = aCamera.getMaxLeftCoord() + finalMarginLeftCoord;
		right = left + this->coordWidth;
	} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
		left = aCamera.getMaxLeftCoord() + finalMarginLeftCoord;
		right = aCamera.getMaxRightCoord() - finalMarginRightCoord;
	} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
		right = aCamera.getMaxRightCoord() - finalMarginRightCoord;
		left = right - this->coordWidth;
	}
	
	//printf("hwtest left2: %f\n", left);
	
	if (this->stagePosition.verticalAlignment == kTopAligned) {
		top = aCamera.getMaxTopCoord() - finalMarginTopCoord;
		bottom = top - this->coordHeight;
	} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
		//printf("hwtest aCamera.getMaxTopCoord(): %f, aCamera.getMaxBottomCoord(): %f\n", aCamera.getMaxTopCoord(), aCamera.getMaxBottomCoord());
		//printf("finalMarginTopCoord: %f\n", finalMarginTopCoord);
		top = aCamera.getMaxTopCoord() - finalMarginTopCoord;
		bottom = aCamera.getMaxBottomCoord() + finalMarginBottomCoord;
	} else if (this->stagePosition.verticalAlignment == kBottomAligned) {
		bottom = aCamera.getMaxBottomCoord() + finalMarginBottomCoord;
		top = bottom + this->coordHeight;
	}
	
	//printf("hwtest coordWidth2a: %f, coordHeight2a: %f\n", this->coordWidth, this->coordHeight);
	//printf("hwtest top1: %f, bottom1: %f, left1: %f, right1: %f\n", top, bottom, left, right);
	
/*
	if (this->stagePosition.verticalAlignment == kTop) {
		bottom = aCamera.getMaxTopCoord() - this->coordHeight - finalMarginTopCoord;
	} else if (this->stagePosition.verticalAlignment == kMiddle) {
		bottom = aCamera.getMaxBottomCoord() + finalMarginBottomCoord + (((size.height - finalMarginBottomCoord) / 2.0) - (this->coordHeight / 2.0));
		if ((minMarginBottomCoord > marginBottomCoord) && ((aCamera.getMaxBottomCoord() - bottom) < minMarginBottomCoord)) {
			//bottom = aCamera.getMaxBottomCoord() + minMarginBottomCoord;
		}
	} else if (this->stagePosition.verticalAlignment == kBottom) {
		bottom = aCamera.getMaxBottomCoord() + finalMarginBottomCoord;
	}
	top = bottom + this->coordHeight;
*/	
/*
	if (this->stagePosition.horizontalAlignment == kLeft) {
		left = aCamera.getMaxLeftCoord() + finalMarginLeftCoord;
	} else if (this->stagePosition.horizontalAlignment == kCenter) {
		left = (aCamera.getMaxLeftCoord() + finalMarginLeftCoord + (size.width / 2.0)) - (this->coordWidth / 2.0);
	} else if (this->stagePosition.horizontalAlignment == kRight) {
		left = aCamera.getMaxRightCoord() - this->coordWidth - finalMarginRightCoord;
	}
	right = left + this->coordWidth;
*/
	// adjust bounds to current bounding rect
	if ((this->scalingBehaviour & kScalingAllowed) == kScalingAllowed) {

		// positioning
/*
		if (this->stagePosition.verticalAlignment == kTop) {
			bottom = aCamera.getMaxTopCoord() - this->coordHeight - finalMarginTopCoord;
		} else if (this->stagePosition.verticalAlignment == kMiddle) {
			bottom = aCamera.getMaxBottomCoord() + ((size.height / 2.0) - (this->coordHeight / 2.0));
		} else if (this->stagePosition.verticalAlignment == kBottom) {
			bottom = aCamera.getMaxBottomCoord() + finalMarginBottomCoord;
		}
		top = bottom + this->coordHeight;
*/
/*
		if (this->stagePosition.horizontalAlignment == kLeft) {
			left = aCamera.getMaxLeftCoord() + finalMarginLeftCoord;
		} else if (this->stagePosition.horizontalAlignment == kCenter) {
			left = aCamera.getMaxLeftCoord() + ((size.width / 2.0) - (this->coordWidth / 2.0));
		} else if (this->stagePosition.horizontalAlignment == kRight) {
			left = aCamera.getMaxRightCoord() - this->coordWidth - finalMarginRightCoord;
		}
		right = left + this->coordWidth;
*/
		// calc distances
		double distanceBottom = 0.0;
		if (bottom > 0.0) {
			distanceBottom = (size.height / 2.0) + bottom;
		} else {
			distanceBottom = (aCamera.getMaxBottomCoord() - bottom) * -1.0;
		}
		double distanceTop = 0.0;
		if (top > 0.0) {
			distanceTop = aCamera.getMaxTopCoord() - top;
		} else {
			distanceTop = (size.height / 2.0) + (top * -1.0);
		}
		double distanceLeft = 0.0;
		if (left > 0.0) {
			distanceLeft = (size.width / 2.0) + left;
		} else {
			distanceLeft = (aCamera.getMaxLeftCoord() - left) * -1.0;
		}
		double distanceRight = 0.0;
		if (right > 0.0) {
			distanceRight = aCamera.getMaxRightCoord() - right;
		} else {
			distanceRight = (size.width / 2.0) + (right * -1.0);
		}
		// rescale because minMargin is exceeded?
		//if ((distanceBottom < minMarginBottomCoord) || (distanceTop < minMarginTopCoord) || (distanceLeft < minMarginLeftCoord) || (distanceRight < minMarginRightCoord)) {
		if (1 == 2) {
			prevCoordVal = bottom;
			if (this->stagePosition.verticalAlignment == kTopAligned) {
				bottom = aCamera.getMaxTopCoord() - this->coordHeight - marginTopCoord;
			} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
				bottom = aCamera.getMaxBottomCoord() + marginBottomCoord + (((size.height - marginBottomCoord) / 2.0) - (this->coordHeight / 2.0));
				if ((minMarginBottomCoord > marginBottomCoord) && ((aCamera.getMaxBottomCoord() - bottom) < minMarginBottomCoord)) {
					bottom = aCamera.getMaxBottomCoord() + minMarginBottomCoord;
				}
			} else if (this->stagePosition.verticalAlignment == kBottomAligned) {
				bottom = aCamera.getMaxBottomCoord() + marginBottomCoord;
			}
			top -= (prevCoordVal - bottom);
			
			prevCoordVal = left;
			if (this->stagePosition.horizontalAlignment == kLeftAligned) {
				left = aCamera.getMaxLeftCoord() + minMarginLeftCoord;
			} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
				left = aCamera.getMaxLeftCoord() + marginLeftCoord + (((size.width - marginLeftCoord) / 2.0) - (this->coordWidth / 2.0));
				if ((minMarginLeftCoord > marginLeftCoord) && ((aCamera.getMaxLeftCoord() - left) < minMarginLeftCoord)) {
					left = aCamera.getMaxLeftCoord() + minMarginLeftCoord;
				}
			} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
				left = aCamera.getMaxRightCoord() - this->coordWidth - marginRightCoord;
			}
			right -= (prevCoordVal - left);
			
			// scale?
			if (top > (aCamera.getMaxTopCoord() - minMarginTopCoord)) {
				prevCoordVal = this->coordHeight;
				this->coordHeight = size.height - minMarginTopCoord - minMarginBottomCoord;
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					if (prevCoordVal > 0.0) {
						this->coordWidth = this->coordWidth * (this->coordHeight / prevCoordVal);
					}
					if (this->stagePosition.horizontalAlignment == kLeftAligned) {
						left = aCamera.getMaxLeftCoord() + marginLeftCoord;
					} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
						left = (aCamera.getMaxLeftCoord() + marginLeftCoord + (size.width / 2.0)) - (this->coordWidth / 2.0);
						//left = aCamera.getMaxLeftCoord() + marginLeftCoord + (this->coordWidth / 2.0);
					} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
						left = aCamera.getMaxRightCoord() - this->coordWidth - marginRightCoord;
					}
					right = left + this->coordWidth;
				}
				top = bottom + this->coordHeight;
			}
			if (right < (aCamera.getMaxRightCoord() - minMarginRightCoord)) {
				prevCoordVal = this->coordWidth;
				//this->coordWidth = size.width - minMarginLeftCoord - minMarginRightCoord;
				/*
				if ((this->scalingBehaviour & kPreserveAspectRatio) == kPreserveAspectRatio) {
					this->coordHeight = this->coordHeight * (this->coordWidth / prevCoordVal);
					if (this->stagePosition.verticalAlignment == kTop) {
						top = aCamera.getMaxTopCoord() - marginTopCoord;
					} else if (this->stagePosition.verticalAlignment == kMiddle) {
						top = (aCamera.getMaxTopCoord() - marginTopCoord - (size.height / 2.0)) + (this->coordHeight / 2.0);
					} else if (this->stagePosition.verticalAlignment == kBottom) {
						top = aCamera.getMaxBottomCoord() + this->coordHeight + marginBottomCoord;
					}
					bottom = top - this->coordHeight;
				}
				*/
				//left = aCamera.getMaxLeftCoord() + minMarginLeftCoord;
				//right = left + this->coordWidth;
			}
		}
	}
	
	//printf("hwtest coordWidth2b: %f, coordHeight2b: %f\n", this->coordWidth, this->coordHeight);
	//printf("hwtest top2: %f, bottom2: %f, left2: %f, right2: %f\n", top, bottom, left, right);

	this->topCoord = top;
	this->bottomCoord = bottom;
	this->leftCoord = left;
	this->rightCoord = right;

	if (this->stagePosition.depthAlignment == kFrontAligned) {
		this->frontPosition = aCamera.getMaxNearPos();
		this->frontPosition += 0.01;
		this->frontPosition *= -1.0;
		this->backPosition = this->frontPosition - this->unscaledCoordDepth;
	} else if (this->stagePosition.depthAlignment == kDepthCenterAligned) {
		/*
		double zCoordAmount = aCamera.getMaxFarPos() - aCamera.getMaxNearPos();
		double zCenter = aCamera.getMaxNearPos() + (zCoordAmount / 2.0);
		this->frontPosition = zCenter - (this->coordDepth / 2.0);
		this->backPosition = zCenter + (this->coordDepth / 2.0);
		*/
		
		this->frontPosition = 0.0 + (this->unscaledCoordDepth / 2.0);
		this->backPosition = 0.0 - (this->unscaledCoordDepth / 2.0);
		//printf("hwtest: kDepthCenterAligned1 font: %f, back: %f\n", this->frontPosition, this->backPosition);
	} else if (this->stagePosition.depthAlignment == kBackAligned) {
		this->backPosition = aCamera.getMaxFarPos();
		this->frontPosition = this->backPosition - this->unscaledCoordDepth;
	}

	this->coordWidth = this->rightCoord - this->leftCoord;
	this->unscaledCoordWidth = this->coordWidth;
	this->coordHeight = this->topCoord - this->bottomCoord;
	this->unscaledCoordHeight = this->coordHeight;
	
	this->leftCoord += this->stagePosition.horizontalCoordOffset;
	this->rightCoord += this->stagePosition.horizontalCoordOffset;
	this->bottomCoord += this->stagePosition.verticalCoordOffset;
	this->topCoord += this->stagePosition.verticalCoordOffset;
	
	//printf("hwtest coordWidth2z: %f, coordHeight2z: %f\n", this->coordWidth, this->coordHeight);
	
	//printf("hwtest top3: %f, bottom3: %f, left3: %f, right3: %f\n", this->topCoord, this->bottomCoord, this->leftCoord, this->rightCoord);

	this->applyScaleFactor();
	
	//printf("hwtest top4: %f, bottom4: %f, left4: %f, right4: %f\n", this->topCoord, this->bottomCoord, this->leftCoord, this->rightCoord);
	
	this->hasLayout = true;
	
	this->updateVertices();
	
	//printf("hwtest top5: %f, bottom5: %f, left5: %f, right5: %f\n", this->topCoord, this->bottomCoord, this->leftCoord, this->rightCoord);
	
	//printf("hwtest coordWidth3: %f, coordHeight3: %f\n", this->coordWidth, this->coordHeight);
	
}


void VisualStageBox::applyScaleFactor() {

	double top = this->topCoord;
	double left = this->leftCoord;
	double bottom = this->bottomCoord;
	double right = this->rightCoord;
	double front = this->frontPosition;
	double back = this->backPosition;
	
	if (this->stagePosition.horizontalAlignment == kLeftAligned) {
		right = left + (this->unscaledCoordWidth * this->scaleFactor);
	} else if (this->stagePosition.horizontalAlignment == kCenterAligned) {
		double widthDelta = this->unscaledCoordWidth - (this->unscaledCoordWidth * this->scaleFactor);
		widthDelta /= 2.0;
		left = this->leftCoord + widthDelta;
		right = this->rightCoord - widthDelta;
		//printf("hwtest left: %f, right: %f\n", left, right);
	} else if (this->stagePosition.horizontalAlignment == kRightAligned) {
		left = right - (this->unscaledCoordWidth * this->scaleFactor);
	}
	
	if (this->stagePosition.verticalAlignment == kTopAligned) {
		bottom = top - (this->unscaledCoordHeight * this->scaleFactor);
	} else if (this->stagePosition.verticalAlignment == kMiddleAligned) {
		double heightDelta = this->unscaledCoordHeight - (this->unscaledCoordHeight * this->scaleFactor);
		heightDelta /= 2.0;
		bottom = this->bottomCoord + heightDelta;
		top = this->topCoord - heightDelta;
	} else if (this->stagePosition.verticalAlignment == kBottomAligned) {
		top = bottom + (this->unscaledCoordHeight * this->scaleFactor);
	}

	if (this->stagePosition.depthAlignment == kFrontAligned) {
		back = front - (this->unscaledCoordDepth * this->scaleFactor);
	} else if (this->stagePosition.depthAlignment == kDepthCenterAligned) {
		//printf("hwtest: kDepthCenterAligned1: front: %f, back: %f\n", front, back);
		double value = this->unscaledCoordDepth / 2.0 * this->scaleFactor;
		front = 0.0 + (value);
		back = 0.0 - (value);
		//printf("hwtest: kDepthCenterAligned2: front: %f, back: %f\n", front, back);
	} else if (this->stagePosition.depthAlignment == kBackAligned) {
		front = back + (this->unscaledCoordDepth * this->scaleFactor);
	}

	this->topCoord = top;
	this->leftCoord = left;
	this->bottomCoord = bottom;
	this->rightCoord = right;
	this->frontPosition = front;
	this->backPosition = back;

	this->coordWidth = this->rightCoord - this->leftCoord;
	this->coordHeight = this->topCoord - this->bottomCoord;
	this->coordDepth = this->backPosition - this->frontPosition;

}


ScalingBehaviour VisualStageBox::getScalingBehaviour() const {
	return this->scalingBehaviour;
}


double VisualStageBox::getScaleFactor() const {
	return this->scaleFactor;
}


double VisualStageBox::getCoordWidth() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->coordWidth;
}


double VisualStageBox::getUnscaledCoordWidth() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->unscaledCoordWidth;
}


double VisualStageBox::getCoordHeight() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->coordHeight;
}


double VisualStageBox::getUnscaledCoordHeight() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->unscaledCoordHeight;
}


double VisualStageBox::getCoordDepth() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->coordDepth;
}


double VisualStageBox::getUnscaledCoordDepth() {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	return this->unscaledCoordDepth;
}


double VisualStageBox::getTopCoord(const VisualItemIdentifier* const vertexChainName) {
	double top = 0.0;
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	if (vertexChainName == NULL) {
		top = this->topCoord;
	} else {
		VertexChain* vertexChain = this->getVertexChain(*vertexChainName);
		if (vertexChain != NULL) {
			for (VertexChainIterator chainIt = vertexChain->begin(); chainIt != vertexChain->end(); chainIt++) {
				if ((*chainIt)->vertexPosition.coord.y > top) {
					top = (*chainIt)->vertexPosition.coord.y;
				}
			}
		}
	}
	return top;
}


double VisualStageBox::getLeftCoord(const VisualItemIdentifier* const vertexChainName) {
	double left = 0.0;
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	if (vertexChainName == NULL) {
		left = this->leftCoord;
	} else {
		VertexChain* vertexChain = this->getVertexChain(*vertexChainName);
		if (vertexChain != NULL) {
			for (VertexChainIterator chainIt = vertexChain->begin(); chainIt != vertexChain->end(); chainIt++) {
				if ((*chainIt)->vertexPosition.coord.x < left) {
					left = (*chainIt)->vertexPosition.coord.x;
				}
			}
		}
	}
	return left;
}


double VisualStageBox::getBottomCoord(const VisualItemIdentifier* const vertexChainName) {
	double bottom = 0.0;
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	if (vertexChainName == NULL) {
		bottom = this->bottomCoord;
	} else {
		VertexChain* vertexChain = this->getVertexChain(*vertexChainName);
		if (vertexChain != NULL) {
			for (VertexChainIterator chainIt = vertexChain->begin(); chainIt != vertexChain->end(); chainIt++) {
				if ((*chainIt)->vertexPosition.coord.y < bottom) {
					bottom = (*chainIt)->vertexPosition.coord.x;
				}
			}
		}
	}
	return bottom;
}


double VisualStageBox::getRightCoord(const VisualItemIdentifier* const vertexChainName) {
	double right = 0.0;
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	if (vertexChainName == NULL) {
		right = this->rightCoord;
	} else {
		VertexChain* vertexChain = this->getVertexChain(*vertexChainName);
		if (vertexChain != NULL) {
			for (VertexChainIterator chainIt = vertexChain->begin(); chainIt != vertexChain->end(); chainIt++) {
				if ((*chainIt)->vertexPosition.coord.x > right) {
					right = (*chainIt)->vertexPosition.coord.x;
				}
			}
		}
	}
	return right;
}


uint32 VisualStageBox::getContentPixelWidth() const {
	return this->contentPixelWidth;
}


uint32 VisualStageBox::getContentPixelHeight() const {
	return this->contentPixelHeight;
}


VisualStagePosition VisualStageBox::getVisualStagePosition() const {
	return this->stagePosition;
}


void VisualStageBox::initializeVertexChain(const VisualItemIdentifier& vertexChainName) {
	this->vertexChainMap[vertexChainName] = new VertexChain;
}
		

void VisualStageBox::addVertexToChain(const VisualItemIdentifier& vertexChainName, VisualVertex* aVertex) {
	this->hasLayout = false;
	this->vertexChainMap[vertexChainName]->push_back(aVertex);
}


VertexChain* VisualStageBox::getVertexChain(const VisualItemIdentifier& vertexChainName) {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	VertexChain* vertexChainRef = NULL;
	ConstVertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		vertexChainRef = mapIt->second;
	} else {
		char errLog[64];
		sprintf(errLog, "unable to find vertex chain to return in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}
	return vertexChainRef;
}


VertexChain* VisualStageBox::getVertexChain(size_t vertexIdx) {
	if (this->hasLayout == false) {
		this->calcCoords(this->assetRef->getCamera());
	}
	if (vertexIdx > this->vertexChainMap.size() - 1) return NULL;
	size_t count = 0;
	for (VertexChainMapIterator mapIt = this->vertexChainMap.begin(); mapIt != this->vertexChainMap.end(); mapIt++) {
		if (count == vertexIdx) {
			return mapIt->second;
		}
		count++;
	}
	return NULL;
}


const VisualItemIdentifier& VisualStageBox::getVertexChainIdentifier(size_t vertexIdx) {
	if (vertexIdx > this->vertexChainMap.size() - 1) return this->notFoundIdentifier;
	size_t count = 0;
	for (VertexChainMapIterator mapIt = this->vertexChainMap.begin(); mapIt != this->vertexChainMap.end(); mapIt++) {
		if (count == vertexIdx) {
			return mapIt->first;
		}
		count++;
	}
	return this->notFoundIdentifier;
}


size_t VisualStageBox::getNumberOfVertexChains(void) const {
	return this->vertexChainMap.size();
}


bool VisualStageBox::hasVertexChain(const VisualItemIdentifier& vertexChainName) const {
	bool result = false;
	ConstVertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		result = true;
	}
	return result;
}


void VisualStageBox::removeAllVertexChains() {
	for (VertexChainMapIterator mapIt = this->vertexChainMap.begin(); mapIt != this->vertexChainMap.end(); mapIt++) {
		for (VertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			delete *chainIt;
			*chainIt = NULL;
		}
		mapIt->second->clear();
	}
	this->vertexChainMap.clear();
}


void VisualStageBox::removeVertexChain(const VisualItemIdentifier& vertexChainName) {
	VertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		for (VertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			delete *chainIt;
			*chainIt = NULL;
		}
		mapIt->second->clear();
		this->vertexChainMap.erase(mapIt);
	}
}


double VisualStageBox::getMinSRelTexCoord() {
	double minSRelCoord = 1.0;
	size_t numberOfVertexChains = this->getNumberOfVertexChains();
	for (size_t i = 0; i < numberOfVertexChains; i++) {
		VertexChain* chain = this->getVertexChain(i);
		for (VertexChainConstIterator chain_it = chain->begin(); chain_it != chain->end(); chain_it++) {
			if ((*chain_it)->texCoordPosition.relCoord.s < minSRelCoord) {
				minSRelCoord = (*chain_it)->texCoordPosition.relCoord.s;
			}
		}
	}
	return minSRelCoord;
}


double VisualStageBox::getMinTRelTexCoord() {
	double minTRelCoord = 1.0;
	size_t numberOfVertexChains = this->getNumberOfVertexChains();
	for (size_t i = 0; i < numberOfVertexChains; i++) {
		VertexChain* chain = this->getVertexChain(i);
		for (VertexChainConstIterator chain_it = chain->begin(); chain_it != chain->end(); chain_it++) {
			if ((*chain_it)->texCoordPosition.relCoord.t < minTRelCoord) {
				minTRelCoord = (*chain_it)->texCoordPosition.relCoord.t;
			}
		}
	}
	return minTRelCoord;
}


double VisualStageBox::getMaxSRelTexCoord() {
	double maxSRelCoord = 0.0;
	size_t numberOfVertexChains = this->getNumberOfVertexChains();
	for (size_t i = 0; i < numberOfVertexChains; i++) {
		VertexChain* chain = this->getVertexChain(i);
		for (VertexChainConstIterator chain_it = chain->begin(); chain_it != chain->end(); chain_it++) {
			if ((*chain_it)->texCoordPosition.relCoord.s > maxSRelCoord) {
				maxSRelCoord = (*chain_it)->texCoordPosition.relCoord.s;
			}
		}
	}
	return maxSRelCoord;
}


double VisualStageBox::getMaxTRelTexCoord() {
	double maxTRelCoord = 0.0;
	size_t numberOfVertexChains = this->getNumberOfVertexChains();
	for (size_t i = 0; i < numberOfVertexChains; i++) {
		VertexChain* chain = this->getVertexChain(i);
		for (VertexChainConstIterator chain_it = chain->begin(); chain_it != chain->end(); chain_it++) {
			if ((*chain_it)->texCoordPosition.relCoord.t > maxTRelCoord) {
				maxTRelCoord = (*chain_it)->texCoordPosition.relCoord.t;
			}
		}
	}
	return maxTRelCoord;
}


void VisualStageBox::setOpacityValue(double anOpacityValue) {
	for (ConstVertexChainMapIterator mapIt = this->vertexChainMap.begin(); mapIt != this->vertexChainMap.end(); ++mapIt) {
		for (ConstVertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			(*chainIt)->vertexColor.r = (*chainIt)->vertexColor.red * anOpacityValue;
			(*chainIt)->vertexColor.g = (*chainIt)->vertexColor.green * anOpacityValue;
			(*chainIt)->vertexColor.b = (*chainIt)->vertexColor.blue * anOpacityValue;
			(*chainIt)->vertexColor.a = (*chainIt)->vertexColor.alpha * anOpacityValue;
		}
	}
}


VisualVertex* VisualStageBox::createVertex(double xPos, double yPos, double zPos, double aTexCoordSPos, double aTexCoordTPos, VertexColor anRGBAColor) {
	
	double xCoordPos = this->getLeftCoord() + (this->getCoordWidth() * xPos);
	double yCoordPos = this->getBottomCoord() + (this->getCoordHeight() * yPos);
	
	double zCoordDistance = this->getBackPosition() - this->getFrontPosition();
	double zCoordPos = zCoordDistance * zPos;
	//printf("back: %f, front: %f\n", this->getBackPosition(), this->getFrontPosition()); // hwtest
	zCoordPos += this->getFrontPosition();

	TexCoord relTexCoord;
	relTexCoord.s = aTexCoordSPos;
	relTexCoord.t = aTexCoordTPos;

	const VisualImage* const anImage = this->assetRef->getImage();
	if (anImage != NULL) {
		aTexCoordSPos = anImage->getLogicalWidth() * aTexCoordSPos;
		aTexCoordTPos = anImage->getLogicalHeight() * aTexCoordTPos;
	}
	
	Coord coord;
	Coord relCoord;
	coord.x = xCoordPos;
	coord.y = yCoordPos;
	coord.z = zCoordPos;
	relCoord.x = xPos;
	relCoord.y = yPos;
	relCoord.z = zPos;

	TexCoord texCoord;
	texCoord.s = aTexCoordSPos;
	texCoord.t = aTexCoordTPos;

	VisualVertex* aVertex = new VisualVertex(coord, relCoord, texCoord, relTexCoord, anRGBAColor);
	return aVertex;
}


VisualVertex* VisualStageBox::createVertex(double xPos, double yPos, double zPos, VertexColor anRGBAColor) {
	
	double xCoordPos = this->getLeftCoord() + (this->getCoordWidth() * xPos);
	double yCoordPos = this->getBottomCoord() + (this->getCoordHeight() * yPos);
	
	double zCoordDistance = this->getBackPosition() - this->getFrontPosition();	
	double zCoordPos = zCoordDistance * zPos;
	zCoordPos += this->getFrontPosition();

	Coord coord;
	Coord relCoord;
	coord.x = xCoordPos;
	coord.y = yCoordPos;
	coord.z = zCoordPos;
	relCoord.x = xPos;
	relCoord.y = yPos;
	relCoord.z = zPos;

	VisualVertex* aVertex = new VisualVertex(coord, relCoord, anRGBAColor);
	return aVertex;
}


void VisualStageBox::setDebugMode(bool requestedDebugMode) {
	this->debugMode = requestedDebugMode;
}


bool VisualStageBox::getDebugMode() {
	return this->debugMode;
}


void VisualStageBox::updateVertices() {

	double xOffset = this->getUnscaledCoordWidth() - this->getCoordWidth();
	xOffset *= 0.5;
	double yOffset = this->getUnscaledCoordHeight() - this->getCoordHeight();
	yOffset *= 0.5;

	double zCoordDistance = this->getBackPosition() - this->getFrontPosition();
	for (VertexChainMapIterator mapIt = this->vertexChainMap.begin(); mapIt != this->vertexChainMap.end(); mapIt++) {
		for (VertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			if ((*chainIt)->vertexPosition.relCoord.x != -1.0) {
				(*chainIt)->vertexPosition.coord.x = this->getLeftCoord() + (this->getCoordWidth() * (*chainIt)->vertexPosition.relCoord.x);
				if ((*chainIt)->vertexPosition.relCoord.x > 0.5) {
					//(*chainIt)->vertexPosition.coord.x -= xOffset;
				} else {
					//printf("hwtest x1: %f\n", (*chainIt)->vertexPosition.coord.x);
					//(*chainIt)->vertexPosition.coord.x += xOffset;
					//printf("hwtest x2: %f\n", (*chainIt)->vertexPosition.coord.x);
				}
			} else {
				//(*chainIt)->vertexPosition.coord.x = this->getRightCoord();
				//printf("coord.x == %f\n", (*chainIt)->vertexPosition.coord.x); // hwtest
			}
			if ((*chainIt)->vertexPosition.relCoord.y != -1.0) {
				(*chainIt)->vertexPosition.coord.y = this->getBottomCoord() + (this->getCoordHeight() * (*chainIt)->vertexPosition.relCoord.y);
				if ((*chainIt)->vertexPosition.relCoord.y > 0.5) {
					//(*chainIt)->vertexPosition.coord.y -= yOffset;
				} else {
					//(*chainIt)->vertexPosition.coord.y += yOffset;
				}
			} else {
				//(*chainIt)->vertexPosition.coord.y = this->getTopCoord();
				//printf("coord.y == %f\n", (*chainIt)->vertexPosition.coord.y); // hwtest
			}
			if ((*chainIt)->vertexPosition.relCoord.z != -1.0) {
				(*chainIt)->vertexPosition.coord.z = zCoordDistance * (*chainIt)->vertexPosition.relCoord.z;
				(*chainIt)->vertexPosition.coord.z += this->getFrontPosition();
			} else {
				//(*chainIt)->vertexPosition.coord.z = this->getFrontPosition();
			}
			if ((*chainIt)->texCoordPosition.relCoord.s != -1.0) {
				const VisualImage* const anImage = this->assetRef->getImage();
				if (anImage != NULL) {
					//double before = (*chainIt)->texCoordPosition.coord.s;
					(*chainIt)->texCoordPosition.coord.s = anImage->getLogicalWidth() * (*chainIt)->texCoordPosition.relCoord.s;
					//printf("before: %f, danach: %f\n", before, (*chainIt)->texCoordPosition.coord.s);
				}
			}
			if ((*chainIt)->texCoordPosition.relCoord.t != -1.0) {
				const VisualImage* const anImage = this->assetRef->getImage();
				if (anImage != NULL) {
					(*chainIt)->texCoordPosition.coord.t = anImage->getLogicalHeight() * (*chainIt)->texCoordPosition.relCoord.t;
				}
			}
		}
	}
}


VisualStagePosition VisualStageBox::tweenVisualStagePosition(const VisualStagePosition& startPosition, const VisualStagePosition& stopPosition, double currPosition, const VisualStageBox& currentAssetBox) {
	VisualStagePosition tweenedPosition(startPosition);
	VisualStageBox startBox(currentAssetBox);
	startBox.setVisualStagePosition(startPosition);
	VisualStageBox stopBox(currentAssetBox);
	stopBox.setVisualStagePosition(stopPosition);
	double distanceX = stopBox.getRightCoord() - startBox.getRightCoord();
	double distanceY = stopBox.getBottomCoord() - startBox.getBottomCoord();
	tweenedPosition.horizontalCoordOffset += (distanceX * currPosition);
	tweenedPosition.verticalCoordOffset += (distanceY * currPosition);
	return tweenedPosition;
}
