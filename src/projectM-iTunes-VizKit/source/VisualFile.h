/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualFile.h
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

#ifndef VisualFile_h
#define VisualFile_h

#include "VisualString.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Files.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#include <QT/files.h>
#endif


namespace VizKit {

	/**
	 * Path information of file or directory and related operations.
	 */
	class VisualFile {
	
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
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus initWithDirectoryOfTemporaryItems(void);

		/**
		 * Initializes the file with the destop directory of the current user.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus initWithUserDesktopDirectory(void);

		/**
		 * Initializes the file with the directory into which the preferences are stored.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus initWithPreferenceStoreDirectory(void);

#if TARGET_OS_MAC
		/**
		 * Initializes the file with the directory where the resources are stored.\ Mac-only.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus initWithResourcesDirectory(void);
#endif

		/**
		 * Appends a file name to the VisualFile.
		 * @param aFileName A file name.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus appendFileName(VisualString& aFileName);

		/**
		 * Opens the file for write access.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus open(void);

		/**
		 * Closes the file.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks The file must have been opened first.
		 */
		OSStatus close(void);

		/**
		 * Reads data from the file.
		 * @param[in,out] buffer The buffer into which the data is to be written.
		 * @param[in,out] numberOfbytes The number of bytes requested.\ The actual number of bytes written on return.
		 * @param startOffset The offset from where reading is supposed to begin.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks The buffer memory has to be allocated on the caller's side.
		 */
		OSStatus readData(char* buffer, UInt32& numberOfbytes, UInt32 startOffset = 0);

		/**
		 * Returns the data of the file.
		 * @param[out] data A pointer to a pointer to the data of the file.
		 * @param[in,out] size The requested size of the data memory block in bytes.\ On return the actual size of the read data in bytes.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks The memory has to be deallocated on the caller's side by calling free on the returned buffer.
		 */
		OSStatus getData(void** data, UInt32& size);
		
		/**
		 * Deletes the file.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks Named remove instead of delete only because delete is a keyword for c++.
		 */
		OSStatus remove(void);

		/**
		 * Answers the question whether a file or directory at the location of the VisualFile exists.
		 * @return True if a file or directory at the location of the VisualFile exists.
		 */
		bool doesExist(void);

		/**
		 * Returns the file path as VisualString.
		 * @param[out] filePath The file path as VisualString.
		 * @return Error state as SInt32 OSStatus.
		 */
		void getFilePath(VisualString& filePath) const;
		
		/**
		 * Returns the logical size of the file in bytes.
		 * @param[out] size The logical size of the file in bytes.
		 * @return Error state as SInt32 OSStatus.
		 */
		OSStatus getSize(UInt32& size);

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
		 * @remarks The caller has to release the allocated memory by calling delete() on the returned VisualFile pointer.\ Mac-only.
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
		 * @remarks Windows-only.\ On the Mac resources are stores are regular files in Resurces directory of bundle.
		 */
		static OSStatus getDataOfResource(int nameId, char* type, void** data, UInt32& size);
#endif

		/**
		 * Utility function to set an FSSpec by passing in a file path.
		 * @param aFile Reference to a file.
		 * @param[out] fsSpec The FSSpec of the file path.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks Data type FSSpec is deprecated but used by QuickTime function calls.
		 */
		static OSStatus setFSSpecByFileRef(const VisualFile& aFile, FSSpec& fsSpec);

		/**
		 * Writes the passed in data bytes into a file.
		 * @param data A pointer to a pointer to the data to write into file.
		 * @param size The number of bytes of the data.
		 * @param aFile The file into which to write the data.
		 * @return Error state as SInt32 OSStatus.
		 */
		static OSStatus writeDataToFile(void** data, UInt32 size, VisualFile& aFile);

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

		/** True if VisualFile denotes a directory.\ False if VisualFile denotes a file. */
		bool isDirectory;

		/** True if it is known whethere a file or directory at the location of the VisualFile exists.\ False if not known. */
		bool knownWhetherFileOrDirectoryExists;

		/** True if a file or directory at the location of the VisualFile exists.\ False if not. */
		bool doesExistBool;

	};

}

#endif /* VisualImage_h */
