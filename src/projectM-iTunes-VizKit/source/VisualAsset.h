/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAsset.h
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

#ifndef VisualAsset_h
#define VisualAsset_h

#include "VisualStagePosition.h"
#include "VisualAnimation.h"

#include <map>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

	class VisualImage; // Forward declaration (to avoid include of header file).
	class VisualStageBox; // Forward declaration (to avoid include of header file).
	class VisualItemIdentifier; // Forward declaration (to avoid include of header file).

	/** The type of vertex chain. */
	typedef enum {
		kFrontVertexChain = 0, /**< Chain of front vertices. */
		kBackVertexChain, /**< Chain of back vertices. */
		kTopVertexChain, /**< Chain of top vertices. */
		kBottomVertexChain, /**< Chain of bottom vertices. */
		kLeftVertexChain, /**< Chain of left vertices. */
		kRightVertexChain /**< Chain of right vertices. */
	} VertexChainName;

	/** The way an animation is added to an asset. */
	typedef enum {
		kContinueWithCurrentValue = 0, /**< The animation continues seemlessly by setting its current value to the current value of the animated attribute of the asset. */
		kIgnoreCurrentValue, /**< The current value of the animated attribute is ignored.\ The animation starts from its beginning.\ May cause jumps in flow of animation. */
		kContinueWithCurrentValueWithPossibleDelay /**< The animation continues seemlessly by setting its current value to the current value of the animated attribute of the asset.\ The start of the animation is delayed so that the end of the animation matches the duration time. */
	} AnimationAttachmentMode;

	/**
	 * Dimensionless asset that includes texture and boxing information.
	 */
	class VisualAsset {

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
		 * Returns the stage box of the asset.
		 * The stage box is the surrounding box.
		 * @return The stage box of the asset.
		 */		
		VisualStageBox* getBox(void);

		/**
		 * Returns the position of the asset's stage box.
		 * @return The position of the asset's stage box.
		 */
		VisualStagePosition getPosition(void);

		/**
		 * Sets the position of the asset's stage box.
		 * @param aPosition The requested position of the asset's stage box.
		 */
		void setPosition(const VisualStagePosition& aPosition);

		/**
		 * Sets the image of the asset.
		 * @param anImage Reference to the image.
		 * @remarks The image is copied.
		 */
		void setImage(VisualImage& anImage);

		/**
		 * Generates a vertex chain for the asset.\ The vertices are set according to texture and position data.
		 * @param vertexChainName The identifier/name (enum) of the vertex chain.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the generated vertex chain.
		 */
		VertexChain* generateVertexChain(VertexChainName vertexChainName, double scaleFactor = 1.0);

		/**
		 * Returns the current vertex chain of the asset.
		 * @param vertexChainName The identifier/name (enum) of the vertex chain.
		 * @return The vertex chain identified with that name.
		 */		
		VertexChain* getVertexChain(VertexChainName vertexChainName);

		/**
		 * Answers the question whether a vertex chain with the identifier exists.
		 * @param vertexChainName The identifier/name (enum) of the vertex chain.
		 * @return True if vertex chain with the identifier exists.
		 */		
		bool hasVertexChain(VertexChainName vertexChainName);
		
		/**
		 * Clears the vertex chain.
		 * @param vertexChainName The identifier of the VertexChain to remove.
		 */
		void removeVertexChain(VertexChainName vertexChainName);

		/**
		 * Draws the asset.
		 * @param vertexChainName The identifier of the VertexChain to draw.
		 */
		void draw(VertexChainName vertexChainName);
		
		/**
		 * Sets the color of the top-left vertex at the front of the asset.
		 * @param vertexColor The color of the top-left vertex at the front of the asset.
		 */
		void setTopLeftFrontVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the top-left vertex at the back of the asset.
		 * @param vertexColor The color of the top-left vertex at the back of the asset.
		 */
		void setTopLeftBackVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the bottom-left vertex at the front of the asset.
		 * @param vertexColor The color of the bottom-left vertex at the front of the asset.
		 */
		void setBottomLeftFrontVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the bottom-left vertex at the back of the asset.
		 * @param vertexColor The color of the bottom-left vertex at the back of the asset.
		 */
		void setBottomLeftBackVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the bottom-right vertex at the front of the asset.
		 * @param vertexColor The color of the bottom-right vertex at the front of the asset.
		 */
		void setBottomRightFrontVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the bottom-right vertex at the back of the asset.
		 * @param vertexColor The color of the bottom-right vertex at the back of the asset.
		 */
		void setBottomRightBackVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the top-right vertex at the front of the asset.
		 * @param vertexColor The color of the top-right vertex at the front of the asset.
		 */
		void setTopRightFrontVertexColor(VertexColor vertexColor);

		/**
		 * Sets the color of the top-right vertex at the back of the asset.
		 * @param vertexColor The color of the top-right vertex at the back of the asset.
		 */
		void setTopRightBackVertexColor(VertexColor vertexColor);

		/**
		 * Sets the scale value.
		 * @param aScaleFactor The factor by which the size of the asset should be multiplied when rendered.
		 * @remarks 1.0 means no magnification/minification.
		 */
		void setScaleFactor(double aScaleFactor);

		/**
		 * Sets the opacity value (alpha value).
		 * @param vertexChainName The identifier of the VertexChain.
		 * @param anOpacityValue The opacity value (alpha value).
		 */
		void setOpacityValue(VertexChainName vertexChainName, float anOpacityValue);

		/**
		 * Returns the current opacity value (alpha value).
		 * @param vertexChainName The identifier of the VertexChain.
		 * @return The current opacity value (alpha value).
		 */	
		float getCurrOpacityValue(VertexChainName vertexChainName);

		/**
		 * Sets the rotation angle value.
		 * @param aRotationAngle The requested rotation angle.
		 */
		void setRotationAngleValue(float aRotationAngle);

		/**
		 * Returns the current rotation angle value.
		 * @return The current rotation angle value.
		 */	
		float getCurrRotationAngleValue(void);
		
		/**
		 * Adds an animation to the asset.
		 * @param anAnimation Reference to an animation.
		 * @param animationAttachmentMode The way the animation is added to the asset.
		 * @remarks Since the VisualAnimation is copied later by the VisualStageControl, the returned VisualItemIdentifier provides access to the animation afterwards for the caller.
		 * The VisualAsset will continue the animation of the property at the currently set position.
		 */
		const VisualItemIdentifier* const addAnimation(VisualAnimation& anAnimation, AnimationAttachmentMode animationAttachmentMode = kContinueWithCurrentValue);

		/**
		 * Clears all currently running animations.
		 * @param anAnimatedProperty Optional parameter that specifies the animated property of the animations.
		 */
		void removeAnimations(AnimatedProperty anAnimatedProperty = kUndefinedAnimatedProperty);

		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static OSStatus animateOpacity(double currentPosition, void* userData);

		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static OSStatus animateRotation(double currentPosition, void* userData);

		/**
		 * Callback function that is called during animation.
		 * @param currentPosition The current position of the animation.
		 * @param userData Pointer to VisualAsset instance.
		 */
		static OSStatus animateSize(double currentPosition, void* userData);
 
	private:

		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualAsset.
		 */
		void copy(const VisualAsset& other);

		/**
		 * The image of the asset.
		 */
		VisualImage* image;

		/**
		 * The stage box of the asset.
		 */
		VisualStageBox* stageBox;
		
		/**
		 * The identifier of the asset.
		 */
		VisualItemIdentifier* assetIdentifier;

		/**
		 * Returns the top-left vertex at the front of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the top-left vertex at the front of the asset.
		 */
		Vertex* createTopLeftFrontVertex(double scaleFactor = 1.0);

		/**
		 * Returns the top-left vertex at the back of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the top-left vertex at the back of the asset.
		 */
		Vertex* createTopLeftBackVertex(double scaleFactor = 1.0);

		/**
		 * Returns the bottom-left vertex at the front of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the bottom-left vertex at the front of the asset.
		 */
		Vertex* createBottomLeftFrontVertex(double scaleFactor = 1.0);

		/**
		 * Returns the bottom-left vertex at the back of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the bottom-left vertex at the back of the asset.
		 */
		Vertex* createBottomLeftBackVertex(double scaleFactor = 1.0);

		/**
		 * Returns the bottom-right vertex at the front of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the bottom-right vertex at the front of the asset.
		 */
		Vertex* createBottomRightFrontVertex(double scaleFactor = 1.0);

		/**
		 * Returns the bottom-right vertex at the back of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the bottom-right vertex at the back of the asset.
		 */
		Vertex* createBottomRightBackVertex(double scaleFactor = 1.0);

		/**
		 * Returns the top-right vertex at the front of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the top-right vertex at the front of the asset.
		 */
		Vertex* createTopRightFrontVertex(double scaleFactor = 1.0);

		/**
		 * Returns the top-right vertex at the back of the asset.
		 * @param scaleFactor The scale factor of the vertex chain.
		 * @return A pointer to the top-right vertex at the back of the asset.
		 */
		Vertex* createTopRightBackVertex(double scaleFactor = 1.0);

		/**
		 * The chain of the front vertices.
		 */
		VertexChain* vertexChainFront;

		/**
		 * The chain of the top vertices.
		 */
		VertexChain* vertexChainTop;

		/**
		 * The chain of the left vertices.
		 */
		VertexChain* vertexChainLeft;
		
		/**
		 * The chain of the right vertices.
		 */
		VertexChain* vertexChainRight;

		/**
		 * The chain of the back vertices.
		 */
		VertexChain* vertexChainBack;

		/**
		 * The chain of the bottom vertices.
		 */
		VertexChain* vertexChainBottom;
		
		/** A VertexChainMap is a map of vertex chains. */
		typedef std::map<VertexChainName, VertexChain*> VertexChainMap;
		
		/** The map of the vertex chains. */
		VertexChainMap vertexChainMap;
		
		/** A VertexChainMapIterator is the iterator of a VertexChainMap. */
		typedef VertexChainMap::iterator VertexChainMapIterator;

		/** The color of the vertex at the top-left-front position. */
		VertexColor topLeftFrontVertexColor;

		/** The color of the vertex at the top-left-back position. */
		VertexColor topLeftBackVertexColor;

		/** The color of the vertex at the bottom-left-front position. */
		VertexColor bottomLeftFrontVertexColor;

		/** The color of the vertex at the bottom-left-back position. */
		VertexColor bottomLeftBackVertexColor;

		/** The color of the vertex at the bottom-right-front position. */
		VertexColor bottomRightFrontVertexColor;

		/** The color of the vertex at the bottom-right-back position. */
		VertexColor bottomRightBackVertexColor;

		/** The color of the vertex at the top-right-front position. */
		VertexColor topRightFrontVertexColor;

		/** The color of the vertex at the top-right-back position. */
		VertexColor topRightBackVertexColor;
		
		/** The current rotation angle. */
		float rotationAngle;

		/** The current opacity value (alpha value). */
		float opacityValue;

	};

}


#endif /* VisualAsset_h */
