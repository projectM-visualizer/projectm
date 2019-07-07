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

#ifndef __NULLSOFT_DX9_EXAMPLE_PLUGIN_H__
#define __NULLSOFT_DX9_EXAMPLE_PLUGIN_H__ 1

#include "pluginshell.h"
#include "md_defines.h"
#include "menu.h"
#include "support.h"
#include "texmgr.h"
#include "state.h"
#include "../nu/Vector.h"

#include "gstring.h"
#include "../ns-eel2/ns-eel.h"



extern "C" int (*warand)(void);

typedef enum { TEX_DISK, TEX_VS, TEX_BLUR0, TEX_BLUR1, TEX_BLUR2, TEX_BLUR3, TEX_BLUR4, TEX_BLUR5, TEX_BLUR6, TEX_BLUR_LAST } tex_code;
typedef enum { UI_REGULAR, UI_MENU, UI_LOAD, UI_LOAD_DEL, UI_LOAD_RENAME, UI_SAVEAS, UI_SAVE_OVERWRITE, UI_EDIT_MENU_STRING, UI_CHANGEDIR, UI_IMPORT_WAVE, UI_EXPORT_WAVE, UI_IMPORT_SHAPE, UI_EXPORT_SHAPE, UI_UPGRADE_PIXEL_SHADER, UI_MASHUP } ui_mode;
typedef struct { float rad; float ang; float a; float c;  } td_vertinfo; // blending: mix = max(0,min(1,a*t + c));
typedef char* CHARPTR;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#define MY_FFT_SAMPLES 512     // for old [pre-vms] milkdrop sound analysis
typedef struct 
{
	float   imm[3];			// bass, mids, treble (absolute)
	float	imm_rel[3];		// bass, mids, treble (relative to song; 1=avg, 0.9~below, 1.1~above)
	float	avg[3];			// bass, mids, treble (absolute)
	float	avg_rel[3];		// bass, mids, treble (relative to song; 1=avg, 0.9~below, 1.1~above)
	float	long_avg[3];	// bass, mids, treble (absolute)
    float   fWave[2][576];
    float   fSpecLeft[MY_FFT_SAMPLES];
} td_mysounddata;

typedef struct
{
	int 	bActive;
	int 	bFilterBadChars;	// if true, it will filter out any characters that don't belong in a filename, plus the & symbol (because it doesn't display properly with DrawText)
	int 	bDisplayAsCode;		// if true, semicolons will be followed by a newline, for display
	int		nMaxLen;			// can't be more than 511
	int		nCursorPos;
	int		nSelAnchorPos;		// -1 if no selection made
	int 	bOvertypeMode;
	wchar_t	szText[48000];
	wchar_t	szPrompt[512];
	wchar_t	szToolTip[512];
	char	szClipboard[48000];
	wchar_t	szClipboardW[48000];
} td_waitstr;

typedef struct
{
	int 	bBold;
	int 	bItal;
	wchar_t	szFace[128];
	int		nColorR;    // 0..255
	int		nColorG;    // 0..255
	int		nColorB;    // 0..255
} 
td_custom_msg_font;

enum
{
	MD2_PS_NONE = 0,
	MD2_PS_2_0 = 2,
	MD2_PS_2_X = 3,
	MD2_PS_3_0 = 4,
	MD2_PS_4_0 = 5, // not supported by milkdrop
};
/*
typedef struct
{
    char szFace[256];
    int nSize;
    int bBold;
    int bItalic;
} td_titlefontinfo;*/

typedef struct
{
	int		nFont;
	float	fSize;	// 0..100
	float	x;
	float	y;
	float	randx;
	float   randy;
	float	growth;
	float	fTime;	// total time to display the message, in seconds
	float	fFade;	// % (0..1) of the time that is spent fading in
	
	// overrides
	int     bOverrideBold;
	int     bOverrideItal;
	int     bOverrideFace;
	int     bOverrideColorR;
	int     bOverrideColorG;
	int     bOverrideColorB;
	int		nColorR;    // 0..255
	int		nColorG;    // 0..255
	int		nColorB;    // 0..255
	int  	nRandR;
	int     nRandG;
	int  	nRandB;
	int     bBold;
	int     bItal;
	wchar_t szFace[128];

	wchar_t	szText[256];
} 
td_custom_msg;

