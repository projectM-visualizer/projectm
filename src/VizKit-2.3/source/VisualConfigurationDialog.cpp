/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualConfigurationDialog.cpp
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

#include "VisualConfigurationDialog.h"

#if TARGET_OS_WIN
#include "VisualPropertySheet.h"
#endif

#if TARGET_OS_MAC
#include "OptionsDialog.h"
#endif


using namespace VizKit;


void VisualConfigurationDialog::show() {
#if TARGET_OS_WIN
	CVisualPropertySheet* thePropertySheet = CVisualPropertySheet::getInstance();
	if (thePropertySheet != NULL) {
		thePropertySheet->BringWindowToTop();
	}
#endif
#if TARGET_OS_MAC
	OptionsDialog* theOptionsDialog = OptionsDialog::getInstance();
	if (theOptionsDialog != NULL) {
		theOptionsDialog->show();
	}
#endif
}


void VisualConfigurationDialog::dispose() {
#if TARGET_OS_WIN
	CVisualPropertySheet::dispose();
#endif
#if TARGET_OS_MAC
	OptionsDialog::dispose();
#endif
}
