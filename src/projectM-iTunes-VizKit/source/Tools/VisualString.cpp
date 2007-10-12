/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualString.cpp
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

#include "VisualString.h"
#include "VisualFile.h"
#include "VisualDispatch.h"
#include "VisualErrorHandling.h"

#if TARGET_OS_WIN
#include "MacErrors.h" // eofErr
#endif


using namespace VizKit;


VisualString::VisualString() {
	theString = NULL;
	utf8representation = NULL;
	utf16representation = NULL;
	numberOfCharacters = 0;
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
}


VisualString::VisualString(const char* const aStringVal, SInt32 numberOfBytes) {

	theString = NULL;
	utf8representation = NULL;
	utf16representation = NULL;
	numberOfCharacters = 0;
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	if (!aStringVal) return;
	
	if (numberOfBytes == -1) {
		initWithUTF8String(aStringVal);
	} else {
		initWithUTF8Buffer(aStringVal, numberOfBytes);
	}
}


VisualString::VisualString(const UniChar* const aStringVal, SInt32 aNumberOfCharacters) {

	theString = NULL;
	utf8representation = NULL;
	utf16representation = NULL;
	numberOfCharacters = 0;
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	if (!aStringVal) return;

	if (aNumberOfCharacters == -1) {
		initWithUTF16String(aStringVal);
	} else {
		initWithUTF16Buffer(aStringVal, aNumberOfCharacters);
	}
}


void VisualString::initWithUTF8String(const char* const aStringVal) {
	this->clear();
	UInt32 numberOfBytes = strlen(aStringVal);
#if TARGET_OS_MAC
	Boolean isExternalRepresentation = false;
	CFStringRef theStringRef = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8*)aStringVal, numberOfBytes, kCFStringEncodingUTF8, isExternalRepresentation);
	numberOfCharacters = CFStringGetLength(theStringRef);
	theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, theStringRef);
	CFRelease(theStringRef);
#endif
#if TARGET_OS_WIN
	// no UTF-8 decoding
	numberOfCharacters = numberOfBytes;
	theString = (wchar_t*)malloc((numberOfCharacters + 1) * sizeof(wchar_t));
	for (UInt32 i = 0; i < numberOfCharacters; i++) {
		theString[i] = aStringVal[i];
	}
	theString[numberOfCharacters] = L'\0';
#endif
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
}


void VisualString::initWithUTF8Buffer(const char* const aStringVal, UInt32 numberOfBytes) {
	this->clear();
#if TARGET_OS_MAC
	Boolean isExternalRepresentation = false;
	CFStringRef theStringRef = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8*)aStringVal, numberOfBytes, kCFStringEncodingUTF8, isExternalRepresentation);
	numberOfCharacters = CFStringGetLength(theStringRef);
	theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, theStringRef);
	CFRelease(theStringRef);
#endif
#if TARGET_OS_WIN
	// no UTF-8 decoding
	numberOfCharacters = numberOfBytes;
	theString = (wchar_t*)malloc((numberOfCharacters + 1) * sizeof(wchar_t));
	for (UInt32 i = 0; i < numberOfCharacters; i++) {
		theString[i] = aStringVal[i];
	}
	theString[numberOfCharacters] = L'\0';
#endif
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
}


void VisualString::initWithUTF16String(const UniChar* const aStringVal) {
	this->clear();
	numberOfCharacters = (sizeof(aStringVal) / 2) - 2;
#if TARGET_OS_WIN
	theString = (wchar_t*)malloc((numberOfCharacters + 1) * sizeof(wchar_t));
	size_t sizeInBytes = numberOfCharacters * sizeof(wchar_t);
	memcpy(theString, aStringVal, sizeInBytes);
	theString[numberOfCharacters] = L'\0';
#endif
#if TARGET_OS_MAC
	theString = CFStringCreateMutable(kCFAllocatorDefault, 0);
	CFStringAppendCharacters(theString, aStringVal, (CFIndex)numberOfCharacters);
#endif
	this->calcStatistics();
}


void VisualString::initWithUTF16Buffer(const UniChar* const aStringVal, UInt32 aNumberOfCharacters) {
	this->clear();
	numberOfCharacters = aNumberOfCharacters;
#if TARGET_OS_WIN
	theString = (wchar_t*)malloc((numberOfCharacters + 1) * sizeof(wchar_t));
	size_t sizeInBytes = numberOfCharacters * sizeof(wchar_t);
	memcpy(theString, aStringVal, sizeInBytes);
	theString[numberOfCharacters] = L'\0';
#endif
#if TARGET_OS_MAC
	theString = CFStringCreateMutable(kCFAllocatorDefault, 0);
	CFStringAppendCharacters(theString, aStringVal, (CFIndex)numberOfCharacters);
#endif
	this->calcStatistics();
}


