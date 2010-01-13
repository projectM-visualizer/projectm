/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualString.cpp
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

#include "VisualString.h"
#include "VisualFile.h"
#include "VisualDispatch.h"
#include "VisualErrorHandling.h"
#include "VisualNetwork.h"
#include "VisualNotification.h"


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


VisualString::VisualString(const char* const aStringVal, sint32 numberOfBytes) {
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


VisualString::VisualString(const uint16* const aStringVal, sint32 aNumberOfCharacters) {
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


VisualString::VisualString(ConstStringDataRef aStringVal) {
	initWithConstStringDataRef(aStringVal);
}


void VisualString::initWithUTF8String(const char* const aStringVal) {
	this->clear();
	if (aStringVal != NULL) {
		uint32 numberOfBytes = strlen(aStringVal);
#if TARGET_OS_MAC
		Boolean isExternalRepresentation = false;
		CFStringRef theStringRef = CFStringCreateWithBytes(kCFAllocatorDefault, (const uint8*)aStringVal, numberOfBytes, kCFStringEncodingUTF8, isExternalRepresentation);
		this->numberOfCharacters = CFStringGetLength(theStringRef);
		theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, theStringRef);
		CFRelease(theStringRef);
#endif
#if TARGET_OS_WIN
		DWORD dwFlags = 0;
		int destinationSize = 0;
		numberOfBytes = -1; // utf8 string is null terminated
		// MSDN: If this parameter is -1, the function processes the entire input string, 
		// including the terminating null character. 
		// Therefore, the resulting Unicode string has a terminating null character, 
		// and the length returned by the function includes this character.
		int requiredSize = MultiByteToWideChar(CP_UTF8, dwFlags, aStringVal, numberOfBytes, theString, destinationSize);
		if (requiredSize > 0) {
			destinationSize = requiredSize;
			this->theString = (wchar_t*)malloc(destinationSize * sizeof(wchar_t));
			int sizeConverted = MultiByteToWideChar(CP_UTF8, dwFlags, aStringVal, numberOfBytes, theString, destinationSize);
			this->numberOfCharacters = destinationSize - 1;
		}
#endif
	}
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
}


void VisualString::initWithUTF8Buffer(const char* const aStringVal, uint32 numberOfBytes) {
	this->clear();
	if (aStringVal != NULL) {
#if TARGET_OS_MAC
		Boolean isExternalRepresentation = false;
		CFStringRef theStringRef = CFStringCreateWithBytes(kCFAllocatorDefault, (const uint8*)aStringVal, numberOfBytes, kCFStringEncodingUTF8, isExternalRepresentation);
		this->numberOfCharacters = CFStringGetLength(theStringRef);
		theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, theStringRef);
		CFRelease(theStringRef);
#endif
#if TARGET_OS_WIN
		DWORD dwFlags = 0;
		int destinationSize = 0;
		int requiredSize = MultiByteToWideChar(CP_UTF8, dwFlags, aStringVal, numberOfBytes, theString, destinationSize);
		if (requiredSize > 0) {
			destinationSize = requiredSize;
			this->theString = (wchar_t*)malloc((destinationSize + 1) * sizeof(wchar_t));
			int sizeConverted = MultiByteToWideChar(CP_UTF8, dwFlags, aStringVal, numberOfBytes, theString, destinationSize);
			this->theString[destinationSize] = L'\0';
			this->numberOfCharacters = destinationSize;
		}
#endif
	}
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
}


void VisualString::initWithUTF16String(const uint16* const aStringVal) {
	this->clear();
	if (aStringVal != NULL) {
#if TARGET_OS_MAC
		uint16 maxStringLength = 65536 - 1;
		const uint16* stringPointer = aStringVal;
		for (uint16 i = 0; i < (maxStringLength - 1); i++) {
			if (*stringPointer == 0) {
				this->numberOfCharacters = stringPointer - aStringVal;
				break;
			}
			stringPointer++;
		}
		this->theString = CFStringCreateMutable(kCFAllocatorDefault, 0);
		CFStringAppendCharacters(this->theString, aStringVal, (CFIndex)this->numberOfCharacters);
#endif
#if TARGET_OS_WIN
		this->numberOfCharacters = wcslen((wchar_t*)aStringVal);
		size_t sizeInBytes = (numberOfCharacters + 1) * sizeof(wchar_t);
		this->theString = (wchar_t*)malloc(sizeInBytes);
		memcpy(theString, aStringVal, sizeInBytes);
#endif
	}
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
}


