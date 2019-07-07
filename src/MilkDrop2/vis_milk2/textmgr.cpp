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

#include "textmgr.h"
#include "support.h"
#include "utility.h"

#define MAX_MSG_CHARS (65536*2)
#define SafeRelease(x) { if (x) {x->Release(); x=NULL;} } 
wchar_t g_szMsgPool[2][MAX_MSG_CHARS];

/*
    NOTES ON CTextManager

    *** -desktop mode was SLOOOW when songtitles are on!, esp. since anim. songtitles...
        -> decided to cache output of ID3DXFont by rendering to a (vidmem) texture, 
           ** only when things change. **  That became CTextManager.
    -uses GDI-based ID3DXFont to draw text to a 2nd (VIDEO MEMORY) surface,
        but each frame, it only draws what is necessary (what's changed
        since last frame).  It then blits that image (additively) to 
        the back buffer each frame.  (note that dark boxes wouldn't work
        w/additive drawing, since they're black, so those have to be
        manually drawn (as black boxes) by the plugin shell, AS WELL AS
        entered into the CTextManager queue as dark boxes, to handle
        erasure, dirty rectangles, etc.)

    PROS/CONS:
    (+) Supports all GDI features: italics, kerning, international fonts, formatting, &, etc.
    (-) takes a lot of memory
    (-) if texture can't be created @ proper size, fonts will appear too big
            -> so don't use texture at all, in that case.
            -> at least this way it will work well on all newer cards [w/memory]
    (-) it's still going to crawl *when the text changes*,
        because d3dx will upload textures to vidmem & blit them *once for each change*.

    OTHER CONCERNS/KIV:
    -what if m_lpDDSText can't be created @ actual size of window?
        If it's bigger, that's ok; but if it's smaller, that should result 
        in a clipped area for the text - hmm....
*/

CTextManager::CTextManager()
{
}

CTextManager::~CTextManager()
{
}

void CTextManager::Init(LPDIRECT3DDEVICE9 lpDevice, IDirect3DTexture9* lpTextSurface, int bAdditive)
{
    m_lpDevice = lpDevice;
    m_lpTextSurface = lpTextSurface;
    m_blit_additively = bAdditive;

    m_b = 0;
    m_nMsg[0] = 0;
    m_nMsg[1] = 0;
    m_next_msg_start_ptr = g_szMsgPool[m_b];
}

void CTextManager::Finish()
{
}

void CTextManager::ClearAll()
{
    m_nMsg[m_b] = 0;
    m_next_msg_start_ptr = g_szMsgPool[m_b];
}

void CTextManager::DrawBox(LPRECT pRect, DWORD boxColor)
{
    if (!pRect)
        return;

    if ((m_nMsg[m_b] < MAX_MSGS) && 
        (DWORD)m_next_msg_start_ptr - (DWORD)g_szMsgPool[m_b] + 0 + 1 < MAX_MSG_CHARS)
    {
        *m_next_msg_start_ptr = 0;

        m_msg[m_b][m_nMsg[m_b]].msg   = m_next_msg_start_ptr;
        m_msg[m_b][m_nMsg[m_b]].pfont = NULL;
        m_msg[m_b][m_nMsg[m_b]].rect  = *pRect;
        m_msg[m_b][m_nMsg[m_b]].flags = 0;
        m_msg[m_b][m_nMsg[m_b]].color = 0xFFFFFFFF;
        m_msg[m_b][m_nMsg[m_b]].bgColor = boxColor;
        m_nMsg[m_b]++;
        m_next_msg_start_ptr += 1;
    }
}

