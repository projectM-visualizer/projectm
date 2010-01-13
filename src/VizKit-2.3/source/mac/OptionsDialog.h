/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: OptionsDialog.h
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

#ifndef OptionsDialog_h
#define OptionsDialog_h

#include <vector>

#include <Carbon/Carbon.h> // UI data types
		

namespace VizKit {

	/**
	 * UI (Mac): Shows a configuration dialog when selecting visualizer and choosing Options... menu entry in iTunes.
	 */
	class OptionsDialog {

	public:

		/**
		 * Returns a pointer to the instance.
		 */
		static OptionsDialog* getInstance(void);

		/**
		 * Displays the options dialog on screen.
		 * If the options dialog is hidden it is revealed. Otherwise it is constructed.
		 */
		void show(void);

		/**
		 * Disposes the allocated resources of the options dialog.
		 */
		static void dispose(void);

		/**
		 * Handles an event that was dispatched by installed EventHandler.
		 * @param inRef Reference to EventHandlerCallRef.
		 * @param inEvent Reference to event.
		 * @param userData Additional data.
		 */
		void handleEvent(EventHandlerCallRef inRef, EventRef inEvent, void* userData);

		/**
		 * Callback which gets called on changes of the font base.
		 */
		static void fontBaseChanged(void);
		
	private:

		/** The constructor. OptionsDialog is a singleton class. The constructor is private. New instance of class can only be created internally. */
		OptionsDialog();
		
		/** The destructor. OptionsDialog is a singleton class. The destructor is private. Instance of class can only be destructed internally. */
		~OptionsDialog();

		/**
		 * Copy constructor.
		 * @param other Another OptionsDialog.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */
		OptionsDialog(const OptionsDialog& other);

		/**
		 * Assignment operator.
		 * @remarks Explicitely declared in private section and not implemented to enforce uniqueness of singleton pattern.
		 */			
		OptionsDialog& operator=(const OptionsDialog& other);

		/**
		 * Callback for control actions.
		 * @param inRef Reference of type EventHandlerCallRef.
		 * @param inEvent Type of event.
		 * @param userData Optional additional user data.
		 */	
		static OSStatus optionsDialogControlHandler(EventHandlerCallRef inRef, EventRef inEvent, void* userData);

		/**
		 * Callback for window actions.
		 * @param inRef Reference of type EventHandlerCallRef.
		 * @param inEvent Type of event.
		 * @param userData Optional additional user data.
		 */	
		static OSStatus optionsDialogWindowHandler(EventHandlerCallRef inRef, EventRef inEvent, void* userData);

		/**
		 * Loads the window from the nib resource file.
		 * @return True on success, false on failure.
		 */
		bool loadWindowFromResource(void);

		/**
		 * Installs the event handler callbacks for the dialog.
		 * @return True on success, false on failure.
		 */
		bool installWindowEventHandler(void);

		/**
		 * Populates the font menu with the list of names of currently available fonts.
		 * @return True on success, false on failure.
		 */
		bool populateFontMenu(void);

		/**
		 * Removes the additonal data of the font menu.
		 */
		void removeFontMenuData(void);

		/**
		 * If true, the dialog window is currently displayed.
		 */
		bool isDisplayedNow;

		/**
		 * If true, the font list is created new at the next show event.
		 */
		bool doGetFontList;

		/**
		 * Shows the selected pane of the tab control (and hides the others).
		 * Unfortunately tab controls have to be made visible and invisible programmatically.
		 */
		void showSelectedPaneOfTabControl(void);
		
		/** OptionsDialog is a singleton class. Pointer to private instance is handled internally. */
		static OptionsDialog* theOptionsDialog;
		
		/** The four-char-code for identifying controls on this window. */
		static const OSType signatureOfOptionsDialog;
		
		/** The id of the tab control. */
		static const SInt32 tabControlId;
		
		/** The ControlID of the tab control. */
		static const ControlID tabControlID;
		
		/** The id of the tab pane that display about information. */
		static const SInt32 aboutTabPaneId;
		
		/** The id of the tab pane where font options can be set. */
		static const SInt32 optionsTabPaneId;
		
		/** The id of the tab pane where check for update on startup can be set. */
		static const SInt32 updateTabPaneId;
		
		/** The TabPaneVector is a vector of all panes of the tab control. */
		typedef std::vector<SInt32> TabPaneVector;
		
		/** The TabPaneVectorIterator is an iterator of the TabPaneVector. */
		typedef TabPaneVector::iterator TabPaneVectorIterator;
		
		/** A vector that contains the id's of all panes of the tab control. */
		TabPaneVector tabPaneVector;
		
		/** Reference of events handler for window events. */
		EventHandlerRef windowEventsHandlerRef;
		
		/** Reference of events handler for window control events. */
		EventHandlerRef windowControlEventsHandlerRef;
		
		/** Event type definition of control hit event. */
		static const EventTypeSpec controlHitEvent;
		
		/** Event type definition of window close event. */
		static const EventTypeSpec windowCloseEvent;

		/** Event type definition of window drag started event. */
		static const EventTypeSpec windowDragStarted;

		/** Event type definition of window drag completed event. */
		static const EventTypeSpec windowDragCompleted;
		
		/** The window of the dialog. */
		WindowRef optionsDialogWindow;

		/** The numeric id of the font pop up menu. */
		static const SInt32 kFontPopUpControlId;
		
		/** The ControlID of the font pop up menu. */
		static const ControlID kFontPopUpControlID;

		/** The numeric id of the check for update checkbox. */
		static const SInt32 kCheckForUpdateCheckboxId;

		/** The ControlID of the check for update checkbox. */
		static const ControlID kCheckForUpdateCheckboxControlID;
		
		/** Menu with font names. */
		MenuRef fontMenu;
		
		/** Sub menu of font. */
		MenuRef selectedFontSubMenu;

		/**
		 * Sets the selected font menu.
		 * @param selectedIdx The selected font menu.
		 */
		void setTrackInfoFontMenuSelectedIdx(UInt32 selectedIdx);

		/**
		 * Sets the selected font sub menu.
		 * @param selectedIdx The selected font sub menu.
		 * @param subMenuRef Reference of the font sub menu.
		 */
		void setTrackInfoFontSubMenuSelectedIdx(UInt32 selectedIdx, MenuRef subMenuRef);
		
		/** Unchecks the currently selected menu item of the font menu. */
		void uncheckTrackInfoFontMenuSelection(void);
		
		/** Selects the currently selected menu item of the font menu. */
		void checkTrackInfoFontMenuSelection(void);
		
		/** The selected index of the font submenu. */
		SInt32 trackInfoFontSubMenuSelectedIdx;
		
		/** The selected index of the font menu. */
		SInt32 trackInfoFontMenuSelectedIdx;
		
		/** The alpha (opacity) value of the window. */
		float windowAlphaValue;

		/** The alpha (opacity) value of the window when moved. */
		float windowAlphaMoveValue;

	};

}

#endif /* OptionsDialog_h */
