/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: OptionsDialog.h
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

#ifndef OptionsDialog_h
#define OptionsDialog_h

#include <vector>

#include <Carbon/Carbon.h>


namespace VizKit {

	/**
	 * UI (Mac): Shows a configuration dialog when selecting visualizer and choosing Options... menu entry in iTunes.
	 */
	class OptionsDialog {

	public:

		/**
		 * Returns a pointer to the instance.
		 */
		static OptionsDialog* getOptionsDialog(void);

		/**
		 * Displays the options dialog on screen.
		 * If the options dialog is hidden it is revealed.\ Otherwise it is constructed.
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
			
	private:

		/** The constructor.\ OptionsDialog is a singleton class.\ The constructor is private.\ New instance of class can only be created internally. */
		OptionsDialog();
		
		/** The destructor.\ OptionsDialog is a singleton class.\ The destructor is private.\ Instance of class can only be destructed internally. */
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
		
		/** The tab control. */
		ControlRef tabControl;
		
		/** The id of the tab pane that display about information. */
		static const SInt32 aboutTabPaneId;
		
		/** The id of the tab pane where fullscreen display resolution can be set. */
		static const SInt32 displayResolutionTabPaneId;
		
		/** The TabPaneVector is a vector of all panes of the tab control. */
		typedef std::vector<SInt32> TabPaneVector;
		
		/** The TabPaneVectorIterator is an iterator of the TabPaneVector. */
		typedef TabPaneVector::iterator TabPaneVectorIterator;
		
		/** A vector that contains the id's of all panes of the tab control. */
		TabPaneVector tabPaneVector;
		
		/** The id of the display resolution pop up menu control. */
		static const SInt32 displayResolutionMenuId;
		
		/** The ControlID of the display resolution pop up menu control. */
		static const ControlID displayResolutionMenuControlID;
		
		/** The pop up control is used to construct the pop up menu. */
		ControlRef displayResolutionPopUpControl;
		
		/** The menu is attached to the pop up control. */
		MenuRef displayResolutionMenu;
		
		/** The currently selected index of the display resolution menu. */
		SInt32 displayResolutionMenuSelectedIdx;
		
		/** Event type definition of control hit event. */
		static const EventTypeSpec controlHitEvent;
		
		/** Event type definition of window close event. */
		static const EventTypeSpec windowCloseEvent;
		
		/** The window of the dialog. */
		WindowRef optionsDialogWindow;
		
	};

}

#endif /* OptionsDialog_h */
