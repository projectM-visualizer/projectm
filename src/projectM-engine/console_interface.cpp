/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include <stdio.h>

#include "projectM.h"
#include "common.h"
#include "fatal.h"
#include "menu.h"
#include "console_interface.h"
#include "Preset.h"
#include "browser.h"
#include "editor.h"
#include "event.h"
#include "BeatDetect.h"

Preset *active_preset;

interface_t current_interface = DEFAULT_INTERFACE;

void refreshConsole() {

  switch (current_interface) {

  case MENU_INTERFACE:
    refreshMenu();
    break;
  case SHELL_INTERFACE:
    break;
  case EDITOR_INTERFACE:
    refreshEditor();
    break;
  case DEFAULT_INTERFACE:
    break;
  case BROWSER_INTERFACE:
    refreshBrowser();
    break;
  default:
    break;
  }
 
}

void projectM::key_handler( projectMEvent event,
                            projectMKeycode keycode, projectMModifier modifier ) {

	switch( event ) {


	case PROJECTM_KEYDOWN:

	  //default_key_handler();
	  switch (current_interface) 
	    {
	      
	    case MENU_INTERFACE:
//	      menu_key_handler(this, event, keycode);
	      break;
	    case SHELL_INTERFACE:
	      //shell_key_handler();
	      break;
	    case EDITOR_INTERFACE:
	      editor_key_handler(event,keycode);
	      break;
	    case BROWSER_INTERFACE:
	      browser_key_handler(event,keycode,modifier);
	      break;
	    case DEFAULT_INTERFACE:
	      default_key_handler(event,keycode);
	      break;
	    default:
	      default_key_handler(event,keycode);
	      break;
	      
	    }
	  break;
	}
}

void projectM::default_key_handler( projectMEvent event, projectMKeycode keycode) {
  

  
	switch( event ) {

	case PROJECTM_KEYDOWN:
	 		    
	  switch( keycode )
	    {
	    case PROJECTM_K_UP:
            beatDetect->beat_sensitivity += 0.25;
			if (beatDetect->beat_sensitivity > 5.0) beatDetect->beat_sensitivity = 5.0;
	      break;
	    case PROJECTM_K_DOWN:
            beatDetect->beat_sensitivity -= 0.25;
			if (beatDetect->beat_sensitivity < 0) beatDetect->beat_sensitivity = 0;
	      break;
	    case PROJECTM_K_F1:
	      showhelp++;
	      showstats=0;
	      showfps=0;
	      break;
	    case PROJECTM_K_F5:
	      if(showhelp%2==0) showfps++;
	      break;
	    case PROJECTM_K_F4:
	       if(showhelp%2==0) showstats++;
	      break;
	    case PROJECTM_K_F3: {
	      showpreset++;
	      printf( "F3 pressed: %d\n", showpreset );
	      break;
	     }
	    case PROJECTM_K_F2:
	      showtitle++;
	      break;
#ifndef MACOS
	    case PROJECTM_K_F9:
#else
        case PROJECTM_K_F8:
#endif
	      studio++;
	      break;

	    case PROJECTM_K_ESCAPE: {
//	        exit( 1 );
	        break;
	      }
	    case PROJECTM_K_f:
	   
	      break; 
	    case PROJECTM_K_a:
	        if (correction) {
	            correction = 0;
	          } else {
	            correction = 1;
	          }
	        break;
	    case PROJECTM_K_b:
	      break;
            case PROJECTM_K_n:
	      if (switchPreset(ALPHA_NEXT, HARD_CUT) < 0) {
		printf("WARNING: Bad preset file, loading idle preset\n");
		switchToIdlePreset();
	      }		
	      break;
	    case PROJECTM_K_r:
	      if (switchPreset(RANDOM_NEXT, HARD_CUT) < 0) {
		printf("WARNING: Bad preset file, loading idle preset\n");
		switchToIdlePreset();
	      }	
	      break;
	    case PROJECTM_K_p:
	      if ((switchPreset(ALPHA_PREVIOUS, HARD_CUT)) < 0){
		printf("WARNING: Bad preset file, loading idle preset\n");
		switchToIdlePreset();
	      }
	      break;
	    case PROJECTM_K_l:
	      if (noSwitch==0)noSwitch=1; else  noSwitch=0;
	      //	      current_interface = BROWSER_INTERFACE;
	      //	      loadBrowser();
	      break;
	    case PROJECTM_K_e:      	      	    
	      current_interface = EDITOR_INTERFACE;
//	      loadEditor(active_preset->per_frame_eqn_string_buffer,(void (*)()) reloadPerFrame, 
//			 80, 24, 140, 60, 0, 0);
	      break;
	    case PROJECTM_K_s:
            studio++;
//	      current_interface = EDITOR_INTERFACE;
//	      loadEditor("[FILE NAME HERE]", (void (*)())savePreset, 
//			 50, 1, 100, 5, 0, .92);
	    case PROJECTM_K_i:
            DWRITE( "PROJECTM_K_i\n" );
	        doIterative = !doIterative;
	        break;
	    case PROJECTM_K_z:      	     	      	     
	      break;
	    case PROJECTM_K_0:      
	      nWaveMode=0;
	      break;
	    case PROJECTM_K_6:
	      nWaveMode=6;
	      break;
	    case PROJECTM_K_7:
	      nWaveMode=7;
	      break;
	    case PROJECTM_K_m:
	      showhelp=0;
	      showstats=0;
	      showfps=0;
	      current_interface = MENU_INTERFACE;
	      showMenu();
	      break;	     
	    case PROJECTM_K_t:
	      break;
	    default:
	      break;
	    }
	}
}