int CTextManager::DrawText(LPD3DXFONT pFont, char* szText, RECT* pRect, DWORD flags, DWORD color, bool bBox, DWORD boxColor)
{
    // these aren't supported by D3DX9:
    flags &= ~(DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_NOPREFIX);

    if (!(pFont && pRect && szText))
        return 0;
        
    if (flags & DT_CALCRECT)
        return pFont->DrawText(NULL, szText, -1, pRect, flags, color);

    if (!m_lpDevice /*|| !m_lpTextSurface*/) 
        return 0;

    int len = strlen(szText);

    if ((m_nMsg[m_b] < MAX_MSGS) && 
        (DWORD)m_next_msg_start_ptr - (DWORD)g_szMsgPool[m_b] + len + 1 < MAX_MSG_CHARS)
    {
        wcscpy(m_next_msg_start_ptr, AutoWide(szText));

        m_msg[m_b][m_nMsg[m_b]].msg   = m_next_msg_start_ptr;
        m_msg[m_b][m_nMsg[m_b]].pfont = pFont;
        m_msg[m_b][m_nMsg[m_b]].rect  = *pRect;
        m_msg[m_b][m_nMsg[m_b]].flags = flags;
        m_msg[m_b][m_nMsg[m_b]].color = color;
        m_msg[m_b][m_nMsg[m_b]].bgColor = boxColor;

        // shrink rects on new frame's text strings; important for deletions
        int h = pFont->DrawText(NULL, szText, len, &m_msg[m_b][m_nMsg[m_b]].rect, flags | DT_CALCRECT, color);

        m_nMsg[m_b]++;
        m_next_msg_start_ptr += len + 1;

        if (bBox) 
        {
            // adds a message with no text, but the rect is the same as the text, so it creates a black box
            DrawBox(&m_msg[m_b][m_nMsg[m_b]-1].rect, boxColor);
            // now swap it with the text that precedes it, so it draws first, and becomes a background
            td_string x = m_msg[m_b][m_nMsg[m_b]-1];
            m_msg[m_b][m_nMsg[m_b]-1] = m_msg[m_b][m_nMsg[m_b]-2];
            m_msg[m_b][m_nMsg[m_b]-2] = x;
        }
        return h;
    }

    // no room for more text? ok, but still return accurate info:
    RECT r2 = *pRect;
    int h = pFont->DrawText(NULL, szText, len, &r2, flags | DT_CALCRECT, color);
    return h;
}

int CTextManager::DrawTextW(LPD3DXFONT pFont, wchar_t* szText, RECT* pRect, DWORD flags, DWORD color, bool bBox, DWORD boxColor)
{
    // these aren't supported by D3DX9:
    flags &= ~(DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_NOPREFIX);

    if (!(pFont && pRect && szText))
        return 0;
        
    if (flags & DT_CALCRECT)
        return pFont->DrawTextW(NULL, szText, -1, pRect, flags, color);

    if (!m_lpDevice /*|| !m_lpTextSurface*/) 
        return 0;

    int len = wcslen(szText);

    if ((m_nMsg[m_b] < MAX_MSGS) && 
        (DWORD)m_next_msg_start_ptr - (DWORD)g_szMsgPool[m_b] + len + 1 < MAX_MSG_CHARS)
    {
        wcscpy(m_next_msg_start_ptr, szText);

        m_msg[m_b][m_nMsg[m_b]].msg   = m_next_msg_start_ptr;
        m_msg[m_b][m_nMsg[m_b]].pfont = pFont;
        m_msg[m_b][m_nMsg[m_b]].rect  = *pRect;
        m_msg[m_b][m_nMsg[m_b]].flags = flags;
        m_msg[m_b][m_nMsg[m_b]].color = color;
        m_msg[m_b][m_nMsg[m_b]].bgColor = boxColor;

        // shrink rects on new frame's text strings; important for deletions
        int h = pFont->DrawTextW(NULL, szText, len, &m_msg[m_b][m_nMsg[m_b]].rect, flags | DT_CALCRECT, color);

        m_nMsg[m_b]++;
        m_next_msg_start_ptr += len + 1;

        if (bBox) 
        {
            // adds a message with no text, but the rect is the same as the text, so it creates a black box
            DrawBox(&m_msg[m_b][m_nMsg[m_b]-1].rect, boxColor);
            // now swap it with the text that precedes it, so it draws first, and becomes a background
            td_string x = m_msg[m_b][m_nMsg[m_b]-1];
            m_msg[m_b][m_nMsg[m_b]-1] = m_msg[m_b][m_nMsg[m_b]-2];
            m_msg[m_b][m_nMsg[m_b]-2] = x;
        }
        return h;
    }

    // no room for more text? ok, but still return accurate info:
    RECT r2 = *pRect;
    int h = pFont->DrawTextW(NULL, szText, len, &r2, flags | DT_CALCRECT, color);
    return h;
}

