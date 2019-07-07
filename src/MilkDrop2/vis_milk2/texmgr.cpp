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

#include "texmgr.h"
//#include "jpegstuff.h"
//#include "evallib/compiler.h"
#include "../ns-eel2/ns-eel.h"
#include "support.h"
#include "plugin.h"
#include "utility.h"
texmgr::texmgr()
{
}

texmgr::~texmgr()
{
	//Finish();
	// DO NOT RELEASE OR DELETE m_lpDD; CLIENT SHOULD DO THIS!
}

void texmgr::Finish()
{
	for (int i=0; i<NUM_TEX; i++)
	{
		KillTex(i);
		/*
		if (m_tex[i].pSurface)
		{
			m_tex[i].pSurface->Release();
			m_tex[i].pSurface = NULL;
		}

		FreeCode(i);
		FreeVars(i);
		*/
		NSEEL_VM_free(m_tex[i].tex_eel_ctx);
	}

	// DO NOT RELEASE OR DELETE m_lpDD; CLIENT SHOULD DO THIS!
}

void texmgr::Init(LPDIRECT3DDEVICE9 lpDD)
{
	m_lpDD = lpDD;

	for (int i=0; i<NUM_TEX; i++)
	{
		m_tex[i].pSurface = NULL;
		m_tex[i].szFileName[0] = 0;
		m_tex[i].m_codehandle = NULL;
		m_tex[i].m_szExpr[0] = 0;
		m_tex[i].tex_eel_ctx = NSEEL_VM_alloc();
	}
}

/*
bool texmgr::TryCreateDDrawSurface(int iSlot, int w, int h)
{
	int loop = 1;
	int done = 0;
	int scaling = false;
	
	do
	{
		//TEMP!!!
		//w = 2048;
		//h = 2048;
		switch(loop)
		{
		case 0:
			// first, try requesting surface w/original dimensions of image
			break;
		case 1:
			// then, try next-highest-power-of-two for w,h
			w = (int)powf(2.0f, ceilf(logf((float)w)/logf(2.0f)));
			h = (int)powf(2.0f, ceilf(logf((float)h)/logf(2.0f)));
			break;
		case 2:
			// then, try making it square
			if (w < h) w = h;
			if (h < w) h = w;
			break;
		default:
			if (!scaling)
			{
				if (w<256 || h<256)
				{
					w *= 2;
					h *= 2;
				}
				else 
				{
					scaling = true;
				}
			}
			else
			{
				if (w>16 && h>16)
				{
					w /= 2;
					h /= 2;
				}
				else
				{
					done = 1;
				}
			}

			break;
		}

		if (done)
			break;

		// TRY TO CREATE THE SURFACE IN SYSTEM MEMORY.
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; 
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;//DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;// | DDSCAPS_3DDEVICE;// | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY; 
		ddsd.dwWidth  = w;
		ddsd.dwHeight = h;

		m_tex[iSlot].pSurface = NULL;
		//if (w<256 && h<256)
			if (m_lpDD->CreateSurface( &ddsd, &m_tex[iSlot].pSurface, NULL ) == DD_OK)
				break;
		m_tex[iSlot].pSurface = NULL;

		loop++;
	} 
	while (!done);
	
	if (m_tex[iSlot].pSurface == NULL)
		return false;

	// find out (& remember) actual size created:
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_tex[iSlot].pSurface->GetSurfaceDesc(&ddsd);
	m_tex[iSlot].tex_w = ddsd.dwWidth;
	m_tex[iSlot].tex_h = ddsd.dwHeight;
	m_tex[iSlot].scale_x = 1.0f;
	m_tex[iSlot].scale_y = 1.0f;

	memcpy(&m_tex[iSlot].ddpf, &ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

	return true;
}
*/

