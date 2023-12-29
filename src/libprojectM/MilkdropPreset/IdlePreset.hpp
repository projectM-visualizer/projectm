#pragma once

#include <memory>
#include <string>

namespace libprojectM {

class Preset;

namespace MilkdropPreset {

/**
 * A preset that does not depend on the file system to be loaded. This allows projectM to render
 * something (ie. self indulgent project advertising) even when no valid preset directory is found.
 */
class IdlePresets
{

public:
    /// Allocate a new idle preset instance
    /// \returns a newly allocated auto pointer of an idle preset instance

    /**
     * @brief Allocate a new idle preset instance.
     * @return A newly allocated auto pointer of an idle preset instance
     */
    static std::unique_ptr<::libprojectM::Preset> allocate();

private:
    static std::string presetText();
};

} // namespace MilkdropPreset
} // namespace libprojectM
