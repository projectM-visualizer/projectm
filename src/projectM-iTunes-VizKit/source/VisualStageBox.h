/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStageBox.h
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

#ifndef VisualStageBox_h
#define VisualStageBox_h


#include "VisualTypes.h"
#include "VisualGraphicTypes.h"
#include "VisualStagePosition.h"
#include "VisualItemIdentifier.h"


namespace VizKit {

	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	class VisualAsset; // Forward declaration (to avoid include of header file).
	class VisualCamera; // Forward declaration (to avoid include of header file).

	/**
	 * Position and dimensions of a box on stage.
	 * Used by VisualAsset as a positioning container. It is the bounding box of the visual asset.
	 */
	class VisualStageBox {

	public:

		/**
		 * The constructor.
		 * @param anAssetRef Pointer to the asset the stage box belongs to.
		 */
		VisualStageBox(VisualAsset* anAssetRef);
		
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
		void setContentPixelWidth(uint32 pixelWidth);
		
		/**
		 * Sets the pixel height of the image data.
		 * @param pixelHeight The pixel height of the image data.
		 */
		void setContentPixelHeight(uint32 pixelHeight);

		/**
		 * Sets the depth of the stage box (in coord values).
		 * @param aCoordDepth The depth of the stage box.
		 */
		void setCoordDepth(double aCoordDepth);

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
		 * Marks the stage box for update of the coord values (e.g. after setting new relative values of vertex chains).
		 * @remarks To enforce immediate update of the coord values, call update().
		 */
		void setDirty(void);

		/**
		 * Updates the coord values (e.g. after canvas reshape).
		 * @remarks It is preferred to just set the dirty flag by calling setDirty(). By calling setDirty() the update is deferred, the coord values are evaluated lazily.
		 */
		void update(void);
		
		/**
		 * Performs the updates of the coord values in case it is needed (because the values have been marked as dirty before).
		 */
		void updateIfNeeded(void);

		/**
		 * Returns the scaling attributes.
		 * @return The scaling attributes.
		 */
		ScalingBehaviour getScalingBehaviour(void) const;

		/**
		 * Returns the magnification/minification of the stage box when drawn.
		 * @return The factor by which the stage box is multiplied when rendered (1.0 means no magnification/minification).
		 */
		double getScaleFactor(void) const;
	
		/**
		 * Returns the width of the box.
		 * @return The width of the box.
		 */
		double getCoordWidth(void);

		/**
		 * Returns the width of the box (without current scaleFactor, only layout constraints taken into account).
		 * @return The width of the box (without current scaleFactor, only layout constraints taken into account).
		 */
		double getUnscaledCoordWidth(void);

		/**
		 * Returns the height of the box.
		 * @return The height of the box.
		 */		
		double getCoordHeight(void);

		/**
		 * Returns the height of the box (without current scaleFactor, only layout constraints taken into account).
		 * @return The height of the box (without current scaleFactor, only layout constraints taken into account).
		 */
		double getUnscaledCoordHeight(void);

		/**
		 * Returns the depth of the box.
		 * @return The depth of the box.
		 */		
		double getCoordDepth(void);

		/**
		 * Returns the depth of the box (without current scaleFactor, only layout constraints taken into account).
		 * @return The depth of the box (without current scaleFactor, only layout constraints taken into account).
		 */
		double getUnscaledCoordDepth(void);

		/**
		 * Returns the top coord of the box.
		 * @param vertexChainName The identifier of the vertex chain. If none is specified (default) then the coord position of the bounding box is returned.
		 * @return The top coord of the box.
		 */			
		double getTopCoord(const VisualItemIdentifier* const vertexChainName = NULL);

		/**
		 * Returns the left coord of the box.
		 * @param vertexChainName The identifier of the vertex chain. If none is specified (default) then the coord position of the bounding box is returned.
		 * @return The left coord of the box.
		 */		
		double getLeftCoord(const VisualItemIdentifier* const vertexChainName = NULL);

		/**
		 * Returns the bottom coord of the box.
		 * @param vertexChainName The identifier of the vertex chain. If none is specified (default) then the coord position of the bounding box is returned.
		 * @return The bottom coord of the box.
		 */
		double getBottomCoord(const VisualItemIdentifier* const vertexChainName = NULL);

		/**
		 * Returns the right coord of the box.
		 * @param vertexChainName The identifier of the vertex chain. If none is specified (default) then the coord position of the bounding box is returned.
		 * @return The right coord of the box.
		 */
		double getRightCoord(const VisualItemIdentifier* const vertexChainName = NULL);

