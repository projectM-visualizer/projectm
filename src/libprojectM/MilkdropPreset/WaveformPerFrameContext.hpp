#pragma once

#include "PresetState.hpp"

namespace libprojectM {
namespace MilkdropPreset {

class PerFrameContext;
class CustomWaveform;

class WaveformPerFrameContext
{
public:

    WaveformPerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    ~WaveformPerFrameContext();


    void RegisterBuiltinVariables();


    void LoadStateVariables(PresetState& state, const PerFrameContext& presetPerFrameContext, CustomWaveform& waveform);


    void EvaluateInitCode(const std::string& perFrameInitCode, CustomWaveform& waveform);


    void CompilePerFrameCode(const std::string& perFrameCode, const CustomWaveform& waveform);


    void ExecutePerFrameCode();

    projectm_eval_context* perFrameCodeContext{nullptr};
    projectm_eval_code* perFrameCodeHandle{nullptr};

    PRJM_EVAL_F* time{};
    PRJM_EVAL_F* fps{};
    PRJM_EVAL_F* frame{};
    PRJM_EVAL_F* progress{};
    PRJM_EVAL_F* q_vars[QVarCount]{};
    PRJM_EVAL_F* t_vars[TVarCount]{};
    PRJM_EVAL_F* bass{};
    PRJM_EVAL_F* mid{};
    PRJM_EVAL_F* treb{};
    PRJM_EVAL_F* bass_att{};
    PRJM_EVAL_F* mid_att{};
    PRJM_EVAL_F* treb_att{};
    PRJM_EVAL_F* r{};
    PRJM_EVAL_F* g{};
    PRJM_EVAL_F* b{};
    PRJM_EVAL_F* a{};
    PRJM_EVAL_F* samples{};
};

}
}