int texmgr::LoadTex(wchar_t *szFilename, int iSlot, char *szInitCode, char *szCode, float time, int frame, unsigned int ck)
{
	if (iSlot < 0) return TEXMGR_ERR_BAD_INDEX;
	if (iSlot >= NUM_TEX) return TEXMGR_ERR_BAD_INDEX;
	
	// first, if this texture is already loaded, just add another instance.
	bool bTextureInstanced = false;
	{
		for (int x=0; x<NUM_TEX; x++)
			if (m_tex[x].pSurface && _wcsicmp(m_tex[x].szFileName, szFilename)==0)
			{
				memcpy(&m_tex[iSlot], &m_tex[x], sizeof(td_tex));
				m_tex[iSlot].m_szExpr[0] = 0;
				m_tex[iSlot].m_codehandle  = 0;

				bTextureInstanced = true;
				break;
			}
	}

	if (!bTextureInstanced)
	{
		// Free old resources:
		/*
		if (m_tex[iSlot].pSurface)
		{
			m_tex[iSlot].pSurface->Release();
			m_tex[iSlot].pSurface = NULL;
		}*/
		KillTex(iSlot);

		wcscpy(m_tex[iSlot].szFileName, szFilename);

        D3DXIMAGE_INFO info;
        HRESULT hr = pCreateTextureFromFileExW(
          m_lpDD,
          szFilename,
          D3DX_DEFAULT,
          D3DX_DEFAULT,
          D3DX_DEFAULT, // create a mip chain
          0,
          D3DFMT_UNKNOWN,
          D3DPOOL_DEFAULT,
          D3DX_DEFAULT,
          D3DX_DEFAULT,
          0xFF000000 | ck,
          &info,
          NULL,
          &m_tex[iSlot].pSurface
        );
        
        if (hr != D3D_OK)
        {
            switch(hr)
            {
            case E_OUTOFMEMORY:
            case D3DERR_OUTOFVIDEOMEMORY:
                return TEXMGR_ERR_OUTOFMEM;
            default:
			    return TEXMGR_ERR_BADFILE;
            }
        }

        m_tex[iSlot].img_w = info.Width;
		m_tex[iSlot].img_h = info.Height;

        /*
		unsigned int w_img;
		unsigned int h_img;
		unsigned int img_color_channels;

		int ret = Begin_Jpeg_Read(szFilename, &w_img, &h_img, &img_color_channels);
		switch(ret)
		{
		case JPEGSTUFF_ERR_SUCCESS: 
			break;
		case JPEGSTUFF_ERR_OPENING:
			return TEXMGR_ERR_OPENING;
			break;
		case JPEGSTUFF_ERR_MISC:
			return TEXMGR_ERR_FORMAT;
			break;
		}
		
		sprintf(buf, "texmgr: w=%d, h=%d, channels=%d", w_img, h_img, img_color_channels);
		//g_dumpmsg(buf);

		m_tex[iSlot].img_w = w_img;
		m_tex[iSlot].img_h = h_img;

		if (img_color_channels != 3)
		{
			// error: not 24-bit!
			//g_dumpmsg("texmgr: image not 24-bit");
			End_Jpeg_Read();
			return TEXMGR_ERR_IMAGE_NOT_24_BIT;
		}

		if ((w_img > 2048) ||
			(h_img > 2048))   // RG
		{
			// error: too large!
			//g_dumpmsg("texmgr: image too large");
			End_Jpeg_Read();
			return TEXMGR_ERR_IMAGE_TOO_LARGE;
		}

		if (!TryCreateDDrawSurface(iSlot, w_img, h_img))
		{
			//g_dumpmsg("texmgr: unable to create ddraw surface");
			End_Jpeg_Read();
			return TEXMGR_ERR_CREATESURFACE_FAILED;
		}

		unsigned int w_tex   = m_tex[iSlot].tex_w;
		unsigned int h_tex   = m_tex[iSlot].tex_h;
		unsigned int bpp_tex = m_tex[iSlot].ddpf.dwRGBBitCount;

		sprintf(buf, "texmgr: created ddraw surface; %d x %d x %d", w_tex, h_tex, bpp_tex);
		//g_dumpmsg(buf);

		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof( ddsd );
		
		if (m_tex[iSlot].pSurface->Lock(0, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT|DDLOCK_NOSYSLOCK, 0) != DD_OK)
		{
			//g_dumpmsg("texmgr: unable to lock ddraw surface");
			End_Jpeg_Read();
			m_tex[iSlot].pSurface->Release();
			m_tex[iSlot].pSurface = NULL;
			return TEXMGR_ERR_LOCKSURFACE_FAILED;
		}

		// analyze surface pixel format
		unsigned int zeroBits[3] = { 0, 0, 0 };
		unsigned int chopBits[3] = { 8, 8, 8 };
		unsigned int mask[3]     = { ddsd.ddpfPixelFormat.dwRBitMask, ddsd.ddpfPixelFormat.dwGBitMask, ddsd.ddpfPixelFormat.dwBBitMask };

		{
			int x,y;

			for (x=0; x<3; x++)
			{
				for (y=0; y<32; y++)
				{
					if ((mask[x] & (1<<y)) == 0)
						zeroBits[x]++;
					else
						break;
				}
				mask[x] >>= zeroBits[x];
				for (y=0; y<32; y++)
				{
					if ((mask[x] & (1<<y)) != 0)
						chopBits[x]--;
				}
			}
		}

		unsigned int ck_r1 = (ck_lo >> 16) & 0xFF;
		unsigned int ck_g1 = (ck_lo >> 8 ) & 0xFF;
		unsigned int ck_b1 = (ck_lo      ) & 0xFF;
		unsigned int ck_r2 = (ck_hi >> 16) & 0xFF;
		unsigned int ck_g2 = (ck_hi >> 8 ) & 0xFF;
		unsigned int ck_b2 = (ck_hi      ) & 0xFF;

		// read jpeg in & store in directdraw surface
		// 2. read image into texture
		if (w_img > w_tex || h_img > h_tex)				
		{
			// DOWNSAMPLING VERSION

			unsigned int new_w_img = min(w_tex, w_img);
			unsigned int new_h_img = min(h_tex, h_img);

			{
				char buf[256];
				sprintf(buf, "texmgr: downsampling image from %dx%d to %dx%d and storing in %dx%d texture.", w_img,h_img, new_w_img,new_h_img, w_tex,h_tex);
				//g_dumpmsg(buf);
			}

			int downsample_buf[2048*3];
			memset(downsample_buf, 0, sizeof(downsample_buf));

			float input_lines_per_output_line = h_img/(float)new_h_img;
			float lines = 0.0f;
			unsigned int out_y = 0;

			for (int y=0; y<(int)h_img; y++)
			{
				unsigned int x;

				unsigned char *buf = Jpeg_Read_Next_Line();
				if (!buf) 
				{
					End_Jpeg_Read();
					m_tex[iSlot].pSurface->Release();
					m_tex[iSlot].pSurface = NULL;
					return TEXMGR_ERR_CORRUPT_JPEG;
				}
				
				lines += 1.0f;
				int portion = (int)(min(256, max(0, (input_lines_per_output_line - lines)*256)));
				for (x=0; x<w_img*3; x++)
					downsample_buf[x] += ((int)buf[x] * portion) >> 4;

				if (portion < 256)
				{
					// commit this line (out_y) & start a new one
					if (out_y < h_tex)
					{
						float input_cols_per_output_col = w_img/(float)new_w_img;
						float cols = 0.0f;
						int out_x = 0;

						int buf2[2048*3];
						memset(buf2, 0, new_w_img*3);

						for (x=0; x<w_img; x++)
						{
							cols += 1.0f;
							int portion = (int)(min(256, max(0, (input_cols_per_output_col - cols)*256)));
							buf2[out_x*3  ] += (downsample_buf[x*3  ] * portion) >> 4;
							buf2[out_x*3+1] += (downsample_buf[x*3+1] * portion) >> 4;
							buf2[out_x*3+2] += (downsample_buf[x*3+2] * portion) >> 4;
							
							if (portion < 256)
							{
								cols -= input_cols_per_output_col;
								portion = 256 - portion;
								out_x++;
								buf2[out_x*3  ] = (downsample_buf[x*3  ] * portion) >> 4;
								buf2[out_x*3+1] = (downsample_buf[x*3+1] * portion) >> 4;
								buf2[out_x*3+2] = (downsample_buf[x*3+2] * portion) >> 4;
							}
						}

						// now buf2[0..w_tex] has r,g,b colors in it (but scaled) -> SAVE TO TEXTURE.
						float scale_factor = 1.0f / (float)(16 * 16 * input_cols_per_output_col * input_lines_per_output_line);

						if (bpp_tex == 16)
						{
							unsigned __int16 *dest16 = (unsigned __int16 *)ddsd.lpSurface;
							unsigned int tex_offset = (ddsd.lPitch/2) * out_y;
							for (x=0; x<new_w_img; x++)
							{
								unsigned int cr = (unsigned int)(buf2[x*3  ]*scale_factor);
								unsigned int cg = (unsigned int)(buf2[x*3+1]*scale_factor);
								unsigned int cb = (unsigned int)(buf2[x*3+2]*scale_factor);
								unsigned int color = (((cr >> chopBits[0]) & mask[0]) << zeroBits[0]) |
													 (((cg >> chopBits[1]) & mask[1]) << zeroBits[1]) |
													 (((cb >> chopBits[2]) & mask[2]) << zeroBits[2]);
								if (!(cr >= ck_r1 && cr <= ck_r2 &&
									  cg >= ck_g1 && cg <= ck_g2 &&
									  cb >= ck_b1 && cb <= ck_b2))
									color |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
								dest16[tex_offset++] = color;
							}
						}
						else if (bpp_tex == 32)
						{
							unsigned __int32 *dest32 = (unsigned __int32 *)ddsd.lpSurface;
							unsigned int tex_offset = (ddsd.lPitch/4) * out_y;
							for (x=0; x<new_w_img; x++)
							{
								unsigned int cr = (unsigned int)(buf2[x*3  ]*scale_factor);
								unsigned int cg = (unsigned int)(buf2[x*3+1]*scale_factor);
								unsigned int cb = (unsigned int)(buf2[x*3+2]*scale_factor);
								unsigned int color = (cr << zeroBits[0]) | (cg << zeroBits[1]) | (cb << zeroBits[2]);
								if (!(cr >= ck_r1 && cr <= ck_r2 &&
									  cg >= ck_g1 && cg <= ck_g2 &&
									  cb >= ck_b1 && cb <= ck_b2))
									color |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
								dest32[tex_offset++] = color;
							}
						}

						out_y++;

					}

					// start next line:
					lines -= input_lines_per_output_line;
					portion = 256 - portion;
					for (x=0; x<w_img*3; x++)
						downsample_buf[x] = ((int)buf[x] * portion) >> 4;
				}
			}

			m_tex[iSlot].scale_x = new_w_img/(float)w_img;
			m_tex[iSlot].scale_y = new_h_img/(float)h_img;
			m_tex[iSlot].img_w = new_w_img;
			m_tex[iSlot].img_h = new_h_img;
			w_img = new_w_img;
			h_img = new_h_img;
		}
		else
		{
			// 1:1 VERSION

			if (bpp_tex == 16)
			{
				unsigned __int16 *dest16 = (unsigned __int16 *)ddsd.lpSurface;
				for (int y=0; y<(int)h_img; y++)
				{
					unsigned char *buf = Jpeg_Read_Next_Line();
					if (!buf) 
					{
						End_Jpeg_Read();
						m_tex[iSlot].pSurface->Release();
						m_tex[iSlot].pSurface = NULL;
						return TEXMGR_ERR_CORRUPT_JPEG;
					}

					unsigned int tex_offset = (ddsd.lPitch/2) * y;
					for (unsigned int x=0; x<w_img; x++)
					{
						unsigned int cr = buf[x*3  ];
						unsigned int cg = buf[x*3+1];
						unsigned int cb = buf[x*3+2];
						unsigned int color = (((cr >> chopBits[0]) & mask[0]) << zeroBits[0]) |
											 (((cg >> chopBits[1]) & mask[1]) << zeroBits[1]) |
											 (((cb >> chopBits[2]) & mask[2]) << zeroBits[2]);
						if (!(cr >= ck_r1 && cr <= ck_r2 &&
							  cg >= ck_g1 && cg <= ck_g2 &&
							  cb >= ck_b1 && cb <= ck_b2))
							color |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
						dest16[tex_offset++] = color;
					}
				}
			}
			else if (bpp_tex == 32)
			{
				unsigned __int32 *dest32 = (unsigned __int32 *)ddsd.lpSurface;
				for (int y=0; y<(int)h_img; y++)
				{
					unsigned char *buf = Jpeg_Read_Next_Line();
					if (!buf) 
					{
						End_Jpeg_Read();
						m_tex[iSlot].pSurface->Release();
						m_tex[iSlot].pSurface = NULL;
						return TEXMGR_ERR_CORRUPT_JPEG;
					}

					unsigned int tex_offset = (ddsd.lPitch/4) * y;
					for (unsigned int x=0; x<w_img; x++)
					{
						unsigned int cr = buf[x*3  ];
						unsigned int cg = buf[x*3+1];
						unsigned int cb = buf[x*3+2];
						unsigned int color = (cr << zeroBits[0]) | (cg << zeroBits[1]) | (cb << zeroBits[2]);
						if (!(cr >= ck_r1 && cr <= ck_r2 &&
							  cg >= ck_g1 && cg <= ck_g2 &&
							  cb >= ck_b1 && cb <= ck_b2))
							color |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
						dest32[tex_offset++] = color;
					}
				}
			}
		}

		m_tex[iSlot].pSurface->Unlock(0);

		End_Jpeg_Read();
        */
	}
	
	m_tex[iSlot].fStartTime = time;
	m_tex[iSlot].nStartFrame = frame;
	
	int ret = TEXMGR_ERR_SUCCESS;

	// compile & run init. code:	
	if (!RunInitCode(iSlot, szInitCode))
		ret |= TEXMGR_WARN_ERROR_IN_INIT_CODE;
	
	// compile & save per-frame code:
	strcpy(m_tex[iSlot].m_szExpr, szCode);
	FreeCode(iSlot);
	if (!RecompileExpressions(iSlot))
		ret |= TEXMGR_WARN_ERROR_IN_REG_CODE;
	
	//g_dumpmsg("texmgr: success");
    
	return ret;
}

