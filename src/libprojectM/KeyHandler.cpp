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

#include "Common.hpp"
#include "fatal.h"
#include "KeyHandler.hpp"
#include "event.h"
#include "BeatDetect.hpp"
#include "PresetChooser.hpp"
#include "Renderer.hpp"
#include "ProjectM.hpp"

#include <iostream>
#include "TimeKeeper.hpp"

/** Interface types */
typedef enum
{
    MENU_INTERFACE,
    SHELL_INTERFACE,
    EDITOR_INTERFACE,
    DEFAULT_INTERFACE,
    BROWSER_INTERFACE
} interface_t;


class Preset;
interface_t current_interface = DEFAULT_INTERFACE;

void selectRandom(const bool hardCut);
void selectNext(const bool hardCut);
void selectPrevious(const bool hardCut);

std::string round_float(float number)
{
    std::string num_text = std::to_string(number);
    std::string rounded = num_text.substr(0, num_text.find(".")+3);
	return rounded;
}

void refreshConsole() {

  switch (current_interface) {

  case MENU_INTERFACE:
    // unimplemented
    break;
  case SHELL_INTERFACE:
    // unimplemented
    break;
  case EDITOR_INTERFACE:
    // unimplemented
    break;
  case DEFAULT_INTERFACE:
    break;
  case BROWSER_INTERFACE:
    // unimplemented
    break;
  default:
    break;
  }

}

void ProjectM::KeyHandler(projectMEvent event,
                          projectMKeycode keyCode, projectMModifier modifier ) {

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
//	      editor_key_handler(event,keycode);
	      break;
	    case BROWSER_INTERFACE:
//	      browser_key_handler(event,keycode,modifier);
	      break;
	    case DEFAULT_INTERFACE:
            DefaultKeyHandler(event, keyCode);
	      break;
	    default:
            DefaultKeyHandler(event, keyCode);
	      break;

	    }
	  break;
	default:
		break;

	}
}

