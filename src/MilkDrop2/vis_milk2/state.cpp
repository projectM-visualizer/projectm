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

#include "api.h"
#include "state.h"
#include "support.h"
#include "../ns-eel2/ns-eel.h"
#include "plugin.h"
#include "utility.h"
#include <windows.h>
#include <locale.h>
#include "resource.h"

extern CPlugin g_plugin;		// declared in main.cpp

#define FRAND ((rand() % 7381)/7380.0f)



// These are intended to replace GetPrivateProfileInt/FloatString, which are very slow
//  for large files (they always start from the top).  (really slow - some preset loads 
//  were taking 90 ms because of these!)
// The trick here is that we assume the file will be ordered correctly.  If not - if 
//  the next line doesn't have the expected token - we rescan from the top.  If the line
//  is never found, we use the default value, and leave MyGetPos untouched.

#include "../nu/Vector.h"
#include "gstring.h"

typedef Vector<GStringA> VarNameList;
typedef Vector<int    > IntList;


FILE* fLastFilePtr = NULL;
void GetFast_CLEAR() { fLastFilePtr = NULL; }
bool _GetLineByName(FILE* f, const char* szVarName, char* szRet, int nMaxRetChars)
{
    // lines in the file look like this:  szVarName=szRet
    //                               OR:  szVarName szRet
    // the part of the line after the '=' sign (or space) goes into szRet.  
    // szVarName can't have any spaces in it.

    static int MyLineNum = 0;
    static VarNameList line_varName;
    static IntList     line_value_bytepos;

    if (f != fLastFilePtr) 
    { 
        fLastFilePtr = f; 
        MyLineNum = 0; 
        line_varName.clear();
        line_value_bytepos.clear();
        
        // start from beginning of file
        fseek(f, 0, SEEK_SET);

        // scan each line in the file for "szVarName=value" pairs, and store info about them.
        #define MAX_VARNAME_LEN 128
        char szThisLineVarName[MAX_VARNAME_LEN];
        while (1) 
        {
            char ch;
            int pos = 0;
            do 
            {
                ch = fgetc(f);
                if (pos < MAX_VARNAME_LEN-3)
                    szThisLineVarName[pos++] = ch;
            } 
            while ( ch != '\r' && 
                    ch != '\n' && 
                    ch != ' '  && 
                    ch != '='  && 
                    ch != EOF );
            if (ch == EOF)
                break;
            
            if (ch == '=' || ch == ' ') 
            {
                szThisLineVarName[pos-1] = 0;  // replace '=' with a null char.
                int bytepos = ftell(f);

                //add an entry
                line_varName.push_back(szThisLineVarName);
                line_value_bytepos.push_back( bytepos );

                // eat chars up to linefeed or EOF
                /*
                do 
                {
                    ch = fgetc(f);
                }
                while (ch != '\r' && ch != '\n' && ch != EOF);
                */
                fgets(szRet, nMaxRetChars-3, f);  // reads chars up til the next linefeed.
            }
            if (ch == EOF)
                break;
                        
            // eat any extra linefeed chars. 
            do 
            {
                ch = fgetc(f);
            } 
            while ((ch == '\r' || ch == '\n') && ch != EOF);
            if (ch == EOF)
                break;

            // back up one char
            fseek(f, -1, SEEK_CUR);

            // on to next line...
        }
    }

    // if current line isn't the one, check all the others...
    if (MyLineNum < 0 || (size_t)MyLineNum >= line_varName.size() || strcmp(line_varName[MyLineNum].c_str(), szVarName) != 0)
    {
        int N = line_varName.size();
        for (int i=0; i<N; i++)
            if (strcmp(line_varName[i].c_str(), szVarName) == 0)
            {
                MyLineNum = i;
                break;
            }
        
        // otherwise, szVarName not found in the file!
        if (i==N)
            return false;
    }

    fseek(f, line_value_bytepos[MyLineNum], SEEK_SET);

    // now we know that we found and ate the '=' sign; return rest of the line.
    int nChars = 0;
    int pos = 0;
    while (pos < nMaxRetChars-3)
    {
        char ch = fgetc(f);
        if (ch == '\r' || ch == '\n' || ch==EOF)
            break;
        szRet[pos++] = ch;
    };
    szRet[pos] = 0;
    //fgets(szRet, nMaxRetChars-3, f);  // reads chars up til the next linefeed.  NAH - it also copies in the linefeed char...

    // move to next line
    MyLineNum++;
    //fseek(f, line_value_bytepos[MyLineNum], SEEK_SET);

    return true;
}

int GetFastInt   (const char* szVarName, int   def, FILE* f)
{
    char buf[256];
    if (!_GetLineByName(f, szVarName, buf, 255))
        return def;
    int ret;
    if (sscanf(buf, "%d", &ret)==1)
        return ret;
    return def;
}

float GetFastFloat (const char* szVarName, float def, FILE* f)
{
    char buf[256];
    if (!_GetLineByName(f, szVarName, buf, 255))
        return def;
    float ret;
    if (_sscanf_l(buf, "%f", g_use_C_locale, &ret)==1)
	{
        return ret;
	}
	return def;
}

void GetFastString(const char* szVarName, const char* szDef, char* szRetLine, int nMaxChars, FILE* f)
{
    if (_GetLineByName(f, szVarName, szRetLine, nMaxChars-1))
        return;
    // otherwise, copy the default string, being careful not to overflow dest buf.  
    // (avoid strncpy because it pads with zeroes all the way out to the max size.)
    int x = 0;
    while (szDef[x] && x < nMaxChars)
    {
        szRetLine[x] = szDef[x];
        x++;
    }
    szRetLine[x] = 0;
}

CState::CState()
{
	//Default();

	// this is the list of variables that can be used for a PER-FRAME calculation;
	// it is a SUBSET of the per-vertex calculation variable list.
	m_pf_codehandle = NULL;
	m_pp_codehandle = NULL;
	m_pf_eel = NSEEL_VM_alloc();
	m_pv_eel = NSEEL_VM_alloc();
    for (int i=0; i<MAX_CUSTOM_WAVES; i++)
    {
        m_wave[i].m_pf_codehandle = NULL;
        m_wave[i].m_pp_codehandle = NULL;
				m_wave[i].m_pf_eel=NSEEL_VM_alloc();
				m_wave[i].m_pp_eel=NSEEL_VM_alloc();
    }
    for (i=0; i<MAX_CUSTOM_SHAPES; i++)
    {
        m_shape[i].m_pf_codehandle = NULL;
				m_shape[i].m_pf_eel=NSEEL_VM_alloc();
        //m_shape[i].m_pp_codehandle = NULL;
    }
	//RegisterBuiltInVariables();
}

CState::~CState()
{
	FreeVarsAndCode();
	NSEEL_VM_free(m_pf_eel);
	NSEEL_VM_free(m_pv_eel);
	for (int i=0; i<MAX_CUSTOM_WAVES; i++)
	{
		NSEEL_VM_free(m_wave[i].m_pf_eel);
		NSEEL_VM_free(m_wave[i].m_pp_eel);
	}
	for (i=0; i<MAX_CUSTOM_SHAPES; i++)
	{
		NSEEL_VM_free(m_shape[i].m_pf_eel);
	}
}

//--------------------------------------------------------------------------------

