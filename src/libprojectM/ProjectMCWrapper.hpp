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

#include "projectM.hpp"
#include "projectM.h"

class projectMWrapper : public projectM
{
public:
    projectMWrapper(std::string configFile, int flags);

    projectMWrapper(Settings settings, int flags);

    void presetSwitchedEvent(bool isHardCut, size_t presetIndex) const override;

    void shuffleEnabledValueChanged(bool shuffle_enabled) const override;

    void presetSwitchFailedEvent(bool isHardCut, unsigned int presetIndex,
                                 const std::string& failureMessage) const override;

    void presetRatingChanged(unsigned int presetIndex, int rating, PresetRatingType ratingType) const override;

    projectm_preset_switched_event _presetSwitchedEventCallback{ nullptr };
    projectm_shuffle_enable_changed_event _shuffleEnableChangedEventCallback{ nullptr };
    projectm_preset_switch_failed_event _presetSwitchFailedEventCallback{ nullptr };
    projectm_preset_rating_changed_event _presetRatingChangedEventCallback{ nullptr };

    void* _presetSwitchedEventUserData{ nullptr };
    void* _shuffleEnableChangedEventUserData{ nullptr };
    void* _presetSwitchFailedEventUserData{ nullptr };
    void* _presetRatingChangedEventUserData{ nullptr };
};