typedef struct
{
	int 	bRedrawSuperText;	// true if it needs redraw
	int 	bIsSongTitle;		// false for custom message, true for song title
	//char	szText[256];
	wchar_t	szTextW[256];
	wchar_t	nFontFace[128];
	int 	bBold;
	int 	bItal;
	float	fX;
	float   fY;
	float	fFontSize;			// [0..100] for custom messages, [0..4] for song titles
	float   fGrowth;			// applies to custom messages only
	int		nFontSizeUsed;		// height IN PIXELS
	float	fStartTime;
	float	fDuration;
	float	fFadeTime;			// applies to custom messages only; song title fade times are handled specially
	int  	nColorR;
	int     nColorG;
	int  	nColorB;
}
td_supertext;

typedef struct
{
    wchar_t        texname[256];   // ~filename, but without path or extension!
    LPDIRECT3DBASETEXTURE9 texptr;    
    int                w,h,d;
    //D3DXHANDLE         texsize_param;
    bool               bEvictable;
    int                 nAge;   // only valid if bEvictable is true
    int                 nSizeInBytes;    // only valid if bEvictable is true
} TexInfo;

typedef struct
{
    GString    texname;  // just for ref
    D3DXHANDLE texsize_param;
    int        w,h;
} TexSizeParamInfo;

typedef struct
{
    LPDIRECT3DBASETEXTURE9 texptr;    
    bool               bBilinear;
    bool               bWrap;
} SamplerInfo;

typedef struct
{
    GString   msg;
    bool      bBold;  // true == red bkg; false == black bkg
    float     birthTime;
    float     expireTime;
    int       category;
} ErrorMsg;
typedef Vector<ErrorMsg> ErrorMsgList;

typedef Vector<CShaderParams*> CShaderParamsList;

class CShaderParams
{
public:
    // float4 handles:
    D3DXHANDLE rand_frame ;
    D3DXHANDLE rand_preset;
    D3DXHANDLE const_handles[24];
    D3DXHANDLE q_const_handles[(NUM_Q_VAR+3)/4];
    D3DXHANDLE rot_mat[24];
            
    typedef Vector<TexSizeParamInfo> TexSizeParamInfoList;
    TexSizeParamInfoList texsize_params;
    
    // sampler stages for various PS texture bindings:
    //int texbind_vs;
    //int texbind_disk[32];
    //int texbind_voronoi;
    //...
    SamplerInfo   m_texture_bindings[16];  // an entry for each sampler slot.  These are ALIASES - DO NOT DELETE.
    tex_code      m_texcode[16];  // if ==TEX_VS, forget the pointer - texture bound @ that stage is the double-buffered VS.

    void Clear();
    void CacheParams(LPD3DXCONSTANTTABLE pCT, bool bHardErrors);
    void OnTextureEvict(LPDIRECT3DBASETEXTURE9 texptr);
    CShaderParams();
    ~CShaderParams();
};

class VShaderInfo
{
public:
    IDirect3DVertexShader9* ptr;
    LPD3DXCONSTANTTABLE     CT;
    CShaderParams           params;
    VShaderInfo()  { ptr=NULL; CT=NULL; params.Clear(); }
    ~VShaderInfo() { Clear(); }
    void Clear();
};

class PShaderInfo
{
public:
    IDirect3DPixelShader9*  ptr;
    LPD3DXCONSTANTTABLE     CT;
    CShaderParams           params;
    PShaderInfo()  { ptr=NULL; CT=NULL; params.Clear(); }
    ~PShaderInfo() { Clear(); }
    void Clear();
};

typedef struct 
{
    VShaderInfo vs;
    PShaderInfo ps;
} ShaderPairInfo;

typedef struct
{
    PShaderInfo warp; 
    PShaderInfo comp;
} PShaderSet;

typedef struct
{
    VShaderInfo warp; 
    VShaderInfo comp;
} VShaderSet;

/*
typedef struct
{
    void*                ptr;  // to IDirect3DPixelShader9 or IDirect3DVertexShader9
    LPD3DXCONSTANTTABLE  CT;
    CShaderParams        params;
} ShaderInfo;

typedef struct
{
    ShaderInfo warp;
    ShaderInfo comp;
} ShaderSet;
*/

