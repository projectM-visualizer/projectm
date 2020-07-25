/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualPropertySheet.h
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


#pragma once


#include "resource.h"
#include <StdAfx.h>

#include "AboutPane.h"
#include "OptionsPane.h"
#include "UpdatePane.h"


namespace VizKit {

	/**
	 * UI (Windows): Property sheet which contains various panes.
	 */
	class CVisualPropertySheet : public CPropertySheet {

	DECLARE_DYNAMIC(CVisualPropertySheet)

	public:

		/**
		 * Constructs a VisualPropertySheet.\ The VisualPropertySheet internally is a singleton.\ Returns a pointer to the initialized VisualPropertySheet.
		 * @return A pointer to the singleton instance.
		 */
		static CVisualPropertySheet* getInstance();

		/**
		 * Disposes the VisualPropertySheet.
		 */
		static void dispose(void);

		/**
		 * Ansers the question whether the property sheet is initialized.
		 * @return False if singleton instance is not initialized, true if initialized. 
		 */
		static bool isInitialized(void);

		/** A constructor. */
		CVisualPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

		/** A constructor. */
		CVisualPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

		/** The destructor. */
		virtual ~CVisualPropertySheet();

		//{{AFX_VIRTUAL(CVisualPropertySheet)
		/**
		 * Called when property sheet is initialized.
		 * @return True on success, false on failure.
		 */
		virtual BOOL OnInitDialog();
		//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(CVisualPropertySheet)
		afx_msg void OnClose();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		/**
		 * Override of DestroyWindow().
		 * @remarks We implement DestroyWindow() to note that the property sheet is destructed.
		 */
		virtual BOOL DestroyWindow();

		/**
		 * Ansers the question whether the property sheet is initialized.
		 * @return False if singleton instance is not initialized, true if initialized. 
		 */
		static bool propertysheetIsInitialized;

		/** We delete any allocated memory here. */
		virtual void PostNcDestroy();

	private:

		/** About pane. */
		CAboutPane mAboutPane;

		/** Options pane. */
		COptionsPane mOptionsPane;

		/** Check for update pane. */
		CUpdatePane mUpdatePane;

	};

}

//{{AFX_INSERT_LOCATION}}

