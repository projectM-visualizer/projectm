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

#include "EyeTuneApp.h"
#include "EyeTunePlatformString.h"
#include "projectM.hpp"
#include "Renderer.hpp"
#include "EyeTunePopUp.h"
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

// Helper to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
inline float ConvertDipsToPixels(float dips, float dpi)
{
    static const float dipsPerInch = 96.0f;
    return floor(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
}

// Implementation of the IFrameworkViewSource interface, necessary to run our app.
ref class SimpleApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return ref new App();
    }
};

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto simpleApplicationSource = ref new SimpleApplicationSource();
    CoreApplication::Run(simpleApplicationSource);
    return 0;
}

App::App() :
    mWindowClosed(false),
    mWindowVisible(true),
	experimental_presets(false),
    mEglDisplay(EGL_NO_DISPLAY),
    mEglContext(EGL_NO_CONTEXT),
    mEglSurface(EGL_NO_SURFACE)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView^ applicationView)
{
    // Register event handlers for app lifecycle. This example includes Activated, so that we
    // can make the CoreWindow active and start rendering on the window.
    applicationView->Activated += 
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

    // Logic for other event handlers could go here.
    // Information about the Suspending and Resuming event handlers can be found here:
    // http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh994930.aspx
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow^ window)
{
    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->SizeChanged +=
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnSizeChanged);
	/*
	window->ResizeCompleted +=
		ref new TypedEventHandler<CoreWindow^, Object^>(this, &App::OnResizeCompleted);
*/
    window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	// https://docs.microsoft.com/en-us/uwp/api/windows.ui.core.corewindow.keydown
	window->KeyDown +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::OnWindowKeyDown);

    // The CoreWindow has been created, so EGL can be initialized.
    InitializeEGL(window);
}

// Initializes scene resources
void App::Load(Platform::String^ entryPoint)
{
    RecreateRenderer();
}


void App::loopback() {
	this->InitializeCapture(nullptr, nullptr);
}

void App::stoploopback() {
	this->StopCapture(nullptr, nullptr);
}

void App::addFakePCM() {
  int i;
  short pcm_data[2][512];
  /** Produce some fake PCM data to stuff into projectM */
  for (i = 0; i < 512; i++) {
    if (i % 2 == 0) {
      pcm_data[0][i] = (float)(rand() / ((float)RAND_MAX) * (pow(2, 14)));
      pcm_data[1][i] = (float)(rand() / ((float)RAND_MAX) * (pow(2, 14)));
    }
    else {
      pcm_data[0][i] = (float)(rand() / ((float)RAND_MAX) * (pow(2, 14)));
      pcm_data[1][i] = (float)(rand() / ((float)RAND_MAX) * (pow(2, 14)));
    }
    if (i % 2 == 1) {
      pcm_data[0][i] = -pcm_data[0][i];
      pcm_data[1][i] = -pcm_data[1][i];
    }
  }

  /** Add the waveform data */
  app->pcm()->addPCM16(pcm_data);
}

void App::RecreateRenderer()
{
	std::string presetdir = "";
	if (experimental_presets)
	{
		presetdir = "\\p\\presets_tryptonaut\\";
	}
	else
	{
		presetdir = "\\p\\presets\\";
	}
	App::RecreateRenderer(presetdir);
}

void App::RecreateRenderer(std::string presetdir)
{
	if (app)
	{
		this->stoploopback();

		app = nullptr;
	}

    if (!app)
	{
		// "Config file not found, using development settings");
		int height = 1080;
		int width = 1920;
		float heightWidthRatio = (float)height / (float)width;
		projectM::Settings settings;
		settings.windowWidth = width;
		settings.windowHeight = height;
		settings.meshX = 300;
		settings.meshY = settings.meshX * heightWidthRatio;
		settings.fps = 60;
		settings.smoothPresetDuration = 3; // seconds
		settings.presetDuration = 30; // seconds
		settings.beatSensitivity = .9;
		settings.aspectCorrection = 1;
		settings.shuffleEnabled = 1;
		settings.softCutRatingsEnabled = 1; // ???
		// get path to our app, use CWD for presets/fonts/etc
		std::wstring wpath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data();
		// wpath += L"\\Assets\\Logo.png";


		//setup converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string converted_str = converter.to_bytes(wpath);

        //  std::string base_path = Managed_Str_To_Std_Str(localFolder->Path->ToString()) + "\\";
		std::string base_path = converted_str;
		// settings.presetURL = base_path + "\\presets_great\\";
		settings.presetURL = base_path + presetdir;
		// settings.presetURL = base_path;
		//        settings.presetURL = base_path + "presets/presets_shader_test";
		settings.menuFontURL = base_path + "fonts/Vera.ttf";
		settings.titleFontURL = base_path + "fonts/Vera.ttf";
		// init with settings
		app = new projectM(settings, 0);
		app->projectM_resetGL(400, 400);
		app->renderFrame();

        EGLint panelWidth = 0;
        EGLint panelHeight = 0;
        eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &panelWidth);
        eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &panelHeight);

        app->projectM_resetGL(panelWidth, panelHeight);
        app->renderFrame();

		this->loopback();

    }
}