typedef struct 
{
    GString  szFilename;    // without path
    float    fRatingThis;
    float    fRatingCum;
} PresetInfo;
typedef Vector<PresetInfo> PresetList;


class CPlugin : public CPluginShell
{
public:
    
    //====[ 1. members added to create this specific example plugin: ]================================================

        /// CONFIG PANEL SETTINGS THAT WE'VE ADDED (TAB #2)
        bool		m_bFirstRun;
        float		m_fBlendTimeAuto;		// blend time when preset auto-switches
        float		m_fBlendTimeUser;		// blend time when user loads a new preset
        float		m_fTimeBetweenPresets;		// <- this is in addition to m_fBlendTimeAuto
        float		m_fTimeBetweenPresetsRand;	// <- this is in addition to m_fTimeBetweenPresets
        bool        m_bSequentialPresetOrder;
        bool		m_bHardCutsDisabled;
        float		m_fHardCutLoudnessThresh;
        float		m_fHardCutHalflife;
        float		m_fHardCutThresh;
        //int			m_nWidth;
        //int			m_nHeight;
        //int			m_nDispBits;
        int         m_nCanvasStretch;   // 0=Auto, 100=None, 125 = 1.25X, 133, 150, 167, 200, 300, 400 (4X). 
        int			m_nTexSizeX;			// -1 = exact match to screen; -2 = nearest power of 2.
        int			m_nTexSizeY;
        float       m_fAspectX;
        float       m_fAspectY;
        float       m_fInvAspectX;
        float       m_fInvAspectY;
		int         m_nTexBitsPerCh;
        int			m_nGridX;
        int			m_nGridY;

        bool		m_bShowPressF1ForHelp;
        //char		m_szMonitorName[256];
        bool		m_bShowMenuToolTips;
        int			m_n16BitGamma;
        bool		m_bAutoGamma;
        //int		m_nFpsLimit;
        //int			m_cLeftEye3DColor[3];
        //int			m_cRightEye3DColor[3];
        bool		m_bEnableRating;
        //bool        m_bInstaScan;
        bool		m_bSongTitleAnims;
        float		m_fSongTitleAnimDuration;
        float		m_fTimeBetweenRandomSongTitles;
        float		m_fTimeBetweenRandomCustomMsgs;
        int			m_nSongTitlesSpawned;
        int			m_nCustMsgsSpawned;

        //bool		m_bAlways3D;
        //float       m_fStereoSep;
        //bool		m_bAlwaysOnTop;
        //bool		m_bFixSlowText;
        //bool		m_bWarningsDisabled;		// messageboxes
        bool		m_bWarningsDisabled2;		// warnings/errors in upper-right corner (m_szUserMessage)
        //bool        m_bAnisotropicFiltering;
        bool        m_bPresetLockOnAtStartup;
		bool		m_bPreventScollLockHandling;
        int         m_nMaxPSVersion_ConfigPanel;  // -1 = auto, 0 = disable shaders, 2 = ps_2_0, 3 = ps_3_0
        int         m_nMaxPSVersion_DX9;          // 0 = no shader support, 2 = ps_2_0, 3 = ps_3_0
        int         m_nMaxPSVersion;              // this one will be the ~min of the other two.  0/2/3.
        int         m_nMaxImages;
        int         m_nMaxBytes;

        /*
        char		m_szFontFace[NUM_FONTS][128];
        int			m_nFontSize[NUM_FONTS];
        bool		m_bFontBold[NUM_FONTS];
        bool		m_bFontItalic[NUM_FONTS];
        char		 m_szTitleFontFace[128];
        int			 m_nTitleFontSize;			// percentage of screen width (0..100)
        bool		 m_bTitleFontBold;
        bool		 m_bTitleFontItalic;
        */
        HFONT       m_gdi_title_font_doublesize;
        LPD3DXFONT  m_d3dx_title_font_doublesize;

