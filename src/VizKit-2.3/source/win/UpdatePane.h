/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: UpdatePane.h
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


namespace VizKit {

	/**
	 * UI (Windows): Pane with checkbox for automatic update.
	 */
	class CUpdatePane : public CPropertyPage {

	DECLARE_DYNCREATE(CUpdatePane)

	public:

		/** The constructor. */
		CUpdatePane();

		/** The destructor. */
		~CUpdatePane();

		//{{AFX_DATA(CUpdatePane)
		enum { IDD = IDD_DIALOG4 };
			//
		//}}AFX_DATA

		//{{AFX_VIRTUAL(CUpdatePane)

		/**
		 * Called when pane becomes active.
		 * @return True on success, false on failure.
		 */
		virtual BOOL OnSetActive();

	protected:

		/**
		 * Exchange of data (DDX/DDV).
		 * @param pDX Pointer to data.
		 */
		virtual void DoDataExchange(CDataExchange* pDX);
		//}}AFX_VIRTUAL

		/**
		 * Called when pane is initialized.
		 * @return True on success, false on failure.
		 */
		virtual BOOL OnInitDialog();

		// Generierte Nachrichtenzuordnungsfunktionen
		//{{AFX_MSG(CUpdatePane)
		afx_msg void OnChangeCheckForUpdate();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	};

}

//{{AFX_INSERT_LOCATION}}