void CState::RegisterBuiltInVariables(int flags)
{
    if (flags & RECOMPILE_PRESET_CODE)
    {
	    NSEEL_VM_resetvars(m_pf_eel);
        var_pf_zoom		= NSEEL_VM_regvar(m_pf_eel, "zoom");		// i/o
	    var_pf_zoomexp  = NSEEL_VM_regvar(m_pf_eel, "zoomexp");	// i/o
	    var_pf_rot		= NSEEL_VM_regvar(m_pf_eel, "rot");		// i/o
	    var_pf_warp		= NSEEL_VM_regvar(m_pf_eel, "warp");		// i/o
	    var_pf_cx		= NSEEL_VM_regvar(m_pf_eel, "cx");		// i/o
	    var_pf_cy		= NSEEL_VM_regvar(m_pf_eel, "cy");		// i/o
	    var_pf_dx		= NSEEL_VM_regvar(m_pf_eel, "dx");		// i/o
	    var_pf_dy		= NSEEL_VM_regvar(m_pf_eel, "dy");		// i/o
	    var_pf_sx		= NSEEL_VM_regvar(m_pf_eel, "sx");		// i/o
	    var_pf_sy		= NSEEL_VM_regvar(m_pf_eel, "sy");		// i/o
	    var_pf_time		= NSEEL_VM_regvar(m_pf_eel, "time");		// i
	    var_pf_fps      = NSEEL_VM_regvar(m_pf_eel, "fps");       // i
	    var_pf_bass		= NSEEL_VM_regvar(m_pf_eel, "bass");		// i
	    var_pf_mid		= NSEEL_VM_regvar(m_pf_eel, "mid");		// i
	    var_pf_treb		= NSEEL_VM_regvar(m_pf_eel, "treb");		// i
	    var_pf_bass_att	= NSEEL_VM_regvar(m_pf_eel, "bass_att");	// i
	    var_pf_mid_att	= NSEEL_VM_regvar(m_pf_eel, "mid_att");	// i
	    var_pf_treb_att	= NSEEL_VM_regvar(m_pf_eel, "treb_att");	// i
	    var_pf_frame    = NSEEL_VM_regvar(m_pf_eel, "frame");
	    var_pf_decay	= NSEEL_VM_regvar(m_pf_eel, "decay");
	    var_pf_wave_a	= NSEEL_VM_regvar(m_pf_eel, "wave_a");
	    var_pf_wave_r	= NSEEL_VM_regvar(m_pf_eel, "wave_r");
	    var_pf_wave_g	= NSEEL_VM_regvar(m_pf_eel, "wave_g");
	    var_pf_wave_b	= NSEEL_VM_regvar(m_pf_eel, "wave_b");
	    var_pf_wave_x	= NSEEL_VM_regvar(m_pf_eel, "wave_x");
	    var_pf_wave_y	= NSEEL_VM_regvar(m_pf_eel, "wave_y");
	    var_pf_wave_mystery = NSEEL_VM_regvar(m_pf_eel, "wave_mystery");
	    var_pf_wave_mode = NSEEL_VM_regvar(m_pf_eel, "wave_mode");
        for (int vi=0; vi<NUM_Q_VAR; vi++)
        {
            char buf[16];
            sprintf(buf, "q%d", vi+1);
            var_pf_q[vi] = NSEEL_VM_regvar(m_pf_eel, buf);
        }
	    var_pf_progress = NSEEL_VM_regvar(m_pf_eel, "progress");
	    var_pf_ob_size	= NSEEL_VM_regvar(m_pf_eel, "ob_size");
	    var_pf_ob_r		= NSEEL_VM_regvar(m_pf_eel, "ob_r");
	    var_pf_ob_g		= NSEEL_VM_regvar(m_pf_eel, "ob_g");
	    var_pf_ob_b		= NSEEL_VM_regvar(m_pf_eel, "ob_b");
	    var_pf_ob_a		= NSEEL_VM_regvar(m_pf_eel, "ob_a");
	    var_pf_ib_size	= NSEEL_VM_regvar(m_pf_eel, "ib_size");
	    var_pf_ib_r		= NSEEL_VM_regvar(m_pf_eel, "ib_r");
	    var_pf_ib_g		= NSEEL_VM_regvar(m_pf_eel, "ib_g");
	    var_pf_ib_b		= NSEEL_VM_regvar(m_pf_eel, "ib_b");
	    var_pf_ib_a		= NSEEL_VM_regvar(m_pf_eel, "ib_a");
	    var_pf_mv_x		= NSEEL_VM_regvar(m_pf_eel, "mv_x");
	    var_pf_mv_y		= NSEEL_VM_regvar(m_pf_eel, "mv_y");
	    var_pf_mv_dx	= NSEEL_VM_regvar(m_pf_eel, "mv_dx");
	    var_pf_mv_dy	= NSEEL_VM_regvar(m_pf_eel, "mv_dy");
	    var_pf_mv_l		= NSEEL_VM_regvar(m_pf_eel, "mv_l");
	    var_pf_mv_r		= NSEEL_VM_regvar(m_pf_eel, "mv_r");
	    var_pf_mv_g		= NSEEL_VM_regvar(m_pf_eel, "mv_g");
	    var_pf_mv_b		= NSEEL_VM_regvar(m_pf_eel, "mv_b");
	    var_pf_mv_a		= NSEEL_VM_regvar(m_pf_eel, "mv_a");
	    var_pf_monitor  = NSEEL_VM_regvar(m_pf_eel, "monitor");
	    var_pf_echo_zoom   = NSEEL_VM_regvar(m_pf_eel, "echo_zoom");
	    var_pf_echo_alpha  = NSEEL_VM_regvar(m_pf_eel, "echo_alpha");
	    var_pf_echo_orient = NSEEL_VM_regvar(m_pf_eel, "echo_orient");
        var_pf_wave_usedots  = NSEEL_VM_regvar(m_pf_eel, "wave_usedots");
        var_pf_wave_thick    = NSEEL_VM_regvar(m_pf_eel, "wave_thick");
        var_pf_wave_additive = NSEEL_VM_regvar(m_pf_eel, "wave_additive");
        var_pf_wave_brighten = NSEEL_VM_regvar(m_pf_eel, "wave_brighten");
        var_pf_darken_center = NSEEL_VM_regvar(m_pf_eel, "darken_center");
        var_pf_gamma         = NSEEL_VM_regvar(m_pf_eel, "gamma");
        var_pf_wrap          = NSEEL_VM_regvar(m_pf_eel, "wrap");
        var_pf_invert        = NSEEL_VM_regvar(m_pf_eel, "invert");
        var_pf_brighten      = NSEEL_VM_regvar(m_pf_eel, "brighten");
        var_pf_darken        = NSEEL_VM_regvar(m_pf_eel, "darken");
        var_pf_solarize      = NSEEL_VM_regvar(m_pf_eel, "solarize");
        var_pf_meshx         = NSEEL_VM_regvar(m_pf_eel, "meshx");
        var_pf_meshy         = NSEEL_VM_regvar(m_pf_eel, "meshy");
        var_pf_pixelsx       = NSEEL_VM_regvar(m_pf_eel, "pixelsx");
        var_pf_pixelsy       = NSEEL_VM_regvar(m_pf_eel, "pixelsy");
        var_pf_aspectx       = NSEEL_VM_regvar(m_pf_eel, "aspectx");
        var_pf_aspecty       = NSEEL_VM_regvar(m_pf_eel, "aspecty");
        var_pf_blur1min      = NSEEL_VM_regvar(m_pf_eel, "blur1_min");
        var_pf_blur2min      = NSEEL_VM_regvar(m_pf_eel, "blur2_min");
        var_pf_blur3min      = NSEEL_VM_regvar(m_pf_eel, "blur3_min");
        var_pf_blur1max      = NSEEL_VM_regvar(m_pf_eel, "blur1_max");
        var_pf_blur2max      = NSEEL_VM_regvar(m_pf_eel, "blur2_max");
        var_pf_blur3max      = NSEEL_VM_regvar(m_pf_eel, "blur3_max");
        var_pf_blur1_edge_darken = NSEEL_VM_regvar(m_pf_eel, "blur1_edge_darken");

	    // this is the list of variables that can be used for a PER-VERTEX calculation:
	    // ('vertex' meaning a vertex on the mesh) (as opposed to a once-per-frame calculation)

        NSEEL_VM_resetvars(m_pv_eel);

        var_pv_zoom		= NSEEL_VM_regvar(m_pv_eel, "zoom");		// i/o
	    var_pv_zoomexp  = NSEEL_VM_regvar(m_pv_eel, "zoomexp");	// i/o
	    var_pv_rot		= NSEEL_VM_regvar(m_pv_eel, "rot");		// i/o
	    var_pv_warp		= NSEEL_VM_regvar(m_pv_eel, "warp");		// i/o
	    var_pv_cx		= NSEEL_VM_regvar(m_pv_eel, "cx");		// i/o
	    var_pv_cy		= NSEEL_VM_regvar(m_pv_eel, "cy");		// i/o
	    var_pv_dx		= NSEEL_VM_regvar(m_pv_eel, "dx");		// i/o
	    var_pv_dy		= NSEEL_VM_regvar(m_pv_eel, "dy");		// i/o
	    var_pv_sx		= NSEEL_VM_regvar(m_pv_eel, "sx");		// i/o
	    var_pv_sy		= NSEEL_VM_regvar(m_pv_eel, "sy");		// i/o
	    var_pv_time		= NSEEL_VM_regvar(m_pv_eel, "time");		// i
	    var_pv_fps 		= NSEEL_VM_regvar(m_pv_eel, "fps");		// i
	    var_pv_bass		= NSEEL_VM_regvar(m_pv_eel, "bass");		// i
	    var_pv_mid		= NSEEL_VM_regvar(m_pv_eel, "mid");		// i
	    var_pv_treb		= NSEEL_VM_regvar(m_pv_eel, "treb");		// i
	    var_pv_bass_att	= NSEEL_VM_regvar(m_pv_eel, "bass_att");	// i
	    var_pv_mid_att	= NSEEL_VM_regvar(m_pv_eel, "mid_att");	// i
	    var_pv_treb_att	= NSEEL_VM_regvar(m_pv_eel, "treb_att");	// i
	    var_pv_frame    = NSEEL_VM_regvar(m_pv_eel, "frame");
	    var_pv_x		= NSEEL_VM_regvar(m_pv_eel, "x");			// i
	    var_pv_y		= NSEEL_VM_regvar(m_pv_eel, "y");			// i
	    var_pv_rad		= NSEEL_VM_regvar(m_pv_eel, "rad");		// i
	    var_pv_ang		= NSEEL_VM_regvar(m_pv_eel, "ang");		// i
        for (vi=0; vi<NUM_Q_VAR; vi++)
        {
            char buf[16];
            sprintf(buf, "q%d", vi+1);
            var_pv_q[vi] = NSEEL_VM_regvar(m_pv_eel, buf);
        }
	    var_pv_progress = NSEEL_VM_regvar(m_pv_eel, "progress");
        var_pv_meshx    = NSEEL_VM_regvar(m_pv_eel, "meshx");
        var_pv_meshy    = NSEEL_VM_regvar(m_pv_eel, "meshy");
        var_pv_pixelsx  = NSEEL_VM_regvar(m_pv_eel, "pixelsx");
        var_pv_pixelsy  = NSEEL_VM_regvar(m_pv_eel, "pixelsy");
        var_pv_aspectx  = NSEEL_VM_regvar(m_pv_eel, "aspectx");
        var_pv_aspecty  = NSEEL_VM_regvar(m_pv_eel, "aspecty");
    }

    if (flags & RECOMPILE_WAVE_CODE)
    {
        for (int i=0; i<MAX_CUSTOM_WAVES; i++)
        {
	        NSEEL_VM_resetvars(m_wave[i].m_pf_eel);
	        m_wave[i].var_pf_time		= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "time");		// i
	        m_wave[i].var_pf_fps 		= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "fps");		// i
	        m_wave[i].var_pf_frame      = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "frame");     // i
	        m_wave[i].var_pf_progress   = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "progress");  // i
            for (int vi=0; vi<NUM_Q_VAR; vi++)
            {
                char buf[16];
                sprintf(buf, "q%d", vi+1);
                m_wave[i].var_pf_q[vi] = NSEEL_VM_regvar(m_wave[i].m_pf_eel, buf);
            }
            for (vi=0; vi<NUM_T_VAR; vi++)
            {
                char buf[16];
                sprintf(buf, "t%d", vi+1);
                m_wave[i].var_pf_t[vi] = NSEEL_VM_regvar(m_wave[i].m_pf_eel, buf);
            }
	        m_wave[i].var_pf_bass		= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "bass");		// i
	        m_wave[i].var_pf_mid		= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "mid");		// i
	        m_wave[i].var_pf_treb		= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "treb");		// i
	        m_wave[i].var_pf_bass_att	= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "bass_att");	// i
	        m_wave[i].var_pf_mid_att	= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "mid_att");	// i
	        m_wave[i].var_pf_treb_att	= NSEEL_VM_regvar(m_wave[i].m_pf_eel, "treb_att");	// i
	        m_wave[i].var_pf_r          = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "r");         // i/o
	        m_wave[i].var_pf_g          = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "g");         // i/o
	        m_wave[i].var_pf_b          = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "b");         // i/o
	        m_wave[i].var_pf_a          = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "a");         // i/o
            m_wave[i].var_pf_samples    = NSEEL_VM_regvar(m_wave[i].m_pf_eel, "samples");   // i/o

	        NSEEL_VM_resetvars(m_wave[i].m_pp_eel);
	        m_wave[i].var_pp_time		= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "time");		// i
	        m_wave[i].var_pp_fps 		= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "fps");		// i
	        m_wave[i].var_pp_frame      = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "frame");     // i
	        m_wave[i].var_pp_progress   = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "progress");  // i
            for (vi=0; vi<NUM_Q_VAR; vi++)
            {
                char buf[16];
                sprintf(buf, "q%d", vi+1);
                m_wave[i].var_pp_q[vi] = NSEEL_VM_regvar(m_wave[i].m_pp_eel, buf);
            }
            for (vi=0; vi<NUM_T_VAR; vi++)
            {
                char buf[16];
                sprintf(buf, "t%d", vi+1);
                m_wave[i].var_pp_t[vi] = NSEEL_VM_regvar(m_wave[i].m_pp_eel, buf);
            }
	        m_wave[i].var_pp_bass		= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "bass");		// i
	        m_wave[i].var_pp_mid		= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "mid");		// i
	        m_wave[i].var_pp_treb		= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "treb");		// i
	        m_wave[i].var_pp_bass_att	= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "bass_att");	// i
	        m_wave[i].var_pp_mid_att	= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "mid_att");	// i
	        m_wave[i].var_pp_treb_att	= NSEEL_VM_regvar(m_wave[i].m_pp_eel, "treb_att");	// i
            m_wave[i].var_pp_sample     = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "sample");    // i
            m_wave[i].var_pp_value1     = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "value1");    // i
            m_wave[i].var_pp_value2     = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "value2");    // i
	        m_wave[i].var_pp_x          = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "x");         // i/o
	        m_wave[i].var_pp_y          = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "y");         // i/o
	        m_wave[i].var_pp_r          = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "r");         // i/o
	        m_wave[i].var_pp_g          = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "g");         // i/o
	        m_wave[i].var_pp_b          = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "b");         // i/o
	        m_wave[i].var_pp_a          = NSEEL_VM_regvar(m_wave[i].m_pp_eel, "a");         // i/o
        }
    }

    if (flags & RECOMPILE_SHAPE_CODE)
    {
        for (int i=0; i<MAX_CUSTOM_SHAPES; i++)
        {
	        NSEEL_VM_resetvars(m_shape[i].m_pf_eel);
	        m_shape[i].var_pf_time		= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "time");		// i
	        m_shape[i].var_pf_fps 		= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "fps");		// i
	        m_shape[i].var_pf_frame      = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "frame");     // i
	        m_shape[i].var_pf_progress   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "progress");  // i
            for (int vi=0; vi<NUM_Q_VAR; vi++)
            {
                char buf[16];
                sprintf(buf, "q%d", vi+1);
                m_shape[i].var_pf_q[vi] = NSEEL_VM_regvar(m_shape[i].m_pf_eel, buf);
            }
            for (vi=0; vi<NUM_T_VAR; vi++)
            {
                char buf[16];
                sprintf(buf, "t%d", vi+1);
                m_shape[i].var_pf_t[vi] = NSEEL_VM_regvar(m_shape[i].m_pf_eel, buf);
            }
	        m_shape[i].var_pf_bass		= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "bass");		// i
	        m_shape[i].var_pf_mid		= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "mid");		// i
	        m_shape[i].var_pf_treb		= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "treb");		// i
	        m_shape[i].var_pf_bass_att	= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "bass_att");	// i
	        m_shape[i].var_pf_mid_att	= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "mid_att");	// i
	        m_shape[i].var_pf_treb_att	= NSEEL_VM_regvar(m_shape[i].m_pf_eel, "treb_att");	// i
	        m_shape[i].var_pf_x          = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "x");         // i/o
	        m_shape[i].var_pf_y          = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "y");         // i/o
	        m_shape[i].var_pf_rad        = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "rad");         // i/o
	        m_shape[i].var_pf_ang        = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "ang");         // i/o
	        m_shape[i].var_pf_tex_ang    = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "tex_ang");         // i/o
	        m_shape[i].var_pf_tex_zoom   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "tex_zoom");         // i/o
	        m_shape[i].var_pf_sides      = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "sides");         // i/o
	        m_shape[i].var_pf_textured   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "textured");         // i/o
	        m_shape[i].var_pf_instance   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "instance");         // i/o
	        m_shape[i].var_pf_instances  = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "num_inst");         // i/o
	        m_shape[i].var_pf_additive   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "additive");         // i/o
	        m_shape[i].var_pf_thick      = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "thick");         // i/o
	        m_shape[i].var_pf_r          = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "r");         // i/o
	        m_shape[i].var_pf_g          = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "g");         // i/o
	        m_shape[i].var_pf_b          = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "b");         // i/o
	        m_shape[i].var_pf_a          = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "a");         // i/o
	        m_shape[i].var_pf_r2         = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "r2");         // i/o
	        m_shape[i].var_pf_g2         = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "g2");         // i/o
	        m_shape[i].var_pf_b2         = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "b2");         // i/o
	        m_shape[i].var_pf_a2         = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "a2");         // i/o
	        m_shape[i].var_pf_border_r   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "border_r");         // i/o
	        m_shape[i].var_pf_border_g   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "border_g");         // i/o
	        m_shape[i].var_pf_border_b   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "border_b");         // i/o
	        m_shape[i].var_pf_border_a   = NSEEL_VM_regvar(m_shape[i].m_pf_eel, "border_a");         // i/o
        }
    }
}
void CState::Default(DWORD ApplyFlags)
{
	// DON'T FORGET TO ADD NEW VARIABLES TO BLEND FUNCTION, IMPORT, and EXPORT AS WELL!!!!!!!!

    if ( (ApplyFlags & STATE_GENERAL) &&    // check for these 3 @ same time,
         (ApplyFlags & STATE_MOTION) &&     // so a preset switch w/ warp/comp lock
         (ApplyFlags & STATE_WAVE)        // updates the name, but mash-ups don't.
        )
    {
        lstrcpyW(m_szDesc, INVALID_PRESET_DESC);
	    //lstrcpy(m_szSection, "n/a");

        m_fPresetStartTime = -1;
    }

    m_nMinPSVersion   = 0;     
    m_nMaxPSVersion   = 0;     

    m_bBlending				= false;

    // general:
    if (ApplyFlags & STATE_GENERAL)
    {
        m_fRating				= 3.0f;
	    m_fDecay				= 0.98f;	// 1.0 = none, 0.95 = heavy decay
	    m_fGammaAdj				= 2.0f;		// 1.0 = reg; +2.0 = double, +3.0 = triple...
	    m_fVideoEchoZoom		= 2.0f;
	    m_fVideoEchoAlpha		= 0.0f;
	    m_nVideoEchoOrientation	= 0;		// 0-3
	    m_bRedBlueStereo		= false;
	    m_bBrighten				= false;
	    m_bDarken				= false;
	    m_bTexWrap				= true;
	    m_bDarkenCenter			= false;
	    m_bSolarize				= false;
	    m_bInvert				= false;
	    m_fShader				= 0.0f;
	    m_fBlur1Min             = 0.0f;
	    m_fBlur2Min             = 0.0f;
	    m_fBlur3Min             = 0.0f;
	    m_fBlur1Max             = 1.0f;
	    m_fBlur2Max             = 1.0f;
	    m_fBlur3Max             = 1.0f;
	    m_fBlur1EdgeDarken      = 0.25f;
    }

 
    // wave:
    if (ApplyFlags & STATE_WAVE)
    {
	    m_nWaveMode				= 0;
	     m_nOldWaveMode			= -1;
	    m_bAdditiveWaves		= false;
	    m_bWaveDots				= false;
	    m_bWaveThick            = false;
        m_fWaveAlpha			= 0.8f;
	    m_fWaveScale			= 1.0f;
	    m_fWaveSmoothing		= 0.75f;	// 0 = no smoothing, 0.9 = HEAVY smoothing
	    m_fWaveParam			= 0.0f;
	    m_bModWaveAlphaByVolume = false;
	    m_fModWaveAlphaStart	= 0.75f;		// when relative volume hits this level, alpha -> 0
	    m_fModWaveAlphaEnd		= 0.95f;		// when relative volume hits this level, alpha -> 1
	    m_fWaveR		= 1.0f;
	    m_fWaveG		= 1.0f;
	    m_fWaveB		= 1.0f;
	    m_fWaveX		= 0.5f;
	    m_fWaveY		= 0.5f;
	    m_bMaximizeWaveColor	= true;
	    m_fMvX				  	= 12.0f;
	    m_fMvY					= 9.0f;
	    m_fMvDX                 = 0.0f;
	    m_fMvDY                 = 0.0f;
	    m_fMvL				  	= 0.9f;
	    m_fMvR                  = 1.0f;
	    m_fMvG                  = 1.0f;
	    m_fMvB                  = 1.0f;
	    m_fMvA                  = 1.0f;

        for (int i=0; i<MAX_CUSTOM_WAVES; i++)
        {
            m_wave[i].enabled = 0;
            m_wave[i].samples = 512;
            m_wave[i].sep = 0;
            m_wave[i].scaling = 1.0f;
            m_wave[i].smoothing = 0.5f;
            m_wave[i].r = 1.0f;
            m_wave[i].g = 1.0f;
            m_wave[i].b = 1.0f;
            m_wave[i].a = 1.0f;
            m_wave[i].bSpectrum = 0;
            m_wave[i].bUseDots = 0;
            m_wave[i].bDrawThick = 0;
            m_wave[i].bAdditive = 0;
        }
        for (i=0; i<MAX_CUSTOM_SHAPES; i++)
        {
            m_shape[i].enabled = 0;
            m_shape[i].sides   = 4;
            m_shape[i].additive = 0;
            m_shape[i].thickOutline = 0;
            m_shape[i].textured = 0;
            m_shape[i].instances = 1;
            m_shape[i].tex_zoom = 1.0f;
            m_shape[i].tex_ang  = 0.0f;
            m_shape[i].x = 0.5f;
            m_shape[i].y = 0.5f;
            m_shape[i].rad = 0.1f;
            m_shape[i].ang = 0.0f;
            m_shape[i].r = 1.0f;
            m_shape[i].g = 0.0f;
            m_shape[i].b = 0.0f;
            m_shape[i].a = 1.0f;
            m_shape[i].r2 = 0.0f;
            m_shape[i].g2 = 1.0f;
            m_shape[i].b2 = 0.0f;
            m_shape[i].a2 = 0.0f;
            m_shape[i].border_r = 1.0f;
            m_shape[i].border_g = 1.0f;
            m_shape[i].border_b = 1.0f;
            m_shape[i].border_a = 0.1f;
        }
        for (i=0; i<MAX_CUSTOM_WAVES; i++)
        {
            m_wave[i].m_szInit[0] = 0;
            m_wave[i].m_szPerFrame[0] = 0;
            m_wave[i].m_szPerPoint[0] = 0;
        }
        for (i=0; i<MAX_CUSTOM_SHAPES; i++)
        {
            m_shape[i].m_szInit[0] = 0;
            m_shape[i].m_szPerFrame[0] = 0;
            //m_shape[i].m_szPerPoint[0] = 0;
        }
    }

    // motion:
    if (ApplyFlags & STATE_MOTION)
    {
        m_fWarpAnimSpeed		= 1.0f;		// additional timescaling for warp animation
	    m_fWarpScale			= 1.0f;
	    m_fZoomExponent			= 1.0f;
	    m_fZoom			= 1.0f;
	    m_fRot 			= 0.0f;
	    m_fRotCX		= 0.5f;
	    m_fRotCY		= 0.5f;
	    m_fXPush		= 0.0f;
	    m_fYPush		= 0.0f;
	    m_fWarpAmount	= 1.0f;
	    m_fStretchX     = 1.0f;
	    m_fStretchY     = 1.0f;
	    m_fOuterBorderSize = 0.01f;
	    m_fOuterBorderR	= 0.0f;
	    m_fOuterBorderG	= 0.0f;
	    m_fOuterBorderB	= 0.0f;
	    m_fOuterBorderA	= 0.0f;
	    m_fInnerBorderSize = 0.01f;
	    m_fInnerBorderR	= 0.25f;
	    m_fInnerBorderG	= 0.25f;
	    m_fInnerBorderB	= 0.25f;
	    m_fInnerBorderA	= 0.0f;

        // clear all code strings:
        m_szPerFrameInit[0] = 0;
        m_szPerFrameExpr[0] = 0;
        m_szPerPixelExpr[0] = 0;
    }

	// DON'T FORGET TO ADD NEW VARIABLES TO BLEND FUNCTION, IMPORT, and EXPORT AS WELL!!!!!!!!
	// ALSO BE SURE TO REGISTER THEM ON THE MAIN MENU (SEE MILKDROP.CPP)

    // warp shader
    if (ApplyFlags & STATE_WARP)
    {
        m_szWarpShadersText[0] = 0;
        m_nWarpPSVersion   = 0;     
    }
    
    // comp shader
    if (ApplyFlags & STATE_COMP)
    {
        m_szCompShadersText[0] = 0;
        m_nCompPSVersion   = 0;     
    }

    RandomizePresetVars();

	FreeVarsAndCode();
}