        // PIXEL SHADERS
        DWORD                   m_dwShaderFlags;       // Shader compilation/linking flags
        //ID3DXFragmentLinker*    m_pFragmentLinker;     // Fragment linker interface
        //LPD3DXBUFFER            m_pCompiledFragments;  // Buffer containing compiled fragments
        LPD3DXBUFFER            m_pShaderCompileErrors;
        VShaderSet              m_fallbackShaders_vs;  // *these are the only vertex shaders used for the whole app.*
        PShaderSet              m_fallbackShaders_ps;  // these are just used when the preset's pixel shaders fail to compile.
        PShaderSet              m_shaders;     // includes shader pointers and constant tables for warp & comp shaders, for cur. preset
        PShaderSet              m_OldShaders;  // includes shader pointers and constant tables for warp & comp shaders, for prev. preset
        PShaderSet              m_NewShaders;  // includes shader pointers and constant tables for warp & comp shaders, for coming preset
        ShaderPairInfo          m_BlurShaders[2];
        bool                    m_bWarpShaderLock;
        bool                    m_bCompShaderLock;
        //bool LoadShaderFromFile( char* szFile, char* szFn, char* szProfile, 
        //                         LPD3DXCONSTANTTABLE* ppConstTable, void** ppShader );
        #define SHADER_WARP  0
        #define SHADER_COMP  1
        #define SHADER_BLUR  2
        #define SHADER_OTHER 3
        bool LoadShaderFromMemory( const char* szShaderText, char* szFn, char* szProfile, 
                                   LPD3DXCONSTANTTABLE* ppConstTable, void** ppShader, int shaderType, bool bHardErrors );
        bool RecompileVShader(const char* szShadersText, VShaderInfo *si, int shaderType, bool bHardErrors);
        bool RecompilePShader(const char* szShadersText, PShaderInfo *si, int shaderType, bool bHardErrors, int PSVersion);
        bool EvictSomeTexture();
        typedef Vector<TexInfo> TexInfoList;
        TexInfoList     m_textures;    
        bool m_bNeedRescanTexturesDir;
        // vertex declarations:
        IDirect3DVertexDeclaration9* m_pSpriteVertDecl;
        IDirect3DVertexDeclaration9* m_pWfVertDecl;
        IDirect3DVertexDeclaration9* m_pMyVertDecl;

        D3DXVECTOR4 m_rand_frame;  // 4 random floats (0..1); randomized once per frame; fed to pixel shaders.

        // RUNTIME SETTINGS THAT WE'VE ADDED
        float       m_prev_time;
        bool		m_bTexSizeWasAutoPow2;
		bool        m_bTexSizeWasAutoExact;
        bool		m_bPresetLockedByUser;
        bool		m_bPresetLockedByCode;
        float		m_fAnimTime;
        float		m_fStartTime;
        float		m_fPresetStartTime;
        float		m_fNextPresetTime;
        float       m_fSnapPoint;
        CState		*m_pState;				// points to current CState
        CState		*m_pOldState;			// points to previous CState
        CState		*m_pNewState;			// points to the coming CState - we're not yet blending to it b/c we're still compiling the shaders for it!
        int         m_nLoadingPreset;
        wchar_t     m_szLoadingPreset[MAX_PATH];
        float       m_fLoadingPresetBlendTime;
        int         m_nPresetsLoadedTotal; //important for texture eviction age-tracking...
        CState		m_state_DO_NOT_USE[3];	// do not use; use pState and pOldState instead.
        ui_mode		m_UI_mode;				// can be UI_REGULAR, UI_LOAD, UI_SAVEHOW, or UI_SAVEAS 

        #define MASH_SLOTS 5
        #define MASH_APPLY_DELAY_FRAMES 1
        int         m_nMashSlot;    //0..MASH_SLOTS-1
        //char        m_szMashDir[MASH_SLOTS][MAX_PATH];
        int         m_nMashPreset[MASH_SLOTS];
        int         m_nLastMashChangeFrame[MASH_SLOTS];

        //td_playlist_entry *m_szPlaylist;	// array of 128-char strings
        //int		m_nPlaylistCurPos;
        //int		m_nPlaylistLength;
        //int		m_nTrackPlaying;
        //int		m_nSongPosMS;
        //int		m_nSongLenMS;
        bool		m_bUserPagedUp;
        bool		m_bUserPagedDown;
        float		m_fMotionVectorsTempDx;
        float		m_fMotionVectorsTempDy;

        td_waitstr  m_waitstring;
        void		WaitString_NukeSelection();
        void		WaitString_Cut();
        void		WaitString_Copy();
        void		WaitString_Paste();
        void		WaitString_SeekLeftWord();
        void		WaitString_SeekRightWord();
        int			WaitString_GetCursorColumn();
        int			WaitString_GetLineLength();
        void		WaitString_SeekUpOneLine();
        void		WaitString_SeekDownOneLine();