void texmgr::KillTex(int iSlot)
{
	if (iSlot < 0) return;
	if (iSlot >= NUM_TEX) return;
	
	// Free old resources:
	if (m_tex[iSlot].pSurface)
	{
		// first, make sure no other sprites reference this texture!
		int refcount = 0;
		for (int x=0; x<NUM_TEX; x++)
			if (m_tex[x].pSurface == m_tex[iSlot].pSurface)
				refcount++;

		if (refcount==1)
			m_tex[iSlot].pSurface->Release();
		m_tex[iSlot].pSurface = NULL;
	}
	m_tex[iSlot].szFileName[0] = 0;

	FreeCode(iSlot);
}

void texmgr::StripLinefeedCharsAndComments(char *src, char *dest)
{
	// replaces all LINEFEED_CONTROL_CHAR characters in src with a space in dest;
	// also strips out all comments (beginning with '//' and going til end of line).
	// Restriction: sizeof(dest) must be >= sizeof(src).

	int i2 = 0;
	int len = strlen(src);
	int bComment = false;
	for (int i=0; i<len; i++)		
	{
		if (bComment)
		{
			if (src[i] == LINEFEED_CONTROL_CHAR)	
				bComment = false;
		}
		else
		{
			if ((src[i] =='\\' && src[i+1] =='\\') || (src[i] =='/' && src[i+1] =='/'))
				bComment = true;
			else if (src[i] != LINEFEED_CONTROL_CHAR)
				dest[i2++] = src[i];
		}
	}
	dest[i2] = 0;
}

