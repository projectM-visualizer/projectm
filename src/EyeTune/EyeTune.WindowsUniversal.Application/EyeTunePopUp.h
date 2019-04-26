//*********************************************************
// Designed for use in EyeTune Windows Store UWP Application.
// RobertPancoast77@gmail.com
// 
//*********************************************************
//
// This file displays a message box to the user
//
//
//
//*********************************************************

#pragma once

using namespace Windows::UI::Popups;

void Popup(Platform::String^ text)
{
	// Show a message dialog with the text
	MessageDialog^ messageDialog = ref new MessageDialog(text);
	messageDialog->ShowAsync();
}