        int			m_nPresets;			// the # of entries in the file listing.  Includes directories and then files, sorted alphabetically.
        int			m_nDirs;			// the # of presets that are actually directories.  Always between 0 and m_nPresets.
        int			m_nPresetListCurPos;// Index of the currently-HIGHLIGHTED preset (the user must press Enter on it to select it).
        int			m_nCurrentPreset;	// Index of the currently-RUNNING preset.  
								        //   Note that this is NOT the same as the currently-highlighted preset! (that's m_nPresetListCurPos)
								        //   Be careful - this can be -1 if the user changed dir. & a new preset hasn't been loaded yet.
        wchar_t		m_szCurrentPresetFile[512];	// w/o path.  this is always valid (unless no presets were found)
        PresetList  m_presets;
	    void		UpdatePresetList(bool bBackground=false, bool bForce=false, bool bTryReselectCurrentPreset=true);
        wchar_t     m_szUpdatePresetMask[MAX_PATH];
        bool        m_bPresetListReady;
	    //void		UpdatePresetRatings();
        //int         m_nRatingReadProgress;  // equals 'm_nPresets' if all ratings are read in & ready to go; -1 if uninitialized; otherwise, it's still reading them in, and range is: [0 .. m_nPresets-1]
        bool        m_bInitialPresetSelected;

        // PRESET HISTORY
        #define PRESET_HIST_LEN (64+2)     // make this 2 more than the # you REALLY want to be able to go back.
        GString     m_presetHistory[PRESET_HIST_LEN];   //circular
        int         m_presetHistoryPos;
        int         m_presetHistoryBackFence;
        int         m_presetHistoryFwdFence;
        void        PrevPreset(float fBlendTime);
        void        NextPreset(float fBlendTime);  // if not retracing our former steps, it will choose a random one.
        void        OnFinishedLoadingPreset();

        FFT            myfft;
        td_mysounddata mysound;
        
        // stuff for displaying text to user:
        //int			m_nTextHeightPixels;	// this is for the menu/detail font; NOT the "fancy font"
        //int			m_nTextHeightPixels_Fancy;
        bool		m_bShowFPS;
        bool		m_bShowRating;
        bool		m_bShowPresetInfo;
        bool		m_bShowDebugInfo;
        bool		m_bShowSongTitle;
        bool		m_bShowSongTime;
        bool		m_bShowSongLen;
        float		m_fShowRatingUntilThisTime;
        //float		m_fShowUserMessageUntilThisTime;
        //char		m_szUserMessage[512];
         //bool        m_bUserMessageIsError;
        
        #define ERR_ALL    0
        #define ERR_INIT   1  //specifically, loading a preset
        #define ERR_PRESET 2  //specifically, loading a preset
        #define ERR_MISC   3
        #define ERR_NOTIFY 4  // a simple notification - not an error at all. ("shuffle is now ON." etc.)
                              // NOTE: each NOTIFY msg clears all the old NOTIFY messages!
        #define ERR_SCANNING_PRESETS 5
        ErrorMsgList m_errors;
        void        AddError(wchar_t* szMsg, float fDuration, int category=ERR_ALL, bool bBold=true);
        void        ClearErrors(int category=ERR_ALL);  // 0=all categories
        
        char		m_szDebugMessage[512];
        wchar_t		m_szSongTitle    [512];
        wchar_t		m_szSongTitlePrev[512];
        //HFONT		m_hfont[3];	// 0=fancy font (for song titles, preset name)
						        // 1=legible font (the main font)
						        // 2=tooltip font (for tooltips in the menu system)
        //HFONT       m_htitlefont[NUM_TITLE_FONTS]; // ~25 different sizes
        // stuff for menu system:
        CMilkMenu	*m_pCurMenu;	// should always be valid!
        CMilkMenu	 m_menuPreset;
        CMilkMenu	  m_menuWave;
        CMilkMenu	  m_menuAugment;
        CMilkMenu	  m_menuCustomWave;
        CMilkMenu	  m_menuCustomShape;
        CMilkMenu	  m_menuMotion;
        CMilkMenu	  m_menuPost;
        CMilkMenu    m_menuWavecode[MAX_CUSTOM_WAVES];
        CMilkMenu    m_menuShapecode[MAX_CUSTOM_SHAPES];
        bool         m_bShowShaderHelp;



