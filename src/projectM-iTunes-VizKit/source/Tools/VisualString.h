/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualString.h
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

#ifndef VisualString_h
#define VisualString_h

#include <vector>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#include <CoreFoundation/CFBase.h> // CFStringRef
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


namespace VizKit {

#if TARGET_OS_MAC
	typedef CFMutableStringRef StringDataRef;
	typedef CFStringRef ConstStringDataRef;
#endif
#if TARGET_OS_WIN
	typedef wchar_t* StringDataRef;
	typedef const wchar_t* const ConstStringDataRef;
#endif

	class VisualFile; // Forward declaration (to avoid include of header file).

	/**
	 * Unicode string without styling or formatting information. 
	 */
	class VisualString {
	
	public:

		/**
		 * The constructor. 
		 */
		VisualString();

		/**
		 * A constructor that constructs a VisualString from a pointer to an UTF-8 string and the number of bytes of the string.
		 * @param aStringVal A pointer to the UTF-8 encoded char string value.
		 * @param numberOfBytes Optional indication of number of bytes of the char string.
		 * @remarks If the number of bytes is not specified, the string must be terminated with a NULL byte.\ The number of bytes is evaluated by calling strlen() then.
		 */
		VisualString(const char* const aStringVal, SInt32 numberOfBytes = -1);

		/**
		 * A constructor that constructs a VisualString from a pointer to a wchar_t string and the number of characters of the string.
		 * @param aStringVal A pointer to the wchar_t string value.
		 * @param aNumberOfCharacters The number of characters of the char string.
		 * @remarks If the number of characters is not specified, the string must be terminated with a NULL byte.\ The number of characters is evaluated by calling sizeof() then.
		 */
		VisualString(const UniChar* const aStringVal, SInt32 aNumberOfCharacters = -1);

		/**
		 * Initializes a VisualString from a pointer to an UTF-8 string.
		 * @param aStringVal A pointer to the UTF-8 encoded char string value.
		 * @remarks The string must be terminated with a NULL byte.
		 */
		void initWithUTF8String(const char* const aStringVal);

		/**
		 * Initializes a VisualString from a pointer to an UTF-8 string buffer.
		 * @param aStringVal A pointer to the UTF-8 encoded char string buffer.
		 * @param numberOfBytes The number of bytes of the UTF-8 encoded char string buffer.
		 */
		void initWithUTF8Buffer(const char* const aStringVal, UInt32 numberOfBytes);

		/**
		 * Initializes a VisualString from a pointer to an UTF-16 string.
		 * @param aStringVal A pointer to the UTF-16 encoded char string value.
		 * @remarks The string must be terminated with a NULL byte.
		 */
		void initWithUTF16String(const UniChar* const aStringVal);

		/**
		 * Initializes a VisualString from a pointer to an UTF-16 string buffer.
		 * @param aStringVal A pointer to the UTF-16 encoded char string buffer.
		 * @param aNumberOfCharacters The number of characters of the UTF-16 encoded char string buffer.
		 */
		void initWithUTF16Buffer(const UniChar* const aStringVal, UInt32 aNumberOfCharacters);

		/**
		 * Initializes a VisualString from the contents of a file.
		 * @param aFile Reference to a file.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks The file is opened, the content is read as a string, then the file is closed again.
		 */
		OSStatus initWithContentsOfFile(VisualFile& aFile);

		/**
		 * A constructor that constructs a VisualString from a ConstStringDataRef.
		 * @param aStringVal A string value in ConstStringDataRef data format.
		 * @remarks ConstStringDataRef is the cross-platform data format of VisualString's internal string buffer.
		 */
		VisualString(ConstStringDataRef aStringVal);

		/**
		 * The destructor. 
		 */
		~VisualString();

		/**
		 * Copy constructor.
		 * @param other Another VisualString.
		 */
		VisualString(const VisualString& other);

		/**
		 * Assignment operator.
		 */			
		VisualString& operator=(const VisualString& other);

		/**
		 * Less than operator.
		 * @param other Another VisualString.
		 * @remarks The less than operator is used by std::map for find() and sorting (if VisualString is used as key_type).
		 */
		bool operator<(const VisualString& other) const;
		
		/**
		 * Equality operator.
		 * @param other Another VisualString.
		 */
		bool operator==(const VisualString& other) const;

		/**
		 * Inequality operator.
		 * @param other Another VisualString.
		 */
		bool operator!=(const VisualString& other) const;

		/**
		 * Plus operator appends another string.
		 * @param other Another VisualString.
		 */
		const VisualString& operator+(const VisualString& other);

		/**
		 * Plus operator appends an 8-bit char string.
		 * @param charString Pointer to 8-bit character string.
		 * @remarks The charString must be NULL-terminated so that strlen can determine the correct length of the string.
		 */
		const VisualString& operator+(const char* charString);

		/**
		 * Returns the number of characters of the string.
		 * @return The number of characters of the string.
		 */
		UInt32 getNumberOfCharacters(void) const;

		/**
		 * Returns the number of non-whitespace characters of the string.
		 * @return The number of non-whitespace characters of the string.
		 */
		UInt32 getNumberOfNonWhitespaceCharacters(void) const;

		/**
		 * Returns the number of words of the string.
		 * @return The number of words of the string.
		 */
		UInt32 getNumberOfWords(void) const;

		/**
		 * Returns the number of lines of the string.
		 * @return The number of lines of the string.
		 * @remarks The number of lines is evaluated by counting the number of lines that contain at least one non-whitespace character.
		 */
		UInt32 getNumberOfLines(void) const;