OSStatus VisualString::initWithContentsOfFile(VisualFile& aFile) {

	OSStatus osStatus = noErr;

	void* data = NULL;

	UInt32 size;
	osStatus = aFile.getSize(size);
	if (osStatus != noErr) {
		return osStatus;
	}

	osStatus = aFile.getData(&data, size);
	if (osStatus != noErr) {
		return osStatus;
	}

	this->initWithUTF8Buffer((const char*)data, size);
	
	free(data);

	return osStatus;
}


VisualString::VisualString(ConstStringDataRef aStringVal) {
	theString = NULL;
	utf8representation = NULL;
	utf16representation = NULL;
	numberOfCharacters = 0;
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	if (!aStringVal) return;
	
#if TARGET_OS_WIN
	numberOfCharacters = wcslen(aStringVal);
	size_t sizeInBytes = numberOfCharacters * sizeof(wchar_t);
	theString = (wchar_t*)malloc((numberOfCharacters + 1) * sizeof(wchar_t));
	memcpy(theString, aStringVal, sizeInBytes);
	theString[numberOfCharacters] = L'\0';
#endif
#if TARGET_OS_MAC
	this->numberOfCharacters = CFStringGetLength(aStringVal);
	this->theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, aStringVal);
#endif

	this->calcStatistics();
}


VisualString::~VisualString() {
	this->clear();
}


VisualString::VisualString(const VisualString& other) {
	this->copy(other);
}


VisualString& VisualString::operator=(const VisualString& other) {
	if (this != &other) {
		this->clear();
		this->copy(other);
	}
	return *this;
}