        wchar_t		m_szMilkdrop2Path[MAX_PATH];		// ends in a backslash
        wchar_t		m_szMsgIniFile[MAX_PATH];
        wchar_t     m_szImgIniFile[MAX_PATH];
        wchar_t		m_szPresetDir[MAX_PATH];
        float		m_fRandStart[4];

        // DIRECTX 9:
        IDirect3DTexture9 *m_lpVS[2];
        #define NUM_BLUR_TEX 6
        #if (NUM_BLUR_TEX>0)
	    IDirect3DTexture9 *m_lpBlur[NUM_BLUR_TEX]; // each is successively 1/2 size of prev.
        int               m_nBlurTexW[NUM_BLUR_TEX];
        int               m_nBlurTexH[NUM_BLUR_TEX];
        #endif
        int m_nHighestBlurTexUsedThisFrame;
        IDirect3DTexture9 *m_lpDDSTitle;    // CAREFUL: MIGHT BE NULL (if not enough mem)!
        int               m_nTitleTexSizeX, m_nTitleTexSizeY;
        MYVERTEX          *m_verts;
        MYVERTEX          *m_verts_temp;
        td_vertinfo       *m_vertinfo;
        int               *m_indices_strip;
        int               *m_indices_list;

        // for final composite grid:
        #define FCGSX 32 // final composite gridsize - # verts - should be EVEN.  
        #define FCGSY 24 // final composite gridsize - # verts - should be EVEN.  
                         // # of grid *cells* is two less,
                         // since we have redundant verts along the center line in X and Y (...for clean 'ang' interp)
        MYVERTEX    m_comp_verts[FCGSX*FCGSY];
        int         m_comp_indices[(FCGSX-2)*(FCGSY-2)*2*3];

        bool		m_bMMX;
        //bool		m_bSSE;
        bool        m_bHasFocus;
        bool        m_bHadFocus;
        bool		m_bOrigScrollLockState;
        //bool      m_bMilkdropScrollLockState;  // saved when focus is lost; restored when focus is regained

        int         m_nNumericInputMode;	// NUMERIC_INPUT_MODE_CUST_MSG, NUMERIC_INPUT_MODE_SPRITE
        int         m_nNumericInputNum;
        int			m_nNumericInputDigits;
        td_custom_msg_font   m_CustomMessageFont[MAX_CUSTOM_MESSAGE_FONTS];
        td_custom_msg        m_CustomMessage[MAX_CUSTOM_MESSAGES];

        texmgr      m_texmgr;		// for user sprites

        td_supertext m_supertext;	// **contains info about current Song Title or Custom Message.**

        IDirect3DTexture9 *m_tracer_tex;

        int         m_nFramesSinceResize;

        char        m_szShaderIncludeText[32768];     // note: this still has char 13's and 10's in it - it's never edited on screen or loaded/saved with a preset.
        int         m_nShaderIncludeTextLen;          //  # of chars, not including the final NULL.
        char        m_szDefaultWarpVShaderText[32768]; // THIS HAS CHAR 13/10 CONVERTED TO LINEFEED_CONTROL_CHAR
        char        m_szDefaultWarpPShaderText[32768]; // THIS HAS CHAR 13/10 CONVERTED TO LINEFEED_CONTROL_CHAR
        char        m_szDefaultCompVShaderText[32768]; // THIS HAS CHAR 13/10 CONVERTED TO LINEFEED_CONTROL_CHAR
        char        m_szDefaultCompPShaderText[32768]; // THIS HAS CHAR 13/10 CONVERTED TO LINEFEED_CONTROL_CHAR
        char        m_szBlurVS[32768];
        char        m_szBlurPSX[32768];
        char        m_szBlurPSY[32768];
        //const char* GetDefaultWarpShadersText() { return m_szDefaultWarpShaderText; }
        //const char* GetDefaultCompShadersText() { return m_szDefaultCompShaderText; }
        void        GenWarpPShaderText(char *szShaderText, float decay, bool bWrap);
        void        GenCompPShaderText(char *szShaderText, float brightness, float ve_alpha, float ve_zoom, int ve_orient, float hue_shader, bool bBrighten, bool bDarken, bool bSolarize, bool bInvert);

