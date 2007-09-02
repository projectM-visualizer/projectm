#ifndef IDLE_PRESET_HPP
#define IDLE_PRESET_HPP
#include <memory>
#include "PresetFrameIO.hpp"
#include "Preset.hpp"

class IdlePreset {

  public:
	/// Allocate a new idle prest instance
	static std::auto_ptr<Preset> allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs);
  private:
	static std::string presetText();
};
#endif