// This method is called after the window becomes active.
void App::Run()
{
    while (!mWindowClosed)
    {
        if (mWindowVisible)
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            /*
			EGLint panelWidth = 0;
			EGLint panelHeight = 0;
			eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &panelWidth);
			eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &panelHeight);

            // Logic to update the scene could go here

						glClearColor(0.0, 0.0, 0.0, 0.0);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						// app->projectM_resetGL(panelWidth, panelHeight);
            */
						app->renderFrame();
            // addFakePCM();

            // The call to eglSwapBuffers might not be successful (e.g. due to Device Lost)
            // If the call fails, then we must reinitialize EGL and the GL resources.
            if (eglSwapBuffers(mEglDisplay, mEglSurface) != GL_TRUE)
            {

                // mCubeRenderer.reset(nullptr);
							 // TODO: anything required here?
                CleanupEGL();

                InitializeEGL(CoreWindow::GetForCurrentThread());
                RecreateRenderer();
            }
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }


	// Perform teardown
    CleanupEGL();

	this->StopCapture(nullptr, nullptr);

}

// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handler.
void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    // Run() won't start until the CoreWindow is activated.
    CoreWindow::GetForCurrentThread()->Activate();
}

// Window event handlers.
void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    mWindowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    mWindowClosed = true;
}

