/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: OptionsPane.cpp
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

#include "OptionsPane.h"
#include "VisualConfiguration.h"
#include "VisualFontManager.h"
#include "VisualDataStore.h"
#include "VisualErrorHandling.h"
#include "VisualPropertySheet.h"
#include "VisualPreferences.h"
#include "VisualMainAction.h"
#include "VisualPlayerState.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace VizKit;

IMPLEMENT_DYNCREATE(COptionsPane, CPropertyPage)

COptionsPane::COptionsPane() : CPropertyPage(COptionsPane::IDD) {
	//{{AFX_DATA_INIT(COptionsPane)
		//
	//}}AFX_DATA_INIT
}


COptionsPane::~COptionsPane() {
}


void COptionsPane::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPane)
		//
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsPane, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsPane)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL COptionsPane::OnInitDialog() {

	CPropertyPage::OnInitDialog();

	CComboBox* pCB = (CComboBox*) GetDlgItem(IDC_COMBO1);

	VisualFontManager* theVisualFontManager = VisualFontManager::getInstance();
	theVisualFontManager->gatherFonts();

	char fontNamePref[512];
	VisualPreferences::getValue(VisualPreferences::kTrackInfoFont, fontNamePref);
	if (VisualFontManager::fontIsAvailableAsVisualizerResource(fontNamePref) == true) {
		VisualFontActivationResult result = VisualFontManager::activateFont(fontNamePref);
		if (result == fontActivationUnableToActivateFont) {
			VisualPreferences::getValue(VisualPreferences::kFallbackSystemFont, fontNamePref);
		} else {
			theVisualFontManager->addFont(fontNamePref);
		}
	}

	theVisualFontManager->sortFontsByName();
	theVisualFontManager->resetFontVectorIterIndex();
	uint16 selIdx = 0;
	uint16 count = 0;
	char fontName[512];
	while (theVisualFontManager->getNextFontName(fontName)) {
		if (pCB->AddString(fontName) == CB_ERR) {
			char errLog[256];
			sprintf(errLog, "An error occurred while adding font item to combo list in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
		if (strcmp(fontName, fontNamePref) == 0) {
			selIdx = count;
		}
		count++;
	}

	pCB->SetCurSel(selIdx);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void COptionsPane::OnSelchangeCombo1() 
{
	char fontName[128];
	CComboBox* pCB = (CComboBox*) GetDlgItem(IDC_COMBO1);
	pCB->GetLBText(pCB->GetCurSel(), fontName);
	VisualPreferences::setValue(VisualPreferences::kTrackInfoFont, fontName);
	VisualPreferences::storeValues();
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		VisualMainAction::postTrackInfoOfCurrentAudioData();
	}
}

BOOL COptionsPane::OnSetActive() 
{
	BOOL success;
	int lastPane = 0;

	success = CPropertyPage::OnSetActive();

	if (CVisualPropertySheet::isInitialized() == true) {
		lastPane = VisualPreferences::getValue(VisualPreferences::kPreferencePane);
		if (lastPane != 1) {
			VisualPreferences::setValue(VisualPreferences::kPreferencePane, 1);
			VisualPreferences::storeValues();
		}
	}

	return success;

}
