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

#include "ProjectM.hpp"
#include "Renderer.hpp"
#include "TimeKeeper.hpp"
#include "libprojectM/event.h"

#include <iostream>


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

		case PROJECTM_K_a:
            m_renderer->correction = !m_renderer->correction;
			break;

	    case PROJECTM_K_l:
            SetPresetLocked(!PresetLocked());
		    break;

	    default:
	      break;
	    }
	default:
		break;

	}
}