void CState::StartBlendFrom(CState *s_from, float fAnimTime, float fTimespan)
{
	CState *s_to = this;

	// bools, ints, and strings instantly change

	s_to->m_fVideoEchoAlphaOld		 = s_from->m_fVideoEchoAlpha.eval(-1);
	s_to->m_nVideoEchoOrientationOld = s_from->m_nVideoEchoOrientation;
	s_to->m_nOldWaveMode			 = s_from->m_nWaveMode;

	/*
	s_to->m_fVideoEchoAlphaOld		 = s_from->m_fVideoEchoAlpha.eval(-1);
	s_to->m_nVideoEchoOrientationOld = s_from->m_nVideoEchoOrientation;

	s_to->m_nOldWaveMode			= s_from->m_nWaveMode;
	s_to->m_nWaveMode				= s_from->m_nWaveMode;
	s_to->m_bAdditiveWaves			= s_from->m_bAdditiveWaves;
	s_to->m_nVideoEchoOrientation	= s_from->m_nVideoEchoOrientation;
	s_to->m_fWarpAnimSpeed			= s_from->m_fWarpAnimSpeed;	// would req. 10 phase-matches to blend this one!!!
	s_to->m_bWaveDots				= s_from->m_bWaveDots;
	s_to->m_bWaveThick				= s_from->m_bWaveThick;
	s_to->m_bModWaveAlphaByVolume	= s_from->m_bModWaveAlphaByVolume;
	s_to->m_bMaximizeWaveColor		= s_from->m_bMaximizeWaveColor;
	s_to->m_bTexWrap				= s_from->m_bTexWrap;			
	s_to->m_bDarkenCenter			= s_from->m_bDarkenCenter;
	s_to->m_bRedBlueStereo			= s_from->m_bRedBlueStereo;
	s_to->m_bBrighten				= s_from->m_bBrighten;
	s_to->m_bDarken					= s_from->m_bDarken;
	s_to->m_bSolarize				= s_from->m_bSolarize;
	s_to->m_bInvert					= s_from->m_bInvert;
	s_to->m_fRating					= s_from->m_fRating;
	*/

	// expr. eval. also copies over immediately (replaces prev.)
	m_bBlending = true;
	m_fBlendStartTime = fAnimTime;
	m_fBlendDuration = fTimespan;
	
	/*
	//for (int e=0; e<MAX_EVALS; e++)
	{
		char szTemp[MAX_BIGSTRING_LEN];

		lstrcpy(szTemp, m_szPerFrameExpr);
		lstrcpy(m_szPerFrameExpr, s_to->m_szPerFrameExpr);
		lstrcpy(s_to->m_szPerFrameExpr, szTemp);

		lstrcpy(szTemp, m_szPerPixelExpr);
		lstrcpy(m_szPerPixelExpr, s_to->m_szPerPixelExpr);
		lstrcpy(s_to->m_szPerPixelExpr, szTemp);

		lstrcpy(szTemp, m_szPerFrameInit);
		lstrcpy(m_szPerFrameInit, s_to->m_szPerFrameInit);
		lstrcpy(s_to->m_szPerFrameInit, szTemp);
	}
	RecompileExpressions();
	s_to->RecompileExpressions();

	lstrcpy(m_szDesc,    s_to->m_szDesc);
	//lstrcpy(m_szSection, s_to->m_szSection);
	*/
	
	// CBlendableFloats & SuperValues blend over time 
	m_fGammaAdj      .StartBlendFrom(&s_from->m_fGammaAdj      , fAnimTime, fTimespan);
	m_fVideoEchoZoom .StartBlendFrom(&s_from->m_fVideoEchoZoom , fAnimTime, fTimespan);
	m_fVideoEchoAlpha.StartBlendFrom(&s_from->m_fVideoEchoAlpha, fAnimTime, fTimespan);
	m_fDecay         .StartBlendFrom(&s_from->m_fDecay         , fAnimTime, fTimespan);
	m_fWaveAlpha     .StartBlendFrom(&s_from->m_fWaveAlpha     , fAnimTime, fTimespan);
	m_fWaveScale     .StartBlendFrom(&s_from->m_fWaveScale     , fAnimTime, fTimespan);
	m_fWaveSmoothing .StartBlendFrom(&s_from->m_fWaveSmoothing , fAnimTime, fTimespan);
	m_fWaveParam     .StartBlendFrom(&s_from->m_fWaveParam     , fAnimTime, fTimespan);
	m_fWarpScale     .StartBlendFrom(&s_from->m_fWarpScale     , fAnimTime, fTimespan);
	m_fZoomExponent  .StartBlendFrom(&s_from->m_fZoomExponent  , fAnimTime, fTimespan);
	m_fShader        .StartBlendFrom(&s_from->m_fShader        , fAnimTime, fTimespan);
	m_fModWaveAlphaStart.StartBlendFrom(&s_from->m_fModWaveAlphaStart, fAnimTime, fTimespan);
	m_fModWaveAlphaEnd  .StartBlendFrom(&s_from->m_fModWaveAlphaEnd, fAnimTime, fTimespan);

	m_fZoom		.StartBlendFrom(&s_from->m_fZoom		, fAnimTime, fTimespan);
	m_fRot 		.StartBlendFrom(&s_from->m_fRot 		, fAnimTime, fTimespan);
	m_fRotCX	.StartBlendFrom(&s_from->m_fRotCX		, fAnimTime, fTimespan);
	m_fRotCY	.StartBlendFrom(&s_from->m_fRotCY		, fAnimTime, fTimespan);
	m_fXPush	.StartBlendFrom(&s_from->m_fXPush		, fAnimTime, fTimespan);
	m_fYPush	.StartBlendFrom(&s_from->m_fYPush		, fAnimTime, fTimespan);
	m_fWarpAmount.StartBlendFrom(&s_from->m_fWarpAmount,fAnimTime, fTimespan);
	m_fStretchX .StartBlendFrom(&s_from->m_fStretchX	, fAnimTime, fTimespan);
	m_fStretchY .StartBlendFrom(&s_from->m_fStretchY	, fAnimTime, fTimespan);
	m_fWaveR	.StartBlendFrom(&s_from->m_fWaveR		, fAnimTime, fTimespan);
	m_fWaveG	.StartBlendFrom(&s_from->m_fWaveG		, fAnimTime, fTimespan);
	m_fWaveB	.StartBlendFrom(&s_from->m_fWaveB		, fAnimTime, fTimespan);
	m_fWaveX	.StartBlendFrom(&s_from->m_fWaveX		, fAnimTime, fTimespan);
	m_fWaveY	.StartBlendFrom(&s_from->m_fWaveY		, fAnimTime, fTimespan);
	m_fOuterBorderSize	.StartBlendFrom(&s_from->m_fOuterBorderSize	, fAnimTime, fTimespan);
	m_fOuterBorderR		.StartBlendFrom(&s_from->m_fOuterBorderR	, fAnimTime, fTimespan);
	m_fOuterBorderG		.StartBlendFrom(&s_from->m_fOuterBorderG	, fAnimTime, fTimespan);
	m_fOuterBorderB		.StartBlendFrom(&s_from->m_fOuterBorderB	, fAnimTime, fTimespan);
	m_fOuterBorderA		.StartBlendFrom(&s_from->m_fOuterBorderA	, fAnimTime, fTimespan);
	m_fInnerBorderSize	.StartBlendFrom(&s_from->m_fInnerBorderSize	, fAnimTime, fTimespan);
	m_fInnerBorderR		.StartBlendFrom(&s_from->m_fInnerBorderR	, fAnimTime, fTimespan);
	m_fInnerBorderG		.StartBlendFrom(&s_from->m_fInnerBorderG	, fAnimTime, fTimespan);
	m_fInnerBorderB		.StartBlendFrom(&s_from->m_fInnerBorderB	, fAnimTime, fTimespan);
	m_fInnerBorderA		.StartBlendFrom(&s_from->m_fInnerBorderA	, fAnimTime, fTimespan);
	m_fMvX				.StartBlendFrom(&s_from->m_fMvX				, fAnimTime, fTimespan);
	m_fMvY				.StartBlendFrom(&s_from->m_fMvY				, fAnimTime, fTimespan);
	m_fMvDX				.StartBlendFrom(&s_from->m_fMvDX			, fAnimTime, fTimespan);
	m_fMvDY				.StartBlendFrom(&s_from->m_fMvDY			, fAnimTime, fTimespan);
	m_fMvL				.StartBlendFrom(&s_from->m_fMvL				, fAnimTime, fTimespan);
	m_fMvR				.StartBlendFrom(&s_from->m_fMvR				, fAnimTime, fTimespan);
	m_fMvG				.StartBlendFrom(&s_from->m_fMvG				, fAnimTime, fTimespan);
	m_fMvB				.StartBlendFrom(&s_from->m_fMvB				, fAnimTime, fTimespan);
	m_fMvA				.StartBlendFrom(&s_from->m_fMvA				, fAnimTime, fTimespan);
    m_fBlur1Min         .StartBlendFrom(&s_from->m_fBlur1Min        , fAnimTime, fTimespan);
    m_fBlur2Min         .StartBlendFrom(&s_from->m_fBlur2Min        , fAnimTime, fTimespan);
    m_fBlur3Min         .StartBlendFrom(&s_from->m_fBlur3Min        , fAnimTime, fTimespan);
    m_fBlur1Max         .StartBlendFrom(&s_from->m_fBlur1Max        , fAnimTime, fTimespan);
    m_fBlur2Max         .StartBlendFrom(&s_from->m_fBlur2Max        , fAnimTime, fTimespan);
    m_fBlur3Max         .StartBlendFrom(&s_from->m_fBlur3Max        , fAnimTime, fTimespan);
    m_fBlur1EdgeDarken  .StartBlendFrom(&s_from->m_fBlur1EdgeDarken , fAnimTime, fTimespan);

	// if motion vectors were transparent before, don't morph the # in X and Y - just
	// start in the right place, and fade them in.
	bool bOldStateTransparent = (s_from->m_fMvA.eval(-1) < 0.001f);
	bool bNewStateTransparent = (s_to->m_fMvA.eval(-1) < 0.001f);
	if (!bOldStateTransparent && bNewStateTransparent)
	{
		s_from->m_fMvX = s_to->m_fMvX.eval(fAnimTime);
		s_from->m_fMvY = s_to->m_fMvY.eval(fAnimTime);
		s_from->m_fMvDX = s_to->m_fMvDX.eval(fAnimTime);
		s_from->m_fMvDY = s_to->m_fMvDY.eval(fAnimTime);
		s_from->m_fMvL = s_to->m_fMvL.eval(fAnimTime);
		s_from->m_fMvR = s_to->m_fMvR.eval(fAnimTime);
		s_from->m_fMvG = s_to->m_fMvG.eval(fAnimTime);
		s_from->m_fMvB = s_to->m_fMvB.eval(fAnimTime);
	}
	if (bNewStateTransparent && !bOldStateTransparent)
	{
		s_to->m_fMvX = s_from->m_fMvX.eval(fAnimTime);
		s_to->m_fMvY = s_from->m_fMvY.eval(fAnimTime);
		s_to->m_fMvDX = s_from->m_fMvDX.eval(fAnimTime);
		s_to->m_fMvDY = s_from->m_fMvDY.eval(fAnimTime);
		s_to->m_fMvL = s_from->m_fMvL.eval(fAnimTime);
		s_to->m_fMvR = s_from->m_fMvR.eval(fAnimTime);
		s_to->m_fMvG = s_from->m_fMvG.eval(fAnimTime);
		s_to->m_fMvB = s_from->m_fMvB.eval(fAnimTime);
	}

}

