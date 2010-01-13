/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualFontManager.h
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

#ifndef VisualFontManager_h
#define VisualFontManager_h


#include "VisualTypes.h"

#if TARGET_OS_MAC
#include <ApplicationServices/../Frameworks/ATS.framework/Headers/ATSFont.h> // ATSFontNotificationInfoRef
#include <Carbon/../Frameworks/HIToolbox.framework/Headers/Menus.h> // MenuRef
#endif

#if TARGET_OS_WIN
#include <windows.h>
#endif

#include <vector>


/** Font base changed callback */
typedef void (*FontBaseChangedCallback)();


#if TARGET_OS_WIN
		class fontItem {
		public:
			char name[64];
		};
#endif


namespace VizKit {

	/**
	 * Return values for explicit font activation.
	 */
	typedef enum {
		fontActivationUndefined = 0, /**< Undefined state. */
		fontActivationNoError, /**< No error. */
		fontActivationFontAlreadyAvailable, /**< The font was already available (no activation of the font resource has been necessary). */
		fontActivationUnableToActivateFont /**< Unable to activate font. */
	} VisualFontActivationResult;

	/**
	 * Management of font resources.
	 */
	class VisualFontManager {

	public:

		/**
		 * Constructs a VisualFontManager. The VisualFontManager internally is a singleton. Returns a pointer to the initialized VisualFontManager.
		 * @return A pointer to the singleton instance.
		 */
		static VisualFontManager* getInstance(void);

		/**
		 * Disposes the VisualFontManager.
		 */	
		static void dispose(void);

#if TARGET_OS_MAC
		/**
		 * Called by system on changes of the font base.
		 * @param info Optional info.
		 * @param refCon Optional reference data.
		 */
		static void fontBaseDidChangeCallback(ATSFontNotificationInfoRef info, void* refCon);
#endif

		/**
		 * Sets up the observer for changes of the base of installed and available fonts (Mac-only implementation).
		 */
		static void setupFontBaseChangeObserver(void);

		/**
		 * Removes the observer for changes of the base of installed and available fonts (Mac-only implementation).
		 */
		static void cleanupFontBaseChangeObserver(void);

		/**
		 * Adds a callback function as observer of font base changes.
		 * @param callback The callback function pointer to call on changes of the font base.
		 */
		static void addFontBaseChangedObserver(FontBaseChangedCallback callback);

		/**
		 * Removes a callback function as observer of font base changes.
		 * @param callback The callback function pointer to remove.
		 */
		static void removeFontBaseChangedObserver(FontBaseChangedCallback callback);

		/**
		 * Answers the question whether a given font is available as private resource of the visualizer.
		 * @param fontName The name of the font.
		 * @return True if font is available as private resource of the visualizer, false if not.
		 * @remarks This function is a stub, custom tailored for VizKit and its packaging.
		 */
		static bool fontIsAvailableAsVisualizerResource(const char* const fontName);

		/**
		 * Tries to activate font by loading a font resource.
		 * @param fontName The name of the font.
		 * @return The result of the operation.
		 */
		static VisualFontActivationResult activateFont(const char* const fontName);

		/**
		 * Returns an array of names of currenty installed and available fonts.
		 * @return An array of names of currenty installed and available fonts.
		 */
#if TARGET_OS_MAC
		static CFArrayRef getListOfFontNames(void);
#endif
		
#if TARGET_OS_WIN
		static int CALLBACK EnumFontFamExProc2(
						ENUMLOGFONTEX *lpelfe,    // logical-font data
						NEWTEXTMETRICEX *lpntme,  // physical-font data
						DWORD FontType,           // type of font
						LPARAM lParam             // application-defined data
		);
		void resetFontVectorIterIndex(void);
		void gatherFonts(void);
		bool getNextFontName(char* fontName);
#endif

		/**
		 * Sorts the gathered fonts by name.
		 */
		void sortFontsByName(void);