		/**
		 * Returns the pixel width of the image data.
		 * @return the pixel width of the image data.
		 */
		uint32 getContentPixelWidth(void) const;
		
		/**
		 * Returns the pixel height of the image data.
		 * @return the pixel height of the image data.
		 */
		uint32 getContentPixelHeight(void) const;

		/**
		 * Returns a copy of the stage position of the box.
		 * @return A copy of the stage position of the box.
		 */
		VisualStagePosition getVisualStagePosition(void) const;

		/**
		 * Initializes a vertex chain.
		 * @param vertexChainName The identifier of the vertex chain.
		 */
		void initializeVertexChain(const VisualItemIdentifier& vertexChainName);

		/**
		 * Adds a vertex to a chain.
		 * @param vertexChainName The identifier of the vertex chain.
		 * @param aVertex The vertex to be added to the vertex chain.
		 */
		void addVertexToChain(const VisualItemIdentifier& vertexChainName, VisualVertex* aVertex);
		
		/**
		 * Returns the vertex chain of the asset identified by that name.
		 * @param vertexChainName The identifier/name (enum) of the vertex chain.
		 * @return The vertex chain identified by that name.
		 */		
		VertexChain* getVertexChain(const VisualItemIdentifier& vertexChainName);

		/**
		 * Returns the indexed vertex chain.
		 * @param vertexIdx The index of the requested vertex chain.
		 * @return The indexed vertex chain.
		 */		
		VertexChain* getVertexChain(size_t vertexIdx);

		/**
		 * Returns the identifier of an indexed vertex chain.
		 * @param vertexIdx The index of the requested vertex chain.
		 * @return The identifier of the indexed vertex chain.
		 */	
		const VisualItemIdentifier& getVertexChainIdentifier(size_t vertexIdx);

		/**
		 * Returns the number of vertex chains of the VisualStageBox.
		 * @return The number of vertex chains of the VisualStageBox.
		 */	
		size_t getNumberOfVertexChains(void) const;

		/**
		 * Answers the question whether a vertex chain with the identifier exists.
		 * @param vertexChainName The identifier/name (enum) of the vertex chain.
		 * @return True if vertex chain with the identifier exists.
		 */		
		bool hasVertexChain(const VisualItemIdentifier& vertexChainName) const;
		
		/**
		 * Clears all vertex chains of the VisualStageBox.
		 */
		void removeAllVertexChains(void);
		
		/**
		 * Clears the vertex chain.
		 * @param vertexChainName The identifier of the VertexChain to remove.
		 */
		void removeVertexChain(const VisualItemIdentifier& vertexChainName);

		/**
		 * Returns the minimum relative s-tex coord of all vertex chains of the stage box.
		 * @return The minimum relative s-tex coord of all vertex chains of the stage box.
		 */
		double getMinSRelTexCoord(void);

		/**
		 * Returns the minimum relative t-tex coord of all vertex chains of the stage box.
		 * @return The minimum relative t-tex coord of all vertex chains of the stage box.
		 */
		double getMinTRelTexCoord(void);

		/**
		 * Returns the maximum relative s-tex coord of all vertex chains of the stage box.
		 * @return The maximum relative s-tex coord of all vertex chains of the stage box.
		 */
		double getMaxSRelTexCoord(void);

		/**
		 * Returns the maximum relative t-tex coord of all vertex chains of the stage box.
		 * @return The maximum relative t-tex coord of all vertex chains of the stage box.
		 */
		double getMaxTRelTexCoord(void);

		/**
		 * Sets the opacity value (alpha value).
		 * @param anOpacityValue The opacity value (alpha value).
		 */
		void setOpacityValue(double anOpacityValue);

		/**
		 * Creates and returns a vertex at the specified position.
		 * @param xPos Position on x-axis (0.0 means: left, 1.0 means: right).
		 * @param yPos Position on y-Axis (0.0 means: bottom, 1.0 means: top).
		 * @param zPos Position on z-Axis (0.0 means: front, 1.0 means: back).
		 * @param aTexCoordSPos An s position of a texture.
		 * @param aTexCoordTPos A t position of a texture.
		 * @param anRGBAColor RGBA color associated with the vertex.
		 * @return A vertex at the specified position.
		 */
		VisualVertex* createVertex(double xPos, double yPos, double zPos, double aTexCoordSPos, double aTexCoordTPos, VertexColor anRGBAColor = white);

