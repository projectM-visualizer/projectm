#pragma once

#include "PresetState.hpp"

namespace libprojectM {
namespace MilkdropPreset {

class CustomWaveform;
class PerFrameContext;

class WaveformPerPointContext
{
public:

    WaveformPerPointContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    ~WaveformPerPointContext();


    void RegisterBuiltinVariables();


    void LoadReadOnlyStateVariables(const PerFrameContext& presetPerFrameContext);


    void CompilePerPointCode(const std::string& perPointCode, const CustomWaveform& waveform);


    void ExecutePerPointCode();

    projectm_eval_context* perPointCodeContext{nullptr};
    projectm_eval_code* perPointCodeHandle{nullptr};

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
    PRJM_EVAL_F* sample{};
    PRJM_EVAL_F* value1{};
    PRJM_EVAL_F* value2{};
    PRJM_EVAL_F* x{};
    PRJM_EVAL_F* y{};
    PRJM_EVAL_F* r{};
    PRJM_EVAL_F* g{};
    PRJM_EVAL_F* b{};
    PRJM_EVAL_F* a{};
};

}
}