void VisualString::initWithUTF16Buffer(const uint16* const aStringVal, uint32 aNumberOfCharacters) {
	this->clear();
	if (aStringVal != NULL) {
		this->numberOfCharacters = aNumberOfCharacters;
#if TARGET_OS_MAC
		this->theString = CFStringCreateMutable(kCFAllocatorDefault, 0);
		CFStringAppendCharacters(this->theString, aStringVal, (CFIndex)this->numberOfCharacters);
#endif
#if TARGET_OS_WIN
		size_t sizeInBytes = (this->numberOfCharacters + 1) * sizeof(wchar_t);
		this->theString = (wchar_t*)malloc(sizeInBytes);
		memcpy(theString, aStringVal, this->numberOfCharacters * sizeof(wchar_t));
		this->theString[this->numberOfCharacters] = L'\0';
#endif
	}
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
}


bool VisualString::initWithContentsOfFile(VisualFile& aFile) {

	bool success = false;

	size_t size;
	success = aFile.getSize(size);
	if (!success) {
		return success;
	}

	void* fileData = NULL;
	success = aFile.getData(&fileData, size);
	if (!success) {
		return success;
	}

	this->initWithUTF8Buffer((const char*)fileData, size);
	
	free(fileData);

	return success;
}


bool VisualString::initWithContentsOfURL(const char* const aURL, const VisualItemIdentifier& anId) {
	this->clear();
	VisualString urlString(aURL);
	bool success = VisualNetwork::addToDownloadQueue(urlString, this, anId);
	return success;
}


bool VisualString::initWithConstStringDataRef(ConstStringDataRef constStringDataRef) {

	theString = NULL;
	utf8representation = NULL;
	utf16representation = NULL;
	numberOfCharacters = 0;
	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	
	if (constStringDataRef != NULL) {
		
#if TARGET_OS_MAC
		this->numberOfCharacters = CFStringGetLength(constStringDataRef);
		this->theString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, constStringDataRef);
#endif
		
#if TARGET_OS_WIN
		numberOfCharacters = wcslen(constStringDataRef);
		size_t sizeInBytes = (numberOfCharacters + 1) * sizeof(wchar_t);
		theString = (wchar_t*)malloc(sizeInBytes);
		memcpy(theString, constStringDataRef, sizeInBytes);
#endif
	}

	numberOfNonWhitespaceCharacters = 0;
	numberOfWords = 0;
	numberOfLines = 0;
	this->calcStatistics();
	return true;
}


VisualString::~VisualString() {
	this->clear();
}


VisualString::VisualString(const VisualString& other) : VisualObject(other) {
	this->copy(other);
}


VisualString& VisualString::operator=(const VisualString& other) {

	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	this->clear();
	
	this->copy(other);

	return *this;
}


bool VisualString::operator<(const VisualString& other) const {
	bool compareResult = false;
	if (other.theString != NULL && this->theString != NULL) {
#if TARGET_OS_MAC
		uint16 compareOptions = 0;
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
		uint16 compareOptions = 0;
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
		size_t sizeInBytes = (other.numberOfCharacters + 1) * sizeof(wchar_t);
		this->theString = (wchar_t *)malloc(sizeInBytes);
		memcpy(theString, other.theString, sizeInBytes);
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
		this->utf16representation = (uint16*)malloc(sizeof(other.utf16representation) * sizeof(char));
		memcpy(this->utf16representation, other.utf16representation, sizeof(other.utf16representation));
	} else {
		this->utf16representation = NULL;
	}
	this->numberOfCharacters = other.numberOfCharacters;
	this->numberOfNonWhitespaceCharacters = other.numberOfNonWhitespaceCharacters;
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
	this->numberOfCharacters = 0;
	this->numberOfNonWhitespaceCharacters = 0;
	this->numberOfWords = 0;
	this->numberOfLines = 0;
	this->calcStatistics();
}


