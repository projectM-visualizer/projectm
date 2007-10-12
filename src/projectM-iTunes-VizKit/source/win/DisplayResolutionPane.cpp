/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: DisplayResolutionPane.cpp
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

#include "DisplayResolutionPane.h"
#include "VisualConfiguration.h"
#include "VisualGraphics.h"
#include "VisualDataStore.h"
#include "VisualPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace VizKit;

IMPLEMENT_DYNCREATE(CDisplayResolutionPane, CPropertyPage)

CDisplayResolutionPane::CDisplayResolutionPane() : CPropertyPage(CDisplayResolutionPane::IDD) {
	//{{AFX_DATA_INIT(CDisplayResolutionPane)
		//
	//}}AFX_DATA_INIT
}


CDisplayResolutionPane::~CDisplayResolutionPane() {
}


void CDisplayResolutionPane::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayResolutionPane)
		//
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayResolutionPane, CPropertyPage)
	//{{AFX_MSG_MAP(CDisplayResolutionPane)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDisplayResolutionPane::OnSelchangeCombo1() {
	char str[32];
	CComboBox* pCB = (CComboBox*) GetDlgItem(IDC_COMBO1);

	pCB->GetLBText(pCB->GetCurSel(), str);

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	UInt16 horizontalPixels;
	UInt16 verticalPixels;
	UInt16 bitsPerPixel;
	UInt16 refreshRate;
	theVisualGraphics->matchDisplayResolutionShowStrWithPrefs(str, horizontalPixels, verticalPixels, bitsPerPixel, refreshRate);

	VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenWidth, horizontalPixels);
	VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenHeight, verticalPixels);
	VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenBitsPerPixel, bitsPerPixel);
	VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenRefreshRate, refreshRate);
	
	VisualDataStore::storePreferences();
	
	UInt16 minBitsPerPixel = 24;
	UInt16 maxBitsPerPixel = 32;
	VisualDataStore::setPreferredDisplayResolution(minBitsPerPixel, maxBitsPerPixel, bitsPerPixel, horizontalPixels, verticalPixels);

}


BOOL CDisplayResolutionPane::OnInitDialog() {

	CPropertyPage::OnInitDialog();

	UInt8 isSelected;

	CComboBox* pCB = (CComboBox*) GetDlgItem(IDC_COMBO1);

	VisualGraphics* theVisualGraphics;
	theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->evaluateFullscreenDisplayResolution();
	//theVisualGraphics->gatherAvailableDisplayResolutions();

	//theVisualGraphics->resetDisplayResolutionIterIndex();
	char showStr[32];
	UInt16 count = 0;
	UInt16 selIdx = 0;
	while(theVisualGraphics->getNextAvailableDisplayResolution(showStr, &isSelected)) {
		if (isSelected == 1) {
			selIdx = count;
		}
		if (pCB->AddString(showStr) == CB_ERR) {
			AfxMessageBox("AnError occurred while adding mon res item to combo list.");
		}
		count++;
	}

	pCB->SetCurSel(selIdx);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


BOOL CDisplayResolutionPane::OnSetActive() 
{
	BOOL success;
	int lastPane = 0;

	success = CPropertyPage::OnSetActive();

	if (CVisualPropertySheet::isInitialized() == true) {
		lastPane = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kPreferencePane);
		if (lastPane != 1) {
			VisualDataStore::setPreferenceValueInt(VisualConfiguration::kPreferencePane, 1);
			VisualDataStore::storePreferences();
		}
	}

	return success;

}