bool VisualString::operator<(const VisualString& other) const {
	bool compareResult = false;
	if (other.theString != NULL && this->theString != NULL) {
#if TARGET_OS_MAC
		UInt16 compareOptions = 0;
		if (CFStringCompare(this->theString, other.theString, compareOptions) == kCFCompareLessThan) {
#endif
#if TARGET_OS_WIN
		if (wcscmp(this->theString, other.theString) < 0) {
#endif
			compareResult = true;
		}
	}
	return compareResult;
}


bool VisualString::operator==(const VisualString& other) const {
	bool compareResult = false;
	if (other.theString != NULL && this->theString != NULL) {
#if TARGET_OS_MAC
		UInt16 compareOptions = 0;
		if (CFStringCompare(this->theString, other.theString, compareOptions) == kCFCompareEqualTo) {
#endif
#if TARGET_OS_WIN
		if (wcscmp(this->theString, other.theString) == 0) {
#endif
			compareResult = true;
		}
	} else if (other.theString == NULL && this->theString == NULL) {
		compareResult = true;
	}
	return compareResult;
}


bool VisualString::operator!=(const VisualString& other) const {
	return !(*this == other);
}


void VisualString::copy(const VisualString& other) {
	if (other.numberOfCharacters > 0) {
#if TARGET_OS_MAC
		this->theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, other.theString);
#endif
#if TARGET_OS_WIN
		this->theString = (wchar_t *)malloc((other.numberOfCharacters + 1) * sizeof(wchar_t));
		size_t sizeInBytes = other.numberOfCharacters * sizeof(wchar_t);
		memcpy(theString, other.theString, sizeInBytes);
		theString[other.numberOfCharacters] = L'\0';
#endif
	} else {
		this->theString = NULL;
	}
	if (other.utf8representation != NULL) {
		this->utf8representation = (char*)malloc(strlen(other.utf8representation) * sizeof(char));
		memcpy(this->utf8representation, other.utf8representation, strlen(other.utf8representation));
	} else {
		this->utf8representation = NULL;
	}
	if (other.utf16representation != NULL) {
		this->utf16representation = (UniChar*)malloc(sizeof(other.utf16representation) * sizeof(char));
		memcpy(this->utf16representation, other.utf16representation, sizeof(other.utf16representation));
	} else {
		this->utf16representation = NULL;
	}
	this->numberOfCharacters = other.numberOfCharacters;
	this->calcStatistics();
}


void VisualString::clear() {
	if (this->theString != NULL) {
#if TARGET_OS_MAC
		CFRelease(this->theString);
#endif
#if TARGET_OS_WIN
		free(this->theString);
#endif
		this->theString = NULL;
	}
	if (this->utf8representation != NULL) {
		free(this->utf8representation);
		this->utf8representation = NULL;
	}
	if (this->utf16representation != NULL) {
		free(this->utf16representation);
		this->utf16representation = NULL;
	}
	if (arrayOfWords.size() > 0) {
		arrayOfWords.clear();
	}
	if (this->arrayOfLines.size() > 0) {
		this->arrayOfLines.clear();
	}
}


const VisualString& VisualString::operator+(const VisualString& other) {
	VisualString* stringCopy = new VisualString(*this);
	UInt32 prevNumberOfCharacters = stringCopy->numberOfCharacters;
	stringCopy->numberOfCharacters += other.numberOfCharacters;
	if (stringCopy->numberOfCharacters > prevNumberOfCharacters) {
#if TARGET_OS_MAC
		CFStringAppend(stringCopy->theString, other.getCharactersPointer());
#endif
#if TARGET_OS_WIN
		size_t sizeInBytes = (stringCopy->numberOfCharacters + 1) * sizeof(wchar_t);
		free(stringCopy->theString);
		stringCopy->theString = (wchar_t*)malloc(sizeInBytes);
		for (UInt32 i = 0; i < prevNumberOfCharacters; i++) {
			stringCopy->theString[i] = this->theString[i];
		}
		UInt32 count = 0;
		for (UInt32 i = prevNumberOfCharacters; i < stringCopy->numberOfCharacters; i++) {
			stringCopy->theString[i] = other.theString[count];
			count++;
		}
		stringCopy->theString[stringCopy->numberOfCharacters] = L'\0';
#endif
	}
	stringCopy->calcStatistics();
	return *stringCopy;
}


const VisualString& VisualString::operator+(const char* charString) {
	VisualString* stringCopy = new VisualString(*this);
	UInt32 prevNumberOfCharacters = stringCopy->numberOfCharacters;
	stringCopy->numberOfCharacters += strlen(charString);
	if (stringCopy->numberOfCharacters > prevNumberOfCharacters) {
#if TARGET_OS_MAC
		CFStringAppendCString(stringCopy->theString, charString, kCFStringEncodingISOLatin1);
#endif
#if TARGET_OS_WIN
		size_t sizeInBytes = (stringCopy->numberOfCharacters + 1) * sizeof(wchar_t);
		free(stringCopy->theString);
		stringCopy->theString = (wchar_t*)malloc(sizeInBytes);
		for (UInt32 i = 0; i < prevNumberOfCharacters; i++) {
			stringCopy->theString[i] = this->theString[i];
		}
		UInt32 count = 0;
		for (UInt32 i = prevNumberOfCharacters; i < stringCopy->numberOfCharacters; i++) {
			stringCopy->theString[i] = (wchar_t)charString[count];
			count++;
		}
		stringCopy->theString[stringCopy->numberOfCharacters] = L'\0';
#endif
	}
	stringCopy->calcStatistics();
	return *stringCopy;
}


UInt32 VisualString::getNumberOfCharacters() const {
	return this->numberOfCharacters;
}


UInt32 VisualString::getNumberOfNonWhitespaceCharacters() const {
	return this->numberOfNonWhitespaceCharacters;
}


UInt32 VisualString::getNumberOfWords(void) const {
	return this->numberOfWords;
}


UInt32 VisualString::getNumberOfLines(void) const {
	return this->numberOfLines;
}


VisualString* VisualString::createStringWithLine(UInt32 index) const {
	VisualString* stringAtLine = NULL;
#if TARGET_OS_WIN
	stringAtLine = new VisualString((UniChar*)&this->theString[this->arrayOfLines[index].startIndex], this->arrayOfLines[index].numberOfCharacters);
#endif
#if TARGET_OS_MAC
	CFStringRef string = CFStringCreateWithSubstring(kCFAllocatorDefault, this->theString, CFRangeMake(this->arrayOfLines[index].startIndex, this->arrayOfLines[index].numberOfCharacters));
	stringAtLine = new VisualString(string);
	CFRelease(string);
#endif
	return stringAtLine;
}


VisualString* VisualString::createStringWithWord(UInt32 index) const {
	VisualString* wordAtIndex = NULL;
#if TARGET_OS_WIN
	wordAtIndex = new VisualString((UniChar*)&this->theString[this->arrayOfWords[index].startIndex], this->arrayOfWords[index].numberOfCharacters);
#endif
#if TARGET_OS_MAC
	CFStringRef string = CFStringCreateWithSubstring(kCFAllocatorDefault, this->theString, CFRangeMake(this->arrayOfWords[index].startIndex, this->arrayOfWords[index].numberOfCharacters));
	wordAtIndex = new VisualString(string);
	CFRelease(string);
#endif
	return wordAtIndex;
}


ConstStringDataRef VisualString::getCharactersPointer() const {
	return this->theString;
}


const char* const VisualString::getUtf8Representation() {
	if (this->utf8representation != NULL) {
		free(this->utf8representation);
		this->utf8representation = NULL;
	}
#if TARGET_OS_MAC
	CFRange range = {0, CFStringGetLength(this->theString)};
	UInt8 lossByte = 0;
	Boolean isExternalRepresentation = false;
	CFIndex maxBufLen = 0;
	CFIndex numberOfBytes = 0;
	CFIndex numberOfCharsConverted;
	numberOfCharsConverted = CFStringGetBytes(this->theString, range, kCFStringEncodingUTF8, lossByte, isExternalRepresentation, NULL, maxBufLen, &numberOfBytes);
	if ((numberOfCharsConverted == CFStringGetLength(this->theString)) && (numberOfBytes > 0)) {
		this->utf8representation = (char*)malloc(numberOfBytes + 1);
		numberOfCharsConverted = CFStringGetBytes(this->theString, range, kCFStringEncodingUTF8, lossByte, isExternalRepresentation, (UInt8*)this->utf8representation, numberOfBytes, &numberOfBytes);
		this->utf8representation[numberOfBytes] = '\0';
	}

/*
	const char* scriptPathConstCStr = CFStringGetCStringPtr(scriptFilePath.getCharactersPointer(), kCFStringEncodingUTF8);
	char* scriptPathCStr = NULL;
	if (scriptPathConstCStr == NULL) {
		CFIndex stringLength = CFStringGetLength(scriptFileSystemPath);
		CFIndex maxSize = CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8);
		scriptPathCStr = (char*)malloc((maxSize + 1) * sizeof(char));
		bool success = CFStringGetCString(scriptFileSystemPath, scriptPathCStr, maxSize, kCFStringEncodingUTF8);
		if (success == false) {
			return false;
		}
	}
	CFRelease(scriptFileSystemPath);
*/

#endif
#if TARGET_OS_WIN
	// no UTF-8 encoding
	this->utf8representation = (char*)malloc(this->numberOfCharacters + 1);
	UInt32 count = 0;
	for (UInt32 i = 0; i < this->numberOfCharacters; i++) {
		this->utf8representation[i] = (char)this->theString[i];
	}
	this->utf8representation[this->numberOfCharacters] = '\0';
#endif
	return this->utf8representation;
}



const UniChar* const VisualString::getUtf16Representation() {
#if TARGET_OS_MAC
	if (this->utf16representation != NULL) {
		free(this->utf16representation);
		this->utf16representation = NULL;
	}
	CFIndex length = CFStringGetLength(this->theString);
	this->utf16representation = (UniChar*)malloc((length * sizeof(UniChar) + 2));
	CFStringGetCharacters(this->theString, CFRangeMake(0, length), this->utf16representation);
	this->utf16representation[length] = L'\0';
	
	return this->utf16representation;
#endif
#if TARGET_OS_WIN
	return (UniChar*)this->theString;
#endif
}


UniChar VisualString::getCharacterAtIndex(UInt32 index) {
#if TARGET_OS_MAC
	return CFStringGetCharacterAtIndex(this->theString, index);
#endif
#if TARGET_OS_WIN
	return this->theString[index];
#endif
}


bool VisualString::characterIsNewlineCharacter(UniChar character) {
#if TARGET_OS_MAC
	UniCharCount uniCharArrayLength = 5;
	UniChar* uniCharArray = (UniChar*)malloc(uniCharArrayLength * sizeof(UniChar));
	uniCharArray[0] = 0x000D; // CR
    uniCharArray[1] = 0x000A; // LF
    uniCharArray[2] = 0x2028; // LSEP
    uniCharArray[3] = 0x2029; // PSEP
    uniCharArray[4] = 0x0085; // NL
	for (UniCharCount i = 0; i < uniCharArrayLength; i++) {
		if (character == uniCharArray[i]) {
			return true;
		}
	}
	delete uniCharArray;
#endif
#if TARGET_OS_WIN
	if ((character == L'\n') || (character == L'\r')) {
		return true;
	}
#endif
	return false;
}


bool VisualString::characterIsWhitespaceCharacter(UniChar character) {

// word separators
// delimiters(" \t\n~;()'\"<>:{}[]+-=&*#.,/\\");

#if TARGET_OS_MAC
	CFCharacterSetRef whitespaceAndNewlineSet = CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline);
	Boolean isWhitespace = CFCharacterSetIsCharacterMember(whitespaceAndNewlineSet, character);
	if (isWhitespace) {
		return true;
	} else {
		return false;
	}
#endif
#if TARGET_OS_WIN
	if ((character == L' ') || (character == L'\n') || (character == L'\r') || (character == L'\t')) {
		return true;
	}
#endif
	return false;
}