const VisualString& VisualString::operator+(const VisualString& other) {
	VisualString* stringCopy = this->clone();
	uint32 prevNumberOfCharacters = stringCopy->numberOfCharacters;
	stringCopy->numberOfCharacters += other.numberOfCharacters;
	if (stringCopy->numberOfCharacters > prevNumberOfCharacters) {
#if TARGET_OS_MAC
		if (prevNumberOfCharacters == 0) {
			stringCopy = other.clone();
		} else {
			if (other.numberOfCharacters > 0) {
				CFStringAppend(stringCopy->theString, other.getCharactersPointer());
			}
		}
#endif
#if TARGET_OS_WIN
		size_t sizeInBytes = (stringCopy->numberOfCharacters + 1) * sizeof(wchar_t);
		free(stringCopy->theString);
		stringCopy->theString = (wchar_t*)malloc(sizeInBytes);
		for (uint32 i = 0; i < prevNumberOfCharacters; i++) {
			stringCopy->theString[i] = this->theString[i];
		}
		uint32 count = 0;
		for (uint32 i = prevNumberOfCharacters; i < stringCopy->numberOfCharacters; i++) {
			stringCopy->theString[i] = other.theString[count];
			count++;
		}
		stringCopy->theString[stringCopy->numberOfCharacters] = L'\0';
#endif
	}
	stringCopy->numberOfNonWhitespaceCharacters = 0;
	stringCopy->numberOfWords = 0;
	stringCopy->numberOfLines = 0;
	stringCopy->calcStatistics();
	return *stringCopy;
}


const VisualString& VisualString::operator+(const char* charString) {
	VisualString* stringCopy = new VisualString(*this);
	uint32 prevNumberOfCharacters = stringCopy->numberOfCharacters;
	stringCopy->numberOfCharacters += strlen(charString);
	if (stringCopy->numberOfCharacters > prevNumberOfCharacters) {
#if TARGET_OS_MAC
		if (prevNumberOfCharacters == 0) {
			stringCopy->initWithUTF8String(charString);
		} else {
			CFStringAppendCString(stringCopy->theString, charString, kCFStringEncodingUTF8);
		}
#endif
#if TARGET_OS_WIN
		size_t sizeInBytes = (stringCopy->numberOfCharacters + 1) * sizeof(wchar_t);
		free(stringCopy->theString);
		stringCopy->theString = (wchar_t*)malloc(sizeInBytes);
		for (uint32 i = 0; i < prevNumberOfCharacters; i++) {
			stringCopy->theString[i] = this->theString[i];
		}
		uint32 count = 0;
		for (uint32 i = prevNumberOfCharacters; i < stringCopy->numberOfCharacters; i++) {
			stringCopy->theString[i] = (wchar_t)charString[count];
			count++;
		}
		stringCopy->theString[stringCopy->numberOfCharacters] = L'\0';
#endif
	}
	stringCopy->numberOfNonWhitespaceCharacters = 0;
	stringCopy->numberOfWords = 0;
	stringCopy->numberOfLines = 0;
	stringCopy->calcStatistics();
	return *stringCopy;
}


VisualString* VisualString::clone(void) const {
	return new VisualString(*this);
}


uint32 VisualString::getNumberOfCharacters() const {
	return this->numberOfCharacters;
}


uint32 VisualString::getNumberOfNonWhitespaceCharacters() const {
	return this->numberOfNonWhitespaceCharacters;
}


bool VisualString::isEmpty(void) const {
	if (this->numberOfNonWhitespaceCharacters == 0) {
		return true;
	}
	return false;
}


uint32 VisualString::getNumberOfWords(void) const {
	return this->numberOfWords;
}


uint32 VisualString::getNumberOfLines(void) const {
	return this->numberOfLines;
}


VisualString* VisualString::createStringWithLine(uint32 index) const {
	VisualString* stringAtLine = NULL;
#if TARGET_OS_WIN
	stringAtLine = new VisualString((uint16*)&this->theString[this->arrayOfLines[index].startIndex], this->arrayOfLines[index].numberOfCharacters);
#endif
#if TARGET_OS_MAC
	CFStringRef string = CFStringCreateWithSubstring(kCFAllocatorDefault, this->theString, CFRangeMake(this->arrayOfLines[index].startIndex, this->arrayOfLines[index].numberOfCharacters));
	stringAtLine = new VisualString(string);
	CFRelease(string);
#endif
	return stringAtLine;
}


