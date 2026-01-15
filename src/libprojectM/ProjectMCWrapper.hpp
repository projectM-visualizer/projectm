/*
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2021 projectM Team
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

#pragma once

#include "ProjectM.hpp"

#include <projectM-4/projectM.h>

namespace libprojectM {

class projectMWrapper : public ProjectM
{
public:
    void PresetSwitchFailedEvent(const std::string& presetFilename,
                                 const std::string& failureMessage) const override;
    void PresetSwitchRequestedEvent(bool isHardCut) const override;

    projectm_preset_switch_failed_event m_presetSwitchFailedEventCallback{nullptr};
    void* m_presetSwitchFailedEventUserData{nullptr};

    projectm_preset_switch_requested_event m_presetSwitchRequestedEventCallback{nullptr};
    void* m_presetSwitchRequestedEventUserData{nullptr};
};

} // namespace libprojectM
