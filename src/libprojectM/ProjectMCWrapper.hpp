

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

}