   //====[ 2. methods added: ]=====================================================================================
        
        void RefreshTab2(HWND hwnd);
        void RenderFrame(int bRedraw);
        void AlignWave(int nSamples);

        void        DrawTooltip(wchar_t* str, int xR, int yB);
        void        RandomizeBlendPattern();
        void        GenPlasma(int x0, int x1, int y0, int y1, float dt);
        void        LoadPerFrameEvallibVars(CState* pState);
        void        LoadCustomWavePerFrameEvallibVars(CState* pState, int i);
        void        LoadCustomShapePerFrameEvallibVars(CState* pState, int i, int instance);
    	void		WriteRealtimeConfig();	// called on Finish()
	    void		dumpmsg(wchar_t *s);
	    void		Randomize();
	    void		LoadRandomPreset(float fBlendTime);
	    void		LoadPreset(const wchar_t *szPresetFilename, float fBlendTime);
        void        LoadPresetTick();
        void        FindValidPresetDir();
	    //char*		GetConfigIniFile() { return m_szConfigIniFile; };
	    wchar_t*	GetMsgIniFile()    { return m_szMsgIniFile; };
	    wchar_t*    GetPresetDir()     { return m_szPresetDir; };
	    void		SavePresetAs(wchar_t *szNewFile);		// overwrites the file if it was already there.
	    void		DeletePresetFile(wchar_t *szDelFile);	
	    void		RenamePresetFile(wchar_t *szOldFile, wchar_t *szNewFile);
	    void		SetCurrentPresetRating(float fNewRating);
	    void		SeekToPreset(wchar_t cStartChar);
	    bool		ReversePropagatePoint(float fx, float fy, float *fx2, float *fy2);
	    int 		HandleRegularKey(WPARAM wParam);
	    bool		OnResizeGraphicsWindow();
	    bool		OnResizeTextWindow();
	    //bool		InitFont();
	    //void		ToggleControlWindow();	// for Desktop Mode only
	    //void		DrawUI();
	    void		ClearGraphicsWindow();	// for windowed mode only
        //bool    Update_Overlay();
	    //void		UpdatePlaylist();
	    void		LaunchCustomMessage(int nMsgNum);
	    void		ReadCustomMessages();
	    void		LaunchSongTitleAnim();

	    bool		RenderStringToTitleTexture();
	    void		ShowSongTitleAnim(/*IDirect3DTexture9* lpRenderTarget,*/ int w, int h, float fProgress);
	    void		DrawWave(float *fL, float *fR);
        void        DrawCustomWaves();
        void        DrawCustomShapes();
	    void		DrawSprites();
        void        ComputeGridAlphaValues();
        //void        WarpedBlit();
                     // note: 'bFlipAlpha' just flips the alpha blending in fixed-fn pipeline - not the values for culling tiles.
	    void		 WarpedBlit_Shaders  (int nPass, bool bAlphaBlend, bool bFlipAlpha, bool bCullTiles, bool bFlipCulling);
        void		 WarpedBlit_NoShaders(int nPass, bool bAlphaBlend, bool bFlipAlpha, bool bCullTiles, bool bFlipCulling);
	    void		 ShowToUser_Shaders  (int nPass, bool bAlphaBlend, bool bFlipAlpha, bool bCullTiles, bool bFlipCulling);
	    void		 ShowToUser_NoShaders();
        void        BlurPasses();
        void        GetSafeBlurMinMax(CState* pState, float* blur_min, float* blur_max);
	    void		RunPerFrameEquations(int code);
	    void		DrawUserSprites();
	    void		MergeSortPresets(int left, int right);
	    void		BuildMenus();
        void        SetMenusForPresetVersion(int WarpPSVersion, int CompPSVersion);
	    //void  ResetWindowSizeOnDisk();
	    bool		LaunchSprite(int nSpriteNum, int nSlot);
	    void		KillSprite(int iSlot);
        void        DoCustomSoundAnalysis();
        void        DrawMotionVectors();
        
