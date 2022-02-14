// Handles audio loopback

#include "loopback.hpp"


// ref https://blogs.msdn.microsoft.com/matthew_van_eerde/2008/12/16/sample-wasapi-loopback-capture-record-what-you-hear/
#ifdef WASAPI_LOOPBACK

IAudioCaptureClient *pAudioCaptureClient;
UINT32 foo = 0;
PUINT32 pnFrames = &foo;
UINT32 nBlockAlign = 0;
UINT32 nPasses = 0;
bool bFirstPacket = true;

HRESULT get_default_device(IMMDevice **ppMMDevice) {
    HRESULT hr = S_OK;
    IMMDeviceEnumerator *pMMDeviceEnumerator;
    
    // activate a device enumerator
    hr = CoCreateInstance(
                          __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator),
                          (void**)&pMMDeviceEnumerator
                          );
    if (FAILED(hr)) {
        ERR(L"CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
        return false;
    }
    
    // get the default render endpoint
    hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, ppMMDevice);
    if (FAILED(hr)) {
        ERR(L"IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x", hr);
        return false;
    }
    
    return S_OK;
}
#endif /** WASAPI_LOOPBACK */

bool initLoopback()
{
#ifdef WASAPI_LOOPBACK
    HRESULT hr;
    
    hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        ERR(L"CoInitialize failed: hr = 0x%08x", hr);
    }
    
    
    IMMDevice *pMMDevice(NULL);
    // open default device if not specified
    if (NULL == pMMDevice) {
        hr = get_default_device(&pMMDevice);
        if (FAILED(hr)) {
            return false;
        }
    }
    
    bool bInt16 = false;
    
    // activate an IAudioClient
    IAudioClient *pAudioClient;
    hr = pMMDevice->Activate(
                             __uuidof(IAudioClient),
                             CLSCTX_ALL, NULL,
                             (void**)&pAudioClient
                             );
    if (FAILED(hr)) {
        ERR(L"IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x", hr);
        return false;
    }
    
    // get the default device periodicity
    REFERENCE_TIME hnsDefaultDevicePeriod;
    hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetDevicePeriod failed: hr = 0x%08x", hr);
        return false;
    }
    
    // get the default device format
    WAVEFORMATEX *pwfx;
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetMixFormat failed: hr = 0x%08x", hr);
        return false;
    }
    
    if (bInt16) {
        // coerce int-16 wave format
        // can do this in-place since we're not changing the size of the format
        // also, the engine will auto-convert from float to int for us
        switch (pwfx->wFormatTag) {
            case WAVE_FORMAT_IEEE_FLOAT:
                pwfx->wFormatTag = WAVE_FORMAT_PCM;
                pwfx->wBitsPerSample = 16;
                pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                break;
                
            case WAVE_FORMAT_EXTENSIBLE:
            {
                // naked scope for case-local variable
                PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
                if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
                    pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    pEx->Samples.wValidBitsPerSample = 16;
                    pwfx->wBitsPerSample = 16;
                    pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                }
                else {
                    ERR(L"%s", L"Don't know how to coerce mix format to int-16");
                    return E_UNEXPECTED;
                }
            }
                break;
                
            default:
                ERR(L"Don't know how to coerce WAVEFORMATEX with wFormatTag = 0x%08x to int-16", pwfx->wFormatTag);
                return E_UNEXPECTED;
        }
    }
    
    nBlockAlign = pwfx->nBlockAlign;
    *pnFrames = 0;
    
    // call IAudioClient::Initialize
    // note that AUDCLNT_STREAMFLAGS_LOOPBACK and AUDCLNT_STREAMFLAGS_EVENTCALLBACK
    // do not work together...
    // the "data ready" event never gets set
    // so we're going to do a timer-driven loop
    hr = pAudioClient->Initialize(
                                  AUDCLNT_SHAREMODE_SHARED,
                                  AUDCLNT_STREAMFLAGS_LOOPBACK,
                                  0, 0, pwfx, 0
                                  );
    if (FAILED(hr)) {
        ERR(L"pAudioClient->Initialize error");
        return false;
    }
    
    // activate an IAudioCaptureClient
    hr = pAudioClient->GetService(
                                  __uuidof(IAudioCaptureClient),
                                  (void**)&pAudioCaptureClient
                                  );
    if (FAILED(hr)) {
        ERR(L"pAudioClient->GetService error");
        return false;
    }
    
    // call IAudioClient::Start
    hr = pAudioClient->Start();
    if (FAILED(hr)) {
        ERR(L"pAudioClient->Start error");
        return false;
    }
    
    bool bDone = false;
#endif /** WASAPI_LOOPBACK */

    return true;
}

void configureLoopback(projectMSDL *app) {
#ifdef WASAPI_LOOPBACK
    // Default to WASAPI loopback if it was enabled at compilation.
    app->wasapi = true;
    // Notify that loopback capture was started.
    SDL_Log("Opened audio capture loopback.");
#endif
}

bool processLoopbackFrame(projectMSDL *app) {
#ifdef WASAPI_LOOPBACK
	HRESULT hr;

    if (app->wasapi) {
        // drain data while it is available
        nPasses++;
        UINT32 nNextPacketSize;
        for (
             hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
             SUCCEEDED(hr) && nNextPacketSize > 0;
             hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize)
             ) {
            // get the captured data
            BYTE *pData;
            UINT32 nNumFramesToRead;
            DWORD dwFlags;
            
            hr = pAudioCaptureClient->GetBuffer(
                                                &pData,
                                                &nNumFramesToRead,
                                                &dwFlags,
                                                NULL,
                                                NULL
                                                );
            if (FAILED(hr)) {
                return false;
            }
            
            LONG lBytesToWrite = nNumFramesToRead * nBlockAlign;
            
            /** Add the waveform data */
            projectm_pcm_add_float_1ch_data(app->projectM(), (float *)pData, nNumFramesToRead);
            
            *pnFrames += nNumFramesToRead;
            
            hr = pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
            if (FAILED(hr)) {
                return false;
            }
            
            bFirstPacket = false;
        }
        
        if (FAILED(hr)) {
            return false;
        }
    }
#endif /** WASAPI_LOOPBACK */

    return true;
}