void WriteCode(FILE* fOut, int i, char* pStr, char* prefix, bool bPrependApostrophe = false)
{
	char szLineName[32];
	int line = 1;
	int start_pos = 0;
	int char_pos = 0;

	while (pStr[start_pos] != 0)
	{
		while (	pStr[char_pos] != 0 &&
				pStr[char_pos] != LINEFEED_CONTROL_CHAR)
			char_pos++;

		sprintf(szLineName, "%s%d", prefix, line);

		char ch = pStr[char_pos];
		pStr[char_pos] = 0;
		//if (!WritePrivateProfileString(szSectionName,szLineName,&pStr[start_pos],szIniFile)) return false;
        fprintf(fOut, "%s=%s%s\n", szLineName, bPrependApostrophe ? "`" : "", &pStr[start_pos]);
		pStr[char_pos] = ch;

		if (pStr[char_pos] != 0) char_pos++;
		start_pos = char_pos;
		line++;
	}
}

bool CState::Export(const wchar_t *szIniFile)
{
	FILE *fOut = _wfopen(szIniFile, L"w");
	if (!fOut) return false;

    // IMPORTANT: THESE MUST BE THE FIRST TWO LINES.  Otherwise it is assumed to be a MilkDrop 1-era preset.
    if (m_nMaxPSVersion > 0)
    {
        fprintf(fOut, "MILKDROP_PRESET_VERSION=%d\n", CUR_MILKDROP_PRESET_VERSION);
        fprintf(fOut, "PSVERSION=%d\n"     ,m_nMaxPSVersion);  // the max
        fprintf(fOut, "PSVERSION_WARP=%d\n",m_nWarpPSVersion);
        fprintf(fOut, "PSVERSION_COMP=%d\n",m_nCompPSVersion);
    }
    
    // just for backwards compatibility; MilkDrop 1 can read MilkDrop 2 presets, minus the new features.
    // (...this section name allows the GetPrivateProfile*() functions to still work on milkdrop 1)
	fprintf(fOut, "[preset00]\n");    

	fprintf(fOut, "%s=%.3f\n", "fRating",                m_fRating);         
	fprintf(fOut, "%s=%.3f\n", "fGammaAdj",              m_fGammaAdj.eval(-1));         
	fprintf(fOut, "%s=%.3f\n", "fDecay",                 m_fDecay.eval(-1));            
	fprintf(fOut, "%s=%.3f\n", "fVideoEchoZoom",         m_fVideoEchoZoom.eval(-1));    
	fprintf(fOut, "%s=%.3f\n", "fVideoEchoAlpha",        m_fVideoEchoAlpha.eval(-1));   
	fprintf(fOut, "%s=%d\n", "nVideoEchoOrientation",  m_nVideoEchoOrientation);      

	fprintf(fOut, "%s=%d\n", "nWaveMode",              m_nWaveMode);                  
	fprintf(fOut, "%s=%d\n", "bAdditiveWaves",         m_bAdditiveWaves);             
	fprintf(fOut, "%s=%d\n", "bWaveDots",              m_bWaveDots);                  
	fprintf(fOut, "%s=%d\n", "bWaveThick",             m_bWaveThick);                  
	fprintf(fOut, "%s=%d\n", "bModWaveAlphaByVolume",  m_bModWaveAlphaByVolume);      
	fprintf(fOut, "%s=%d\n", "bMaximizeWaveColor",     m_bMaximizeWaveColor);         
	fprintf(fOut, "%s=%d\n", "bTexWrap",               m_bTexWrap			);         
	fprintf(fOut, "%s=%d\n", "bDarkenCenter",          m_bDarkenCenter		);         
	fprintf(fOut, "%s=%d\n", "bRedBlueStereo",         m_bRedBlueStereo     );
	fprintf(fOut, "%s=%d\n", "bBrighten",              m_bBrighten			);         
	fprintf(fOut, "%s=%d\n", "bDarken",                m_bDarken			);         
	fprintf(fOut, "%s=%d\n", "bSolarize",              m_bSolarize			);         
	fprintf(fOut, "%s=%d\n", "bInvert",                m_bInvert			);         

	fprintf(fOut, "%s=%.3f\n", "fWaveAlpha",             m_fWaveAlpha.eval(-1)); 		  
	fprintf(fOut, "%s=%.3f\n", "fWaveScale",             m_fWaveScale.eval(-1));        
	fprintf(fOut, "%s=%.3f\n", "fWaveSmoothing",         m_fWaveSmoothing.eval(-1));    
	fprintf(fOut, "%s=%.3f\n", "fWaveParam",             m_fWaveParam.eval(-1));        
	fprintf(fOut, "%s=%.3f\n", "fModWaveAlphaStart",     m_fModWaveAlphaStart.eval(-1));
	fprintf(fOut, "%s=%.3f\n", "fModWaveAlphaEnd",       m_fModWaveAlphaEnd.eval(-1));  
	fprintf(fOut, "%s=%.3f\n", "fWarpAnimSpeed",         m_fWarpAnimSpeed);             
	fprintf(fOut, "%s=%.3f\n", "fWarpScale",             m_fWarpScale.eval(-1));        
	fprintf(fOut, "%s=%.5f\n", "fZoomExponent",          m_fZoomExponent.eval(-1));     
	fprintf(fOut, "%s=%.3f\n", "fShader",                m_fShader.eval(-1));           

	fprintf(fOut, "%s=%.5f\n", "zoom",                   m_fZoom      .eval(-1));       
	fprintf(fOut, "%s=%.5f\n", "rot",                    m_fRot       .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "cx",                     m_fRotCX     .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "cy",                     m_fRotCY     .eval(-1));       
	fprintf(fOut, "%s=%.5f\n", "dx",                     m_fXPush     .eval(-1));       
	fprintf(fOut, "%s=%.5f\n", "dy",                     m_fYPush     .eval(-1));       
	fprintf(fOut, "%s=%.5f\n", "warp",                   m_fWarpAmount.eval(-1));       
	fprintf(fOut, "%s=%.5f\n", "sx",                     m_fStretchX  .eval(-1));       
	fprintf(fOut, "%s=%.5f\n", "sy",                     m_fStretchY  .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "wave_r",                 m_fWaveR     .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "wave_g",                 m_fWaveG     .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "wave_b",                 m_fWaveB     .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "wave_x",                 m_fWaveX     .eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "wave_y",                 m_fWaveY     .eval(-1));       

	fprintf(fOut, "%s=%.3f\n", "ob_size",             m_fOuterBorderSize.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ob_r",                m_fOuterBorderR.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ob_g",                m_fOuterBorderG.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ob_b",                m_fOuterBorderB.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ob_a",                m_fOuterBorderA.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ib_size",             m_fInnerBorderSize.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ib_r",                m_fInnerBorderR.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ib_g",                m_fInnerBorderG.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ib_b",                m_fInnerBorderB.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "ib_a",                m_fInnerBorderA.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "nMotionVectorsX",     m_fMvX.eval(-1));         
	fprintf(fOut, "%s=%.3f\n", "nMotionVectorsY",     m_fMvY.eval(-1));         
	fprintf(fOut, "%s=%.3f\n", "mv_dx",               m_fMvDX.eval(-1));         
	fprintf(fOut, "%s=%.3f\n", "mv_dy",               m_fMvDY.eval(-1));         
	fprintf(fOut, "%s=%.3f\n", "mv_l",                m_fMvL.eval(-1));         
	fprintf(fOut, "%s=%.3f\n", "mv_r",                m_fMvR.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "mv_g",                m_fMvG.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "mv_b",                m_fMvB.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "mv_a",                m_fMvA.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b1n",                 m_fBlur1Min.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b2n",                 m_fBlur2Min.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b3n",                 m_fBlur3Min.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b1x",                 m_fBlur1Max.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b2x",                 m_fBlur2Max.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b3x",                 m_fBlur3Max.eval(-1));       
	fprintf(fOut, "%s=%.3f\n", "b1ed",                m_fBlur1EdgeDarken.eval(-1));       

    for (int i=0; i<MAX_CUSTOM_WAVES; i++)
        m_wave[i].Export(fOut, L"dummy_filename", i);

    for (i=0; i<MAX_CUSTOM_SHAPES; i++)
        m_shape[i].Export(fOut, L"dummy_filename", i);

	// write out arbitrary expressions, one line at a time
    WriteCode(fOut, i, m_szPerFrameInit, "per_frame_init_");
    WriteCode(fOut, i, m_szPerFrameExpr, "per_frame_"); 
    WriteCode(fOut, i, m_szPerPixelExpr, "per_pixel_"); 
    if (m_nWarpPSVersion >= MD2_PS_2_0)
        WriteCode(fOut, i, m_szWarpShadersText, "warp_", true);
    if (m_nCompPSVersion >= MD2_PS_2_0)
        WriteCode(fOut, i, m_szCompShadersText, "comp_", true);

	fclose(fOut);

	return true;
}