void VisualString::writeDebugLog() const {
#if TARGET_OS_MAC
	logString(this->theString);
	CFShowStr(this->theString);
#endif
#if TARGET_OS_WIN
	writeLogW(this->theString);
#endif
}


void VisualString::calcStatistics() {

	if (this->arrayOfWords.size() > 0) {
		this->arrayOfWords.clear();
	}
	if (this->arrayOfLines.size() > 0) {
		this->arrayOfLines.clear();
	}

	if (this->numberOfCharacters == 0) return;

	this->numberOfNonWhitespaceCharacters = 0;
	this->numberOfWords = 0;
	this->numberOfLines = 0;

	bool scannerIsInWord = false;
	bool scannerIsInLine = false;
	SInt32 lineStartPos = -1;
	SInt32 wordStartPos = -1;
	UInt32 numberOfLineCharsScanned = 0;
	UInt32 numberOfWordCharsScanned = 0;

	VisualStringSubstringPointer visualStringSubstringPointer;

	for (UInt32 i = 0; i < this->numberOfCharacters; i++) {
		if (this->characterIsNewlineCharacter(this->getCharacterAtIndex(i)) == true) {
			if (scannerIsInLine == true) {
				visualStringSubstringPointer.startIndex = lineStartPos;
				visualStringSubstringPointer.numberOfCharacters = numberOfLineCharsScanned;
				this->arrayOfLines.push_back(visualStringSubstringPointer);
				this->numberOfLines++;
				lineStartPos = -1;
				numberOfLineCharsScanned = 0;
				scannerIsInLine = false;
			}
		} else {
			if (scannerIsInLine == false) {
				scannerIsInLine = true;
				lineStartPos = i;
			}
			numberOfLineCharsScanned++;
		}
		if (this->characterIsWhitespaceCharacter(this->getCharacterAtIndex(i)) == true) {
			if (scannerIsInWord == true) {
				visualStringSubstringPointer.startIndex = wordStartPos;
				visualStringSubstringPointer.numberOfCharacters = numberOfWordCharsScanned;
				this->arrayOfWords.push_back(visualStringSubstringPointer);
				this->numberOfWords++;
				wordStartPos = -1;
				numberOfWordCharsScanned = 0;
				scannerIsInWord = false;
			}
		} else {
			if (scannerIsInWord == false) {
				scannerIsInWord = true;
				wordStartPos = i;
			}
			numberOfWordCharsScanned++;
			this->numberOfNonWhitespaceCharacters++;
		}
	}
	if (this->characterIsNewlineCharacter(this->getCharacterAtIndex(this->numberOfCharacters - 1)) == false) {
		visualStringSubstringPointer.startIndex = lineStartPos;
		visualStringSubstringPointer.numberOfCharacters = numberOfLineCharsScanned;
		this->arrayOfLines.push_back(visualStringSubstringPointer);
		this->numberOfLines++;
	}
	if (this->characterIsWhitespaceCharacter(this->getCharacterAtIndex(this->numberOfCharacters - 1)) == false) {
		visualStringSubstringPointer.startIndex = wordStartPos;
		visualStringSubstringPointer.numberOfCharacters = numberOfWordCharsScanned;
		this->arrayOfWords.push_back(visualStringSubstringPointer);
		this->numberOfWords++;
	}

}


VisualString* createWithUTF8String(const char* const aStringVal) {
	VisualString* aVisualString = new VisualString(aStringVal);
	return aVisualString;
}


VisualString* createWithUTF8Buffer(const char* const aStringVal, UInt32 numberOfBytes) {
	VisualString* aVisualString = new VisualString(aStringVal, numberOfBytes);
	return aVisualString;
}


VisualString* createWithUTF16String(const UniChar* const aStringVal) {
	VisualString* aVisualString = new VisualString(aStringVal);
	return aVisualString;
}


VisualString* createWithUTF16Buffer(const UniChar* const aStringVal, UInt32 aNumberOfCharacters) {
	VisualString* aVisualString = new VisualString(aStringVal, aNumberOfCharacters);
	return aVisualString;
}


VisualString* createWithContentsOfFile(VisualFile& aFile) {
	VisualString* aVisualString = new VisualString;
	OSStatus status = aVisualString->initWithContentsOfFile(aFile);
	if (status != noErr) {
		delete aVisualString;
		aVisualString = NULL;
	}
	return aVisualString;
}