		/**
		 * Adds a font name to the gathered list of font names.
		 * @param fontName The name of the font to add.
		 */
		void addFont(const char* const fontName);


#if TARGET_OS_WIN
		/**
		 * The function queries the system to determine if the font is installed. Windows-only.
		 * @param currHdc Pointer to the current device.
		 * @param fontName The name of the font.
		 * @return 1 if font is installed, 0 if font is not installed.
		 */
		uint8 fontIsInstalled(HDC currHdc, const char* const fontName);
#endif


#if TARGET_OS_MAC
		/**
		 * Populates a font menu.
		 * @param fontMenu The font menu.
		 * @param[out] numberOfFontNames The number of font names.
		 * @param selectedFontName The font name to be selected.
		 * @param[out] menuSelectedIdx The index of the selected menu item.
		 * @param[out] subMenuSelectedIdx The index of the selected sub menu item.
		 * @param[out] fontSubMenu The MenuRef of the currently selected sub menu.
		 */
		static void populateFontMenu(MenuRef fontMenu, CFIndex& numberOfFontNames, CFStringRef selectedFontName, SInt32& menuSelectedIdx, SInt32& subMenuSelectedIdx, MenuRef* fontSubMenu);

		/**
		 * Returns the name of a selected font of a font menu.
		 * @param fontMenu The font menu.
		 * @param selectedMenuItem The selected menu item.
		 * @param selectedSubMenu[out] The selected sub menu.
		 * @param selectedSubMenuItem[out] The selected sub menu item.
		 * @param[out] selectedFontName The name of the selected font.
		 */
		static void getSelectedFontName(MenuRef fontMenu, SInt32 selectedMenuItem, MenuRef selectedSubMenu, MenuItemIndex selectedSubMenuItem, char** selectedFontName);

		/**
		 * Removes the data of a populated font menu.
		 * @param fontMenu The font menu.
		 */
		static void removeFontMenuData(MenuRef fontMenu);

#endif

	private:
	
		/** The constructor. VisualFontManager is a singleton class. The constructor is private. New instance of class can only be created internally. */
		VisualFontManager();

		/** The destructor. VisualFontManager is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~VisualFontManager();

		/**
		 * Copy constructor.
		 * @param other Another VisualFontManager.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		VisualFontManager(const VisualFontManager& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		VisualFontManager& operator=(const VisualFontManager& other);

		/** VisualFontManager is a singleton class. Pointer to private instance is handled internally. */
		static VisualFontManager* theVisualFontManager;

		/**
		 * Sets the function pointers for the Cocoa function calls.
		 */	
		void initCocoaFunctionPointers(void);

		/** Reference of notification for font base changes (Mac-only). */
#if TARGET_OS_MAC
		ATSFontNotificationRef* fontBaseChangedNotificationRef;
#endif

		/** A FontBaseChangedCallbackVector is a vector of registered callbacks. */
		typedef std::vector<FontBaseChangedCallback> FontBaseChangedCallbackVector;
		
		/** The fontBaseChangedCallbackVector is a vector of registered callbacks. */
		FontBaseChangedCallbackVector fontBaseChangedCallbackVector;

		/** A FontBaseChangedCallbackVectorIterator is an iterator of a FontBaseChangedCallbackVector. */
		typedef FontBaseChangedCallbackVector::iterator FontBaseChangedCallbackVectorIterator;

#if TARGET_OS_WIN
		std::vector<fontItem*> FontVector;
		uint16 fontVectorIterIdx;
#endif

#if TARGET_OS_MAC
		//	Function pointer prototypes to the Mach-O Cocoa wrappers
		typedef Boolean (*FontIsAvailableProc)(CFStringRef fontName);
		FontIsAvailableProc doFontIsAvailable;

		typedef OSStatus (*ActivateFontProc)(CFStringRef fontName);
		ActivateFontProc doActivateFont;

		typedef CFArrayRef (*GetListOfFontNamesProc)();
		GetListOfFontNamesProc doGetListOfFontNames;

		typedef void (*ReleaseCocoaAutoreleasePoolProc)();
		ReleaseCocoaAutoreleasePoolProc doReleaseCocoaAutoreleasePool;

		CFBundleRef bundleRef;
#endif

	};
	
}

#endif /* VisualFontManager_h */
