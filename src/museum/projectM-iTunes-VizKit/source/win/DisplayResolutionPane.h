/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: DisplayResolutionPane.h
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

#if !defined(AFX_DISPLAYRESOLUTIONPANE_H__E6486A83_29C1_4F7E_84D9_D4C8037CBEA4__INCLUDED_)
#define AFX_DISPLAYRESOLUTIONPANE_H__E6486A83_29C1_4F7E_84D9_D4C8037CBEA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"
#include <StdAfx.h>

namespace VizKit {

	/**
	 * UI (Windows): Pane with information about possible display resolutions.
	 */
	class CDisplayResolutionPane : public CPropertyPage {

	DECLARE_DYNCREATE(CDisplayResolutionPane)

	public:

		/** The constructor. */
		CDisplayResolutionPane();

		/** The destructor. */
		~CDisplayResolutionPane();

		//{{AFX_DATA(CDisplayResolutionPane)
//		enum { IDD = IDD_DIALOG3 };
			//
		//}}AFX_DATA

		//{{AFX_VIRTUAL(CDisplayResolutionPane)
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

		//{{AFX_MSG(CDisplayResolutionPane)
		/**
		 * Called when value of dropdown menu changed.
		 */
		afx_msg void OnSelchangeCombo1();

		/**
		 * Called when pane is initialized.
		 * @return True on success, false on failure.
		 */
		virtual BOOL OnInitDialog();

		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	};

}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DISPLAYRESOLUTIONPANE_H__E6486A83_29C1_4F7E_84D9_D4C8037CBEA4__INCLUDED_
