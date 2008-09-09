/*
 * Preset.hpp
 *
 *  Created on: Aug 5, 2008
 *      Author: carm
 */

#ifndef __COMPILED_PRESET_HPP_
#define __COMPILED_PRESET_HPP_

#include <string>

#include "BeatDetect.hpp"
#include "Pipeline.hpp"
#include "PipelineContext.hpp"


/// A templated preset class to build different various hard coded presets and 
/// compile them into object files to be loaded into a playlist
template <class PipelineT>
class CompiledPreset : public Preset {
public:

	inline CompiledPreset(const std::string & name=std::string(), 
		const std::string & author = std::string()) : Preset(name, author) {}

	virtual ~CompiledPreset() {}

	inline const PipelineT & pipeline() const { return _pipeline; }
	inline virtual void Render(const BeatDetect &music, const PipelineContext &context) {
		_pipeline.Render(music, context);
	}

private:
	PipelineT _pipeline;
};

#endif /* PRESET_HPP_ */
