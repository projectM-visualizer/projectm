/*
  LICENSE
  -------
Copyright 2005-2013 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "support.h"
#include "utility.h"
#include "../Winamp/wa_ipc.h"

bool g_bDebugOutput = false;
bool g_bDumpFileCleared = false;

//---------------------------------------------------
void PrepareFor3DDrawing(
        IDirect3DDevice9 *pDevice, 
        int viewport_width,
        int viewport_height,
        float fov_in_degrees, 
        float near_clip,
        float far_clip,
        D3DXVECTOR3* pvEye,
        D3DXVECTOR3* pvLookat,
        D3DXVECTOR3* pvUp
    )
{
    // This function sets up DirectX up for 3D rendering.
    // Only call it once per frame, as it is VERY slow.
    // INPUTS:
    //    pDevice           a pointer to the D3D device
    //    viewport_width    the width of the client area of the window
    //    viewport_height   the height of the client area of the window
    //    fov_in_degrees    the field of view, in degrees
    //    near_clip         the distance to the near clip plane; should be > 0!
    //    far_clip          the distance to the far clip plane
    //    eye               the eyepoint coordinates, in world space
    //    lookat            the point toward which the eye is looking, in world space
    //    up                a vector indicating which dir. is up; usually <0,1,0>
    //
    // What this function does NOT do:
    //    1. set the current texture (SetTexture)
    //    2. set up the texture stages for texturing (SetTextureStageState)
    //    3. set the current vertex format (SetVertexShader)
    //    4. set up the world matrix (SetTransform(D3DTS_WORLD, &my_world_matrix))

    
    // set up render state to some nice defaults:
    {
        // some defaults
        pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        pDevice->SetRenderState( D3DRS_ZFUNC,     D3DCMP_LESSEQUAL );
        pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        pDevice->SetRenderState( D3DRS_CLIPPING, TRUE );
        pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        pDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
        pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
        pDevice->SetRenderState( D3DRS_FILLMODE,  D3DFILL_SOLID );
        pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

        // turn fog off
        pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
        pDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
    
        // turn on high-quality bilinear interpolations
        pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); 
        pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        pDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    }    

    // set up view & projection matrices (but not the world matrix!)
    {
        // if the window is not square, instead of distorting the scene,
        // clip it so that the longer dimension of the window has the
        // regular FOV, and the shorter dimension has a reduced FOV.
        float fov_x = fov_in_degrees * 3.1415927f/180.0f;
        float fov_y = fov_in_degrees * 3.1415927f/180.0f;
        float aspect = (float)viewport_height / (float)viewport_width;
        if (aspect < 1)
            fov_y *= aspect;
        else
            fov_x /= aspect;
        
        if (near_clip < 0.1f)
            near_clip = 0.1f;
        if (far_clip < near_clip + 1.0f)
            far_clip = near_clip + 1.0f;

        D3DXMATRIX proj;
        MakeProjectionMatrix(&proj, near_clip, far_clip, fov_x, fov_y);
        pDevice->SetTransform(D3DTS_PROJECTION, &proj);
        
        D3DXMATRIX view;
        pMatrixLookAtLH(&view, pvEye, pvLookat, pvUp);
        pDevice->SetTransform(D3DTS_VIEW, &view);

        // Optimization note: "You can minimize the number of required calculations 
        // by concatenating your world and view matrices into a world-view matrix 
        // that you set as the world matrix, and then setting the view matrix 
        // to the identity."
        //D3DXMatrixMultiply(&world, &world, &view);                
        //D3DXMatrixIdentity(&view);
    }
}

void PrepareFor2DDrawing(IDirect3DDevice9 *pDevice)
{
    // New 2D drawing area will have x,y coords in the range <-1,-1> .. <1,1>
    //         +--------+ Y=-1
    //         |        |
    //         | screen |             Z=0: front of scene
    //         |        |             Z=1: back of scene
    //         +--------+ Y=1
    //       X=-1      X=1
    // NOTE: After calling this, be sure to then call (at least):
    //  1. SetVertexShader()
    //  2. SetTexture(), if you need it
    // before rendering primitives!
    // Also, be sure your sprites have a z coordinate of 0.
    pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_ZFUNC,     D3DCMP_LESSEQUAL );
    pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
    pDevice->SetRenderState( D3DRS_FILLMODE,  D3DFILL_SOLID );
    pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
    pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    pDevice->SetRenderState( D3DRS_CLIPPING, TRUE ); 
    pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pDevice->SetRenderState( D3DRS_LOCALVIEWER, FALSE );
    
    pDevice->SetTexture(0, NULL);
    pDevice->SetTexture(1, NULL);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);//D3DTEXF_LINEAR);
    pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);//D3DTEXF_LINEAR);
    pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);    
    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );
    pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );

    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    
    // set up for 2D drawing:
    {
        D3DXMATRIX Ortho2D;    
        D3DXMATRIX Identity;
        
        pMatrixOrthoLH(&Ortho2D, 2.0f, -2.0f, 0.0f, 1.0f);
        D3DXMatrixIdentity(&Identity);

        pDevice->SetTransform(D3DTS_PROJECTION, &Ortho2D);
        pDevice->SetTransform(D3DTS_WORLD, &Identity);
        pDevice->SetTransform(D3DTS_VIEW, &Identity);
    }
}

//---------------------------------------------------

void MakeWorldMatrix( D3DXMATRIX* pOut, 
                      float xpos, float ypos, float zpos, 
                      float sx,   float sy,   float sz, 
                      float pitch, float yaw, float roll)
{
    /*
     * The m_xPos, m_yPos, m_zPos variables contain the model's
     * location in world coordinates.
     * The m_fPitch, m_fYaw, and m_fRoll variables are floats that 
     * contain the model's orientation in terms of pitch, yaw, and roll
     * angles, in radians.
     */

    D3DXMATRIX MatTemp;
    D3DXMatrixIdentity(pOut);

    // 1. first, rotation
    if (pitch || yaw || roll) 
    {
        D3DXMATRIX MatRot;
        D3DXMatrixIdentity(&MatRot);

        pMatrixRotationX(&MatTemp, pitch);         // Pitch
        pMatrixMultiply(&MatRot, &MatRot, &MatTemp);
        pMatrixRotationY(&MatTemp, yaw);           // Yaw
        pMatrixMultiply(&MatRot, &MatRot, &MatTemp);
        pMatrixRotationZ(&MatTemp, roll);          // Roll
        pMatrixMultiply(&MatRot, &MatRot, &MatTemp);
 
        pMatrixMultiply(pOut, pOut, &MatRot);
    }

    // 2. then, scaling
    pMatrixScaling(&MatTemp, sx, sy, sz);
    pMatrixMultiply(pOut, pOut, &MatTemp);

    // 3. last, translation to final world pos.
    pMatrixTranslation(&MatTemp, xpos, ypos, zpos);
    pMatrixMultiply(pOut, pOut, &MatTemp);
}