bool texmgr::RunInitCode(int iSlot, char *szInitCode)
{
	// warning: destroys contents of m_tex[iSlot].m_szExpr,
	//   so be sure to call RunInitCode before writing or
	//   compiling that string!

	FreeCode(iSlot);
	FreeVars(iSlot);
	RegisterBuiltInVariables(iSlot);

	strcpy(m_tex[iSlot].m_szExpr, szInitCode);
	bool ret = RecompileExpressions(iSlot);

	// set default values of output variables:
	// (by not setting these every frame, we allow the values to persist from frame-to-frame.)
	*(m_tex[iSlot].var_x)        = 0.5;
	*(m_tex[iSlot].var_y)        = 0.5;
	*(m_tex[iSlot].var_sx)       = 1.0;
	*(m_tex[iSlot].var_sy)       = 1.0;
	*(m_tex[iSlot].var_repeatx)  = 1.0;
	*(m_tex[iSlot].var_repeaty)  = 1.0;
	*(m_tex[iSlot].var_rot)      = 0.0;
	*(m_tex[iSlot].var_flipx)    = 0.0;
	*(m_tex[iSlot].var_flipy)    = 0.0;
	*(m_tex[iSlot].var_r)        = 1.0;
	*(m_tex[iSlot].var_g)        = 1.0;
	*(m_tex[iSlot].var_b)        = 1.0;
	*(m_tex[iSlot].var_a)        = 1.0;
	*(m_tex[iSlot].var_blendmode)= 0.0;
	*(m_tex[iSlot].var_done)     = 0.0;
	*(m_tex[iSlot].var_burn)     = 1.0;

	#ifndef _NO_EXPR_
		if (m_tex[iSlot].m_codehandle)
			NSEEL_code_execute(m_tex[iSlot].m_codehandle);
	#endif

	return ret;
}