void ProjectM::DefaultKeyHandler(projectMEvent event, projectMKeycode keyCode) {

	switch( event ) {

	case PROJECTM_KEYDOWN:

		switch (keyCode)
		{
		case PROJECTM_K_HOME:
			if (m_renderer->showmenu) {
				if (!IsTextInputActive()) {
                    ShowPreset(0);  // jump to top of presets.
				} 
				else {
                    m_renderer->m_activePresetID = 1; // jump to top of search results.
                    ShowPresetByName(m_renderer->m_presetList[0].name, true);
				}
			}
			break;
		case PROJECTM_K_END:
			if (m_renderer->showmenu) {
				if (!IsTextInputActive()) {
                    ShowPreset(m_presetLoader->size() - 1);  // jump to bottom of presets.
				}
				else {
                    m_renderer->m_activePresetID = m_renderer->m_presetList.size();  // jump to top of search results.
                    ShowPresetByName(m_renderer->m_presetList[m_renderer->m_activePresetID - 1].name, true);
				}
			}
			break;
		case PROJECTM_K_PAGEUP:
			if (IsTextInputActive()) break; // don't handle this key if search menu is up.
			if (m_renderer->showmenu) {
				int upPreset = m_presetPos->lastIndex() - (m_renderer->textMenuPageSize / 2.0f); // jump up by page size / 2
				if (upPreset < 0) // handle lower boundary
					upPreset = m_presetLoader->size() - 1;
                ShowPreset(upPreset);  // jump up menu half a page.
			}
			break;
		case PROJECTM_K_PAGEDOWN:
			if (IsTextInputActive()) break; // don't handle this key if search menu is up.
			if (m_renderer->showmenu) {
				int downPreset = m_presetPos->lastIndex() + (m_renderer->textMenuPageSize / 2.0f); // jump down by page size / 2
				if (static_cast<std::size_t>(downPreset) >= (m_presetLoader->size() - 1)) // handle upper boundary
					downPreset = 0;
                ShowPreset(downPreset); // jump down menu half a page.
			}
			break;
		case PROJECTM_K_UP:
			if (m_renderer->showmenu) {
                ShowPreviousPreset(true);
			}
			else {
                m_beatDetect->beatSensitivity += 0.01;
				if (m_beatDetect->beatSensitivity > 5.0) m_beatDetect->beatSensitivity = 5.0;
                m_renderer->setToastMessage("Beat Sensitivity: " + round_float(m_beatDetect->beatSensitivity));
			}
			break;
		case PROJECTM_K_DOWN:
			if (m_renderer->showmenu) {
                ShowNextPreset(true);
			}
			else {
                m_beatDetect->beatSensitivity -= 0.01;
				if (m_beatDetect->beatSensitivity < 0) m_beatDetect->beatSensitivity = 0;
                m_renderer->setToastMessage("Beat Sensitivity: " + round_float(m_beatDetect->beatSensitivity));
			}
			break;
		case PROJECTM_K_h:
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showhelp = !m_renderer->showhelp;
                m_renderer->showstats = false;
                m_renderer->showmenu = false;
			break;
		case PROJECTM_K_F1:
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showhelp = !m_renderer->showhelp;
                m_renderer->showstats = false;
                m_renderer->showmenu = false;
			break;
		case PROJECTM_K_y:
			if (IsTextInputActive(true)) break; // disable when searching.
                this->SetShuffleEnabled(!this->GetShuffleEnabled());
			if (this->GetShuffleEnabled()) {
                m_renderer->setToastMessage("Shuffle Enabled");
			}
			else {
                m_renderer->setToastMessage("Shuffle Disabled");
			}
			break;
		case PROJECTM_K_F5:
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showfps = !m_renderer->showfps;
			// Initialize counters and reset frame count.
			m_renderer->lastTimeFPS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                m_renderer->currentTimeFPS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                m_renderer->totalframes = 0;
			// Hide preset name from screen and replace it with FPS counter.
			if (m_renderer->showfps)
			{
                m_renderer->showpreset = false;
			}
			break;
		case PROJECTM_K_F4:
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showstats = !m_renderer->showstats;
			if (m_renderer->showstats) {
                m_renderer->showhelp = false;
                m_renderer->showmenu = false;
			}
			break;
		case PROJECTM_K_F3: {
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showpreset = !m_renderer->showpreset;
			// Hide FPS from screen and replace it with preset name.
			if (m_renderer->showpreset)
			{
                m_renderer->showsearch = false;
                m_renderer->showfps = false;
			}
			break;
		}
		case PROJECTM_K_F2:
            m_renderer->showtitle = !m_renderer->showtitle;
			break;
#ifndef MACOS
		case PROJECTM_K_F9:
#else
		case PROJECTM_K_F8:
#endif

                m_renderer->studio = !m_renderer->studio;
			break;

		case PROJECTM_K_ESCAPE: {
            m_renderer->showsearch = false; // hide input menu
            SetShuffleEnabled(m_renderer->shuffletrack); // restore shuffle
			m_renderer->showmenu = false; // hide input
			break;
		}
		case PROJECTM_K_f:

			break;
		case PROJECTM_K_a:
            m_renderer->correction = !m_renderer->correction;
			break;
		case PROJECTM_K_b:
			break;
		case PROJECTM_K_H:
		case PROJECTM_K_m:
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showmenu = !m_renderer->showmenu;
			if (m_renderer->showmenu) {
                m_renderer->showfps = false;
                m_renderer->showhelp = false;
                m_renderer->showstats = false;
                PopulatePresetMenu();
			}
			break;
		case PROJECTM_K_M:
			if (IsTextInputActive(true)) break; // disable when searching.
			m_renderer->showmenu = !m_renderer->showmenu;
			if (m_renderer->showmenu)
			{
                m_renderer->showhelp = false;
                m_renderer->showstats = false;
                PopulatePresetMenu();
			}
			break;
      case PROJECTM_K_n:
	  	    if (IsTextInputActive(true)) break; // disable when searching.
                ShowNextPreset(true);
            break;
      case PROJECTM_K_N:
		    if (IsTextInputActive(true)) break; // disable when searching.
                ShowNextPreset(false);
            break;
	    case PROJECTM_K_r:
		    if (IsTextInputActive(true)) break; // disable when searching.
                ShowRandomPreset(true);
		    break;
	    case PROJECTM_K_R:
		    if (IsTextInputActive(true)) break; // disable when searching.
                ShowRandomPreset(false);
		    break;
	    case PROJECTM_K_p:
            ShowPreviousPreset(true);
	        break;
	    case PROJECTM_K_P:
	    case PROJECTM_K_BACKSPACE:
            ShowPreviousPreset(false);
	        break;
	    case PROJECTM_K_l:
		    if (IsTextInputActive(true)) break; // disable when searching.
                SetPresetLock(!GetPresetLock());
		    break;
	    case PROJECTM_K_s:
            m_renderer->studio = !m_renderer->studio;
	    case PROJECTM_K_i:
	        break;
		case PROJECTM_K_d:	// d stands for write DEBUG output.
			m_renderer->writeNextFrameToFile = true;
			break;
	    case PROJECTM_K_RETURN:
            m_renderer->toggleSearchText();
			if (m_renderer->showsearch) {
                m_renderer->shuffletrack = this->GetShuffleEnabled(); // track previous shuffle state.
                SetShuffleEnabled(false); // disable shuffle
				m_renderer->showhelp = false;
                m_renderer->showstats = false;
                m_renderer->showtitle = false;
                m_renderer->showpreset = false;
                m_renderer->showmenu = true;
                PopulatePresetMenu();
			} else {
                SetShuffleEnabled(m_renderer->shuffletrack); // restore shuffle
				m_renderer->showmenu = false;
			}
			break;
	    case PROJECTM_K_0:
//	        nWaveMode=0;
	        break;
	    case PROJECTM_K_6:
//	        nWaveMode=6;
	        break;
	    case PROJECTM_K_7:
//	        nWaveMode=7;
	        break;
	    case PROJECTM_K_t:
	        break;
	    case PROJECTM_K_EQUALS:
	    case PROJECTM_K_PLUS:

	    	unsigned int index;

	    	if (GetSelectedPresetIndex(index)) {

	    		const int oldRating = GetPresetRating(index, HARD_CUT_RATING_TYPE);

	    		if (oldRating >= 6)
	    			  break;

	    		const int rating = oldRating + 1;

                SetPresetRating(index, rating, HARD_CUT_RATING_TYPE);
	    	}

	    	break;

	    case PROJECTM_K_MINUS:
	    	if (GetSelectedPresetIndex(index)) {

	    		const int oldRating = GetPresetRating(index, HARD_CUT_RATING_TYPE);

	    		if (oldRating <= 1)
	    			  break;

	    		const int rating = oldRating - 1;

                SetPresetRating(index, rating, HARD_CUT_RATING_TYPE);
	    	}
	    	break;

	    default:
	      break;
	    }
	default:
		break;

	}
}