void App::OnWindowKeyDown(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args)
{
	// renderer ready?
	if (app)
	{
		// avoid duplicate press
		if (!args->KeyStatus.WasKeyDown)
		{
			// this needs to support gamepad::
			// https://support.xbox.com/en-US/xbox-one/accessories/xbox-one-wireless-controller

			bool tester;
			std::string presetname;
			switch (args->VirtualKey)
			{
			case Windows::System::VirtualKey::R:
			case Windows::System::VirtualKey::Enter:
			case Windows::System::VirtualKey::GamepadA:
			//case Windows::System::VirtualKey::GamepadB:
			case Windows::System::VirtualKey::GamepadX:
			case Windows::System::VirtualKey::GamepadY:
				app->selectRandom(true);
				break;

			case Windows::System::VirtualKey::GamepadLeftShoulder:
			case Windows::System::VirtualKey::GamepadRightShoulder:
			case Windows::System::VirtualKey::Space:
			case Windows::System::VirtualKey::Control:
				// handler for space key
				app->setPresetLock(!app->isPresetLocked());
				break;

			case Windows::System::VirtualKey::GamepadDPadRight:
			case Windows::System::VirtualKey::Right:
			case Windows::System::VirtualKey::GamepadRightTrigger:
			case Windows::System::VirtualKey::N:
				app->selectNext(true);
				break;


/*
			case Windows::System::VirtualKey::GamepadDPadUp:
			case Windows::System::VirtualKey::Up:

			*/

			// enable experimental presets
			case Windows::System::VirtualKey::GamepadDPadDown:
			case Windows::System::VirtualKey::Down:
				experimental_presets = !experimental_presets;
				RecreateRenderer();
				Popup("Experimental Presets : " + experimental_presets.ToString());

			case Windows::System::VirtualKey::GamepadDPadLeft:
			case Windows::System::VirtualKey::Left:
			case Windows::System::VirtualKey::GamepadLeftTrigger:
			case Windows::System::VirtualKey::P:
				app->selectPrevious(true);
				break;

			case Windows::System::VirtualKey::GamepadView:
			case Windows::System::VirtualKey::GamepadMenu:
			case Windows::System::VirtualKey::F1:
			case Windows::System::VirtualKey::Escape:
				tester  = app->isPresetLocked();
				presetname = "PresetEyeD == ";
				if (tester) presetname += "LOCKED: ";
				presetname += app->renderer->presetName();
				presetname = presetname +
					"""\n"
					"This help menu : F1 or ESC : GamepadMenu or GamepadView""\n"
					"Lock/Unlock Preset : SPACE or CTRL : GamepadRightShoulder or GamepadLeftShoulder""\n"
					"Next preset : RIGHT or N : GamepadRightTrigger or GamepadDPadRight""\n"
					"Previous preset : LEFT or P : GamepadLeftTrigger or GamepadDPadLeft""\n"
					"Random preset : ENTER or R : GamepadA or GamepadX or GamepadY""\n"
					"Experimental Presets : DOWN : GamepadDPadDown""\n";

					//					"UP \t / GamepadDPadUp \t : Next preset""\n"
				Popup(Std_Str_To_Managed_Str(presetname));
				break;

			default:
				break;
			}
		}
	}
}

void App::OnSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args)
{
	ApplicationView^ view = ApplicationView::GetForCurrentView();
	bool isFullScreenMode = view->IsFullScreen; //->IsFullScreenMode();
	if (!isFullScreenMode) ResetSize();
}

void App::OnResizeCompleted(Windows::UI::Core::CoreWindow^ sender, Object^ args)
{
	ResetSize();
}

void App::ResetSize()
{
	if (app)
	{
		EGLint panelWidth = 0;
		EGLint panelHeight = 0;
		eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &panelWidth);
		eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &panelHeight);

		app->projectM_resetGL(panelWidth, panelHeight);
	}
}

void App::OnWindowKeyUp(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args)
{
	if (app)
	{
		// app->setPresetLock(!app->isPresetLocked());
	}
}

void App::InitializeEGL(CoreWindow^ window)
{
    const EGLint configAttributes[] = 
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    const EGLint contextAttributes[] = 
    { 
        EGL_CONTEXT_CLIENT_VERSION, 3, 
        EGL_NONE
    };

    const EGLint surfaceAttributes[] =
    {
        // EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
        // If you have compilation issues with it then please update your Visual Studio templates.
        EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE
    };

    const EGLint defaultDisplayAttributes[] =
    {
        // These are the default display attributes, used to request ANGLE's D3D11 renderer.
        // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

        // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
        // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        
        // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
        // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
        // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    
    const EGLint fl9_3DisplayAttributes[] =
    {
        // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
        // These attributes are used if the call to eglInitialize fails with the default display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };

    const EGLint warpDisplayAttributes[] =
    {
        // These attributes can be used to request D3D11 WARP.
        // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    
    EGLConfig config = NULL;

    // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (!eglGetPlatformDisplayEXT)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
    }

    //
    // To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying 
    // parameters passed to eglGetPlatformDisplayEXT:
    // 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
    // 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again 
    //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
    // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again 
    //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.
    //
    
    // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
    mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
    if (mEglDisplay == EGL_NO_DISPLAY)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
    }

    if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE)
    {
        // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
        mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
        if (mEglDisplay == EGL_NO_DISPLAY)
        {
            throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
        }

        if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE)
        {
            // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
            mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
            if (mEglDisplay == EGL_NO_DISPLAY)
            {
                throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
            }

            if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE)
            {
                // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
            }
        }
    }

    EGLint numConfigs = 0;
    if ((eglChooseConfig(mEglDisplay, configAttributes, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
    {
        throw Exception::CreateException(E_FAIL, L"Failed to choose first EGLConfig");
    }

    // Create a PropertySet and initialize with the EGLNativeWindowType.
    PropertySet^ surfaceCreationProperties = ref new PropertySet();
    surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), window);

    // You can configure the surface to render at a lower resolution and be scaled up to 
    // the full window size. The scaling is often free on mobile hardware.
    //
    // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
    // Size customRenderSurfaceSize = Size(800, 600);
    // surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(customRenderSurfaceSize));
    //
    // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
    // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
    // float customResolutionScale = 0.5f;
    // surfaceCreationProperties->Insert(ref new String(EGLRenderResolutionScaleProperty), PropertyValue::CreateSingle(customResolutionScale));

    mEglSurface = eglCreateWindowSurface(mEglDisplay, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
    if (mEglSurface == EGL_NO_SURFACE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to create EGL fullscreen surface");
    }

    mEglContext = eglCreateContext(mEglDisplay, config, EGL_NO_CONTEXT, contextAttributes);
    if (mEglContext == EGL_NO_CONTEXT)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to create EGL context");
    }

    if (eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext) == EGL_FALSE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to make fullscreen EGLSurface current");
    }
}

void App::CleanupEGL()
{
    if (mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(mEglDisplay, mEglSurface);
        mEglSurface = EGL_NO_SURFACE;
    }

    if (mEglDisplay != EGL_NO_DISPLAY && mEglContext != EGL_NO_CONTEXT)
    {
        eglDestroyContext(mEglDisplay, mEglContext);
        mEglContext = EGL_NO_CONTEXT;
    }

    if (mEglDisplay != EGL_NO_DISPLAY)
    {
        eglTerminate(mEglDisplay);
        mEglDisplay = EGL_NO_DISPLAY;
    }
}