bool texmgr::RecompileExpressions(int iSlot)
{
	char *expr = m_tex[iSlot].m_szExpr;

	// QUICK FIX: if the string ONLY has spaces and linefeeds, erase it, 
	// because for some strange reason this would cause an error in compileCode().
	{
		char *p = expr;
		while (*p==' ' || *p==LINEFEED_CONTROL_CHAR) p++;
		if (*p == 0) expr[0] = 0;
	}

	// replace linefeed control characters with spaces, so they don't mess up the code compiler,
	// and strip out any comments ('//') before sending to CompileCode().
	char buf[sizeof(m_tex[iSlot].m_szExpr)];
	StripLinefeedCharsAndComments(expr, buf);

	if (buf[0])
	{
		#ifndef _NO_EXPR_
			//resetVars(m_tex[iSlot].m_vars);
			//g_dumpmsg("texmgr: compiling string: ");
			//g_dumpmsg(buf);
			if ( ! (m_tex[iSlot].m_codehandle = NSEEL_code_compile(m_tex[iSlot].tex_eel_ctx, buf)))
			{
				//g_dumpmsg(" -error!");
				//MessageBox( NULL, "error in per-frame code", "MILKDROP ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST );
				//sprintf(pg->m_szUserMessage, "warning: preset \"%s\": error in 'per_frame' code", m_szDesc);
				//pg->m_fShowUserMessageUntilThisTime = pg->m_fAnimTime + 6.0f;
			}
			else
			{
				//g_dumpmsg(" -ok!");
				//pg->m_fShowUserMessageUntilThisTime = pg->m_fAnimTime;	// clear any old error msg.
			}
			//resetVars(NULL);
			
			return (m_tex[iSlot].m_codehandle != 0);

		#endif
	}

	return true;
}

