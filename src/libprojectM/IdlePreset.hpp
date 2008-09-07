#ifndef IDLE_PRESET_HPP
#define IDLE_PRESET_HPP
#include <memory>
#include "PresetFrameIO.hpp"
#include "Preset.hpp"

/// A preset that does not depend on the file system to be loaded. This allows projectM to render
/// something (ie. self indulgent project advertisting) even when no valid preset directory is found.
class IdlePresetFactory {

  public:
	/// Allocate a new idle preset instance 
	/// \returns a newly allocated auto pointer of an idle preset instance
	std::auto_ptr<Preset> allocate();
  private:
	static std::string presetText();
	static const std::string IDLE_PRESET_NAME;
};
#endif
