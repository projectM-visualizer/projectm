#pragma once

#include <memory>
#include <string>

namespace libprojectM {

class Preset;

namespace MilkdropPreset {


class IdlePresets
{

public:




    static std::unique_ptr<::libprojectM::Preset> allocate();

private:
    static std::string presetText();
};

}
}
