//*********************************************************
// Designed for use in EyeTune Windows Store UWP Application.
// RobertPancoast77@gmail.com
// 
//*********************************************************
//
// This file initializes EGL in Windows Store app, using ICoreWindow.
//
//
//
//*********************************************************

#pragma once

#include <string>

#include <memory>
#include <wrl.h>

// OpenGL ES includes
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

// ANGLE include for Windows Store
#include <angle_windowsstore.h>
#include "WASAPICapture.h"

namespace EyeTune
{
    ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        App();

        // IFrameworkView Methods.
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

    private:
		void loopback();
		void stoploopback();
		void addFakePCM();
		void RecreateRenderer(std::string presetdir);
		void RecreateRenderer();
		projectM* app;

        // Application lifecycle event handlers.
        void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);

        // Window event handlers.
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);
		void OnWindowKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
		void OnSizeChanged(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::WindowSizeChangedEventArgs ^ args);
		void OnResizeCompleted(Windows::UI::Core::CoreWindow ^ sender, Object ^ args);
		void ResetSize();
		void OnWindowKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);

        void InitializeEGL(Windows::UI::Core::CoreWindow^ window);
        void CleanupEGL();
		bool experimental_presets;
        bool mWindowClosed;
        bool mWindowVisible;
        
        EGLDisplay mEglDisplay;
        EGLContext mEglContext;
        EGLSurface mEglSurface;


		void OnDeviceStateChange(Object ^ sender, DeviceStateChangedEventArgs ^ e);
		void InitializeCapture(Object^ sender, Object^ e);
		void StopCapture(Object^ sender, Object^ e);

	private:
		Windows::UI::Core::CoreDispatcher^              m_CoreDispatcher;
		Windows::UI::Xaml::Shapes::Polyline^            m_Oscilloscope;

		Windows::Foundation::EventRegistrationToken     m_deviceStateChangeToken;
		Windows::Foundation::EventRegistrationToken     m_plotDataReadyToken;

		int                         m_DiscontinuityCount;
		Platform::Boolean           m_IsMFLoaded;
		Platform::Boolean           m_IsLowLatency;
		DeviceStateChangedEvent^    m_StateChangedEvent;
		ComPtr<WASAPICapture>       m_spCapture;
    };

}