		/**
		 * Returns one line of the string.
		 * @param index The requested line number.\ The first line has an index of 0.
		 * @return One line of the string.
		 * @remarks The caller has to delete the VisualString.
		 */
		VisualString* createStringWithLine(UInt32 index) const;
		
		/**
		 * Returns one word of the string.
		 * @param index The requested index number.\ The first word has an index of 0.
		 * @return One word of the string.
		 * @remarks The caller has to delete the VisualString.
		 */
		VisualString* createStringWithWord(UInt32 index) const;

		/**
		 * Returns a pointer to the internal string data.
		 * @return A pointer to the internal string data.
		 * @remarks On Mac the string data is stored as a CFString.\ On Windows the string data is stored as a wchar_t type.
		 */
		ConstStringDataRef getCharactersPointer(void) const;

		/**
		 * Returns a pointer to a char buffer with utf-8 encoded string data.
		 * @return A pointer to a char buffer with utf-8 encoded string data.
		 * @remarks The memory is owned by VisualString and deallocated on destruct of VisualString instance.
		 */
		const char* const getUtf8Representation(void);

		/**
		 * Returns a pointer to a 16-bit buffer with utf-16 encoded string data.
		 * @return A pointer to a 16-bit buffer with utf-16 encoded string data.
		 * @remarks The memory is owned by VisualString and deallocated on destruct of VisualString instance.
		 */
		const UniChar* const getUtf16Representation(void);

		/**
		 * Writes information about the VisualString to the log file.
		 * @remarks Useful for debuggin and tracing purposes.
		 */
		void writeDebugLog(void) const;
		
		/**
		 * Creates a VisualString from a pointer to an UTF-8 string.
		 * @param aStringVal A pointer to the UTF-8 encoded char string value.
		 * @remarks The string must be terminated with a NULL byte.\ The caller has to delete the VisualString.
		 */
		static VisualString* createWithUTF8String(const char* const aStringVal);

		/**
		 * Creates a VisualString from a pointer to an UTF-8 string buffer.
		 * @param aStringVal A pointer to the UTF-8 encoded char string buffer.
		 * @param numberOfBytes The number of bytes of the UTF-8 encoded char string buffer.
		 * @remarks The caller has to delete the VisualString.
		 */
		static VisualString* createWithUTF8Buffer(const char* const aStringVal, UInt32 numberOfBytes);

		/**
		 * Creates a VisualString from a pointer to an UTF-16 string.
		 * @param aStringVal A pointer to the UTF-16 encoded char string value.
		 * @remarks The string must be terminated with a NULL byte.\ The caller has to delete the VisualString.
		 */
		static VisualString* createWithUTF16String(const UniChar* const aStringVal);

		/**
		 * Creates a VisualString from a pointer to an UTF-16 string buffer.
		 * @param aStringVal A pointer to the UTF-16 encoded char string buffer.
		 * @param aNumberOfCharacters The number of characters of the UTF-16 encoded char string buffer.
		 * @remarks The caller has to delete the VisualString.
		 */
		static VisualString* createWithUTF16Buffer(const UniChar* const aStringVal, UInt32 aNumberOfCharacters);

		/**
		 * Creates a VisualString from the contents of a file.
		 * @param aFile Reference to a file.
		 * @return Error state as SInt32 OSStatus.
		 * @remarks The file is opened, the content is read as a string, then the file is closed again.
		 */
		static VisualString* createWithContentsOfFile(VisualFile& aFile);

	private:
		
		/** A pointer to the internally stored string data. */
		StringDataRef theString;
		
		/** Optionally available utf-8 representation of string data. */
		char* utf8representation;

		/** Optionally available utf-16 representation of string data. */
		UniChar* utf16representation;

		/** The number of characters of the string. */
		UInt32 numberOfCharacters;

		/** The number of characters of the string. */
		UInt32 numberOfNonWhitespaceCharacters;

		/** The number of words of the string. */
		UInt32 numberOfWords;

		/** The number of lines of the string. */
		UInt32 numberOfLines;

		/** Substring of the VisualString with start position and length. */
		typedef struct {
			SInt32 startIndex; /**< The start position of the substring. */
			UInt32 numberOfCharacters; /**< The length in number of characters. */
		} VisualStringSubstringPointer;
		
		/** VisualStringSubstringPointers can be collected in a vector of structs. */
		typedef std::vector<VisualStringSubstringPointer> ArrayOfSubstringPointers;
		
		/** The lines (rows) of the VisualString collected as substrings. */
		ArrayOfSubstringPointers arrayOfLines;
		
		/** The words of the VisualString collected as substrings. */
		ArrayOfSubstringPointers arrayOfWords;	

		/** The number of lines and words are calculated and stored internally. */
		void calcStatistics(void);

		/**
		 * Copy method for assignment operator and copy constructor.
		 * @param other Another VisualString.
		 */
		void copy(const VisualString& other);

		/** Resets internally used variables and releases allocated memory. */
		void clear(void);

		/**
		 * Returns a single character of the string data.
		 * @param index The position of the character in the string.\ First position starts with 0.
		 * @return A single character of the string data.
		 */
		UniChar getCharacterAtIndex(UInt32 index);

		/**
		 * Returns true if character is new line character.
		 * @param character The character to inspect.
		 * @return True if character is new line character, false if not.
		 */
		bool characterIsNewlineCharacter(UniChar character);

		/**
		 * Returns true if character is whitespace character.
		 * @param character The character to inspect.
		 * @return True if character is whitespace character, false if not.
		 */
		bool characterIsWhitespaceCharacter(UniChar character);
	};
	
	/** Constant VisualString. */
	typedef const VisualString ConstVisualString;
	
}

#endif /* VisualString_h */
