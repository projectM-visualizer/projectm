/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualObjectData.h
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

#ifndef VisualObjectData_h
#define VisualObjectData_h


#include "VisualTypes.h"

#include "VisualObjectMutex.h"

#include "VisualItemIdentifier.h"


namespace VizKit {

	/**
	 * VisualObjectData allows incremental data load of visual objects.
	 */
	class VisualObjectData {
		
		friend class VisualObject;
	
	public:

		/**
		 * The constructor.
		 */
		VisualObjectData();

		/**
		 * Another constructor.
		 * @param anIdentifier Identifier of the object.
		 */
		VisualObjectData(const VisualItemIdentifier& anIdentifier);

		/**
		 * The destructor.
		 */		
		~VisualObjectData();

		/**
		 * Copy constructor.
		 * @param other Reference to another VisualObjectData.
		 */
		VisualObjectData(const VisualObjectData& other);

		/**
		 * Assignment operator.
		 * @param other Reference to another VisualObjectData.
		 */
		VisualObjectData& operator=(const VisualObjectData& other);

		/**
		 * Less than operator.
		 * @param other Another VisualObjectData.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualObjectData is used as key_type).
		 */
		bool operator<(const VisualObjectData& other) const;

		/**
		 * Greater than operator.
		 * @param other Another VisualObjectData.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualObjectData is used as key_type).
		 */
		bool operator>(const VisualObjectData& other) const;
		
		/**
		 * Equality operator.
		 * @param other Another VisualObjectData.
		 */
		bool operator==(const VisualObjectData& other) const;

		/**
		 * Inequality operator.
		 * @param other Another VisualObjectData.
		 */
		bool operator!=(const VisualObjectData& other) const;

		/**
		 * Returns the identifier of the object item.
		 * @return The identifier of the object item.
		 * @remarks Fully implemeted by VisualObjectData class.
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
		 * Decrements the refCount so that the VisualObjectData is deleted the next time the enclosing VisualObject is deleted.
		 */	
		void release(void);

		/**
		 * Returns the ref count of the object.
		 * @return The ref count of the object.
		 */	
		size_t getRefCount(void);

		/**
		 * Answers the question whether VisualObjectData is embedded in VisualObject.
		 * @return True if VisualObjectData is embedded in VisualObject, false otherwise.
		 */	
		bool hasObject(void);
		
		/**
		 * Enter the critical region of the VisualObjectData.
		 * @remarks The destruction of the VisualObject is marshalled in this critical region phase.
		 */	
		void enterCriticalRegion(void);
		
		/**
		 * Exit critical region of the VisualObjectData.
		 * @remarks The destruction of the VisualObject is marshalled in this critical region phase.
		 */	
		void exitCriticalRegion(void);

		/**
		 * Answers the question whether VisualObjectData is currently in VisualDownloadQueue and downloading data.
		 * @return True if VisualObjectData is currently in VisualDownloadQueue and downloading data.
		 */	
		bool isDownloading(void);

		/**
		 * Sets the boolean flag to indicate that the VisualObjectData is beginning to oder ending to download data.
		 * @param isDownloadingBoolean True if VisualObjectData starting with download, false if not.
		 */	
		void setIsDownloading(bool isDownloadingBoolean);
		
	private:
	
		/**
		 * Each VisualObjectData can contain arbitrary data.
		 * @remarks The strategic memory allocation for the data is controlled internally when receiving addData() call;
		 */	
		char* data;
		
		/**
		 * The number of bytes used for data. This number reflects the number of valid bytes copied.
		 */	
		size_t dataSizeUsed;

		/**
		 * The number of bytes declared. This number answers the question how many bytes are expected to be used by this object according to a declaration, found e.g. in a remote header file.
		 */	
		size_t dataSizeDeclared;

		/**
		 * Flag denotes whether the declared data size has been set yet.
		 */	
		bool dataSizeDeclaredHasBeenSetBool;

		/**
		 * Each VisualObjectData contains a unique identifier.
		 */	
		VisualItemIdentifier* itemIdentifier;

		/**
		 * The number of bytes allocated for potential data use. This number reflects the number of bytes allocated in advance.
		 */	
		size_t dataSizeAllocated;
		
		/**
		 * The internal refCount. Pointers to VisualObjectData might be shared, in concurrent use at different places, so that the final destruction and memory ownership is not determined. Deleting a VisualObject therefore decrements the refCount of VisualObjectData.
		 */	
		size_t refCount;

		/**
		 * True if VisualObjectData is embedded in VisualObject.
		 * @return True if VisualObjectData is embedded in VisualObject, false otherwise.
		 */	
		bool hasObjectBool;

		/**
		 * True if VisualObjectData is currently in VisualDownloadQueue and downloading data.
		 * @return True if VisualObjectData is currently in VisualDownloadQueue and downloading data.
		 */	
		bool isDownloadingBool;
		
		/** Pointer to object mutex. */
		VisualObjectMutex* visualObjectMutex;
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualObjectData.
		 */
		void copyObject(const VisualObjectData& other);
		
	};
	
}

#endif /* VisualObjectData_h */