void MakeProjectionMatrix( D3DXMATRIX* pOut,
                           const float near_plane, // Distance to near clipping plane
                           const float far_plane,  // Distance to far clipping plane
                           const float fov_horiz,  // Horizontal field of view angle, in radians
                           const float fov_vert)   // Vertical field of view angle, in radians
{
    float w = (float)1/tanf(fov_horiz*0.5f);  // 1/tan(x) == cot(x)
    float h = (float)1/tanf(fov_vert*0.5f);   // 1/tan(x) == cot(x)
    float Q = far_plane/(far_plane - near_plane);
 
    ZeroMemory(pOut, sizeof(D3DXMATRIX));
    pOut->_11 = w;
    pOut->_22 = h;
    pOut->_33 = Q;
    pOut->_43 = -Q*near_plane;
    pOut->_34 = 1;
}

void GetWinampSongTitle(HWND hWndWinamp, wchar_t *szSongTitle, int nSize)
{
    szSongTitle[0] = 0;
	lstrcpynW(szSongTitle, (wchar_t*)SendMessage(hWndWinamp, WM_WA_IPC,
									 SendMessage(hWndWinamp, WM_WA_IPC, 0 , IPC_GETLISTPOS),
									 IPC_GETPLAYLISTTITLEW), nSize);
}

void GetWinampSongPosAsText(HWND hWndWinamp, wchar_t *szSongPos)
{
    // note: size(szSongPos[]) must be at least 64.
    szSongPos[0] = 0;
	int nSongPosMS = SendMessage(hWndWinamp,WM_USER,0,105);
    if (nSongPosMS > 0)
    {
		wchar_t tmp[16];
		float time_s = nSongPosMS*0.001f;
		int minutes = (int)(time_s/60);
		time_s -= minutes*60;
		int seconds = (int)time_s;
		time_s -= seconds;
		int dsec = (int)(time_s*100);
		swprintf(tmp, L"%.02f", dsec/100.0f);
		swprintf(szSongPos, L"%d:%02d%s", minutes, seconds, tmp+1);
    }
}

