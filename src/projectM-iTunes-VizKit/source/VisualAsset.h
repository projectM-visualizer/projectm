/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAsset.h
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

#ifndef VisualAsset_h
#define VisualAsset_h


#include "VisualTypes.h"
#include "VisualStagePosition.h"
#include "VisualAnimationTypes.h"
#include "VisualObject.h"


namespace VizKit {
	
	class VisualImage; // Forward declaration (to avoid include of header file).
	class VisualStageBox; // Forward declaration (to avoid include of header file).
	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).
	class VisualAnimationComponent; // Forward declaration (to avoid include of header file).
	class VisualCamera; // Forward declaration (to avoid include of header file).
	
	/**
	 * Dimensionless asset that contains texture, boxing information, and optional animations.
	 */
	class VisualAsset : public VisualObject {
		
	public:
		
		/**
		 * The constructor.
		 */
		VisualAsset();
		
		/**
		 * The destructor.
		 */
		~VisualAsset();
		
		/**
		 * Copy constructor.
		 * @param other Reference to another VisualAsset.
		 */
		VisualAsset(const VisualAsset& other);
		
		/**
		 * Assignment operator.
		 * @param other Reference to another VisualAsset.
		 */
		VisualAsset& operator=(const VisualAsset& other);
		
		/**
		 * Less than operator.
		 * @param other Another VisualAsset.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualAsset is used as key_type).
		 */
		bool operator<(const VisualAsset& other) const;
		
		/**
		 * Equality operator.
		 * @param other Another VisualAsset.
		 */
		bool operator==(const VisualAsset& other) const;
		
		/**
		 * Inequality operator.
		 * @param other Another VisualAsset.
		 */
		bool operator!=(const VisualAsset& other) const;
		
		/**
		 * Copies the current VisualAsset and returns a pointer to a new VisualAsset.
		 */
		virtual VisualAsset* clone(void) const;
		
		/**
		 * Returns the stage box of the asset.
		 * The stage box is the bounding box.
		 * @return The stage box of the asset.
		 */		
		VisualStageBox* getBox(void) const;
		
		/**
		 * Sets the dimensions and direction of the camera view.
		 * @param cameraRef Camera whose dimensions and direction should be used by asset.
		 */	
		void setCamera(const VisualCamera& cameraRef);
		
		/**
		 * Returns the camera of the asset.
		 * @return The camera of the asset.
		 */		
		VisualCamera& getCamera(void) const;
		
		/**
		 * Returns the position of the asset's stage box.
		 * @return The position of the asset's stage box.
		 */
		VisualStagePosition getPosition(void) const;
		
		/**
		 * Sets the position of the asset's stage box.
		 * @param aPosition The requested position of the asset's stage box.
		 */
		void setPosition(const VisualStagePosition& aPosition);
		
		/** 
		 * Updates the coords of the assets.
		 * @remarks Might be useful after a resize event of the enclosing canvas for instance.
		 */
		void updateCoords(void);
		
		/**
		 * Sets the image of the asset.
		 * @param anImage Reference to the image.
		 * @remarks The image is copied.
		 */
		void setImage(const VisualImage& anImage);
		
		/**
		 * Answers the question whether the asset has image data.
		 * @return True if asset has image data, false if not.
		 */
		bool hasImage(void) const;
		
		/**
		 * Returns the image data of the asset.
		 * @return The image data of the asset.
		 */
		VisualImage* getImage(void) const;
		
		/**
		 * Removes the image of the asset.
		 */
		void removeImage(void);
		
		/**
		 * Draws the asset.
		 * @param vertexChainName The identifier of the VertexChain to draw.
		 */
		void draw(const VisualItemIdentifier& vertexChainName) const;
		
		/**
		 * Draws the asset.
		 * @remarks All vertex chains are drawn.
		 */
		void draw(void) const;
		
		/**
		 * Sets the scale value.
		 * @param aScaleFactor The factor by which the size of the asset should be multiplied when rendered.
		 * @remarks 1.0 means no magnification/minification.
		 */
		void setScaleFactor(double aScaleFactor);
		
		/**
		 * Returns the current scale factor.
		 * @return The current scale factor.
		 */	
		double getScaleFactor(void) const;
		
		/**
		 * Sets the opacity value (alpha value).
		 * @param anOpacityValue The opacity value (alpha value).
		 */
		void setOpacityValue(double anOpacityValue);
		
		/**
		 * Returns the current opacity value (alpha value).
		 * @return The current opacity value (alpha value).
		 */	
		double getOpacityValue(void) const;
		
		/**
		 * Sets the rotation angle value.
		 * @param aRotationAngle The requested rotation angle.
		 */
		void setRotationAngle(double aRotationAngle);
		
		/**
		 * Returns the current rotation angle value.
		 * @return The current rotation angle value.
		 */	
		double getRotationAngle(void) const;
		
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
		 * Adds an animation to the asset.
		 * @param anAnimation Reference to an animation.
		 * @remarks It is ensured that only one animation animates a distinct property of the asset. When the asset is destructed, all animations of the asset are removed.
		 */
		void addAnimation(VisualAnimationComponent& anAnimation);
		
		/**
		 * Clears all currently running animations.
		 * @param anAnimatedProperty Optional parameter that specifies the animated property of the animations.
		 */
		void removeAnimations(AnimatedProperty anAnimatedProperty = kUndefinedAnimatedProperty);
		
		/**
		 * Returns the VisualAnimation where the animated property matches.
		 * @param anAnimatedProperty Parameter that specifies the animated property of the animation.
		 * @return The VisualAnimation where the animated property matches.
		 * @remarks Return value can be NULL.
		 */
		const VisualAnimationComponent* const getAnimation(const AnimatedProperty& anAnimatedProperty) const;
		
		/**
		 * Returns the current animation value of a property which can be animated.
		 * @param anAnimatedProperty Parameter that specifies the queried property of the animation.
		 * @return The current animation value of the property.
		 * @remarks The returned value can differ from the current actual value of the asset property (e\.g\. for rotation return values are clamped between 0\.0 and 1\.0 instead of 0\.0 and 360\.0).
		 */
		double getCurrentAnimationValueOfProperty(const AnimatedProperty& anAnimatedProperty) const;
		
		/**
		 * Returns the current animation value of an animated VisualStagePosition.
		 * @param startPosition The start position.
		 * @param stopPosition The stop position.
		 * @return The current animation value of the property.
		 * @remarks The returned value can differ from the current actual value of the asset property (e\.g\. for rotation return values are clamped between 0\.0 and 1\.0 instead of 0\.0 and 360\.0).
		 */
		double getCurrentAnimationValueForAnimatedLocation(const VisualStagePosition& startPosition, const VisualStagePosition& stopPosition) const;
		
		/**
		 * Updates the location for an animation.
		 * @param currPosition The current position of the animation.
		 * @remarks Location animations are tweened between startValueVisualStagePosition and stopValueVisualStagePosition.
		 */
		void updateLocation(double currPosition);
		
		/**
		 * Sets the start position as it is used for an animation of the VisualStagePosition property of the asset's VisualStageBox.
		 * @param position The start position.
		 */
		void setStartValueVisualStagePosition(const VisualStagePosition& position);
		
 		/**
		 * Sets the start position as it is used for an animation of the VisualStagePosition property of the asset's VisualStageBox.
		 * @param position The start position.
		 */
		void setStopValueVisualStagePosition(const VisualStagePosition& position);
		
		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static void animateOpacity(double currentPosition, void* userData);
		
		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static void animateRotation(double currentPosition, void* userData);
		
		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static void animateScaleFactor(double currentPosition, void* userData);
		
		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static void animateLocation(double currentPosition, void* userData);
		
	private:
		
		/**
		 * Initializes the VisualAsset.
		 */
		void init(void);
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAsset.
		 */
		void copy(const VisualAsset& other);
		
		/**
		 * Draws a vertex chain (including texture data if available).
		 * @param aVertexChain Vertex chain to draw.
		 */
		void doDraw(const VertexChain& aVertexChain) const;
		
		/**
		 * Draws the outline of the stage box.
		 * @remarks Useful method for debugging or diagnostic purposes.
		 */
		void drawBoxOutline(void) const;
		
		/**
		 * The image of the asset.
		 */
		VisualImage* image;
		
		/**
		 * The camera of the asset.
		 */
		VisualCamera* camera;
		
		/**
		 * The stage box of the asset.
		 */
		VisualStageBox* stageBox;
		
		/**
		 * The identifier of the default vertex chain.
		 * @remarks The default vertex chain is used in case no vertex chain has been initialized explicitely.
		 */
		VisualItemIdentifier* defaultVertexChainIdentifier;
		
		/** The current rotation angle. */
		double rotationAngle;
		
		/** The current opacity value (alpha value). */
		double opacityValue;
		
		/** The current scale factor value. */
		double scaleFactor;
		
		/** The start value of an animation of the VisualStagePosition property. */
		VisualStagePosition startValueVisualStagePosition;
		
		/** The stop value of an animation of the VisualStagePosition property. */
		VisualStagePosition stopValueVisualStagePosition;
		
	};
	
}


#endif /* VisualAsset_h */
