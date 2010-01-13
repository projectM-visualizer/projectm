/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualObject.h
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

#ifndef VisualObject_h
#define VisualObject_h


#include "VisualTypes.h"
#include "VisualItemIdentifier.h"


namespace VizKit {
	
	class VisualObjectData; // Forward declaration (to avoid include of header file).
	class VisualObjectMutex; // Forward declaration (to avoid include of header file).

	/**
	 * Basic abstract object data type of core objects.
	 */
	class VisualObject {
	
	public:

		/**
		 * The constructor.
		 */
		VisualObject();

		/**
		 * Another constructor.
		 * @param anIdentifier Identifier of the object.
		 */
		VisualObject(const VisualItemIdentifier& anIdentifier);

		/**
		 * The destructor.
		 */		
		virtual ~VisualObject();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualObject.
		 */
		VisualObject(const VisualObject& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualObject.
		 */
		VisualObject& operator=(const VisualObject& other);

		/**
		 * Less than operator.
		 * @param other Another VisualObject.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualObject is used as key_type).
		 */
		bool operator<(const VisualObject& other) const;

		/**
		 * Greater than operator.
		 * @param other Another VisualObject.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualObject is used as key_type).
		 */
		bool operator>(const VisualObject& other) const;
		
		/**
		 * Equality operator.
		 * @param other Another VisualObject.
		 */
		bool operator==(const VisualObject& other) const;

		/**
		 * Inequality operator.
		 * @param other Another VisualObject.
		 */
		bool operator!=(const VisualObject& other) const;

		/**
		 * Copies the current VisualObject and returns a pointer to a new VisualObject.
		 */
		virtual VisualObject* clone(void) const = 0;

		/**
		 * Returns the identifier of the object item.
		 * @return The identifier of the object item.
		 */	
		const VisualItemIdentifier getIdentifier(void) const;

		/**
		 * Adds data to the object.
		 * @param dataPtr Pointer to data.
		 * @param dataSize The number of bytes of the dataPtr.
		 * @return True on success, false on failure.
		 * @remarks The data is copied.
		 */	
		bool addData(void* dataPtr, uint32 dataSize);

		/**
		 * Deallocates the data if there has been data allocated.
		 */	
		void freeData(void);

		/**
		 * Returns the number of bytes currently used by the internal data buffer.
		 * @return The number of bytes currently used by the internal data buffer.
		 */	
		size_t getDataSizeUsed(void);

		/**
		 * Returns the declared size in bytes of the object.
		 * @return The declared size in bytes of the object.
		 * @remarks The size can be declared by a remote header file before the actual data has been fetched.
		 */	
		size_t getDataSizeDeclared(void);

		/**
		 * Sets the declared size in bytes of the object.
		 * @param aDataSizeDeclared The number of bytes declared.
		 * @remarks The size can be declared by a remote header file before the actual data has been fetched.
		 */	
		void setDataSizeDeclared(const size_t& aDataSizeDeclared);

		/**
		 * Returns whether the declared data size has been set yet.
		 * @return Boolean flag answers the question whether the declared data size has been set yet.
		 */	
		bool dataSizeDeclaredHasBeenSet(void);

		/**
		 * Returns a shared pointer to the visualObjectData.
		 * @return A shared pointer to the visualObjectData.
		 * @remarks The caller has to call release() on the visualObjectData pointer after use.
		 */	
		VisualObjectData* getVisualObjectData(void);
		
		/**
		 * Notification that the loading of data ended.
		 * @param identifier The identifier of the data load request.
		 * @remarks Hook method that can be overridden by subclasses of VisualObject for post-processing of received data.
		 */	
		virtual void dataLoadDidEnd(const VisualItemIdentifier& identifier);
		
	protected:

		/**
		 * Answers the question whether the internal data buffer has data available.
		 * @return True if the internal data buffer has data available, false otherwise.
		 */	
		bool hasData(void);
		
		/**
		 * Returns a pointer to the internal data buffer.
		 * @return A pointer to the internal data buffer.
		 */	
		char* getData(void);

		/**
		 * Returns the number of bytes of the internal data buffer.
		 * @return The number of bytes of the internal data buffer.
		 */	
		size_t getDataSize(void);

	private:
		
		/** Pointer to object data. */
		VisualObjectData* visualObjectData;
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualObject.
		 */
		void copyObject(const VisualObject& other);
		
	};
	
}

#endif /* VisualObject_h */