void GetWinampSongLenAsText(HWND hWndWinamp, wchar_t *szSongLen)
{
    // note: size(szSongLen[]) must be at least 64.
    szSongLen[0] = 0;
	int nSongLenMS = SendMessage(hWndWinamp,WM_USER,1,105)*1000;
    if (nSongLenMS > 0)
    {
		int len_s = nSongLenMS/1000;
		int minutes = len_s/60;
		int seconds = len_s - minutes*60;
		swprintf(szSongLen, L"%d:%02d", minutes, seconds);
    }    
}

float GetWinampSongPos(HWND hWndWinamp)
{
    // returns answer in seconds
    return (float)SendMessage(hWndWinamp,WM_USER,0,105)*0.001f;
}

float GetWinampSongLen(HWND hWndWinamp)
{
    // returns answer in seconds
	return (float)SendMessage(hWndWinamp,WM_USER,1,105);
}

int GetDX9TexFormatBitsPerPixel(D3DFORMAT fmt)
{
    switch(fmt)
    {
    case D3DFMT_DXT1:   // 64 bits for each 4x4 pixels = 4 bits per pixel.  No Alpha channel.
      return 4; // bytes per pixel
  
    case D3DFMT_DXT2:   // 128 bits for each 4x4 pixels = 8 bits per pixel.  RGB+A.
    case D3DFMT_DXT3:   // 128 bits for each 4x4 pixels = 8 bits per pixel.  RGB+A.
    case D3DFMT_DXT4:   // 128 bits for each 4x4 pixels = 8 bits per pixel.  RGB+A.
    case D3DFMT_DXT5:   // 128 bits for each 4x4 pixels = 8 bits per pixel.  RGB+A.
    case D3DFMT_R3G3B2: // 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
    case D3DFMT_A8:   // 8-bit alpha only. 
    case D3DFMT_A8P8: // 8-bit color indexed with 8 bits of alpha. 
    case D3DFMT_P8:   // 8-bit color indexed. 
    case D3DFMT_L8:   // 8-bit luminance only. 
    case D3DFMT_A4L4: // 8-bit using 4 bits each for alpha and luminance. 
      return 8;

    case D3DFMT_R5G6B5:   // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
    case D3DFMT_X1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color. 
    case D3DFMT_A1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
    case D3DFMT_A4R4G4B4: // 16-bit ARGB pixel format with 4 bits for each channel. 
    case D3DFMT_R16F:
    case D3DFMT_A8R3G3B2: // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
    case D3DFMT_X4R4G4B4: // 16-bit RGB pixel format using 4 bits for each color. 
    case D3DFMT_L16:      // 16-bit luminance only. 
    case D3DFMT_A8L8:     // 16-bit using 8 bits each for alpha and luminance. 
    case D3DFMT_CxV8U8:
    case D3DFMT_V8U8:
    case D3DFMT_L6V5U5:
      return 16;

    case D3DFMT_G16R16F: 
    case D3DFMT_R32F:          // 32-bit float format using 32 bits for the red channel. 
    case D3DFMT_A8R8G8B8:      // 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
    case D3DFMT_X8R8G8B8:      // 32-bit RGB pixel format, where 8 bits are reserved for each color. 
    case D3DFMT_A8B8G8R8:      // 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
    case D3DFMT_X8B8G8R8:      // 32-bit RGB pixel format, where 8 bits are reserved for each color. 
    case D3DFMT_G16R16:        // 32-bit pixel format using 16 bits each for green and red. 
    case D3DFMT_A2R10G10B10:   // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
    case D3DFMT_A2B10G10R10:   // 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
    case D3DFMT_R8G8B8:        // 24-bit RGB pixel format with 8 bits per channel. 
    case D3DFMT_X8L8V8U8:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
      return 32;
  
    case D3DFMT_A16B16G16R16F: 
    case D3DFMT_A16B16G16R16:  // 64-bit pixel format using 16 bits for each component. 
    case D3DFMT_G32R32F:       // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel. 
      return 64;

    case D3DFMT_A32B32G32R32F: 
      return 128;
    }
    
    return 32;
}