/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualFile.cpp
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
   you must not claim that you wrote the original software. hand
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#include "VisualFile.h"
#include "VisualString.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Folders.h>
#endif

#if TARGET_OS_WIN
#include "MacErrors.h" // eofErr
#include <shlobj.h> // SHGetFolderPath
//#include <io.h> // _access
#include <QT/TextUtils.h> // CopyCStringToPascal()
#endif


using namespace VizKit;


VisualFile::VisualFile() {
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = false;
	isDirectory = false;
#if TARGET_OS_MAC
	currForkRefNum = 0;
#endif
#if TARGET_OS_WIN
	filePathWin[0] = L'\0';
	currHandle = NULL;
#endif
}


VisualFile::~VisualFile() {
	this->clear();
}


VisualFile::VisualFile(const VisualFile& other) {
	copy(other);
}


VisualFile& VisualFile::operator=(const VisualFile& other) {
	this->clear();
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


OSStatus VisualFile::initWithDirectoryOfTemporaryItems() {
	OSStatus osStatus = noErr;

	this->clear();
	
#if TARGET_OS_MAC
	OSErr osErr = noErr;
	osErr = FSFindFolder(kUserDomain, kTemporaryFolderType, kCreateFolder, &(this->fsRef)); // ~/Library/Caches/TemporaryItems/
	if (osErr != noErr) {
		return static_cast<OSStatus>(osErr);
	}
#endif

#if TARGET_OS_WIN
	BOOL createFolderIfNotAlreadyThere = true;
	HRESULT hr = SHGetSpecialFolderPathW(NULL, (LPWSTR)filePathWin, CSIDL_INTERNET_CACHE, createFolderIfNotAlreadyThere);
#endif

	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;

	return osStatus;
}


OSStatus VisualFile::initWithUserDesktopDirectory() {
	OSStatus osStatus = noErr;
	
	this->clear();

#if TARGET_OS_MAC
	OSErr osErr = noErr;
	osErr = FSFindFolder(kUserDomain, kDesktopFolderType, kCreateFolder, &(this->fsRef));
	if (osErr != noErr) {
		return static_cast<OSStatus>(osErr);
	}
#endif

#if TARGET_OS_WIN
	BOOL createFolderIfNotAlreadyThere = true;
	HRESULT hr = SHGetSpecialFolderPathW(NULL, (LPWSTR)this->filePathWin, CSIDL_DESKTOPDIRECTORY, createFolderIfNotAlreadyThere);
#endif

	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;

	return osStatus;
}


OSStatus VisualFile::initWithPreferenceStoreDirectory() {
	OSStatus osStatus = noErr;
	
	this->clear();

#if TARGET_OS_MAC
	OSErr osErr = noErr;
	osErr = FSFindFolder(kUserDomain, kPreferencesFolderType, kCreateFolder, &(this->fsRef));
	if (osErr != noErr) {
		return static_cast<OSStatus>(osErr);
	}
#endif

#if TARGET_OS_WIN
	BOOL createFolderIfNotAlreadyThere = true;
	HRESULT hr = SHGetSpecialFolderPathW(NULL, (LPWSTR)this->filePathWin, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
#endif

	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;

	return osStatus;
}


#if TARGET_OS_MAC
OSStatus VisualFile::initWithResourcesDirectory() {
	OSStatus osStatus = noErr;
	
	this->clear();

	CFStringRef pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingWindowsLatin1);
	CFBundleRef bundleRef = CFBundleGetBundleWithIdentifier(pluginName);
	if (!bundleRef) {
		return (OSStatus)1001;
	}
	CFURLRef resourcesDirectoryUrlRef = CFBundleCopyResourcesDirectoryURL(bundleRef);
	if (!resourcesDirectoryUrlRef) {
		return (OSStatus)1002;
	}

	FSRef newFsRef;
	Boolean success = CFURLGetFSRef(resourcesDirectoryUrlRef, &newFsRef);
	if (!success) {
		CFRelease(resourcesDirectoryUrlRef);
		return (OSStatus)1003;
	} else {
		this->fsRef = newFsRef;
	}
	
	CFRelease(resourcesDirectoryUrlRef);

	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;

	return osStatus;
}
#endif


OSStatus VisualFile::appendFileName(VisualString& aFileName) {
	OSStatus osStatus = noErr;

#if TARGET_OS_MAC
	Boolean isDir = false;
	CFURLRef url = CFURLCreateFromFSRef(kCFAllocatorDefault, &(this->fsRef));
	CFURLRef urlWithFileName = CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, url, aFileName.getCharactersPointer(), isDir);
	CFRelease(url);
	FSRef newFsRef;
	Boolean success = CFURLGetFSRef(urlWithFileName, &newFsRef);
	if (!success) {
		// FSRef can only be set for existing files or directories
				
		CFIndex fileNameLength = CFStringGetLength(aFileName.getCharactersPointer());
		const UniChar* fileNameConstChars = CFStringGetCharactersPtr(aFileName.getCharactersPointer());
		UniChar* fileNameChars = NULL;
		if (fileNameConstChars == NULL) {
			fileNameChars = (UniChar*)malloc(fileNameLength * sizeof(UniChar));
			CFStringGetCharacters(aFileName.getCharactersPointer(), CFRangeMake(0, fileNameLength), fileNameChars);
		}

		FSCatalogInfoBitmap whichInfo = (FSCatalogInfoBitmap)NULL;
		const FSCatalogInfo* catalogInfo = NULL;
		FSSpec newSpec;
		OSErr osErr = noErr;
		
		FSRef prevFsRef = this->fsRef;
		if (fileNameConstChars == NULL) {
			osErr = FSCreateFileUnicode(&prevFsRef, fileNameLength, fileNameChars, whichInfo, catalogInfo, &(this->fsRef), &newSpec);
		} else {
			osErr = FSCreateFileUnicode(&prevFsRef, fileNameLength, fileNameConstChars, whichInfo, catalogInfo, &(this->fsRef), &newSpec);
		}
		if (osErr != noErr) {
			osStatus = 1000;
		}

		if (fileNameChars != NULL) {
			free(fileNameChars);
			fileNameChars = NULL;
		}

	} else {
		this->fsRef = newFsRef;
	}
	
	CFRelease(urlWithFileName);
	
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;

#endif

#if TARGET_OS_WIN
	wchar_t backslash[2];
	backslash[0] = 0x5c;
	backslash[1] = L'\0';
	wcscat(this->filePathWin, backslash);
	wcscat(this->filePathWin, aFileName.getCharactersPointer());
	knownWhetherFileOrDirectoryExists = false;
	doesExistBool = false;
#endif

	isDirectory = false;

	return osStatus;

}


