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

#include "fatal.h"
#include "KeyHandler.hpp"
#include "event.h"
#include "BeatDetect.hpp"
#include "PresetChooser.hpp"
#include "Renderer.hpp"
#include "ProjectM.hpp"

#include <iostream>
#include "TimeKeeper.hpp"


class Preset;
void selectRandom(const bool hardCut);
void selectNext(const bool hardCut);
void selectPrevious(const bool hardCut);

std::string round_float(float number)
{
    std::string num_text = std::to_string(number);
    std::string rounded = num_text.substr(0, num_text.find(".")+3);
	return rounded;
}

void ProjectM::KeyHandler(projectMEvent event,
                          projectMKeycode keyCode,
                          projectMModifier modifier)
{
    switch (event)
    {
        case PROJECTM_KEYDOWN:
            DefaultKeyHandler(event, keyCode);
            break;

        default:
            break;
    }
}

void ProjectM::DefaultKeyHandler(projectMEvent event, projectMKeycode keycode) {

	switch( event ) {

	case PROJECTM_KEYDOWN:

		switch (keycode)
		{

		case PROJECTM_K_UP:
            m_beatDetect->beatSensitivity += 0.01;
            if (m_beatDetect->beatSensitivity > 5.0)
            {
                m_beatDetect->beatSensitivity = 5.0;
            }
			break;
		case PROJECTM_K_DOWN:
            m_beatDetect->beatSensitivity -= 0.01;
            if (m_beatDetect->beatSensitivity < 0)
            {
                m_beatDetect->beatSensitivity = 0;
            }
			break;
		case PROJECTM_K_y:
            this->SetShuffleEnabled(!this->ShuffleEnabled());
			break;
		case PROJECTM_K_a:
            m_renderer->correction = !m_renderer->correction;
			break;
      case PROJECTM_K_n:
            SelectNext(true);
            break;
      case PROJECTM_K_N:
            SelectNext(false);
            break;
	    case PROJECTM_K_r:
            SelectRandom(true);
		    break;
	    case PROJECTM_K_R:
            SelectRandom(false);
		    break;
	    case PROJECTM_K_p:
            SelectPrevious(true);
	        break;
	    case PROJECTM_K_P:
	    case PROJECTM_K_BACKSPACE:
            SelectPrevious(false);
	        break;
	    case PROJECTM_K_l:
            SetPresetLocked(!PresetLocked());
		    break;
	    case PROJECTM_K_i:
	        break;
		case PROJECTM_K_d:	// d stands for write DEBUG output.
            m_renderer->writeNextFrameToFile = true;
			break;
	    case PROJECTM_K_EQUALS:
	    case PROJECTM_K_PLUS:

	    	unsigned int index;

	    	if (SelectedPresetIndex(index)) {

	    		const int oldRating = PresetRating(index, HARD_CUT_RATING_TYPE);

	    		if (oldRating >= 6)
	    			  break;

	    		const int rating = oldRating + 1;

                ChangePresetRating(index, rating, HARD_CUT_RATING_TYPE);
	    	}

	    	break;

	    case PROJECTM_K_MINUS:
	    	if (SelectedPresetIndex(index)) {

	    		const int oldRating = PresetRating(index, HARD_CUT_RATING_TYPE);

	    		if (oldRating <= 1)
	    			  break;

	    		const int rating = oldRating - 1;

                ChangePresetRating(index, rating, HARD_CUT_RATING_TYPE);
	    	}
	    	break;

	    default:
	      break;
	    }
	default:
		break;

	}
}