        bool        LoadShaders(PShaderSet* sh, CState* pState, bool bTick);
        void        UvToMathSpace(float u, float v, float* rad, float* ang);
        void        ApplyShaderParams(CShaderParams* p, LPD3DXCONSTANTTABLE pCT, CState* pState);
        void        RestoreShaderParams();
        bool        AddNoiseTex(const wchar_t* szTexName, int size, int zoom_factor);
        bool        AddNoiseVol(const wchar_t* szTexName, int size, int zoom_factor);


    //====[ 3. virtual functions: ]===========================================================================

        virtual void OverrideDefaults();
        virtual void MyPreInitialize();
        virtual void MyReadConfig();
        virtual void MyWriteConfig();
        virtual int  AllocateMyNonDx9Stuff();
        virtual void  CleanUpMyNonDx9Stuff();
        virtual int  AllocateMyDX9Stuff();
        virtual void  CleanUpMyDX9Stuff(int final_cleanup);
        virtual void MyRenderFn(int redraw);
        virtual void MyRenderUI(int *upper_left_corner_y, int *upper_right_corner_y, int *lower_left_corner_y, int *lower_right_corner_y, int xL, int xR);
        virtual LRESULT MyWindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
        virtual BOOL    MyConfigTabProc(int nPage, HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        virtual void OnAltK();

        //====[ 4. methods from base class: ]===========================================================================
    /*
        // 'GET' METHODS
        // ------------------------------------------------------------
        int     GetFrame();            // returns current frame # (starts at zero)
        float   GetTime();             // returns current animation time (in seconds) (starts at zero) (updated once per frame)
        float   GetFps();              // returns current estimate of framerate (frames per second)
        eScrMode GetScreenMode();      // returns WINDOWED, FULLSCREEN, FAKE_FULLSCREEN, or NOT_YET_KNOWN (if called before or during OverrideDefaults()).
        HWND    GetWinampWindow();     // returns handle to Winamp main window
        HINSTANCE GetInstance();       // returns handle to the plugin DLL module; used for things like loading resources (dialogs, bitmaps, icons...) that are built into the plugin.
        char*   GetPluginsDirPath();   // usually returns 'c:\\program files\\winamp\\plugins\\'
        char*   GetConfigIniFile();    // usually returns 'c:\\program files\\winamp\\plugins\\something.ini' - filename is determined from identifiers in 'defines.h'

        // GET METHODS THAT ONLY WORK ONCE DIRECTX IS READY
        // ------------------------------------------------------------
        //  The following 'Get' methods are only available after DirectX has been initialized.
        //  If you call these from OverrideDefaults, MyPreInitialize, or MyReadConfig, 
        //    they will fail and return NULL (zero).
        // ------------------------------------------------------------
        HWND         GetPluginWindow();    // returns handle to the plugin window.  NOT persistent; can change.  
        int          GetWidth();           // returns width of plugin window interior, in pixels.
        int          GetHeight();          // returns height of plugin window interior, in pixels.
        D3DFORMAT    GetBackBufFormat();   // returns the pixelformat of the back buffer (probably D3DFMT_R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_R3G3B2, D3DFMT_A8R3G3B2, D3DFMT_X4R4G4B4, or D3DFMT_UNKNOWN)
        D3DFORMAT    GetBackBufZFormat();  // returns the pixelformat of the back buffer's Z buffer (probably D3DFMT_D16_LOCKABLE, D3DFMT_D32, D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D16, D3DFMT_D24X8, D3DFMT_D24X4S4, or D3DFMT_UNKNOWN)
        D3DCAPS8*    GetCaps();            // returns a pointer to the D3DCAPS8 structer for the device.  NOT persistent; can change.
        LPDIRECT3DDEVICE8 GetDevice();     // returns a pointer to the DirectX 8 Device.  NOT persistent; can change.

        // FONTS & TEXT
        // ------------------------------------------------------------
        LPD3DXFONT   GetFont(eFontIndex idx);        // returns a handle to a D3DX font you can use to draw text on the screen
        int          GetFontHeight(eFontIndex idx);  // returns the height of the font, in pixels

        // MISC
        // ------------------------------------------------------------
        td_soundinfo m_sound;                   // a structure always containing the most recent sound analysis information; defined in pluginshell.h.
        void         SuggestHowToFreeSomeMem(); // gives the user a 'smart' messagebox that suggests how they can free up some video memory.
    */
    //=====================================================================================================================
};

#endif