OSStatus VisualFile::open() {
	OSErr osErr = noErr;
#if TARGET_OS_MAC
	osErr = FSOpenFork(&(this->fsRef), 0, NULL, fsWrDenyPerm, &(this->currForkRefNum));
#endif
#if TARGET_OS_WIN
	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	DWORD dwCreationDisposition = OPEN_ALWAYS; // open the file if it exists, else create new one
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE hTemplateFile = NULL;
	this->currHandle = CreateFileW((LPCWSTR)this->filePathWin, 
									dwDesiredAccess, 
									dwShareMode, 
									lpSecurityAttributes,
									dwCreationDisposition,
									dwFlagsAndAttributes,
									hTemplateFile);
	if (this->currHandle == INVALID_HANDLE_VALUE) {
		osErr = 1001;
	}
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;
#endif
	return (OSStatus)osErr;
}


OSStatus VisualFile::close() {
	OSErr osErr = noErr;
#if TARGET_OS_MAC
	osErr = FSCloseFork(this->currForkRefNum);
	this->currForkRefNum = 0;
	return (OSStatus)osErr;
#endif
#if TARGET_OS_WIN
	BOOL success = CloseHandle(this->currHandle);
	if (!success) {
		osErr = 1001;
	}
#endif
	return (OSStatus)osErr;
}


OSStatus VisualFile::getData(void** data, UInt32& size) {
	OSStatus status = noErr;

	bool debug = false;
	char logStr[2048];
		
	status = this->open();
	if (debug == true) {
		writeLog("aFile.open()");
	}
	if (status != noErr) {
		return status;
	}
	
	bool readOn = true;
	UInt32 actualCount = 0;
	UInt32 requestCount = 4096;
	UInt32 offset = 0;
	UInt32 maxNumberOfBufferFills = (size / requestCount) + 1;
	UInt16 bufferFillCount = 0;
	UInt32 numberOfBytesRead = 0;
	char* buffer = (char*)malloc(requestCount * maxNumberOfBufferFills);
	char* bufferStart = buffer;
	while (readOn == true) {
		actualCount = requestCount;
		status = this->readData(buffer, actualCount, offset);
		bufferFillCount++;
		if (status != noErr) {
			readOn = false;
			if (status == eofErr) {
				if (debug == true) {
					writeLog("OK: readData: eofErr");
				}
			} else {
				sprintf(logStr, "ERR: readData: %ld", status);
				writeLog(logStr);
				status = 1007;
			}
		} else {
			offset += actualCount;
			buffer = buffer + actualCount;
			if (debug == true) {
				sprintf(logStr, "OK: readData (cnt: %d, totalBytes: %ld, req: %ld, read: %ld)", bufferFillCount, numberOfBytesRead, requestCount, actualCount);
				writeLog(logStr);
				
			}
		}
		numberOfBytesRead += actualCount;
		if (debug == true) {
			printf("----------------------\n");
			printf("BUFFER:\n%s\n", bufferStart);
			printf("----------------------\n");
		}
	}
	
	status = this->close();
	if (debug == true) {
		writeLog("aFile.close()");
	}
	if (status != noErr) {
		return status;
	}
	
	*data = (void*)bufferStart;
	size = numberOfBytesRead;

	return status;

}


