/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: AboutPane.cpp
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

#include "AboutPane.h"
#include "VisualDataStore.h"
#include "VisualPropertySheet.h"
#include "VisualPreferences.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace VizKit;

IMPLEMENT_DYNCREATE(CAboutPane, CPropertyPage)

CAboutPane::CAboutPane() : CPropertyPage(CAboutPane::IDD) {
	//{{AFX_DATA_INIT(CAboutPane)
		//
	//}}AFX_DATA_INIT
}


CAboutPane::~CAboutPane() {
}


void CAboutPane::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutPane)
		// 
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutPane, CPropertyPage)
	//{{AFX_MSG_MAP(CAboutPane)
		//
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CAboutPane::OnSetActive() 
{
	BOOL success;
	int lastPane = 0;
	
	success = CPropertyPage::OnSetActive();

	if (CVisualPropertySheet::isInitialized() == true) {
		lastPane = VisualPreferences::getValue(VisualPreferences::kPreferencePane);
		if (lastPane != 0) {
			VisualPreferences::setValue(VisualPreferences::kPreferencePane, 0);
			VisualPreferences::storeValues();
		}
	}

	return success;
}