		/**
		 * Creates and returns a vertex at the specified position.
		 * @param xPos Position on x-axis (0.0 means: left, 1.0 means: right).
		 * @param yPos Position on y-Axis (0.0 means: bottom, 1.0 means: top).
		 * @param zPos Position on z-Axis (0.0 means: front, 1.0 means: back).
		 * @param anRGBAColor RGBA color associated with the vertex.
		 * @return A vertex at the specified position.
		 */
		VisualVertex* createVertex(double xPos, double yPos, double zPos, VertexColor anRGBAColor = white);

		/**
		 * Sets the debug mode.
		 * @param requestedDebugMode The debug mode. True turns debug mode on, false turns it off.
		 */
		void setDebugMode(bool requestedDebugMode);

		/**
		 * Returns the current debug mode.
		 * @return The current debug mode. True means debug mode is on, false means it is turned off.
		 */
		bool getDebugMode(void);

		/**
		 * Calculates the VisualStagePosition according to start position, stop position and current value (between 0.0 and 1.0).
		 * @param startPosition The start position.
		 * @param stopPosition The stop position.
		 * @param currPosition The current value of the animation.
		 * @param currentAssetBox The visual stage box of the current asset which can be queried for additional info.
		 * @return The tweened position.
		 */
		static VisualStagePosition tweenVisualStagePosition(const VisualStagePosition& startPosition, const VisualStagePosition& stopPosition, double currPosition, const VisualStageBox& currentAssetBox);

	private:
	
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualStageBox.
		 */
		void copy(const VisualStageBox& other);
		
		/**
		 * Calculates the coord dimension of the box according to alignment and margin settings.
		 * @param aCamera The camera with the dimensions of the stage.
		 */
		void calcCoords(const VisualCamera& aCamera);

		/**
		 * Applies the scale factor to the coord values.
		 */
		void applyScaleFactor(void);

		/**
		 * Updates the position of the existing vertices according to current top, left, bottom, right, front, back coordinates.
		 */
		void updateVertices(void);

		bool hasLayout; /**< True if calcCoords() was executed at least once, initial state is false. */
		
		double topCoord; /**< The top coord value of the positioned box. */
		
		double leftCoord; /**< The left coord value of the positioned box. */
		
		double bottomCoord; /**< The bottom coord value of the positioned box. */
		
		double rightCoord; /**< The right coord value of the positioned box. */
		
		double frontPosition; /**< The value of the front position. */
		
		double backPosition; /**< The value of the back position. */
		
		double coordWidth; /**< The width of the box measured in coord values. */
		
		double unscaledCoordWidth; /**< The width of the box measured in coord values (without scaleFactor). */
		
		double coordHeight; /**< The height of the box measured in coord values. */
		
		double unscaledCoordHeight; /**< The width of the box measured in coord values (without scaleFactor). */
		
		double coordDepth; /**< The depth of the box measured in coord values. */
		
		double unscaledCoordDepth; /**< The depth of the box measured in coord values (without scaleFactor). */
		
		uint32 contentPixelWidth; /**< The width of the box measured in pixels. */
		
		uint32 contentPixelHeight; /**< The height of the box measured in pixels. */
		
		VisualStagePosition stagePosition; /**< The dimensionless position of the box. */
		
		ScalingBehaviour scalingBehaviour; /**< Specifies the scaling behaviour. */
		
		double scaleFactor; /**< The factor by which the stage box should be multiplied when rendered (1.0 means no magnification/minification). */

		/** A VertexChainMap is a map of vertex chains. */
		typedef std::map<VisualItemIdentifier, VertexChain*> VertexChainMap;
		
		/** The map of the vertex chains. */
		VertexChainMap vertexChainMap;
		
		/** A VertexChainMapIterator is the iterator of a VertexChainMap. */
		typedef VertexChainMap::iterator VertexChainMapIterator;
		
		/** A VertexChainMapIterator is the const iterator of a VertexChainMap. */
		typedef VertexChainMap::const_iterator ConstVertexChainMapIterator;
		
		/** A dummy identifier returned in case a search result was empty. */
		VisualItemIdentifier notFoundIdentifier;
		
		/** Pointer to the asset the stage box belongs to. */
		VisualAsset* assetRef;

		/** True if stage box is in debug mode, false otherwise. */
		bool debugMode;
	};

}

#endif /* VisualStageBox_h */