OSStatus VisualFile::readData(char* buffer, UInt32& numberOfbytes, UInt32 startOffset) {
	OSErr osErr = noErr;
#if TARGET_OS_MAC
	UInt32 numberOfbytesRequested = numberOfbytes;
	osErr = FSReadFork(this->currForkRefNum, fsFromStart, startOffset, numberOfbytesRequested, buffer, &numberOfbytes);
#endif
#if TARGET_OS_WIN
	DWORD nNumberOfBytesToRead = numberOfbytes;
	LPOVERLAPPED lpOverlapped = NULL;
	BOOL success = ReadFile(this->currHandle, buffer, nNumberOfBytesToRead, &numberOfbytes, lpOverlapped);
	if (numberOfbytes == 0) osErr = eofErr;
	if (!success) {
		osErr = 1001;
	}
#endif
	return (OSStatus)osErr;
}


OSStatus VisualFile::remove() {
	OSErr osErr = noErr;
#if TARGET_OS_MAC
	osErr = FSDeleteObject(&(this->fsRef));
#endif
#if TARGET_OS_WIN
	return (OSStatus)osErr;
#endif
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = false;
	return (OSStatus)osErr;
}


bool VisualFile::doesExist(void) {
#if TARGET_OS_WIN
	if (this->knownWhetherFileOrDirectoryExists == false) {
		this->doesExistBool = 
		this->knownWhetherFileOrDirectoryExists = true;
	}
#endif
	return this->doesExistBool;
}


void VisualFile::getFilePath(VisualString& filePath) const {
#if TARGET_OS_MAC
	CFURLRef url = CFURLCreateFromFSRef(kCFAllocatorDefault, &(this->fsRef));
	CFStringRef cfString = NULL;
	if (url != NULL) {
		cfString = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
		CFRelease(url);
	}
	if (cfString != NULL) {
		filePath = VisualString(cfString);
		CFRelease(cfString);
	}
#endif
#if TARGET_OS_WIN
	if (wcslen(filePathWin) > 0) {
		filePath = VisualString(this->filePathWin);
	}
#endif
}


OSStatus VisualFile::getSize(UInt32& size) {
	OSErr osErr = noErr;
#if TARGET_OS_MAC
	FSCatalogInfo info;
	osErr = FSGetCatalogInfo(&(this->fsRef), kFSCatInfoDataSizes, &info, NULL, NULL, NULL);
	size = (UInt32)(info.dataLogicalSize);
#endif
#if TARGET_OS_WIN
	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	DWORD dwCreationDisposition = OPEN_ALWAYS; // open the file if it exists, else create new one
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE hTemplateFile = NULL;
	HANDLE fileHandle = CreateFileW((LPCWSTR)this->filePathWin, 
									dwDesiredAccess, 
									dwShareMode, 
									lpSecurityAttributes,
									dwCreationDisposition,
									dwFlagsAndAttributes,
									hTemplateFile);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		osErr = 1001;
	}
	LARGE_INTEGER sizeUInt64;
	BOOL success = GetFileSizeEx(fileHandle, &sizeUInt64);
	CloseHandle(fileHandle);
	size = (UInt32)sizeUInt64.QuadPart;
#endif


	return (OSStatus)osErr;
}


VisualFile* VisualFile::createWithDirectoryOfTemporaryItems() {
	VisualFile* aVisualFile = new VisualFile;
	OSStatus status = aVisualFile->initWithDirectoryOfTemporaryItems();
	if (status != noErr) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}


VisualFile* VisualFile::createWithUserDesktopDirectory() {
	VisualFile* aVisualFile = new VisualFile;
	OSStatus status = aVisualFile->initWithUserDesktopDirectory();
	if (status != noErr) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}


