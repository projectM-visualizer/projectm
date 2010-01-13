/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualFile.cpp
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

#include "VisualFile.h"
#include "VisualString.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Folders.h> // FSFindFolder
#endif

#if TARGET_OS_WIN
#include <shlobj.h> // SHGetSpecialFolderPathW
//#include <io.h> // _access
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


VisualFile::VisualFile(const VisualFile& other) : VisualObject(other) {
	copy(other);
}


VisualFile& VisualFile::operator=(const VisualFile& other) {
	
	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	this->clear();
	
	this->copy(other);
	
	return *this;
}


bool VisualFile::initWithDirectoryOfTemporaryItems() {
	
	bool success = true;
	
	this->clear();
	
#if TARGET_OS_MAC
	OSErr osErr = FSFindFolder(kUserDomain, kTemporaryFolderType, kCreateFolder, &(this->fsRef)); // ~/Library/Caches/TemporaryItems/
	if (osErr != noErr) {
		success = false;
	}
#endif
	
#if TARGET_OS_WIN
	BOOL createFolderIfNotAlreadyThere = true;
	HRESULT hr = SHGetSpecialFolderPathW(NULL, (LPWSTR)filePathWin, CSIDL_INTERNET_CACHE, createFolderIfNotAlreadyThere);
	if (!hr) {
		success = false;
	}
#endif
	
	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;
	
	return success;
}


bool VisualFile::initWithUserDesktopDirectory() {
	
	bool success = true;
	
	this->clear();
	
#if TARGET_OS_MAC
	OSErr osErr = FSFindFolder(kUserDomain, kDesktopFolderType, kCreateFolder, &(this->fsRef));
	if (osErr != noErr) {
		success = false;
	}
#endif
	
#if TARGET_OS_WIN
	BOOL createFolderIfNotAlreadyThere = true;
	HRESULT hr = SHGetSpecialFolderPathW(NULL, (LPWSTR)this->filePathWin, CSIDL_DESKTOPDIRECTORY, createFolderIfNotAlreadyThere);
	if (!hr) {
		success = false;
	}
#endif
	
	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;
	
	return success;
}


bool VisualFile::initWithPreferenceStoreDirectory() {
	
	bool success = true;
	
	this->clear();
	
#if TARGET_OS_MAC
	OSErr osErr = FSFindFolder(kUserDomain, kPreferencesFolderType, kCreateFolder, &(this->fsRef));
	if (osErr != noErr) {
		return false;
	}
#endif
	
#if TARGET_OS_WIN
	BOOL createFolderIfNotAlreadyThere = true;
	HRESULT hr = SHGetSpecialFolderPathW(NULL, (LPWSTR)this->filePathWin, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	if (!hr) {
		return false;
	}
#endif
	
	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;
	
	return success;
}


#if TARGET_OS_MAC
bool VisualFile::initWithResourcesDirectory() {
	
	bool success = true;
	
	this->clear();
	
	CFStringRef pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingASCII);
	CFBundleRef bundleRef = CFBundleGetBundleWithIdentifier(pluginName);
	if (!bundleRef) {
		return false;
	}
	CFURLRef resourcesDirectoryUrlRef = CFBundleCopyResourcesDirectoryURL(bundleRef);
	if (!resourcesDirectoryUrlRef) {
		return false;
	}
	
	FSRef newFsRef;
	Boolean CFURLGetFSRefSuccess = CFURLGetFSRef(resourcesDirectoryUrlRef, &newFsRef);
	if (!CFURLGetFSRefSuccess) {
		CFRelease(resourcesDirectoryUrlRef);
		return false;
	} else {
		this->fsRef = newFsRef;
	}
	
	CFRelease(resourcesDirectoryUrlRef);
	
	isDirectory = true;
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = true;
	
	return success;
}
#endif


VisualFile* VisualFile::clone(void) const {
	return new VisualFile(*this);
}


