/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualFile.h
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

#ifndef VisualFile_h
#define VisualFile_h


#include "VisualTypes.h"
#include "VisualString.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Files.h> // FSRef
#endif


#if TARGET_OS_WIN
#include <windows.h>
#endif


#include "VisualObject.h"


namespace VizKit {
	
	/**
	 * Path information of file or directory and related operations.
	 */
	class VisualFile : public VisualObject {
		
	public:
		
		/**
		 * The constructor. 
		 */
		VisualFile();
		
		/**
		 * The destructor.
		 */
		~VisualFile();
		
		/**
		 * Copy constructor.
		 * @param other Reference to another VisualFile.
		 */
		VisualFile(const VisualFile& other);
		
		/**
		 * Assignment operator.
		 * @param other Reference to another VisualFile.
		 */
		VisualFile& operator=(const VisualFile& other);
		
		/**
		 * Initializes the file with the directory of temporary items.
		 * @return True on success, false on failure.
		 */
		bool initWithDirectoryOfTemporaryItems(void);
		
		/**
		 * Initializes the file with the destop directory of the current user.
		 * @return True on success, false on failure.
		 */
		bool initWithUserDesktopDirectory(void);
		
		/**
		 * Initializes the file with the directory where the preferences are stored.
		 * @return True on success, false on failure.
		 */
		bool initWithPreferenceStoreDirectory(void);
		
#if TARGET_OS_MAC
		/**
		 * Initializes the file with the directory where the resources are stored. Mac-only.
		 * @return True on success, false on failure.
		 */
		bool initWithResourcesDirectory(void);
#endif
		
		/**
		 * Copies the current VisualFile and returns a pointer to a new VisualFile.
		 */
		virtual VisualFile* clone(void) const;
		
		/**
		 * Appends a file name to the VisualFile.
		 * @param aFileName A file name.
		 * @return True on success, false on failure.
		 */
		bool appendFileName(VisualString& aFileName);
		
		/**
		 * Opens the file for write access.
		 * @return True on success, false on failure.
		 */
		bool open(void);
		
		/**
		 * Closes the file.
		 * @return True on success, false on failure.
		 * @remarks The file must have been opened first.
		 */
		bool close(void);
		
		/**
		 * Returns the data of the file.
		 * @param[out] data A pointer to a pointer to the data of the file.
		 * @param[out] size The size of the read data in bytes.
		 * @return True on success, false on failure.
		 * @remarks The memory has to be deallocated on the caller's side by calling free() on the returned buffer.
		 */
		bool getData(void** data, size_t& size);
		
		/**
		 * Deletes the file.
		 * @return True on success, false on failure.
		 * @remarks Named remove instead of delete only because delete is a keyword for c++.
		 */
		bool remove(void);
		
		//bool doesExist(void);
		
		/**
		 * Returns the file path as VisualString.
		 * @param[out] filePath The file path as VisualString.
		 */
		void getFilePath(VisualString& filePath) const;
		
		/**
		 * Returns the size of the file in bytes.
		 * @param[out] size The size of the file in bytes.
		 * @return True on success, false on failure.
		 */
		bool getSize(size_t& size) const;
		
		/**
		 * Creates a file with the directory of temporary items.
		 * @return Created instance of VisualFile.
		 * @remarks The caller has to release the allocated memory by calling delete() on the returned VisualFile pointer.
		 */
		static VisualFile* createWithDirectoryOfTemporaryItems(void);
		
		/**
		 * Creates a file with the destop directory of the current user.
		 * @return Created instance of VisualFile.
		 * @remarks The caller has to release the allocated memory by calling delete() on the returned VisualFile pointer.
		 */
		static VisualFile* createWithUserDesktopDirectory(void);
		
		/**
		 * Creates a file with the directory into which the preferences are stored.
		 * @return Created instance of VisualFile.
		 * @remarks The caller has to release the allocated memory by calling delete() on the returned VisualFile pointer.
		 */
		static VisualFile* createWithPreferenceStoreDirectory(void);
		
#if TARGET_OS_MAC
		/**
		 * Creates a file with the directory where the resources are stored.
		 * @return Created instance of VisualFile.
		 * @remarks The caller has to release the allocated memory by calling delete() on the returned VisualFile pointer. Mac-only.
		 */
		static VisualFile* createWithResourcesDirectory(void);
#endif
		
#if TARGET_OS_WIN
		/**
		 * Returns a pointer to the data of a resource.
		 * @param nameId The integer id of the name of the resource.
		 * @param type The type of the resource as c-string.
		 * @param[out] data Pointer to pointer of the data.
		 * @param[out] size The size of the data memory block in bytes.
		 * @return True on success, false on failure.
		 * @remarks Windows-only. On the Mac resources are stores are regular files in Resurces directory of bundle.
		 */
		static bool getDataOfResource(int nameId, char* type, void** data, uint32& size);
#endif
		
		/**
		 * Writes the passed in data bytes into a file.
		 * @param data A pointer to a pointer to the data to write into file.
		 * @param size The number of bytes of the data.
		 * @param aFile The file into which to write the data.
		 * @return True on success, false on failure.
		 */
		static bool writeDataToFile(void** data, uint32 size, VisualFile& aFile);
		
	private:
		
		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualImage.
		 */
		void copy(const VisualFile& other);
		
		/** Resets internally used variables and releases allocated memory. */
		void clear(void);
		
#if TARGET_OS_MAC
		/** The FSRef of the VisualFile. */
		FSRef fsRef;
		/** Current forkRefNum used for reading and closing operations. */
		SInt16 currForkRefNum;
#endif
		
#if TARGET_OS_WIN
		/** The file path information of the VisualFile. */
		wchar_t filePathWin[1024];
		/** Current file handle used for reading and closing operations. */
		HANDLE currHandle;
#endif
		
		/** True if VisualFile denotes a directory. False if VisualFile denotes a file. */
		bool isDirectory;
		
		/** True if it is known whethere a file or directory at the location of the VisualFile exists. False if not known. */
		bool knownWhetherFileOrDirectoryExists;
		
		/** True if a file or directory at the location of the VisualFile exists. False if not. */
		bool doesExistBool;
		
		/**
		 * Reads data from the file.
		 * @param[in,out] buffer The buffer into which the data is to be written.
		 * @param[in,out] numberOfbytes The number of bytes requested. The actual number of bytes written on return.
		 * @param startOffset The offset from where reading is supposed to begin.
		 * @return 1 if still data available, 0 on error, -1 on eof.
		 * @remarks The buffer memory has to be allocated on the caller's side.
		 */
		sint8 readData(char* buffer, uint32& numberOfbytes, uint32 startOffset = 0);
		
	};
	
}

#endif /* VisualImage_h */
