/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: UpdatePane.cpp
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

#include "UpdatePane.h"
#include "VisualDataStore.h"
#include "VisualPropertySheet.h"
#include "VisualPreferences.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace VizKit;

IMPLEMENT_DYNCREATE(CUpdatePane, CPropertyPage)

CUpdatePane::CUpdatePane() : CPropertyPage(CUpdatePane::IDD) {
	//{{AFX_DATA_INIT(CUpdatePane)
		//
	//}}AFX_DATA_INIT
}


CUpdatePane::~CUpdatePane() {
}


void CUpdatePane::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdatePane)
		// 
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdatePane, CPropertyPage)
	//{{AFX_MSG_MAP(CUpdatePane)
	ON_BN_CLICKED(IDC_CHECK_FOR_UPDATE, OnChangeCheckForUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CUpdatePane::OnChangeCheckForUpdate() {

	int checkState;

	CButton* pB = (CButton*) GetDlgItem(IDC_CHECK_FOR_UPDATE);
	checkState = pB->GetCheck();

	if (checkState == BST_CHECKED) {
		VisualPreferences::setValue(VisualPreferences::kCheckForUpdate, true);
	} else {
		VisualPreferences::setValue(VisualPreferences::kCheckForUpdate, false);
	}
	VisualPreferences::storeValues();
}


BOOL CUpdatePane::OnInitDialog() {

	CPropertyPage::OnInitDialog();

	CButton* pB = (CButton*) GetDlgItem(IDC_CHECK_FOR_UPDATE);
	bool doCheckForUpdate = VisualPreferences::getValue(VisualPreferences::kCheckForUpdate);
	if (doCheckForUpdate == true) {
		pB->SetCheck(BST_CHECKED);
	} else {
		pB->SetCheck(BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


BOOL CUpdatePane::OnSetActive() 
{
	BOOL success;
	int lastPane = 0;
	
	success = CPropertyPage::OnSetActive();

	if (CVisualPropertySheet::isInitialized() == true) {
		lastPane = VisualPreferences::getValue(VisualPreferences::kPreferencePane);
		if (lastPane != 0) {
			VisualPreferences::setValue(VisualPreferences::kPreferencePane, 2);
			VisualPreferences::storeValues();
		}
	}

	return success;
}
