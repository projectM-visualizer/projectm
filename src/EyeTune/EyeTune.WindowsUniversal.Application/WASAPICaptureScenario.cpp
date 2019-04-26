//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario4.xaml.cpp
// Implementation of the Scenario4 class
//


// The M$ header is required to be preserved, this file: modified.
// After making all the required changes for Loopback, etc
// there is extensive divergence from the original to this file.

// https://github.com/mancoast/Windows-universal-samples
// Microsoft / Windows - universal - samples licensed under the
// MIT License
//
// Modified for use in EyeTune Windows Store UWP Application.
// RobertPancoast77@gmail.com
// 


#include "EyeTuneApp.h"
#include "projectM.hpp"
#include "Renderer.hpp"
#include <locale> 
#include <codecvt>

using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Microsoft::WRL;
using namespace Platform;

using namespace EyeTune;

// Event callback from WASAPI capture for changes in device state
void App::OnDeviceStateChange(Object^ sender, DeviceStateChangedEventArgs^ e)
{
	String^ strMessage = "";

	// Get the current time for messages
	auto t = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::LongTime;
	Windows::Globalization::Calendar^ calendar = ref new Windows::Globalization::Calendar();
	calendar->SetToNow();

	// Update Control Buttons
	//m_CoreDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
	//	[this, e]()
	//{
	//	UpdateMediaControlUI(e->State);
	//}));

	// Handle state specific messages
	switch (e->State)
	{
	case DeviceState::DeviceStateInitialized:
		m_spCapture->StartCaptureAsync();
		break;

	case DeviceState::DeviceStateCapturing:
		if (m_IsLowLatency == true)
		{
			strMessage = "Capture Started (minimum latency) @" + t->Format(calendar->GetDateTime());
		}
		else
		{
			strMessage = "Capture Started (normal latency) @" + t->Format(calendar->GetDateTime());
		}

		//ShowStatusMessage(strMessage, NotifyType::StatusMessage);
		break;

	case DeviceState::DeviceStateDiscontinuity:
	{
		m_DiscontinuityCount++;

		// Should always have a discontinuity when starting the capture, so will disregard it
		if (m_DiscontinuityCount > 1)
		{
			strMessage = "DISCONTINUITY DETECTED: " + t->Format(calendar->GetDateTime()) + " (Count = " + (m_DiscontinuityCount - 1) + ")";
			//ShowStatusMessage(strMessage, NotifyType::StatusMessage);
		}
	}
	break;

	case DeviceState::DeviceStateFlushing:
		//PlotDataReadyEvent::PlotDataReady -= m_plotDataReadyToken;
		m_plotDataReadyToken.Value = 0;

		//ShowStatusMessage("Finalizing WAV Header.  This may take a few minutes...", NotifyType::StatusMessage);

		//m_CoreDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
		//	[this]()
		//{
		//	m_Oscilloscope->Points->Clear();

		//	Windows::Foundation::Point p;
		//	p.X = OSC_START_X;
		//	p.Y = OSC_START_Y;

		//	m_Oscilloscope->Points->Append(p);

		//	p.X = OSC_X_LENGTH;
		//	p.Y = OSC_START_Y;

		//	m_Oscilloscope->Points->Append(p);
		//}));
		break;

	case DeviceState::DeviceStateStopped:
		// For the stopped state, completely tear down the audio device
		m_spCapture = nullptr;

		if (m_deviceStateChangeToken.Value != 0)
		{
			m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
			m_StateChangedEvent = nullptr;
			m_deviceStateChangeToken.Value = 0;
		}

		//ShowStatusMessage("Capture Stopped", NotifyType::StatusMessage);
		break;

	case DeviceState::DeviceStateInError:
		HRESULT hr = e->hr;

		if (m_deviceStateChangeToken.Value != 0)
		{
			m_StateChangedEvent->StateChangedEvent -= m_deviceStateChangeToken;
			m_StateChangedEvent = nullptr;
			m_deviceStateChangeToken.Value = 0;
		}

		m_spCapture = nullptr;

		wchar_t hrVal[11];
		swprintf_s(hrVal, 11, L"0x%08x\0", hr);
		String^ strHRVal = ref new String(hrVal);

		// Specifically handle a couple of known errors
		switch (hr)
		{
		case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
			strMessage = "ERROR: File Not Found (" + strHRVal + "). Check the Sound control panel for an active recording device.";
			//ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
			break;

		case AUDCLNT_E_RESOURCES_INVALIDATED:
			strMessage = "ERROR: Endpoint Lost Access To Resources (" + strHRVal + ")";
			//ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
			break;

		case E_ACCESSDENIED:
			strMessage = "ERROR: Access Denied (" + strHRVal + ").  Check 'Settings->Permissions' for access to Microphone.";
			//ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
			break;

		default:
			strMessage = "ERROR: " + strHRVal + " has occurred.";
			//ShowStatusMessage(strMessage, NotifyType::ErrorMessage);
		}
	}
}


//
//  InitializeCapture()
//
void App::InitializeCapture(Platform::Object^ sender, Object^ e)
{
	HRESULT hr = S_OK;


	// Initialize MF
	hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);
	if (SUCCEEDED(hr))
	{
		m_IsMFLoaded = true;
	}
	else
	{
		ThrowIfFailed(hr);
	}

	if (m_spCapture)
	{
		m_spCapture = nullptr;
	}

	// Create a new WASAPI capture instance
	m_spCapture = Make<WASAPICapture>();

	if (nullptr == m_spCapture)
	{
		//OnDeviceStateChange(this, ref new DeviceStateChangedEventArgs(DeviceState::DeviceStateInError, E_OUTOFMEMORY));
		return;
	}

	// Get a pointer to the device event interface
	m_StateChangedEvent = m_spCapture->GetDeviceStateEvent();

	if (nullptr == m_StateChangedEvent)
	{
		//OnDeviceStateChange(this, ref new DeviceStateChangedEventArgs(DeviceState::DeviceStateInError, E_FAIL));
		return;
	}

	// Register for events
	m_deviceStateChangeToken = m_StateChangedEvent->StateChangedEvent += ref new DeviceStateChangedHandler(this, &App::OnDeviceStateChange);

	// Reset discontinuity counter
	m_DiscontinuityCount = 0;

	// Configure user based properties
	CAPTUREDEVICEPROPS props;

	//m_IsLowLatency = static_cast<Platform::Boolean>(toggleMinimumLatency->IsOn);
	props.IsLowLatency = m_IsLowLatency;
	m_spCapture->SetProperties(props);

	// Perform the initialization
	m_spCapture->InitializeAudioDeviceAsync(app);
}

//
//  StopCapture()
//
void App::StopCapture(Platform::Object^ sender, Object^ e)
{
	if (m_spCapture)
	{
		// Set the event to stop playback
		m_spCapture->StopCaptureAsync();
	}

	// Uninitialize MF
	if (m_IsMFLoaded)
	{
		MFShutdown();
		m_IsMFLoaded = false;
	}
}