int  CWave::Export(FILE* fOut, const wchar_t *szFile, int i)
{
    FILE* f2 = fOut;
    if (!fOut)
    {
	    f2 = _wfopen(szFile, L"w");
        if (!f2) return 0;
    }

	fprintf(f2, "wavecode_%d_%s=%d\n", i, "enabled",    enabled);
	fprintf(f2, "wavecode_%d_%s=%d\n", i, "samples",    samples);
	fprintf(f2, "wavecode_%d_%s=%d\n", i, "sep",        sep    );
	fprintf(f2, "wavecode_%d_%s=%d\n", i, "bSpectrum",  bSpectrum);
	fprintf(f2, "wavecode_%d_%s=%d\n", i, "bUseDots",   bUseDots);
	fprintf(f2, "wavecode_%d_%s=%d\n", i, "bDrawThick", bDrawThick);
	fprintf(f2, "wavecode_%d_%s=%d\n", i, "bAdditive",  bAdditive);
	fprintf(f2, "wavecode_%d_%s=%.5f\n", i, "scaling",    scaling);
	fprintf(f2, "wavecode_%d_%s=%.5f\n", i, "smoothing",  smoothing);
	fprintf(f2, "wavecode_%d_%s=%.3f\n", i, "r",          r);
	fprintf(f2, "wavecode_%d_%s=%.3f\n", i, "g",          g);
	fprintf(f2, "wavecode_%d_%s=%.3f\n", i, "b",          b);
	fprintf(f2, "wavecode_%d_%s=%.3f\n", i, "a",          a);

    // READ THE CODE IN
    char prefix[64];
    sprintf(prefix, "wave_%d_init",      i); WriteCode(f2, i, m_szInit,     prefix);
    sprintf(prefix, "wave_%d_per_frame", i); WriteCode(f2, i, m_szPerFrame, prefix);
    sprintf(prefix, "wave_%d_per_point", i); WriteCode(f2, i, m_szPerPoint, prefix);

    if (!fOut)
	    fclose(f2); // [sic]

    return 1;
}

int  CShape::Export(FILE* fOut, const wchar_t *szFile, int i)
{
    FILE* f2 = fOut;
    if (!fOut)
    {
	    f2 = _wfopen(szFile, L"w");
        if (!f2) return 0;
	    //fprintf(f2, "[%s]\n", szSection);
    }

	fprintf(f2, "shapecode_%d_%s=%d\n", i, "enabled",    enabled);
	fprintf(f2, "shapecode_%d_%s=%d\n", i, "sides",      sides);
	fprintf(f2, "shapecode_%d_%s=%d\n", i, "additive",   additive);
	fprintf(f2, "shapecode_%d_%s=%d\n", i, "thickOutline",thickOutline);
	fprintf(f2, "shapecode_%d_%s=%d\n", i, "textured",   textured);
	fprintf(f2, "shapecode_%d_%s=%d\n", i, "num_inst",   instances);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "x",          x);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "y",          y);
	fprintf(f2, "shapecode_%d_%s=%.5f\n", i, "rad",        rad);
	fprintf(f2, "shapecode_%d_%s=%.5f\n", i, "ang",        ang);
	fprintf(f2, "shapecode_%d_%s=%.5f\n", i, "tex_ang",    tex_ang);
	fprintf(f2, "shapecode_%d_%s=%.5f\n", i, "tex_zoom",   tex_zoom);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "r",          r);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "g",          g);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "b",          b);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "a",          a);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "r2",         r2);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "g2",         g2);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "b2",         b2);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "a2",         a2);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "border_r",   border_r);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "border_g",   border_g);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "border_b",   border_b);
	fprintf(f2, "shapecode_%d_%s=%.3f\n", i, "border_a",   border_a);

    char prefix[64];
    sprintf(prefix, "shape_%d_init",      i); WriteCode(f2, i, m_szInit,     prefix);
    sprintf(prefix, "shape_%d_per_frame", i); WriteCode(f2, i, m_szPerFrame, prefix);
    //sprintf(prefix, "shape_%d_per_point", i); WriteCode(f2, i, m_szPerPoint, prefix);

    if (!fOut)
	    fclose(f2); // [sic]

    return 1;
}

void ReadCode(FILE* f, char* pStr, char* prefix)
{
    if (!pStr) 
        return;
    pStr[0] = 0;

	// read in & compile arbitrary expressions
	char szLineName[32];
	char szLine[MAX_BIGSTRING_LEN];
	int len;

	int line = 1;
	int char_pos = 0;
	bool bDone = false;

	while (!bDone)
	{
		sprintf(szLineName, "%s%d", prefix, line); 

		GetFastString(szLineName, "~!@#$", szLine, MAX_BIGSTRING_LEN, f);	// fixme
		len = strlen(szLine);

		if ((strcmp(szLine, "~!@#$")==0) ||		// if the key was missing,
			(len >= MAX_BIGSTRING_LEN-1-char_pos-1))			// or if we're out of space
		{
			bDone = true;
		}
		else 
		{
            sprintf(&pStr[char_pos], "%s%c", (szLine[0]=='`') ? &szLine[1] : szLine, LINEFEED_CONTROL_CHAR);
            if (szLine[0] == '`')
                len--;
		}
	
		char_pos += len + 1;
		line++;
	}
	pStr[char_pos++] = 0;	// null-terminate

	// read in & compile arbitrary expressions
    /*
    int n2 = 3 + MAX_CUSTOM_WAVES*3 + MAX_CUSTOM_SHAPES*2;
	for (int n=0; n<n2; n++)
	{
		char *pStr;
        char prefix[64];
		char szLineName[32];
		char szLine[MAX_BIGSTRING_LEN];
		int len;

		int line = 1;
		int char_pos = 0;
		bool bDone = false;

		switch(n)
		{
		case 0: pStr = m_szPerFrameExpr; strcpy(prefix, "per_frame_"); break;
		case 1: pStr = m_szPerPixelExpr; strcpy(prefix, "per_pixel_"); break;
		case 2: pStr = m_szPerFrameInit; strcpy(prefix, "per_frame_init_"); break;
        default:
            if (n < 3 + 3*MAX_CUSTOM_WAVES)
            {
                int i = (n-3) / 3;
                int j = (n-3) % 3;
                switch(j)
                {
                case 0: pStr = m_wave[i].m_szInit;     sprintf(prefix, "wave_%d_init",      i); break;
                case 1: pStr = m_wave[i].m_szPerFrame; sprintf(prefix, "wave_%d_per_frame", i); break;
                case 2: pStr = m_wave[i].m_szPerPoint; sprintf(prefix, "wave_%d_per_point", i); break;
                }
            }
            else
            {
                int i = (n-3-3*MAX_CUSTOM_WAVES) / 2;
                int j = (n-3-3*MAX_CUSTOM_WAVES) % 2;
                switch(j)
                {
                case 0: pStr = m_shape[i].m_szInit;     sprintf(prefix, "shape_%d_init",      i); break;
                case 1: pStr = m_shape[i].m_szPerFrame; sprintf(prefix, "shape_%d_per_frame", i); break;
                }
            }
		}
		
		while (!bDone)
		{
			sprintf(szLineName, "%s%d", prefix, line); 

			GetPrivateProfileString(szSectionName, szLineName, "~!@#$", szLine, MAX_BIGSTRING_LEN, szIniFile);	// fixme
			len = strlen(szLine);

			if ((strcmp(szLine, "~!@#$")==0) ||		// if the key was missing,
				(len >= MAX_BIGSTRING_LEN-1-char_pos-1))			// or if we're out of space
			{
				bDone = true;
			}
			else 
			{
				sprintf(&pStr[char_pos], "%s%c", szLine, LINEFEED_CONTROL_CHAR);
			}
		
			char_pos += len + 1;
			line++;
		}
		pStr[char_pos++] = 0;	// null-terminate
	}
    */
}

