

#include "loopback.hpp"



#ifdef WASAPI_LOOPBACK

IAudioCaptureClient* pAudioCaptureClient;
UINT32 foo = 0;
PUINT32 pnFrames = &foo;
UINT32 nBlockAlign = 0;
UINT32 nPasses = 0;
bool bFirstPacket = true;

HRESULT get_default_device(IMMDevice** ppMMDevice)
{
    HRESULT hr = S_OK;
    IMMDeviceEnumerator* pMMDeviceEnumerator;

    
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**) &pMMDeviceEnumerator);
    if (FAILED(hr))
    {
        ERR(L"CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
        return false;
    }

    
    hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, ppMMDevice);
    if (FAILED(hr))
    {
        ERR(L"IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x", hr);
        return false;
    }

    return S_OK;
}
#endif

bool initLoopback()
{
#ifdef WASAPI_LOOPBACK
    HRESULT hr;

    hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        ERR(L"CoInitialize failed: hr = 0x%08x", hr);
    }


    IMMDevice* pMMDevice(NULL);
    
    if (NULL == pMMDevice)
    {
        hr = get_default_device(&pMMDevice);
        if (FAILED(hr))
        {
            return false;
        }
    }

    bool bInt16 = false;

    
    IAudioClient* pAudioClient;
    hr = pMMDevice->Activate(
        __uuidof(IAudioClient),
        CLSCTX_ALL, NULL,
        (void**) &pAudioClient);
    if (FAILED(hr))
    {
        ERR(L"IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x", hr);
        return false;
    }

    
    REFERENCE_TIME hnsDefaultDevicePeriod;
    hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
    if (FAILED(hr))
    {
        ERR(L"IAudioClient::GetDevicePeriod failed: hr = 0x%08x", hr);
        return false;
    }

    
    WAVEFORMATEX* pwfx;
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr))
    {
        ERR(L"IAudioClient::GetMixFormat failed: hr = 0x%08x", hr);
        return false;
    }

    if (bInt16)
    {



        switch (pwfx->wFormatTag)
        {
            case WAVE_FORMAT_IEEE_FLOAT:
                pwfx->wFormatTag = WAVE_FORMAT_PCM;
                pwfx->wBitsPerSample = 16;
                pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                break;

            case WAVE_FORMAT_EXTENSIBLE: {
                
                PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
                if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
                {
                    pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    pEx->Samples.wValidBitsPerSample = 16;
                    pwfx->wBitsPerSample = 16;
                    pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                }
                else
                {
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





    
    hr = pAudioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK,
        0, 0, pwfx, 0);
    if (FAILED(hr))
    {
        ERR(L"pAudioClient->Initialize error");
        return false;
    }

    
    hr = pAudioClient->GetService(
        __uuidof(IAudioCaptureClient),
        (void**) &pAudioCaptureClient);
    if (FAILED(hr))
    {
        ERR(L"pAudioClient->GetService error");
        return false;
    }

    
    hr = pAudioClient->Start();
    if (FAILED(hr))
    {
        ERR(L"pAudioClient->Start error");
        return false;
    }

    bool bDone = false;
#endif

    return true;
}

void configureLoopback(projectMSDL* app)
{
#ifdef WASAPI_LOOPBACK

    app->wasapi = true;

    SDL_Log("Opened audio capture loopback.");
#endif
}

bool processLoopbackFrame(projectMSDL* app)
{
#ifdef WASAPI_LOOPBACK
    HRESULT hr;

    if (app->wasapi)
    {

        nPasses++;
        UINT32 nNextPacketSize;
        for (
            hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
            SUCCEEDED(hr) && nNextPacketSize > 0;
            hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize))
        {

            BYTE* pData;
            UINT32 nNumFramesToRead;
            DWORD dwFlags;

            hr = pAudioCaptureClient->GetBuffer(
                &pData,
                &nNumFramesToRead,
                &dwFlags,
                NULL,
                NULL);
            if (FAILED(hr))
            {
                return false;
            }

            LONG lBytesToWrite = nNumFramesToRead * nBlockAlign;

            
            projectm_pcm_add_float(app->projectM(), reinterpret_cast<float*>(pData), nNumFramesToRead, PROJECTM_STEREO);

            *pnFrames += nNumFramesToRead;

            hr = pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
            if (FAILED(hr))
            {
                return false;
            }

            bFirstPacket = false;
        }

        if (FAILED(hr))
        {
            return false;
        }
    }
#endif

    return true;
}