#define MATCH(i,j) ( m_msg[m_b][i].pfont == m_msg[1-m_b][j].pfont && \
                     m_msg[m_b][i].flags == m_msg[1-m_b][j].flags && \
                     m_msg[m_b][i].color == m_msg[1-m_b][j].color && \
                     m_msg[m_b][i].bgColor == m_msg[1-m_b][j].bgColor && \
                     memcmp(&m_msg[m_b][i].rect, &m_msg[1-m_b][j].rect, sizeof(RECT))==0 && \
                     wcscmp(m_msg[m_b][i].msg, m_msg[1-m_b][j].msg)==0 )

void CTextManager::DrawNow()
{
    if (!m_lpDevice)
        return;
        
    if (m_nMsg[m_b] > 0 || m_nMsg[1-m_b] > 0)  // second condition req'd for clearing text in VJ mode
    {
        D3DXMATRIX Ortho2D;    
        pMatrixOrthoLH(&Ortho2D, 2.0f, -2.0f, 0.0f, 1.0f);
        m_lpDevice->SetTransform(D3DTS_PROJECTION, &Ortho2D);

        #define NUM_DIRTY_RECTS 3
        RECT dirty_rect[NUM_DIRTY_RECTS];
        int dirty_rects_ready = 0;
 
        int bRTT = (m_lpTextSurface==NULL) ? 0 : 1;
        LPDIRECT3DSURFACE9 pBackBuffer=NULL;//, pZBuffer=NULL;
        D3DSURFACE_DESC desc_backbuf, desc_text_surface;

        // clear added/deleted flags
        void* last_dark_box = NULL;
        for (int i=0; i<m_nMsg[m_b]; i++)
        {
            m_msg[m_b][i].deleted = m_msg[m_b][i].added = 0;
            m_msg[m_b][i].prev_dark_box_ptr = last_dark_box;
            last_dark_box = (m_msg[m_b][i].pfont) ? last_dark_box : (void*)&m_msg[m_b][i];
        }
        last_dark_box = NULL;
        for (int j=0; j<m_nMsg[1-m_b]; j++)
        {
            m_msg[1-m_b][j].deleted = m_msg[1-m_b][j].added = 0;
            m_msg[1-m_b][j].prev_dark_box_ptr = last_dark_box;
            last_dark_box = (m_msg[1-m_b][j].pfont) ? last_dark_box : (void*)&m_msg[1-m_b][j];
        }

        int bRedrawText = 0;
        if (!bRTT || (m_nMsg[m_b]>0 && m_nMsg[1-m_b]==0))
        {
            bRedrawText = 2;    // redraw ALL
        }
        else
        {
            // try to synchronize the text strings from last frame + this frame,
            // and label additions & deletions.  algorithm will catch:
            //      -insertion of any # of items in one spot
            //      -deletion of any # of items from one spot
            //      -changes to 1 item
            //      -changes to 2 consecutive items
            // (provided that the 2 text strings immediately bounding the 
            //  additions/deletions/change(s) are left unchanged.)
            // in any other case, all the text is just re-rendered.

            int i = 0;
            int j = 0;
            while (i < m_nMsg[m_b] && j < m_nMsg[1-m_b])
            {
                // MATCH macro: first idx is record # for current stuff; second idx is record # for prev frame stuff.
                if (MATCH(i,j))
                {
                    i++;
                    j++;
                }
                else
                {
                    int continue_now = 0;

                    // scan to see if something was added:
                    for (int i2=i+1; i2<m_nMsg[m_b]; i2++)
                        if (MATCH(i2,j))
                        {
                            for (int i3=i; i3<i2; i3++)
                                m_msg[m_b][i3].added = 1;
                            i = i2;
                            bRedrawText = 1;
                            continue_now = 1;
                            break;
                        }
                    if (continue_now)
                        continue;

                    // scan to see if something was deleted:
                    for (int j2=j+1; j2<m_nMsg[1-m_b]; j2++)
                        if (MATCH(i,j2))
                        {
                            for (int j3=j; j3<j2; j3++)
                                m_msg[1-m_b][j3].deleted = 1;
                            j = j2;
                            bRedrawText = 1;
                            continue_now = 1;
                            break;
                        }
                    if (continue_now)
                        continue;

                    // scan to see if just a small group of 1-4 items were changed
                    // [and are followed by two identical items again]
                    int break_now = 0;
                    for (int chgd=1; chgd<=4; chgd++)
                    {
                        if (i>=m_nMsg[m_b]-chgd || j>=m_nMsg[1-m_b]-chgd)
                        {
                            // only a few items left in one of the lists -> just finish it
                            bRedrawText = 1;
                            break_now = 1;
                            break;
                        }
                        if (i<m_nMsg[m_b]-chgd && j<m_nMsg[1-m_b]-chgd && MATCH(i+chgd, j+chgd))
                        {
                            for (int k=0; k<chgd; k++)
                            {
                                m_msg[  m_b][i+k].added   = 1;
                                m_msg[1-m_b][j+k].deleted = 1;
                            }
                            i += chgd;
                            j += chgd;

                            bRedrawText = 1;
                            continue_now = 1;
                            break;
                        }
                    }
                    if (break_now)
                        break;
                    if (continue_now)
                        continue;

                    // otherwise, nontrivial case -> just re-render whole thing
                    bRedrawText = 2;    // redraw ALL
                    break;
                }
            }

            if (bRedrawText < 2)
            {
                while (i < m_nMsg[m_b])
                {
                    m_msg[m_b][i].added = 1;
                    bRedrawText = 1;
                    i++;
                }
                
                while (j < m_nMsg[1-m_b])
                {
                    m_msg[1-m_b][j].deleted = 1;
                    bRedrawText = 1;
                    j++;
                }
            }
        }

        // ------------------------------------------------------------

        // 0. remember old render target & get surface descriptions
        m_lpDevice->GetRenderTarget( 0, &pBackBuffer );
        pBackBuffer->GetDesc(&desc_backbuf);
            
        if (bRTT)
        {
            //if (m_lpDevice->GetDepthStencilSurface( &pZBuffer ) != D3D_OK)
            //    pZBuffer = NULL;                  // ok if return val != D3D_OK - just means there is no zbuffer.
            if (m_lpTextSurface->GetLevelDesc(0, &desc_text_surface) != D3D_OK)
                bRTT = 0;

            m_lpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
            m_lpDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_lpDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );
            m_lpDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );

            m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
            m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
            m_lpDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

            m_lpDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        }
        else
        {
            desc_text_surface = desc_backbuf;
        }

        if (bRTT && bRedrawText)
        do
        {
            // 1. change render target
	        m_lpDevice->SetTexture(0, NULL);
        
            IDirect3DSurface9* pNewTarget = NULL;
            if (m_lpTextSurface->GetSurfaceLevel(0, &pNewTarget) != D3D_OK) 
            {
                bRTT = 0;
                break;
            }
            if (m_lpDevice->SetRenderTarget(0, pNewTarget) != D3D_OK)
            {
                pNewTarget->Release();
                bRTT = 0;
                break;
            }
			//m_lpDevice->SetDepthStencilSurface( ??? );
            pNewTarget->Release();

	        m_lpDevice->SetTexture(0, NULL);

            // 2. clear to black
	        //m_lpDevice->SetTexture(0, NULL);
	        m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            m_lpDevice->SetVertexShader( NULL );
            m_lpDevice->SetFVF( WFVERTEX_FORMAT );
            m_lpDevice->SetPixelShader( NULL );
	        WFVERTEX v3[4];
            if (bRedrawText==2)
            {
                DWORD clearcolor = m_msg[m_b][j].bgColor;//0xFF000000;// | ((rand()%32)<<16) | ((rand()%32)<<8) | ((rand()%32));
                for (int i=0; i<4; i++)
                {
                    v3[i].x = -1.0f + 2.0f*(i%2);
                    v3[i].y = -1.0f + 2.0f*(i/2);
                    v3[i].z = 0;
                    v3[i].Diffuse = clearcolor;
                }
                m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(WFVERTEX));
            }
            else
            {
                // 1. erase (draw black box over) any old text items deleted.
                // also, update the dirty rects; stuff that was ABOVE/BELOW these guys will need redrawn!
                //   (..picture them staggered)
                for (int j=0; j<m_nMsg[1-m_b]; j++)
                {
                    // erase text from PREV frame if it was deleted.
                    if (m_msg[1-m_b][j].deleted)
                    {
                        float x0 = -1.0f + 2.0f*m_msg[1-m_b][j].rect.left/(float)desc_text_surface.Width;
                        float x1 = -1.0f + 2.0f*m_msg[1-m_b][j].rect.right/(float)desc_text_surface.Width;
                        float y0 = -1.0f + 2.0f*m_msg[1-m_b][j].rect.top/(float)desc_text_surface.Height;
                        float y1 = -1.0f + 2.0f*m_msg[1-m_b][j].rect.bottom/(float)desc_text_surface.Height;
                        for (int i=0; i<4; i++)
                        {
                            v3[i].x = (i%2) ? x0 : x1;
                            v3[i].y = (i/2) ? y0 : y1;
                            v3[i].z = 0;
                            v3[i].Diffuse = m_msg[m_b][j].bgColor;//0xFF000000;//0xFF300000;
                        }
                        m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(WFVERTEX));

                        //----------------------------------

                        // special case: 
                        //   if something is erased, but it's totally inside a dark box,
                        //   then don't add it to the dirty rectangle.
                        td_string* pDarkBox = (td_string*)m_msg[1-m_b][j].prev_dark_box_ptr;
                        int add_to_dirty_rect = 1;
                        while (pDarkBox && add_to_dirty_rect)
                        {
                            RECT t;
                            UnionRect(&t, &pDarkBox->rect, &m_msg[1-m_b][j].rect);
                            if (EqualRect(&t, &pDarkBox->rect))
                                add_to_dirty_rect = 0;
                            pDarkBox = (td_string*)pDarkBox->prev_dark_box_ptr;
                        }

                        // also, update dirty rects
                        // first, check to see if this shares area or a border w/any of the going dirty rects,
                        // and if so, expand that dirty rect.
                        if (add_to_dirty_rect)
                        {
                            int done = 0;
                            RECT t;
                            RECT r1 = m_msg[1-m_b][j].rect;
                            RECT r2 = m_msg[1-m_b][j].rect;
                            r2.top -= 1;
                            r2.left -= 1;
                            r2.right += 1;
                            r2.bottom += 1;
                            for (i=0; i<dirty_rects_ready; i++)
                            {
                                if (IntersectRect(&t, &r2, &dirty_rect[i]))
                                {
                                    // expand the dirty rect to include r1
                                    UnionRect(&t, &r1, &dirty_rect[i]);
                                    dirty_rect[i] = t;
                                    done = 1;
                                    break;
                                }
                            }
                            if (done==1) continue;

                            // if it's in a new spot, and there are still unused dirty rects, use those
                            if (dirty_rects_ready < NUM_DIRTY_RECTS)
                            {
                                dirty_rect[dirty_rects_ready] = r1;
                                dirty_rects_ready++;
                                continue;
                            }

                            // otherwise, find the closest dirty rect...
                            float nearest_dist;
                            int nearest_id;
                            for (i=0; i<NUM_DIRTY_RECTS; i++)
                            {
                                int dx=0, dy=0;

                                if      (r1.left > dirty_rect[i].right)
                                    dx = r1.left - dirty_rect[i].right;
                                else if (dirty_rect[i].left > r1.right)
                                    dx = dirty_rect[i].left - r1.right;

                                if      (r1.top > dirty_rect[i].bottom)
                                    dy = r1.top - dirty_rect[i].bottom;
                                else if (dirty_rect[i].top > r1.bottom)
                                    dy = dirty_rect[i].top - r1.bottom;

                                float dist = sqrtf((float)(dx*dx + dy*dy));
                                if (i==0 || dist < nearest_dist)
                                {
                                    nearest_dist = dist;
                                    nearest_id   = i;
                                }
                            }
                            //...and expand it to include this one.
                            UnionRect(&t, &r1, &dirty_rect[nearest_id]);
                            dirty_rect[nearest_id] = t;
                        }
                    }
                }

                // 2. erase AND REDRAW any of *this* frame's text that falls in dirty rects
                //     from erasures of *prev* frame's deleted text:
                for (j=0; j<m_nMsg[m_b]; j++)
                {
                    RECT t;
                    // note: none of these could be 'deleted' status yet.
                    if (!m_msg[m_b][j].added)
                    {
                        // check vs. dirty rects so far; if intersects any, erase + redraw this one.
                        for (i=0; i<dirty_rects_ready; i++)
                            if (m_msg[m_b][j].pfont &&   // exclude dark boxes... //fixme?
                                IntersectRect(&t, &dirty_rect[i], &m_msg[m_b][j].rect))
                            {
                                float x0 = -1.0f + 2.0f*m_msg[m_b][j].rect.left/(float)desc_text_surface.Width;
                                float x1 = -1.0f + 2.0f*m_msg[m_b][j].rect.right/(float)desc_text_surface.Width;
                                float y0 = -1.0f + 2.0f*m_msg[m_b][j].rect.top/(float)desc_text_surface.Height;
                                float y1 = -1.0f + 2.0f*m_msg[m_b][j].rect.bottom/(float)desc_text_surface.Height;
                                for (int i=0; i<4; i++)
                                {
                                    v3[i].x = (i%2) ? x0 : x1;
                                    v3[i].y = (i/2) ? y0 : y1;
                                    v3[i].z = 0;
                                    v3[i].Diffuse = m_msg[m_b][j].bgColor;//0xFF000000;//0xFF000030;
                                }
                                m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(WFVERTEX));

                                m_msg[m_b][j].deleted = 1;
                                m_msg[m_b][j].added   = 1;
                                bRedrawText = 1;
                            }
                    }
                }
            }
        }
        while (0);

        // 3. render text to TEXT surface
        if (bRedrawText)
        {
	        m_lpDevice->SetTexture(0, NULL);
	        m_lpDevice->SetTexture(1, NULL);
            m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            m_lpDevice->SetVertexShader( NULL );
            m_lpDevice->SetPixelShader( NULL );
			m_lpDevice->SetFVF( WFVERTEX_FORMAT );

            for (int i=0; i<m_nMsg[m_b]; i++)
                if (bRedrawText==2 || m_msg[m_b][i].added==1)
                    if (m_msg[m_b][i].pfont) // dark boxes have pfont==NULL
                        // warning: in DX9, the DT_WORD_ELLIPSIS and DT_NOPREFIX flags cause no text to render!!
                        m_msg[m_b][i].pfont->DrawTextW(NULL, m_msg[m_b][i].msg, -1, &m_msg[m_b][i].rect, m_msg[m_b][i].flags, m_msg[m_b][i].color);
                    else if (m_msg[m_b][i].added || bRedrawText==2 || !bRTT)
                    {
	                    WFVERTEX v3[4];
                        float x0 = -1.0f + 2.0f*m_msg[m_b][i].rect.left/(float)desc_text_surface.Width;
                        float x1 = -1.0f + 2.0f*m_msg[m_b][i].rect.right/(float)desc_text_surface.Width;
                        float y0 = -1.0f + 2.0f*m_msg[m_b][i].rect.top/(float)desc_text_surface.Height;
                        float y1 = -1.0f + 2.0f*m_msg[m_b][i].rect.bottom/(float)desc_text_surface.Height;
                        for (int k=0; k<4; k++)
                        {
                            v3[k].x = (k%2) ? x0 : x1;
                            v3[k].y = (k/2) ? y0 : y1;
                            v3[k].z = 0;
                            v3[k].Diffuse = m_msg[m_b][i].bgColor;//0xFF303000;
                        }
                        m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(WFVERTEX));
                    }
        }

        if (bRTT)
        {
            // 4. restore render target 
            if (bRedrawText)
            {
                m_lpDevice->SetTexture(0, NULL);
                m_lpDevice->SetRenderTarget( 0, pBackBuffer );//, pZBuffer );
				//m_lpDevice->SetDepthStencilSurface( pZBuffer );
            }

            // 5. blit text surface to backbuffer
	        m_lpDevice->SetTexture(0, m_lpTextSurface);
            m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_blit_additively ? TRUE : FALSE);
            m_lpDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
            m_lpDevice->SetRenderState(D3DRS_DESTBLEND, m_blit_additively ? D3DBLEND_ONE : D3DBLEND_ZERO);
            m_lpDevice->SetVertexShader( NULL );
            m_lpDevice->SetPixelShader( NULL );
            m_lpDevice->SetFVF( SPRITEVERTEX_FORMAT );

	        SPRITEVERTEX v3[4];
	        ZeroMemory(v3, sizeof(SPRITEVERTEX)*4);
            float fx = desc_text_surface.Width  / (float)desc_backbuf.Width ;
            float fy = desc_text_surface.Height / (float)desc_backbuf.Height;
            for (int i=0; i<4; i++)
            {
                v3[i].x = (i%2==0) ? -1 : -1 + 2*fx;
                v3[i].y = (i/2==0) ? -1 : -1 + 2*fy;
                v3[i].z = 0;
                v3[i].tu = ((i%2==0) ? 0.0f : 1.0f) + 0.5f/desc_text_surface.Width;  // FIXES BLURRY TEXT even when bilinear interp. is on (which can't be turned off on all cards!)
                v3[i].tv = ((i/2==0) ? 0.0f : 1.0f) + 0.5f/desc_text_surface.Height; // FIXES BLURRY TEXT even when bilinear interp. is on (which can't be turned off on all cards!)
                v3[i].Diffuse = 0xFFFFFFFF;
            }
        
            DWORD oldblend[3];
            //m_lpDevice->GetTextureStageState(0, D3DTSS_MAGFILTER, &oldblend[0]);
            //m_lpDevice->GetTextureStageState(1, D3DTSS_MINFILTER, &oldblend[1]);
            //m_lpDevice->GetTextureStageState(2, D3DTSS_MIPFILTER, &oldblend[2]);
			m_lpDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &oldblend[0]); 
			m_lpDevice->GetSamplerState(1, D3DSAMP_MINFILTER, &oldblend[1]); 
			m_lpDevice->GetSamplerState(2, D3DSAMP_MIPFILTER, &oldblend[2]); 
			m_lpDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	        m_lpDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	        m_lpDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

            m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v3, sizeof(SPRITEVERTEX));

            m_lpDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, oldblend[0]);
            m_lpDevice->SetSamplerState(1, D3DSAMP_MINFILTER, oldblend[1]);
            m_lpDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, oldblend[2]);

	        m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }

        SafeRelease(pBackBuffer);
        //SafeRelease(pZBuffer);

	    m_lpDevice->SetTexture(0, NULL);
	    m_lpDevice->SetTexture(1, NULL);
        m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        m_lpDevice->SetVertexShader( NULL );
        m_lpDevice->SetPixelShader( NULL );
        m_lpDevice->SetFVF( SPRITEVERTEX_FORMAT );
        
        //D3DXMATRIX ident;    
        //D3DXMatrixIdentity(&ident);
        //m_lpDevice->SetTransform(D3DTS_PROJECTION, &ident);
    }

    // flip:
    m_b = 1 - m_b;
    
    ClearAll();
}