int CWave::Import(FILE* f, const wchar_t* szFile, int i)
{
    FILE* f2 = f;
    if (!f)
    {
	    f2 = _wfopen(szFile, L"rb");
        if (!f2) return 0;
        GetFast_CLEAR();
    }

    char buf[64];
    sprintf(buf, "wavecode_%d_%s", i, "enabled"   ); enabled    = GetFastInt  (buf, enabled   , f2);
    sprintf(buf, "wavecode_%d_%s", i, "samples"   ); samples    = GetFastInt  (buf, samples   , f2);
    sprintf(buf, "wavecode_%d_%s", i, "sep"       ); sep        = GetFastInt  (buf, sep       , f2);
    sprintf(buf, "wavecode_%d_%s", i, "bSpectrum" ); bSpectrum  = GetFastInt  (buf, bSpectrum , f2);
    sprintf(buf, "wavecode_%d_%s", i, "bUseDots"  ); bUseDots   = GetFastInt  (buf, bUseDots  , f2);
    sprintf(buf, "wavecode_%d_%s", i, "bDrawThick"); bDrawThick = GetFastInt  (buf, bDrawThick, f2);
    sprintf(buf, "wavecode_%d_%s", i, "bAdditive" ); bAdditive  = GetFastInt  (buf, bAdditive , f2);
    sprintf(buf, "wavecode_%d_%s", i, "scaling"   ); scaling    = GetFastFloat(buf, scaling   , f2);
    sprintf(buf, "wavecode_%d_%s", i, "smoothing" ); smoothing  = GetFastFloat(buf, smoothing , f2);
    sprintf(buf, "wavecode_%d_%s", i, "r"         ); r          = GetFastFloat(buf, r         , f2);
    sprintf(buf, "wavecode_%d_%s", i, "g"         ); g          = GetFastFloat(buf, g         , f2);
    sprintf(buf, "wavecode_%d_%s", i, "b"         ); b          = GetFastFloat(buf, b         , f2);
    sprintf(buf, "wavecode_%d_%s", i, "a"         ); a          = GetFastFloat(buf, a         , f2);

    // READ THE CODE IN
    char prefix[64];
    sprintf(prefix, "wave_%d_init",      i); ReadCode(f2, m_szInit,     prefix);
    sprintf(prefix, "wave_%d_per_frame", i); ReadCode(f2, m_szPerFrame, prefix);
    sprintf(prefix, "wave_%d_per_point", i); ReadCode(f2, m_szPerPoint, prefix);

    if (!f)
	    fclose(f2); // [sic]

    return 1;
}

int  CShape::Import(FILE* f, const wchar_t* szFile, int i)
{
    FILE* f2 = f;
    if (!f)
    {
	    f2 = _wfopen(szFile, L"rb");
        if (!f2) return 0;
        GetFast_CLEAR();
    }

    char buf[64];
	sprintf(buf, "shapecode_%d_%s", i, "enabled"     ); enabled      = GetFastInt  (buf, enabled     , f2);
	sprintf(buf, "shapecode_%d_%s", i, "sides"       ); sides        = GetFastInt  (buf, sides       , f2);
	sprintf(buf, "shapecode_%d_%s", i, "additive"    ); additive     = GetFastInt  (buf, additive    , f2);
	sprintf(buf, "shapecode_%d_%s", i, "thickOutline"); thickOutline = GetFastInt  (buf, thickOutline, f2);
	sprintf(buf, "shapecode_%d_%s", i, "textured"    ); textured     = GetFastInt  (buf, textured    , f2);
	sprintf(buf, "shapecode_%d_%s", i, "num_inst"   ); instances    = GetFastInt  (buf, instances   , f2);
	sprintf(buf, "shapecode_%d_%s", i, "x"           ); x            = GetFastFloat(buf, x           , f2);
	sprintf(buf, "shapecode_%d_%s", i, "y"           ); y            = GetFastFloat(buf, y           , f2);
	sprintf(buf, "shapecode_%d_%s", i, "rad"         ); rad          = GetFastFloat(buf, rad         , f2);
	sprintf(buf, "shapecode_%d_%s", i, "ang"         ); ang          = GetFastFloat(buf, ang         , f2);
	sprintf(buf, "shapecode_%d_%s", i, "tex_ang"     ); tex_ang      = GetFastFloat(buf, tex_ang     , f2);
	sprintf(buf, "shapecode_%d_%s", i, "tex_zoom"    ); tex_zoom     = GetFastFloat(buf, tex_zoom    , f2);
	sprintf(buf, "shapecode_%d_%s", i, "r"           ); r            = GetFastFloat(buf, r           , f2);
	sprintf(buf, "shapecode_%d_%s", i, "g"           ); g            = GetFastFloat(buf, g           , f2);
	sprintf(buf, "shapecode_%d_%s", i, "b"           ); b            = GetFastFloat(buf, b           , f2);
	sprintf(buf, "shapecode_%d_%s", i, "a"           ); a            = GetFastFloat(buf, a           , f2);
	sprintf(buf, "shapecode_%d_%s", i, "r2"          ); r2           = GetFastFloat(buf, r2          , f2);
	sprintf(buf, "shapecode_%d_%s", i, "g2"          ); g2           = GetFastFloat(buf, g2          , f2);
	sprintf(buf, "shapecode_%d_%s", i, "b2"          ); b2           = GetFastFloat(buf, b2          , f2);
	sprintf(buf, "shapecode_%d_%s", i, "a2"          ); a2           = GetFastFloat(buf, a2          , f2);
	sprintf(buf, "shapecode_%d_%s", i, "border_r"    ); border_r     = GetFastFloat(buf, border_r    , f2);
	sprintf(buf, "shapecode_%d_%s", i, "border_g"    ); border_g     = GetFastFloat(buf, border_g    , f2);
	sprintf(buf, "shapecode_%d_%s", i, "border_b"    ); border_b     = GetFastFloat(buf, border_b    , f2);
	sprintf(buf, "shapecode_%d_%s", i, "border_a"    ); border_a     = GetFastFloat(buf, border_a    , f2);

    // READ THE CODE IN
    char prefix[64];
    sprintf(prefix, "shape_%d_init",      i); ReadCode(f2, m_szInit,     prefix);
    sprintf(prefix, "shape_%d_per_frame", i); ReadCode(f2, m_szPerFrame, prefix);

    if (!f)
	    fclose(f2); // [sic]

    return 1;
}

