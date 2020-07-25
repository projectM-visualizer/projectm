/////////////////////////////////////////////////////////////////////////////
//
// projectM-wmp.cpp : Implementation of CProjectMwmp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "projectM-wmp.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::CProjectMwmp
// Constructor

HGLRC hrc = NULL; 
bool starting = true;
int width, height;
projectM *globalPM;

CProjectMwmp::CProjectMwmp() :
m_hwndParent(NULL),
m_clrForeground(0x0000FF),
m_nPreset(0)
{
    m_dwAdviseCookie = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::~CProjectMwmp
// Destructor

CProjectMwmp::~CProjectMwmp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CProjectMwmp:::FinalConstruct
// Called when an effect is first loaded. Use this function to do one-time
// intializations that could fail (i.e. creating offscreen buffers) instead
// of doing this in the constructor, which cannot return an error.

HRESULT CProjectMwmp::FinalConstruct()
{
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectMwmp:::FinalRelease
// Called when an effect is unloaded. Use this function to free any
// resources allocated in FinalConstruct.

void CProjectMwmp::FinalRelease()
{
    ReleaseCore();
}




//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::Render
// Called when an effect should render itself to the screen.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::Render(TimedLevel *pLevels, HDC hdc, RECT *prc)
{   
	if (starting) {
		static PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR), 1,							
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,				
			PFD_TYPE_RGBA, 24,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0
		};
		SetPixelFormat(hdc, ChoosePixelFormat(hdc,&pfd), &pfd);
		hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);		
		starting = false;

		width = (int)(prc->right - prc->left);
		height = (int)(prc->bottom - prc->top);
	    
		globalPM = new projectM("C:\\Program Files\\projectM\\config.inp");//24,18,0,128,width,height,"C:\\Documents and Settings\\DEV2\\My Documents\\svn\\presets_projectM\\");
		
	}

	int newwidth = (int)(prc->right - prc->left);
	int newheight = (int)(prc->bottom - prc->top);
	if (width!= newwidth || height != newheight)
	{
		width=newwidth;
		height=newheight;
	 	globalPM->projectM_resetGL(width,height);
	}

	//PCM* pcm = globalPM->pcm;

	//pcm->addPCM8(pLevels->waveform);
	wglMakeCurrent(hdc, hrc);	
	globalPM->projectM_resetGL(width,height);
    globalPM->renderFrame();

    SwapBuffers(hdc);
    
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::MediaInfo
// Everytime new media is loaded, this method is called to pass the
// number of channels (mono/stereo), the sample rate of the media, and the
// title of the media
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::MediaInfo(LONG lChannelCount, LONG lSampleRate, BSTR bstrTitle )
{
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::GetCapabilities
// Returns the capabilities of this effect. Flags that can be returned are:
//	EFFECT_CANGOFULLSCREEN		-- effect supports full-screen rendering
//	EFFECT_HASPROPERTYPAGE		-- effect supports a property page
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::GetCapabilities(DWORD * pdwCapabilities)
{
    if (NULL == pdwCapabilities)
    {
        return E_POINTER;
    }

    *pdwCapabilities = EFFECT_CANGOFULLSCREEN;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::GetTitle
// Invoked when a host wants to obtain the title of the effect
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::GetTitle(BSTR* bstrTitle)
{
    USES_CONVERSION;

    if (NULL == bstrTitle)
    {
        return E_POINTER;
    }

    CComBSTR bstrTemp;
    bstrTemp.LoadString(IDS_EFFECTNAME); 
        
    if ((!bstrTemp) || (0 == bstrTemp.Length()))
    {
        return E_FAIL;
    }

    *bstrTitle = bstrTemp.Detach();

    return S_OK;
}


STDMETHODIMP CProjectMwmp::RenderFullScreen(TimedLevel *pLevels)
{

  // NULL parent window should not happen 
    if (NULL == m_hwndParent)
    {
        return E_UNEXPECTED;
    }

    // At this point the visualization should draw directly into the parent
    // window. This sample just calls windowless render for simplicity.

    HDC hdc = ::GetDC(m_hwndParent);

    if (NULL == hdc)
    {
        return E_FAIL;
    }

    RECT rParent = { 0 };
    ::GetClientRect(m_hwndParent, &rParent);

    Render(pLevels, hdc, &rParent);
    
    ::ReleaseDC(m_hwndParent, hdc);

	return S_OK;
}

STDMETHODIMP CProjectMwmp::GoFullscreen(BOOL fFullScreen)
{
	return S_OK;
}
											
//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::GetPresetTitle
// Invoked when a host wants to obtain the title of the given preset
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::GetPresetTitle(LONG nPreset, BSTR *bstrPresetTitle)
{
    USES_CONVERSION;

    if (NULL == bstrPresetTitle)
    {
        return E_POINTER;
    }    

    CComBSTR bstrTemp;
        
	bstrTemp = "projectM 1.0";
			            
    if ((!bstrTemp) || (0 == bstrTemp.Length()))
    {
        return E_FAIL;
    }

    *bstrPresetTitle = bstrTemp.Detach();

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::GetPresetCount
// Invoked when a host wants to obtain the number of supported presets
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::GetPresetCount(LONG *pnPresetCount)
{
    if (NULL == pnPresetCount)
    {
        return E_POINTER;
    }

    *pnPresetCount = 1;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::SetCurrentPreset
// Invoked when a host wants to change the index of the current preset
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::SetCurrentPreset(LONG nPreset)
{   
    m_nPreset = 0;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::GetCurrentPreset
// Invoked when a host wants to obtain the index of the current preset
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::GetCurrentPreset(LONG *pnPreset)
{
    if (NULL == pnPreset)
    {
        return E_POINTER;
    }

    *pnPreset = 0;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::SetCore
// Set WMP core interface
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::SetCore(IWMPCore * pCore)
{
    HRESULT hr = S_OK;

    // release any existing WMP core interfaces
    ReleaseCore();

    // If we get passed a NULL core, this  means
    // that the plugin is being shutdown.

    if (pCore == NULL)
    {
        return S_OK;
    }

    m_spCore = pCore;

    // connect up the event interface
    CComPtr<IConnectionPointContainer>  spConnectionContainer;

    hr = m_spCore->QueryInterface( &spConnectionContainer );

    if (SUCCEEDED(hr))
    {
        hr = spConnectionContainer->FindConnectionPoint( __uuidof(IWMPEvents), &m_spConnectionPoint );
    }

    if (SUCCEEDED(hr))
    {
        hr = m_spConnectionPoint->Advise( GetUnknown(), &m_dwAdviseCookie );

        if ((FAILED(hr)) || (0 == m_dwAdviseCookie))
        {
            m_spConnectionPoint = NULL;
        }
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::Create
// Invoked when the visualization should be initialized.
//
// If hwndParent != NULL, RenderWindowed() will be called and the visualization
// should draw into the window specified by hwndParent. This will be the
// behavior when the visualization is hosted in a window.
//
// If hwndParent == NULL, Render() will be called and the visualization
// should draw into the DC passed to Render(). This will be the behavior when
// the visualization is hosted windowless (like in a skin for example).
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::Create(HWND hwndParent)
{

    m_hwndParent = hwndParent;
	if(!starting) 
	{
		delete(globalPM);
		starting = true;
	}

//
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::Destroy
// Invoked when the visualization should be released.
//
// Any resources allocated for ing should be released.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::Destroy()
{
	if (!starting)
	{
	    m_hwndParent = NULL;
		starting = true;
		delete(globalPM);
		return S_OK;
	}
}


char* ConvertBSTRToLPSTR (BSTR bstrIn)
   {
   	LPSTR pszOut = NULL;
   
   	if (bstrIn != NULL)
   	{
   		int nInputStrLen = SysStringLen (bstrIn);
   
   		// Double NULL Termination
   		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, bstrIn, nInputStrLen, NULL, 0, 0, 0) + 2;	
   
   		pszOut = new char [nOutputStrLen];
   
   		if (pszOut)
   		{
   		    memset (pszOut, 0x00, sizeof (char)*nOutputStrLen);
   
 		 WideCharToMultiByte (CP_ACP, 0, bstrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
   		}
   	 }
   
   	return pszOut;
   }
//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::NotifyNewMedia
// Invoked when a new media stream begins playing.
//
// The visualization can inspect this object for properties (like name or artist)
// that might be interesting for visualization.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::NotifyNewMedia(IWMPMedia *pMedia)
{
	if (starting == false && pMedia != NULL)
	{
	CComBSTR name;
	pMedia->get_name(&name);	

	LPTSTR pszConvertedCharStr = ConvertBSTRToLPSTR (name);
    std::string strConverted (pszConvertedCharStr);
  
    delete [] pszConvertedCharStr;

	globalPM->projectM_setTitle(strConverted);
    return S_OK;
	}
}



//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::OnWindowMessage
// Window messages sent to the parent window.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::OnWindowMessage(UINT msg, WPARAM WParam, LPARAM LParam, LRESULT *plResultParam )
{
    // return S_OK only if the plugin has handled the window message
    // return S_FALSE to let the defWindowProc handle the message
    return S_FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::Windowed
// Called when an effect should render itself to the screen.
//
// The fRequiredRender flag specifies if an update is required, otherwise the
// update is optional. This allows visualizations that are fairly static (for example,
// album art visualizations) to only render when the parent window requires it,
// instead of n times a second for dynamic visualizations.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::RenderWindowed(TimedLevel *pLevels, BOOL fRequiredRender )
{
    // NULL parent window should not happen 
    if (NULL == m_hwndParent)
    {
        return E_UNEXPECTED;
    }

    // At this point the visualization should draw directly into the parent
    // window. This sample just calls windowless render for simplicity.

    HDC hdc = ::GetDC(m_hwndParent);

    if (NULL == hdc)
    {
        return E_FAIL;
    }

    RECT rParent = { 0 };
    ::GetClientRect(m_hwndParent, &rParent);

    Render(pLevels, hdc, &rParent);
    
    ::ReleaseDC(m_hwndParent, hdc);

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::ReleaseCore
// Release WMP core interfaces
//////////////////////////////////////////////////////////////////////////////
void CProjectMwmp::ReleaseCore()
{
    if (m_spConnectionPoint)
    {
        if (0 != m_dwAdviseCookie)
        {
            m_spConnectionPoint->Unadvise(m_dwAdviseCookie);
            m_dwAdviseCookie = 0;
        }
        m_spConnectionPoint = NULL;
    }

    if (m_spCore)
    {
        m_spCore = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::get_foregroundColor
// Property get to retrieve the foregroundColor prop via the public interface.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::get_foregroundColor(BSTR *pVal)
{
	return ColorToWz( pVal, m_clrForeground);
}


//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::put_foregroundColor
// Property put to set the foregroundColor prop via the public interface.
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CProjectMwmp::put_foregroundColor(BSTR newVal)
{
	return WzToColor(newVal, &m_clrForeground);
}


//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::WzToColor
// Helper function used to convert a string into a COLORREF.
//////////////////////////////////////////////////////////////////////////////
HRESULT CProjectMwmp::WzToColor(const WCHAR *pwszColor, COLORREF *pcrColor)
{ 
    if (NULL == pwszColor)
    {
        //NULL color string passed in
        return E_POINTER;
    }

    if (0 == lstrlenW(pwszColor))
    {
        //Empty color string passed in
        return E_INVALIDARG;
    }

    if (NULL == pcrColor)
    {
        //NULL output color DWORD passed in
        return E_POINTER;
    }
    
    if (lstrlenW(pwszColor) != 7)
    {
        //hex color string is not of the correct length
        return E_INVALIDARG;
    }

    DWORD dwRet = 0;
    for (int i = 1; i < 7; i++)
    {
        // shift dwRet by 4
        dwRet <<= 4;
        // and add in the value of this string

		if ((pwszColor[i] >= L'0') && (pwszColor[i] <= L'9'))
		{
            dwRet += pwszColor[i] - '0';
        }
        else if ((pwszColor[i] >= L'A') && (pwszColor[i] <= L'F'))
        {
            dwRet += 10 + (pwszColor[i] - L'A');
        }
        else if ((pwszColor[i] >= L'a') && (pwszColor[i] <= L'f'))
        {
            dwRet += 10 + (pwszColor[i] - L'a');
        }
        else
        {
           //Invalid hex digit in color string
            return E_INVALIDARG;
        }
    }

    *pcrColor = SwapBytes(dwRet);

    return S_OK;
} 


//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::ColorToWz
// Helper function used to convert a COLORREF to a BSTR.
//////////////////////////////////////////////////////////////////////////////
HRESULT CProjectMwmp::ColorToWz( BSTR* pbstrColor, COLORREF crColor)
{
    _ASSERT( NULL != pbstrColor );
    _ASSERT( (crColor & 0x00FFFFFF) == crColor );

    *pbstrColor = NULL;

    WCHAR wsz[8];
    HRESULT hr  = S_OK;

    swprintf_s( wsz, sizeof(wsz)/sizeof(wsz[0]), L"#%06X", SwapBytes(crColor) );
    
    *pbstrColor = ::SysAllocString( wsz );

    if (!pbstrColor)
    {
        hr = E_FAIL;
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////////////
// CProjectMwmp::SwapBytes
// Used to convert between a DWORD and COLORREF.  Simply swaps the lowest 
// and 3rd order bytes.
//////////////////////////////////////////////////////////////////////////////
inline DWORD CProjectMwmp::SwapBytes(DWORD dwRet)
{
    return ((dwRet & 0x0000FF00) | ((dwRet & 0x00FF0000) >> 16) | ((dwRet & 0x000000FF) << 16));
}