VisualFile* VisualFile::createWithPreferenceStoreDirectory() {
	VisualFile* aVisualFile = new VisualFile;
	OSStatus status = aVisualFile->initWithPreferenceStoreDirectory();
	if (status != noErr) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}


#if TARGET_OS_MAC
VisualFile* VisualFile::createWithResourcesDirectory() {
	VisualFile* aVisualFile = new VisualFile;
	OSStatus status = aVisualFile->initWithResourcesDirectory();
	if (status != noErr) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}
#endif


void VisualFile::copy(const VisualFile& other) {
	this->doesExistBool = other.doesExistBool;
	this->knownWhetherFileOrDirectoryExists = other.knownWhetherFileOrDirectoryExists;
	this->isDirectory = other.isDirectory;
#if TARGET_OS_MAC
	this->fsRef = other.fsRef;
	this->currForkRefNum = other.currForkRefNum;
#endif
#if TARGET_OS_WIN
	this->currHandle = other.currHandle;
#endif
#if TARGET_OS_WIN
	wcscpy(this->filePathWin, other.filePathWin);
#endif
}


void VisualFile::clear() {
#if TARGET_OS_MAC
	this->currForkRefNum = 0;
#endif
#if TARGET_OS_WIN
	this->currHandle = NULL;
#endif
	this->knownWhetherFileOrDirectoryExists = true;
	this->doesExistBool = false;
	this->isDirectory = false;
}


OSStatus VisualFile::setFSSpecByFileRef(const VisualFile& aFile, FSSpec& fsSpec) {

	OSStatus osStatus = noErr;
	OSErr osErr = noErr;
	char errLog[256];

	VisualString filePathString;
	aFile.getFilePath(filePathString);
	const char* const filePathCString = filePathString.getUtf8Representation();
	
#if TARGET_OS_MAC
	FSRef fsRef;
	
	osStatus = FSPathMakeRef((UInt8*)filePathCString, &fsRef, false);
	if (osStatus != noErr) {
		sprintf(errLog, "err (%ld) in file: %s (line: %d) [%s])", osStatus, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}
	if (osStatus == noErr) {
		osErr = FSGetCatalogInfo(&fsRef, kFSCatInfoNone, NULL, NULL, &fsSpec, NULL);
		if (osErr != noErr) {
			sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", osErr, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			osStatus = static_cast<OSStatus>(osErr);
		}
	}
#endif

#if TARGET_OS_WIN
	if (strlen(filePathCString) > 255) {
		return static_cast<OSStatus>(1001);
	}
	Str255 filePathPascal;
	CopyCStringToPascal(filePathCString, filePathPascal);
	osErr = FSMakeFSSpec(0, 0, filePathPascal, &fsSpec);
	if (osErr != noErr) {
		sprintf(errLog, "err (%d) in file: %s (line: %d) [%s])", osErr, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		osStatus = static_cast<OSStatus>(osErr);
	}
#endif

	return osStatus;
}


#if TARGET_OS_WIN
OSStatus VisualFile::getDataOfResource(int nameId, char* type, void** data, UInt32& size) {

	OSStatus osStatus = noErr;

	HRSRC resInfoHandle;
	HGLOBAL resHandle;
	HMODULE plugInHandle;
	char pluginFileName[256];
	
	sprintf(pluginFileName, "%s.dll", VisualConfiguration::kVisualPluginName);
	plugInHandle = GetModuleHandle(pluginFileName);
	if (plugInHandle == NULL) {
		writeLog("pluginHandle is NULL");
	}
	resInfoHandle = FindResource(plugInHandle, MAKEINTRESOURCE(nameId), type);
	if (resInfoHandle == NULL) {
		writeLog("resInfoHandle is NULL");
	}
	resHandle = LoadResource(plugInHandle, resInfoHandle);
	*data = LockResource(resHandle);
	size = SizeofResource(plugInHandle, resInfoHandle);

	return osStatus;
}
#endif


OSStatus VisualFile::writeDataToFile(void** data, UInt32 size, VisualFile& aFile) {
	OSStatus osStatus = noErr;
	VisualString filePathString;
	aFile.getFilePath(filePathString);
	const char* const filePathCString = filePathString.getUtf8Representation();
#if TARGET_OS_WIN
	FILE* file;
	fopen_s(&file, filePathCString, "w+b");
	int bytesWritten = 0;
	bytesWritten = fwrite(*data, sizeof(char), size, file);
	fclose(file);
#endif
#if TARGET_OS_MAC
	FILE* file = fopen(filePathCString, "w+b");
	size_t bytesWritten = 0;
	bytesWritten = fwrite(*data, sizeof(char), size, file);
	fclose(file);
#endif
	return osStatus;
}