bool CState::Import(const wchar_t *szIniFile, float fTime, CState* pOldState, DWORD ApplyFlags)
{
    // if any ApplyFlags are missing, the settings will be copied from pOldState.  =)

    if (!pOldState) 
        ApplyFlags = STATE_ALL;
    
    if (ApplyFlags!=STATE_ALL && this != pOldState)
    {
        assert(pOldState);
        // in order to copy the old state, we have to byte copy it.
        memcpy(this, pOldState, sizeof(CState));
        // clear all the copied code pointers, WITHOUT actually freeing it (since ptrs were copied)
        // so that the Default() call below won't release pOldState's copied pointers.
        // [all expressions will be recompiled @ end of this fn, whether we updated them or not]
        FreeVarsAndCode(false);  
    }

    // apply defaults for the stuff we will overwrite.
    Default(ApplyFlags);//RandomizePresetVars();

    GetFast_CLEAR();

    if ( (ApplyFlags & STATE_GENERAL) &&    // check for these 3 @ same time,
         (ApplyFlags & STATE_MOTION) &&     // so a preset switch w/ warp/comp lock
         (ApplyFlags & STATE_WAVE)        // updates the name, but mash-ups don't.
        )
    {
        m_fPresetStartTime = fTime;

	    // extract a description of the preset from the filename
	    {
		    // copy get the filename (without the path)
		    const wchar_t *p = wcsrchr(szIniFile, '\\');
		    if (p==NULL) p = szIniFile;
		    lstrcpyW(m_szDesc, p+1);		

		    // next remove the extension
		    RemoveExtension(m_szDesc);
	    }
    }
    
    FILE* f = _wfopen(szIniFile, L"rb");
    if (!f)
        return false;

    int nMilkdropPresetVersion = GetFastInt("MILKDROP_PRESET_VERSION",100,f);
    //if (ApplyFlags != STATE_ALL)
    //    nMilkdropPresetVersion = CUR_MILKDROP_PRESET_VERSION;  //if we're mashing up, force it up to now
    

    int nWarpPSVersionInFile;
    int nCompPSVersionInFile;
    if (nMilkdropPresetVersion < 200) {
        nWarpPSVersionInFile = 0;
        nCompPSVersionInFile = 0;
    }
    else if (nMilkdropPresetVersion == 200) {
        nWarpPSVersionInFile = GetFastInt("PSVERSION", 2, f);
        nCompPSVersionInFile = nWarpPSVersionInFile;
    }
    else {
        nWarpPSVersionInFile = GetFastInt("PSVERSION_WARP", 2, f);
        nCompPSVersionInFile = GetFastInt("PSVERSION_COMP", 2, f);
    }

    // general:
    if (ApplyFlags & STATE_GENERAL)
    {
        m_fRating				= GetFastFloat("fRating",m_fRating,f);
	    m_fDecay                = GetFastFloat("fDecay",m_fDecay.eval(-1),f);
	    m_fGammaAdj             = GetFastFloat("fGammaAdj" ,m_fGammaAdj.eval(-1),f);
	    m_fVideoEchoZoom        = GetFastFloat("fVideoEchoZoom",m_fVideoEchoZoom.eval(-1),f);
	    m_fVideoEchoAlpha       = GetFastFloat("fVideoEchoAlpha",m_fVideoEchoAlpha.eval(-1),f);
	    m_nVideoEchoOrientation = GetFastInt  ("nVideoEchoOrientation",m_nVideoEchoOrientation,f);
        m_bRedBlueStereo        = (GetFastInt ("bRedBlueStereo", m_bRedBlueStereo,f) != 0);
	    m_bBrighten				= (GetFastInt ("bBrighten",m_bBrighten	,f) != 0);
	    m_bDarken				= (GetFastInt ("bDarken"  ,m_bDarken	,f) != 0);
	    m_bSolarize				= (GetFastInt ("bSolarize",m_bSolarize	,f) != 0);
	    m_bInvert				= (GetFastInt ("bInvert"  ,m_bInvert	,f) != 0);
	    m_fShader               = GetFastFloat("fShader",m_fShader.eval(-1),f);
        m_fBlur1Min			= GetFastFloat("b1n",    m_fBlur1Min.eval(-1),f);       
        m_fBlur2Min			= GetFastFloat("b2n",    m_fBlur2Min.eval(-1),f);       
        m_fBlur3Min			= GetFastFloat("b3n",    m_fBlur3Min.eval(-1),f);       
        m_fBlur1Max			= GetFastFloat("b1x",    m_fBlur1Max.eval(-1),f);       
        m_fBlur2Max			= GetFastFloat("b2x",    m_fBlur2Max.eval(-1),f);       
        m_fBlur3Max			= GetFastFloat("b3x",    m_fBlur3Max.eval(-1),f);       
        m_fBlur1EdgeDarken  = GetFastFloat("b1ed",   m_fBlur1EdgeDarken.eval(-1),f); 
    }

    // wave:
    if (ApplyFlags & STATE_WAVE)
    {
	    m_nWaveMode             = GetFastInt  ("nWaveMode",m_nWaveMode,f);
	    m_bAdditiveWaves		= (GetFastInt ("bAdditiveWaves",m_bAdditiveWaves,f) != 0);
	    m_bWaveDots		        = (GetFastInt ("bWaveDots",m_bWaveDots,f) != 0);
	    m_bWaveThick            = (GetFastInt ("bWaveThick",m_bWaveThick,f) != 0);
	    m_bModWaveAlphaByVolume	= (GetFastInt ("bModWaveAlphaByVolume",m_bModWaveAlphaByVolume,f) != 0);
	    m_bMaximizeWaveColor    = (GetFastInt ("bMaximizeWaveColor" ,m_bMaximizeWaveColor,f) != 0);
	    m_fWaveAlpha            = GetFastFloat("fWaveAlpha",m_fWaveAlpha.eval(-1),f);
	    m_fWaveScale            = GetFastFloat("fWaveScale",m_fWaveScale.eval(-1),f);
	    m_fWaveSmoothing        = GetFastFloat("fWaveSmoothing",m_fWaveSmoothing.eval(-1),f);
	    m_fWaveParam            = GetFastFloat("fWaveParam",m_fWaveParam.eval(-1),f);
	    m_fModWaveAlphaStart    = GetFastFloat("fModWaveAlphaStart",m_fModWaveAlphaStart.eval(-1),f);
	    m_fModWaveAlphaEnd      = GetFastFloat("fModWaveAlphaEnd",m_fModWaveAlphaEnd.eval(-1),f);
	    m_fWaveR				= GetFastFloat("wave_r",m_fRot.eval(-1),f);	
	    m_fWaveG				= GetFastFloat("wave_g",m_fRot.eval(-1),f);	
	    m_fWaveB				= GetFastFloat("wave_b",m_fRot.eval(-1),f);	
	    m_fWaveX				= GetFastFloat("wave_x",m_fRot.eval(-1),f);	
	    m_fWaveY				= GetFastFloat("wave_y",m_fRot.eval(-1),f);	
	    m_fMvX				= GetFastFloat("nMotionVectorsX",  m_fMvX.eval(-1),f);
	    m_fMvY           	= GetFastFloat("nMotionVectorsY",  m_fMvY.eval(-1),f);
	    m_fMvDX				= GetFastFloat("mv_dx",  m_fMvDX.eval(-1),f);
	    m_fMvDY				= GetFastFloat("mv_dy",  m_fMvDY.eval(-1),f);
	    m_fMvL				= GetFastFloat("mv_l",   m_fMvL.eval(-1),f);
	    m_fMvR				= GetFastFloat("mv_r",   m_fMvR.eval(-1),f);	
	    m_fMvG				= GetFastFloat("mv_g",   m_fMvG.eval(-1),f);	
	    m_fMvB				= GetFastFloat("mv_b",   m_fMvB.eval(-1),f);	
	    m_fMvA				= (GetFastInt ("bMotionVectorsOn",false,f) == 0) ? 0.0f : 1.0f; // for backwards compatibility
	    m_fMvA				= GetFastFloat("mv_a",   m_fMvA.eval(-1),f);	
        for (int i=0; i<MAX_CUSTOM_WAVES; i++)
        {
            m_wave[i].Import(f, L"dummy_filename", i);
        }
        for (i=0; i<MAX_CUSTOM_SHAPES; i++)
        {
            m_shape[i].Import(f, L"dummy_filename", i);
        }
    }

    // motion:
    if (ApplyFlags & STATE_MOTION)
    {
	    m_fZoom					= GetFastFloat("zoom",m_fZoom.eval(-1),f);	
	    m_fRot					= GetFastFloat("rot",m_fRot.eval(-1),f);	
	    m_fRotCX				= GetFastFloat("cx",m_fRotCX.eval(-1),f);	
	    m_fRotCY				= GetFastFloat("cy",m_fRotCY.eval(-1),f);	
	    m_fXPush				= GetFastFloat("dx",m_fXPush.eval(-1),f);	
	    m_fYPush				= GetFastFloat("dy",m_fYPush.eval(-1),f);	
	    m_fWarpAmount			= GetFastFloat("warp",m_fWarpAmount.eval(-1),f);	
	    m_fStretchX				= GetFastFloat("sx",m_fStretchX.eval(-1),f);	
	    m_fStretchY				= GetFastFloat("sy",m_fStretchY.eval(-1),f);	
        m_bTexWrap			    = (GetFastInt ("bTexWrap", m_bTexWrap,f) != 0);
	    m_bDarkenCenter			= (GetFastInt ("bDarkenCenter", m_bDarkenCenter,f) != 0);
	    m_fWarpAnimSpeed        = GetFastFloat("fWarpAnimSpeed",m_fWarpAnimSpeed,f);
	    m_fWarpScale            = GetFastFloat("fWarpScale",m_fWarpScale.eval(-1),f);
	    m_fZoomExponent         = GetFastFloat("fZoomExponent",m_fZoomExponent.eval(-1),f);
	    m_fOuterBorderSize	= GetFastFloat("ob_size",m_fOuterBorderSize.eval(-1),f);	
	    m_fOuterBorderR		= GetFastFloat("ob_r",   m_fOuterBorderR.eval(-1),f);	
	    m_fOuterBorderG		= GetFastFloat("ob_g",   m_fOuterBorderG.eval(-1),f);	
	    m_fOuterBorderB		= GetFastFloat("ob_b",   m_fOuterBorderB.eval(-1),f);	
	    m_fOuterBorderA		= GetFastFloat("ob_a",   m_fOuterBorderA.eval(-1),f);	
	    m_fInnerBorderSize	= GetFastFloat("ib_size",m_fInnerBorderSize.eval(-1),f);	
	    m_fInnerBorderR		= GetFastFloat("ib_r",   m_fInnerBorderR.eval(-1),f);	
	    m_fInnerBorderG		= GetFastFloat("ib_g",   m_fInnerBorderG.eval(-1),f);	
	    m_fInnerBorderB		= GetFastFloat("ib_b",   m_fInnerBorderB.eval(-1),f);	
	    m_fInnerBorderA		= GetFastFloat("ib_a",   m_fInnerBorderA.eval(-1),f);	
        //m_szPerFrameInit[0] = 0;
        //m_szPerFrameExpr[0] = 0;
        //m_szPerPixelExpr[0] = 0;
        ReadCode(f, m_szPerFrameInit, "per_frame_init_");
        ReadCode(f, m_szPerFrameExpr, "per_frame_");
        ReadCode(f, m_szPerPixelExpr, "per_pixel_");
    }
    
    // warp shader
    if (ApplyFlags & STATE_WARP)
    {
        //m_szWarpShadersText[0] = 0;
        ReadCode(f, m_szWarpShadersText, "warp_");
        if (!m_szWarpShadersText[0]) 
            g_plugin.GenWarpPShaderText(m_szWarpShadersText, m_fDecay.eval(-1), m_bTexWrap);
        m_nWarpPSVersion = nWarpPSVersionInFile;
    }
    
    // comp shader
    if (ApplyFlags & STATE_COMP) 
    {
        //m_szCompShadersText[0] = 0;
        ReadCode(f, m_szCompShadersText, "comp_");
        if (!m_szCompShadersText[0])
            g_plugin.GenCompPShaderText(m_szCompShadersText, m_fGammaAdj.eval(-1), m_fVideoEchoAlpha.eval(-1), m_fVideoEchoZoom.eval(-1), m_nVideoEchoOrientation, m_fShader.eval(-1), m_bBrighten, m_bDarken, m_bSolarize, m_bInvert);
        m_nCompPSVersion = nCompPSVersionInFile;
    }

    m_nMaxPSVersion = max(m_nWarpPSVersion, m_nCompPSVersion);
    m_nMinPSVersion = min(m_nWarpPSVersion, m_nCompPSVersion);
	

	RecompileExpressions();

    fclose(f);

    return true;
}

void CState::GenDefaultWarpShader()
{
    if (m_nWarpPSVersion>0)
        g_plugin.GenWarpPShaderText(m_szWarpShadersText, m_fDecay.eval(-1), m_bTexWrap);
}
void CState::GenDefaultCompShader()
{
    if (m_nCompPSVersion>0)
        g_plugin.GenCompPShaderText(m_szCompShadersText, m_fGammaAdj.eval(-1), m_fVideoEchoAlpha.eval(-1), m_fVideoEchoZoom.eval(-1), m_nVideoEchoOrientation, m_fShader.eval(-1), m_bBrighten, m_bDarken, m_bSolarize, m_bInvert);
}

void CState::FreeVarsAndCode(bool bFree)
{
	// free the compiled expressions
	if (m_pf_codehandle)
	{
        if (bFree)
		    NSEEL_code_free(m_pf_codehandle);
		m_pf_codehandle = NULL;
	}
	if (m_pp_codehandle)
	{
        if (bFree)
    		NSEEL_code_free(m_pp_codehandle);
		m_pp_codehandle = NULL;
	}

    for (int i=0; i<MAX_CUSTOM_WAVES; i++)
    {
	    if (m_wave[i].m_pf_codehandle)
        {
            if (bFree)
                NSEEL_code_free(m_wave[i].m_pf_codehandle);
            m_wave[i].m_pf_codehandle = NULL;
        }
	    if (m_wave[i].m_pp_codehandle)
        {
            if (bFree)
                NSEEL_code_free(m_wave[i].m_pp_codehandle);
            m_wave[i].m_pp_codehandle = NULL;
        }
    }

    for (i=0; i<MAX_CUSTOM_SHAPES; i++)
    {
	    if (m_shape[i].m_pf_codehandle)
        {
            if (bFree)
                NSEEL_code_free(m_shape[i].m_pf_codehandle);
            m_shape[i].m_pf_codehandle = NULL;
        }
	    /*if (m_shape[i].m_pp_codehandle)
        {
            freeCode(m_shape[i].m_pp_codehandle);
            m_shape[i].m_pp_codehandle = NULL;
        }*/
    }

	// free our text version of the expressions? - no!
	//m_szPerFrameExpr[0] = 0;
	//m_szPerPixelExpr[0] = 0;

	// free the old variable names & reregister the built-in variables (since they got nuked too)
	RegisterBuiltInVariables(0xFFFFFFFF);
}

void CState::StripLinefeedCharsAndComments(char *src, char *dest)
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