VisualString* VisualString::createStringWithWord(uint32 index) const {
	VisualString* wordAtIndex = NULL;
#if TARGET_OS_WIN
	wordAtIndex = new VisualString((uint16*)&this->theString[this->arrayOfWords[index].startIndex], this->arrayOfWords[index].numberOfCharacters);
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
	if (this->theString != NULL) {
		CFRange range = {0, CFStringGetLength(this->theString)};
		uint8 lossByte = 0;
		Boolean isExternalRepresentation = false;
		CFIndex maxBufLen = 0;
		CFIndex numberOfBytes = 0;
		CFIndex numberOfCharsConverted;
		numberOfCharsConverted = CFStringGetBytes(this->theString, range, kCFStringEncodingUTF8, lossByte, isExternalRepresentation, NULL, maxBufLen, &numberOfBytes);
		if ((numberOfCharsConverted == CFStringGetLength(this->theString)) && (numberOfBytes > 0)) {
			this->utf8representation = (char*)malloc(numberOfBytes + 1);
			numberOfCharsConverted = CFStringGetBytes(this->theString, range, kCFStringEncodingUTF8, lossByte, isExternalRepresentation, (uint8*)this->utf8representation, numberOfBytes, &numberOfBytes);
			this->utf8representation[numberOfBytes] = '\0';
		}
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
	DWORD dwFlags = 0;
	int cbMultiByte = 0;
	LPCSTR lpDefaultChar = NULL;
	LPBOOL lpUsedDefaultChar = NULL;
	int numberOfDestinationBytes = 0;
	int numberOfBytesRequired = WideCharToMultiByte(CP_UTF8, dwFlags, this->theString, this->numberOfCharacters, this->utf8representation, numberOfDestinationBytes, lpDefaultChar, lpUsedDefaultChar);
	if (numberOfBytesRequired > 0) {
		numberOfDestinationBytes = numberOfBytesRequired;
		this->utf8representation = (char*)malloc(numberOfDestinationBytes + 1);
		int bytesConverted = WideCharToMultiByte(CP_UTF8, dwFlags, this->theString, this->numberOfCharacters, this->utf8representation, numberOfDestinationBytes, lpDefaultChar, lpUsedDefaultChar);
		this->utf8representation[numberOfDestinationBytes] = '\0';
	}
#endif

	return this->utf8representation;
}



const uint16* const VisualString::getUtf16Representation() {
#if TARGET_OS_MAC
	if (this->utf16representation != NULL) {
		free(this->utf16representation);
		this->utf16representation = NULL;
	}
	if (this->theString != NULL) {
		CFIndex length = CFStringGetLength(this->theString);
		this->utf16representation = (uint16*)malloc(((length + 1) * sizeof(uint16)));
		CFStringGetCharacters(this->theString, CFRangeMake(0, length), this->utf16representation);
		this->utf16representation[length] = 0;
	}
	
	return this->utf16representation;
#endif
#if TARGET_OS_WIN
	return (uint16*)this->theString;
#endif
}


VisualString VisualString::getSubstring(uint32 startIdx, uint32 numberOfSubstringCharacters) const {
	VisualString subString;
	if ((startIdx + numberOfSubstringCharacters) > this->numberOfCharacters) {
		return subString;
	}
	uint32 numberOfUTF16Characters = 1;
	for (uint32 i = startIdx; i < (startIdx + numberOfSubstringCharacters); i++) {
		uint16 characterAtIndex = this->getCharacterAtIndex(i);
		VisualString* character = VisualString::createWithUTF16Buffer(&characterAtIndex, numberOfUTF16Characters);
		if (character) {
			subString = subString + *character;
			delete character;
		}
	}
	return subString;
}


void VisualString::normalizeLineEndings() {
#if TARGET_OS_WIN
	VisualString* normalizedString = new VisualString;
	size_t numberOfCharacters = this->getNumberOfCharacters();
	uint32 numberOfCharactersOfSubstring = 1;
	uint16 verticalTabChar = 0x000B; // 11
	uint16 lineFeedChar = 0x000A; // 10
	uint16 carriageReturnChar = 0x000D; // 13
	VisualString verticalTab;
	verticalTab.initWithUTF16Buffer(&verticalTabChar, numberOfCharactersOfSubstring);
	VisualString lineFeed;
	lineFeed.initWithUTF16Buffer(&lineFeedChar, numberOfCharactersOfSubstring);
	VisualString carriageReturn;
	carriageReturn.initWithUTF16Buffer(&carriageReturnChar, numberOfCharactersOfSubstring);

	for (size_t i = 0; i < numberOfCharacters; i++) {
		VisualString character = this->getSubstring(i, numberOfCharactersOfSubstring);
		if ((character == carriageReturn) && (i < (numberOfCharacters - 1))) {
			*normalizedString = *normalizedString + character;
			VisualString nextCharacter = this->getSubstring(i + 1, numberOfCharactersOfSubstring);
			if (nextCharacter != lineFeed) {
				*normalizedString = *normalizedString + lineFeed;
			}
		} else if ((character == carriageReturn) && (i == (numberOfCharacters - 1))) {
			*normalizedString = *normalizedString + character;
			*normalizedString = *normalizedString + lineFeed;
		} else if (character == verticalTab) {
			*normalizedString = *normalizedString + carriageReturn;
			*normalizedString = *normalizedString + lineFeed;
		} else {
			*normalizedString = *normalizedString + character;
		}
	}
#endif
#if TARGET_OS_MAC
	VisualString* normalizedString = this->clone();
#endif

	this->initWithUTF16Buffer(normalizedString->getUtf16Representation(), normalizedString->getNumberOfCharacters());

	delete normalizedString;
}


uint16 VisualString::getCharacterAtIndex(uint32 index) const {
#if TARGET_OS_MAC
	return CFStringGetCharacterAtIndex(this->theString, index);
#endif
#if TARGET_OS_WIN
	return this->theString[index];
#endif
}


bool VisualString::characterIsNewlineCharacter(uint16 character) {
#if TARGET_OS_MAC
	uint8 uniCharArrayLength = 5;
	uint16* uniCharArray = (uint16*)malloc(uniCharArrayLength * sizeof(uint16));
	uniCharArray[0] = 0x000D; // CR
    uniCharArray[1] = 0x000A; // LF
    uniCharArray[2] = 0x2028; // LSEP
    uniCharArray[3] = 0x2029; // PSEP
    uniCharArray[4] = 0x0085; // NL
	for (uint8 i = 0; i < uniCharArrayLength; i++) {
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


bool VisualString::characterIsWhitespaceCharacter(uint16 character) {

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
	logString(this);
	CFShowStr(this->theString);
#endif
#if TARGET_OS_WIN
	writeLogW(this->theString);
#endif
}


std::vector<VisualString> VisualString::splitWithDelimiter(const VisualString& delimiter) {
	std::vector<VisualString> stringArray;
	// todo
/*
#if TARGET_OS_MAC
	CFArrayRef array = CFStringCreateArrayBySeparatingStrings(kCFAllocatorDefault, this->getCharactersPointer(), delimiter.getCharactersPointer());
	for (CFIndex i = 0; i < CFArrayGetCount(array); i++) {
		CFStringRef stringRef = (CFStringRef)CFArrayGetValueAtIndex(array, i);
		CFIndex length = CFStringGetLength(stringRef);
		uint16* unicodeBuffer = (uint16*)malloc(length * sizeof(uint16));
		CFStringGetCharacters(stringRef, CFRangeMake(0, length), unicodeBuffer);
		VisualString visualString;
		visualString.initWithUTF16Buffer(unicodeBuffer, length);
		stringArray.push_back(visualString);
		free(unicodeBuffer);
	}
	CFRelease(array);
#endif
*/

//#if TARGET_OS_WIN
	uint16 delimiterChar = const_cast<VisualString&>(delimiter).getCharacterAtIndex(0);
	VisualStringSubstringPointer substring;
	substring.startIndex = 0;
	substring.numberOfCharacters = 0;
	uint32 currCharPointerEnd = 0;
	uint32 prevCharPointerEnd = 0;
	for (uint32 i = 0; i < this->getNumberOfCharacters(); i++) {
		uint16 currChar = this->getCharacterAtIndex(i);
		if (currChar == delimiterChar) {
			substring.numberOfCharacters = currCharPointerEnd - prevCharPointerEnd;
			uint16* unicodeBuffer = (uint16*)malloc(substring.numberOfCharacters * sizeof(uint16));
			uint16* stringBufferSrc = const_cast<uint16*>(this->getUtf16Representation());
			stringBufferSrc += prevCharPointerEnd;
			memcpy(unicodeBuffer, stringBufferSrc, substring.numberOfCharacters * sizeof(uint16));
			VisualString visualString;
			visualString.initWithUTF16Buffer(unicodeBuffer, substring.numberOfCharacters);
			stringArray.push_back(visualString);
			free(unicodeBuffer);
			substring.startIndex = i + 1;
			prevCharPointerEnd = i + 1;
		}
		currCharPointerEnd++;
	}
	substring.numberOfCharacters = currCharPointerEnd - prevCharPointerEnd;
	if (substring.numberOfCharacters > 0) {
		uint16* unicodeBuffer = (uint16*)malloc(substring.numberOfCharacters * sizeof(uint16));
		uint16* stringBufferSrc = const_cast<uint16*>(this->getUtf16Representation());
		stringBufferSrc += prevCharPointerEnd;
		memcpy(unicodeBuffer, stringBufferSrc, substring.numberOfCharacters * sizeof(uint16));
		VisualString visualString;
		visualString.initWithUTF16Buffer(unicodeBuffer, substring.numberOfCharacters);
		stringArray.push_back(visualString);
		free(unicodeBuffer);
	}
//#endif
	return stringArray;
}


void VisualString::trim() {
#if TARGET_OS_MAC
	CFStringTrimWhitespace(this->theString);
#endif
}


VisualString* VisualString::createJoinedString(std::vector<VisualString> stringVector) {
	VisualString string;
	uint32 numberOfLines = stringVector.size();
	for (uint32 i = 0; i < numberOfLines; i++) {
		string = string + stringVector[i];
		string = string + "\n";
	}
	VisualString* returnString = VisualString::createWithUTF8String(string.getUtf8Representation());
	return returnString;
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
	sint32 lineStartPos = -1;
	sint32 wordStartPos = -1;
	uint32 numberOfLineCharsScanned = 0;
	uint32 numberOfWordCharsScanned = 0;

	VisualStringSubstringPointer visualStringSubstringPointer;

	for (uint32 i = 0; i < this->numberOfCharacters; i++) {
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


VisualString* VisualString::createWithUTF8String(const char* const aStringVal) {
	VisualString* aVisualString = new VisualString(aStringVal);
	return aVisualString;
}


VisualString* VisualString::createWithUTF8Buffer(const char* const aStringVal, uint32 numberOfBytes) {
	VisualString* aVisualString = new VisualString(aStringVal, numberOfBytes);
	return aVisualString;
}


VisualString* VisualString::createWithUTF16String(const uint16* const aStringVal) {
	VisualString* aVisualString = new VisualString(aStringVal);
	return aVisualString;
}


VisualString* VisualString::createWithUTF16Buffer(const uint16* const aStringVal, uint32 aNumberOfCharacters) {
	VisualString* aVisualString = new VisualString(aStringVal, aNumberOfCharacters);
	return aVisualString;
}


VisualString* VisualString::createWithContentsOfFile(VisualFile& aFile) {
	VisualString* aVisualString = new VisualString;
	bool success = aVisualString->initWithContentsOfFile(aFile);
	if (!success) {
		delete aVisualString;
		aVisualString = NULL;
	}
	return aVisualString;
}


VisualString* VisualString::createWithContentsOfURL(const char* const aURL, const VisualItemIdentifier& anId) {
	VisualString* aVisualString = new VisualString;
	bool success = aVisualString->initWithContentsOfURL(aURL, anId);
	if (!success) {
		delete aVisualString;
		aVisualString = NULL;
	}
	return aVisualString;
}


VisualString* VisualString::createWithConstStringDataRef(ConstStringDataRef constStringDataRef) {
	VisualString* aVisualString = new VisualString;
	bool success = aVisualString->initWithConstStringDataRef(constStringDataRef);
	if (!success) {
		delete aVisualString;
		aVisualString = NULL;
	}
	return aVisualString;
}


void VisualString::dataLoadDidEnd(const VisualItemIdentifier& identifier) {
	if (this->hasData()) {
		this->initWithUTF8Buffer((const char*)this->getData(), this->getDataSize());
	}
	this->freeData();
	if (this->theString != NULL) {
		VisualNotification aNotification(identifier);
		aNotification.setKey(kStringWithIdentifierLoadedAndCreatedMsg);
		aNotification.setPointer(this);
		aNotification.post();
	}
}