void texmgr::FreeVars(int iSlot)
{
	// free the built-in variables AND any user variables
}

void texmgr::FreeCode(int iSlot)
{
	// free the compiled expressions
	if (m_tex[iSlot].m_codehandle)
	{
		NSEEL_code_free(m_tex[iSlot].m_codehandle);
		m_tex[iSlot].m_codehandle = NULL;
	}
}

void texmgr::RegisterBuiltInVariables(int iSlot)
{
	NSEEL_VMCTX eel_ctx = m_tex[iSlot].tex_eel_ctx;
	NSEEL_VM_resetvars(eel_ctx);
	
	// input variables
    m_tex[iSlot].var_time        = NSEEL_VM_regvar(eel_ctx, "time");     
	m_tex[iSlot].var_frame       = NSEEL_VM_regvar(eel_ctx, "frame");    
	m_tex[iSlot].var_fps         = NSEEL_VM_regvar(eel_ctx, "fps");      
	m_tex[iSlot].var_progress    = NSEEL_VM_regvar(eel_ctx, "progress"); 
	m_tex[iSlot].var_bass        = NSEEL_VM_regvar(eel_ctx, "bass");     
	m_tex[iSlot].var_bass_att    = NSEEL_VM_regvar(eel_ctx, "bass_att"); 
	m_tex[iSlot].var_mid         = NSEEL_VM_regvar(eel_ctx, "mid");      
	m_tex[iSlot].var_mid_att     = NSEEL_VM_regvar(eel_ctx, "mid_att");  
	m_tex[iSlot].var_treb        = NSEEL_VM_regvar(eel_ctx, "treb");     
	m_tex[iSlot].var_treb_att    = NSEEL_VM_regvar(eel_ctx, "treb_att"); 
	
	// output variables
	m_tex[iSlot].var_x           = NSEEL_VM_regvar(eel_ctx, "x");        
	m_tex[iSlot].var_y           = NSEEL_VM_regvar(eel_ctx, "y");        
	m_tex[iSlot].var_sx          = NSEEL_VM_regvar(eel_ctx, "sx");       
	m_tex[iSlot].var_sy          = NSEEL_VM_regvar(eel_ctx, "sy");       
	m_tex[iSlot].var_repeatx     = NSEEL_VM_regvar(eel_ctx, "repeatx");       
	m_tex[iSlot].var_repeaty     = NSEEL_VM_regvar(eel_ctx, "repeaty");       
	m_tex[iSlot].var_rot         = NSEEL_VM_regvar(eel_ctx, "rot");      
	m_tex[iSlot].var_flipx       = NSEEL_VM_regvar(eel_ctx, "flipx");    
	m_tex[iSlot].var_flipy       = NSEEL_VM_regvar(eel_ctx, "flipy");    
	m_tex[iSlot].var_r           = NSEEL_VM_regvar(eel_ctx, "r");        
	m_tex[iSlot].var_g           = NSEEL_VM_regvar(eel_ctx, "g");        
	m_tex[iSlot].var_b           = NSEEL_VM_regvar(eel_ctx, "b");        
	m_tex[iSlot].var_a           = NSEEL_VM_regvar(eel_ctx, "a");        
	m_tex[iSlot].var_blendmode   = NSEEL_VM_regvar(eel_ctx, "blendmode");
	m_tex[iSlot].var_done        = NSEEL_VM_regvar(eel_ctx, "done"); 
	m_tex[iSlot].var_burn        = NSEEL_VM_regvar(eel_ctx, "burn"); 

//	resetVars(NULL);
}