void CState::RecompileExpressions(int flags, int bReInit)
{
    // before we get started, if we redo the init code for the preset, we have to redo
    // other things too, because q1-q8 could change.
    if ((flags & RECOMPILE_PRESET_CODE) && bReInit)
    {
        flags |= RECOMPILE_WAVE_CODE;
        flags |= RECOMPILE_SHAPE_CODE;
    }

    // free old code handles
    if (flags & RECOMPILE_PRESET_CODE)
    {
	    if (m_pf_codehandle)
	    {
		    NSEEL_code_free(m_pf_codehandle);
		    m_pf_codehandle = NULL;
	    }
	    if (m_pp_codehandle)
	    {
		    NSEEL_code_free(m_pp_codehandle);
		    m_pp_codehandle = NULL;
	    }
    }
    if (flags & RECOMPILE_WAVE_CODE)
    {
        for (int i=0; i<MAX_CUSTOM_WAVES; i++)
        {
		    if (m_wave[i].m_pf_codehandle)
		    {
			    NSEEL_code_free(m_wave[i].m_pf_codehandle);
			    m_wave[i].m_pf_codehandle = NULL;
		    }
		    if (m_wave[i].m_pp_codehandle)
		    {
			    NSEEL_code_free(m_wave[i].m_pp_codehandle);
			    m_wave[i].m_pp_codehandle = NULL;
		    }
        }
    }
    if (flags & RECOMPILE_SHAPE_CODE)
    {
        for (int i=0; i<MAX_CUSTOM_SHAPES; i++)
        {
		    if (m_shape[i].m_pf_codehandle)
		    {
			    NSEEL_code_free(m_shape[i].m_pf_codehandle);
			    m_shape[i].m_pf_codehandle = NULL;
		    }
		    /*if (m_shape[i].m_pp_codehandle)
		    {
			    freeCode(m_shape[i].m_pp_codehandle);
			    m_shape[i].m_pp_codehandle = NULL;
		    }*/
        }
    }

    // if we're recompiling init code, clear vars to zero, and re-register built-in variables.
	if (bReInit)
	{
		RegisterBuiltInVariables(flags);
	}

	// QUICK FIX: if the code strings ONLY have spaces and linefeeds, erase them, 
	// because for some strange reason this causes errors in compileCode().
    int n2 = 3 + MAX_CUSTOM_WAVES*3 + MAX_CUSTOM_SHAPES*2; 
	for (int n=0; n<n2; n++)
	{
		char *pOrig;
		switch(n)
		{
		case 0: pOrig = m_szPerFrameExpr; break;
		case 1: pOrig = m_szPerPixelExpr; break;
		case 2: pOrig = m_szPerFrameInit; break;
        default:
            if (n < 3 + 3*MAX_CUSTOM_WAVES)
            {
                int i = (n-3) / 3;
                int j = (n-3) % 3;
                switch(j)
                {
                case 0: pOrig = m_wave[i].m_szInit;     break;
                case 1: pOrig = m_wave[i].m_szPerFrame; break;
                case 2: pOrig = m_wave[i].m_szPerPoint; break;
                }
            }
            else
            {
                int i = (n-3-3*MAX_CUSTOM_WAVES) / 2;
                int j = (n-3-3*MAX_CUSTOM_WAVES) % 2;
                switch(j)
                {
                case 0: pOrig = m_shape[i].m_szInit;     break;
                case 1: pOrig = m_shape[i].m_szPerFrame; break;
                }
            }
		}
		char *p = pOrig;
		while (*p==' ' || *p==LINEFEED_CONTROL_CHAR) p++;
		if (*p == 0) pOrig[0] = 0;
	}

    // COMPILE NEW CODE.
	#ifndef _NO_EXPR_   
    {
    	// clear any old error msg.:
    	//g_plugin.m_fShowUserMessageUntilThisTime = g_plugin.GetTime();	

	    char buf[MAX_BIGSTRING_LEN*3];

        if (flags & RECOMPILE_PRESET_CODE)
        {
            // 1. compile AND EXECUTE preset init code
		    StripLinefeedCharsAndComments(m_szPerFrameInit, buf);
	        if (buf[0] && bReInit)
	        {
		        NSEEL_CODEHANDLE	pf_codehandle_init;	

			    if ( ! (pf_codehandle_init = NSEEL_code_compile(m_pf_eel, buf)))
			    {
                    wchar_t buf[1024];
				    swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_PRESET_INIT_CODE), m_szDesc);
                    g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);

                    for (int vi=0; vi<NUM_Q_VAR; vi++)
				        q_values_after_init_code[vi] = 0;
                    monitor_after_init_code = 0;
			    }
			    else
			    {
				    // now execute the code, save the values of q1..q32, and clean up the code!

                    g_plugin.LoadPerFrameEvallibVars(g_plugin.m_pState);

				    NSEEL_code_execute(pf_codehandle_init);

                    for (int vi=0; vi<NUM_Q_VAR; vi++)
				        q_values_after_init_code[vi] = *var_pf_q[vi];
                    monitor_after_init_code = *var_pf_monitor;

				    NSEEL_code_free(pf_codehandle_init);
				    pf_codehandle_init = NULL;
			    }
	        }

            // 2. compile preset per-frame code
            StripLinefeedCharsAndComments(m_szPerFrameExpr, buf);
	        if (buf[0])
	        {
			    if ( ! (m_pf_codehandle = NSEEL_code_compile(m_pf_eel, buf)))
			    {
                    wchar_t buf[1024];
				    swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_PER_FRAME_CODE), m_szDesc);
                    g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);
			    }
	        }

            // 3. compile preset per-pixel code
		    StripLinefeedCharsAndComments(m_szPerPixelExpr, buf);
	        if (buf[0])
	        {
			    if ( ! (m_pp_codehandle = NSEEL_code_compile(m_pv_eel, buf)))
			    {
                    wchar_t buf[1024];
				    swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_PER_VERTEX_CODE), m_szDesc);
                    g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);
			    }
	        }
	        
            //resetVars(NULL);
        }

        if (flags & RECOMPILE_WAVE_CODE)
        {
            for (int i=0; i<MAX_CUSTOM_WAVES; i++)
            {
                // 1. compile AND EXECUTE custom waveform init code
		        StripLinefeedCharsAndComments(m_wave[i].m_szInit, buf);
	            if (buf[0] && bReInit)
                {
		            #ifndef _NO_EXPR_
		            {
		                NSEEL_CODEHANDLE	codehandle_temp;	
			            if ( ! (codehandle_temp = NSEEL_code_compile(m_wave[i].m_pf_eel, buf)))
			            {
                            wchar_t buf[1024];
				            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_WAVE_X_INIT_CODE), m_szDesc, i);
                            g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);

                            for (int vi=0; vi<NUM_Q_VAR; vi++)
                                *m_wave[i].var_pf_q[vi] = q_values_after_init_code[vi];
                            for (vi=0; vi<NUM_T_VAR; vi++)
				                m_wave[i].t_values_after_init_code[vi] = 0;
			            }
			            else
			            {
				            // now execute the code, save the values of t1..t8, and clean up the code!
                    
                            g_plugin.LoadCustomWavePerFrameEvallibVars(g_plugin.m_pState, i);
                                // note: q values at this point will actually be same as 
                                //       q_values_after_init_code[], since no per-frame code
                                //       has actually been executed yet!

				            NSEEL_code_execute(codehandle_temp);

                            for (int vi=0; vi<NUM_T_VAR; vi++)
				                m_wave[i].t_values_after_init_code[vi] = *m_wave[i].var_pf_t[vi];

				            NSEEL_code_free(codehandle_temp);
				            codehandle_temp = NULL;
			            }
		            }
                    #endif
                }

                // 2. compile custom waveform per-frame code
		        StripLinefeedCharsAndComments(m_wave[i].m_szPerFrame, buf);
	            if (buf[0])
                {
		            #ifndef _NO_EXPR_
			            if ( ! (m_wave[i].m_pf_codehandle = NSEEL_code_compile(m_wave[i].m_pf_eel, buf)))
			            {
                            wchar_t buf[1024];
				            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_WAVE_X_PER_FRAME_CODE), m_szDesc, i);
                            g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);
			            }
                    #endif
                }

                // 3. compile custom waveform per-point code
		        StripLinefeedCharsAndComments(m_wave[i].m_szPerPoint, buf);
	            if (buf[0])
                {
			        if ( ! (m_wave[i].m_pp_codehandle = NSEEL_code_compile(m_wave[i].m_pp_eel, buf)))
			        {
                        wchar_t buf[1024];
				        swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_WAVE_X_PER_POINT_CODE), m_szDesc, i);
                        g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);
			        }
                }
            }
        }

        if (flags & RECOMPILE_SHAPE_CODE)
        {
            for (int i=0; i<MAX_CUSTOM_SHAPES; i++)
            {
                // 1. compile AND EXECUTE custom shape init code
		        StripLinefeedCharsAndComments(m_shape[i].m_szInit, buf);
	            if (buf[0] && bReInit)
                {
		            #ifndef _NO_EXPR_
		            {
		                NSEEL_CODEHANDLE	codehandle_temp;	
			            if ( ! (codehandle_temp = NSEEL_code_compile(m_shape[i].m_pf_eel, buf)))
			            {
                            wchar_t buf[1024];
				            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_SHAPE_X_INIT_CODE), m_szDesc, i);
                            g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);

                            for (int vi=0; vi<NUM_Q_VAR; vi++)
                                *m_shape[i].var_pf_q[vi] = q_values_after_init_code[vi];
                            for (vi=0; vi<NUM_T_VAR; vi++)
				                m_shape[i].t_values_after_init_code[vi] = 0;
			            }
			            else
			            {
				            // now execute the code, save the values of q1..q8, and clean up the code!
                    
                            g_plugin.LoadCustomShapePerFrameEvallibVars(g_plugin.m_pState, i, 0);
                                // note: q values at this point will actually be same as 
                                //       q_values_after_init_code[], since no per-frame code
                                //       has actually been executed yet!

				            NSEEL_code_execute(codehandle_temp);

                            for (int vi=0; vi<NUM_T_VAR; vi++)
                                m_shape[i].t_values_after_init_code[vi] = *m_shape[i].var_pf_t[vi];

				            NSEEL_code_free(codehandle_temp);
				            codehandle_temp = NULL;
			            }
		            }
		            #endif
                }

                // 2. compile custom shape per-frame code
		        StripLinefeedCharsAndComments(m_shape[i].m_szPerFrame, buf);
	            if (buf[0])
                {
		            #ifndef _NO_EXPR_
			            if ( ! (m_shape[i].m_pf_codehandle = NSEEL_code_compile(m_shape[i].m_pf_eel, buf)))
			            {
                            wchar_t buf[1024];
				            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_WARNING_PRESET_X_ERROR_IN_SHAPE_X_PER_FRAME_CODE), m_szDesc, i);
                            g_plugin.AddError(buf, 6.0f, ERR_PRESET, true);
			            }
		            #endif
                }

                /*
                // 3. compile custom shape per-point code
		        StripLinefeedCharsAndComments(m_shape[i].m_szPerPoint, buf);
	            if (buf[0])
                {
                    resetVars(m_shape[i].m_pp_vars);
		            #ifndef _NO_EXPR_
			            if ( ! (m_shape[i].m_pp_codehandle = compileCode(buf)))
			            {
				            sprintf(g_plugin.m_szUserMessage, "warning: preset \"%s\": error in shape %d per-point code", m_szDesc, i);
				            g_plugin.m_fShowUserMessageUntilThisTime = g_plugin.GetTime() + 6.0f;
                            g_plugin.m_bUserMessageIsError = true;
			            }
		            #endif
                    resetVars(NULL);
                }
                */
            }
        }
    }
    #endif
}

void CState::RandomizePresetVars()
{
    m_rand_preset = D3DXVECTOR4(FRAND, FRAND, FRAND, FRAND);

    int k = 0;
    do
    {
        for (int i=0; i<4; i++) 
        {
            float xlate_mult = 1;//(j==0) ? 1 : 0;
            float rot_mult = 0.9f*powf(k/8.0f, 3.2f);
            m_xlate[k].x = (FRAND*2-1)*xlate_mult;
            m_xlate[k].y = (FRAND*2-1)*xlate_mult;
            m_xlate[k].z = (FRAND*2-1)*xlate_mult;
            m_rot_base[k].x = FRAND*6.28f;
            m_rot_base[k].y = FRAND*6.28f;
            m_rot_base[k].z = FRAND*6.28f;
            m_rot_speed[k].x = (FRAND*2-1)*rot_mult;
            m_rot_speed[k].y = (FRAND*2-1)*rot_mult;
            m_rot_speed[k].z = (FRAND*2-1)*rot_mult;
            k++;
        }
    }
    while (k < sizeof(m_xlate)/sizeof(m_xlate[0]));
}

CBlendableFloat::CBlendableFloat()
{
	m_bBlending  = false;
}

CBlendableFloat::~CBlendableFloat()
{
}

//--------------------------------------------------------------------------------

float CBlendableFloat::eval(float fTime)
{
	if (fTime < 0)
	{
		return val;
	}

	if (m_bBlending && (fTime > m_fBlendStartTime + m_fBlendDuration) || (fTime < m_fBlendStartTime))
	{
		m_bBlending = false;
	}

	if (!m_bBlending)
	{
		return val;
	}
	else
	{
		float mix = (fTime - m_fBlendStartTime) / m_fBlendDuration;
		return (m_fBlendFrom*(1.0f - mix) + val*mix);
	}
}

//--------------------------------------------------------------------------------

void CBlendableFloat::StartBlendFrom(CBlendableFloat *f_from, float fAnimTime, float fDuration)
{
	if (fDuration < 0.001f)
		return;

	m_fBlendFrom		= f_from->eval(fAnimTime);
	m_bBlending			= true;
	m_fBlendStartTime	= fAnimTime;
	m_fBlendDuration	= fDuration;
}