bool VisualFile::appendFileName(VisualString& aFileName) {
	
	bool success = false;
	
#if TARGET_OS_MAC
	OSStatus osStatus = noErr;
	Boolean isDir = false;
	CFURLRef url = CFURLCreateFromFSRef(kCFAllocatorDefault, &(this->fsRef));
	CFURLRef urlWithFileName = CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, url, aFileName.getCharactersPointer(), isDir);
	CFRelease(url);
	FSRef newFsRef;
	Boolean CFURLGetFSRefSuccess = CFURLGetFSRef(urlWithFileName, &newFsRef);
	if (!CFURLGetFSRefSuccess) {
		// FSRef can only be set for existing files or directories
		
		CFIndex fileNameLength = CFStringGetLength(aFileName.getCharactersPointer());
		const uint16* fileNameConstChars = CFStringGetCharactersPtr(aFileName.getCharactersPointer());
		uint16* fileNameChars = NULL;
		if (fileNameConstChars == NULL) {
			fileNameChars = (uint16*)malloc(fileNameLength * sizeof(uint16));
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
	
	if (osStatus == noErr)
	{
		success = true;
	}
	
#endif
	
#if TARGET_OS_WIN
	wchar_t backslash[2];
	backslash[0] = 0x5c;
	backslash[1] = L'\0';
	wcscat(this->filePathWin, backslash);
	wcscat(this->filePathWin, aFileName.getCharactersPointer());
	knownWhetherFileOrDirectoryExists = false;
	doesExistBool = false;
	success = true;
#endif
	
	this->isDirectory = false;
	
	return success;
	
}


bool VisualFile::open() {
	bool success = false;
#if TARGET_OS_MAC
	OSErr osErr = FSOpenFork(&(this->fsRef), 0, NULL, fsWrDenyPerm, &(this->currForkRefNum));
	if (osErr == noErr) {
		success = true;
	}
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
	if (this->currHandle != INVALID_HANDLE_VALUE) {
		knownWhetherFileOrDirectoryExists = true;
		doesExistBool = true;
		success = true;
	}
#endif
	return success;
}


bool VisualFile::close() {
	bool success = false;
#if TARGET_OS_MAC
	OSErr osErr = FSCloseFork(this->currForkRefNum);
	this->currForkRefNum = 0;
	if (osErr == noErr) {
		success = true;
	}
#endif
#if TARGET_OS_WIN
	BOOL closeHandleSuccess = CloseHandle(this->currHandle);
	if (closeHandleSuccess) {
		success = true;
	}
#endif
	return success;
}


bool VisualFile::getData(void** dataOut, size_t& size) {
	
	bool success = true;
	
	bool debug = false;
	char logStr[2048];
	
	if (debug == true) {
		writeLog("getData() start");
	}
	
	success = this->open();
	if (!success) {
		return success;
	}
	if (debug == true) {
		writeLog("aFile.open()");
	}
	
	uint32 numberOfBytesToReadPerFill = 0;
	// some small optimization
	size = 0;
	success = this->getSize(size);
	if (!success) {
		success = false;
	} else {
		if (size > 4096) {
			numberOfBytesToReadPerFill = 4096;
		} else {
			numberOfBytesToReadPerFill = 512;
		}
	}
	if (!success) {
		size = 0;
		return success;
	}
	bool readOn = true;
	uint32 actualCount = 0;
	uint32 offset = 0;
	uint32 numberOfBufferFills = (size / numberOfBytesToReadPerFill) + 1;
	uint16 bufferFillCount = 0;
	uint32 numberOfBytesRead = 0;
	char* buffer = (char*)malloc(numberOfBytesToReadPerFill * numberOfBufferFills);
	char* bufferStart = buffer;
	sint8 readDataValue = 0;
	while (readOn == true) {
		actualCount = numberOfBytesToReadPerFill;
		readDataValue = this->readData(buffer, actualCount, offset);
		bufferFillCount++;
		if (readDataValue != 1) {
			readOn = false;
			if (readDataValue == -1) {
				if (debug == true) {
					writeLog("OK: readData: eofErr");
				}
			} else {
				writeLog("ERR: readData");
				success = false;
			}
		} else {
			offset += actualCount;
			buffer = buffer + actualCount;
			if (debug == true) {
				sprintf(logStr, "OK: readData (cnt: %d, totalBytes: %d, req: %d, read: %d)", bufferFillCount, numberOfBytesRead, numberOfBytesToReadPerFill, actualCount);
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
	
	success = this->close();
	if (debug == true) {
		writeLog("aFile.close()");
	}
	
	*dataOut = (void*)bufferStart;
	size = numberOfBytesRead;
	
	return success;
	
}


sint8 VisualFile::readData(char* buffer, uint32& numberOfbytes, uint32 startOffset) {
	sint8 retVal = 0;
#if TARGET_OS_MAC
	ByteCount numberOfbytesRequested = static_cast<ByteCount>(numberOfbytes);
	//osErr = FSReadFork(this->currForkRefNum, fsFromStart, startOffset, numberOfbytesRequested, buffer, static_cast<ByteCount*>(&numberOfbytes));
	OSErr osErr = FSReadFork(this->currForkRefNum, fsFromStart, startOffset, numberOfbytesRequested, buffer, reinterpret_cast<ByteCount*>(&numberOfbytes));
	if (osErr == noErr) {
		retVal = 1;
	} else if (osErr == eofErr) {
		retVal = -1; // eof
	}
#endif
#if TARGET_OS_WIN
	DWORD nNumberOfBytesToRead = numberOfbytes;
	LPOVERLAPPED lpOverlapped = NULL;
	DWORD numOfBytes = 0;
	BOOL success = ReadFile(this->currHandle, buffer, nNumberOfBytesToRead, &numOfBytes, lpOverlapped);
	if (success) {
		numberOfbytes = static_cast<uint32>(numOfBytes);
		if (numberOfbytes == 0) {
			retVal = -1; // eof
		} else {
			retVal = 1;
		}
	}
#endif
	return retVal;
}


bool VisualFile::remove() {
	bool success = true;
#if TARGET_OS_MAC
	OSErr osErr = FSDeleteObject(&(this->fsRef));
	if (osErr != noErr) {
		success = false;
	}
#endif
#if TARGET_OS_WIN
	return false; // todo
#endif
	knownWhetherFileOrDirectoryExists = true;
	doesExistBool = false;
	return success;
}

/*
 bool VisualFile::doesExist(void) {
 #if TARGET_OS_WIN
 if (this->knownWhetherFileOrDirectoryExists == false) {
 this->doesExistBool = 
 test test
 
 this->knownWhetherFileOrDirectoryExists = true;
 }
 #endif
 return this->doesExistBool;
 }
 */

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


bool VisualFile::getSize(size_t& size) const {
	bool success = false;
	size = 0;
#if TARGET_OS_MAC
	FSCatalogInfo info;
	OSErr osErr = FSGetCatalogInfo(&(this->fsRef), kFSCatInfoDataSizes, &info, NULL, NULL, NULL);
	if (osErr == noErr) {
		size = (size_t)(info.dataLogicalSize); // UInt64
		success = true;
	}
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
	if (fileHandle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER sizeUInt64;
		BOOL GetFileSizeExSuccess = GetFileSizeEx(fileHandle, &sizeUInt64);
		CloseHandle(fileHandle);
		if (GetFileSizeExSuccess) {
			size = (size_t)sizeUInt64.QuadPart;
			success = true;
		}
	}
#endif
	
	return success;
}


VisualFile* VisualFile::createWithDirectoryOfTemporaryItems() {
	VisualFile* aVisualFile = new VisualFile;
	bool success = aVisualFile->initWithDirectoryOfTemporaryItems();
	if (!success) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}


VisualFile* VisualFile::createWithUserDesktopDirectory() {
	VisualFile* aVisualFile = new VisualFile;
	bool success = aVisualFile->initWithUserDesktopDirectory();
	if (!success) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}


VisualFile* VisualFile::createWithPreferenceStoreDirectory() {
	VisualFile* aVisualFile = new VisualFile;
	bool success = aVisualFile->initWithPreferenceStoreDirectory();
	if (!success) {
		delete aVisualFile;
		aVisualFile = NULL;
	}
	return aVisualFile;
}


#if TARGET_OS_MAC
VisualFile* VisualFile::createWithResourcesDirectory() {
	VisualFile* aVisualFile = new VisualFile;
	bool success = aVisualFile->initWithResourcesDirectory();
	if (!success) {
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


#if TARGET_OS_WIN
bool VisualFile::getDataOfResource(int nameId, char* type, void** data, uint32& size) {
	
	bool success = true;
	
	HRSRC resInfoHandle;
	HGLOBAL resHandle;
	HMODULE plugInHandle;
	char pluginFileName[256];
	
	sprintf(pluginFileName, "%s.dll", VisualConfiguration::visualizerPluginIdentifierName);
	plugInHandle = GetModuleHandle(pluginFileName);
	if (plugInHandle == NULL) {
		writeLog("pluginHandle is NULL");
		return false;
	}
	resInfoHandle = FindResource(plugInHandle, MAKEINTRESOURCE(nameId), type);
	if (resInfoHandle == NULL) {
		writeLog("resInfoHandle is NULL");
		return false;
	}
	resHandle = LoadResource(plugInHandle, resInfoHandle);
	*data = LockResource(resHandle);
	size = SizeofResource(plugInHandle, resInfoHandle);
	
	return success;
}
#endif


bool VisualFile::writeDataToFile(void** data, uint32 size, VisualFile& aFile) {
	bool success = true;
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
	return success;
}
