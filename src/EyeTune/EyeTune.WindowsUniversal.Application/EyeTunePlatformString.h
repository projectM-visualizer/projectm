//*********************************************************
// Designed for use in EyeTune Windows Store UWP Application.
// RobertPancoast77@gmail.com
// 
//*********************************************************
//
// This file converts strings for Windows Store apps.
//
//
//
//*********************************************************

#pragma once
#include <string>

// https://gist.github.com/mancoast/05f74b1a407f9a8ae91e8291e5bec0da
std::string Managed_Str_To_Std_Str(Platform::String^ ms)
{
	std::wstring w_str(ms->Begin());
	return std::string(w_str.begin(), w_str.end());
}

Platform::String^ Std_Str_To_Managed_Str(const std::string & input)
{
	std::wstring w_str = std::wstring(input.begin(), input.end());
	const wchar_t* w_chars = w_str.c_str();
	return (ref new Platform::String(w_chars));
}

