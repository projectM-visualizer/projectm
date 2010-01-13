/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualPropertySheet.cpp
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

#include "VisualPropertySheet.h"
#include "VisualDataStore.h"
#include "VisualErrorHandling.h"
#include "VisualPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace VizKit;


bool CVisualPropertySheet::propertysheetIsInitialized = false;


IMPLEMENT_DYNAMIC(CVisualPropertySheet, CPropertySheet)


BEGIN_MESSAGE_MAP(CVisualPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CVisualPropertySheet)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CVisualPropertySheet::CVisualPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage) {
}


CVisualPropertySheet::CVisualPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage) {
	AddPage(&mAboutPane);
	AddPage(&mOptionsPane);
	AddPage(&mUpdatePane);
}


CVisualPropertySheet* CVisualPropertySheet::getInstance() {
	static CVisualPropertySheet* theCVisualPropertySheet = NULL;
	if (CVisualPropertySheet::propertysheetIsInitialized == false) {
		theCVisualPropertySheet = new CVisualPropertySheet("Visualizer Options");
		BOOL success = theCVisualPropertySheet->Create();
		if (!success) {
			delete theCVisualPropertySheet;
			theCVisualPropertySheet = NULL;
			char errLog[256];
			sprintf(errLog, "err after CVisualPropertySheet::Create() in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
	}
	return theCVisualPropertySheet;
}


void CVisualPropertySheet::dispose() {
	if (CVisualPropertySheet::propertysheetIsInitialized == true) {
		CVisualPropertySheet* theCVisualPropertySheet = CVisualPropertySheet::getInstance();
		theCVisualPropertySheet->DestroyWindow();
	}
}


bool CVisualPropertySheet::isInitialized() {
	return CVisualPropertySheet::propertysheetIsInitialized;
}


CVisualPropertySheet::~CVisualPropertySheet() {
	CVisualPropertySheet::propertysheetIsInitialized = false;
}


void CVisualPropertySheet::OnClose() {
	CPropertySheet::OnClose();
	DestroyWindow();
}


BOOL CVisualPropertySheet::DestroyWindow() {
	return CPropertySheet::DestroyWindow();
}


BOOL CVisualPropertySheet::OnInitDialog() 
{
	BOOL success = CPropertySheet::OnInitDialog();

	CRect rectWnd;
	CRect rectBtn;
	int lastActivePage = 0;

	GetWindowRect(rectWnd);
	
	CWnd* pOKButton = GetDlgItem(IDOK);
	pOKButton->GetWindowRect(rectBtn);

	ASSERT(pOKButton);
	pOKButton->ShowWindow(SW_HIDE);

	CWnd* pCancelButton = GetDlgItem(IDCANCEL);
	ASSERT(pCancelButton);
	pCancelButton->ShowWindow(SW_HIDE);

	CWnd* pApplyButton = GetDlgItem(ID_APPLY_NOW);
	ASSERT(pApplyButton);
	pApplyButton->ShowWindow(SW_HIDE);

	rectWnd.bottom = rectBtn.top;
	
	MoveWindow(rectWnd);

	lastActivePage = VisualPreferences::getValue(VisualPreferences::kPreferencePane);

	if (lastActivePage != 0) {
		SetActivePage(lastActivePage);
	}

	CVisualPropertySheet::propertysheetIsInitialized = true;

	return success;
}


void CVisualPropertySheet::PostNcDestroy() {
	delete this;
	CPropertySheet::PostNcDestroy();
}
