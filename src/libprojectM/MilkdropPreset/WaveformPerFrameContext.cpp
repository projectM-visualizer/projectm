#include "WaveformPerFrameContext.hpp"

#include "CustomWaveform.hpp"
#include "MilkdropPresetExceptions.hpp"
#include "PerFrameContext.hpp"

#define REG_VAR(var) \
    var = projectm_eval_context_register_variable(perFrameCodeContext, #var);

WaveformPerFrameContext::WaveformPerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100])
    : perFrameCodeContext(projectm_eval_context_create(gmegabuf, globalRegisters))
{
}

WaveformPerFrameContext::~WaveformPerFrameContext()
{
    if (perFrameCodeHandle != nullptr)
    {
        projectm_eval_code_destroy(perFrameCodeHandle);
    }

    if (perFrameCodeContext != nullptr)
    {
        projectm_eval_context_destroy(perFrameCodeContext);
    }
}

void WaveformPerFrameContext::RegisterBuiltinVariables()
{
    projectm_eval_context_reset_variables(perFrameCodeContext);

    REG_VAR(time);
    REG_VAR(fps);
    REG_VAR(frame);
    REG_VAR(progress);

    for (int q = 0; q < QVarCount; q++)
    {
        std::string qvar = "q" + std::to_string(q + 1);
        q_vars[q] = projectm_eval_context_register_variable(perFrameCodeContext, qvar.c_str());
    }

    for (int t = 0; t < TVarCount; t++)
    {
        std::string tvar = "t" + std::to_string(t + 1);
        q_vars[t] = projectm_eval_context_register_variable(perFrameCodeContext, tvar.c_str());
    }

    REG_VAR(bass);
    REG_VAR(mid);
    REG_VAR(treb);
    REG_VAR(bass_att);
    REG_VAR(mid_att);
    REG_VAR(treb_att);
    REG_VAR(r);
    REG_VAR(g);
    REG_VAR(b);
    REG_VAR(a);
    REG_VAR(samples);
}

void WaveformPerFrameContext::LoadStateVariables(PresetState& state, const PerFrameContext& presetPerFrameContext, CustomWaveform& waveform)
{
    *time = static_cast<double>(state.presetTime);
    *frame = static_cast<double>(state.frame);
    *fps = static_cast<double>(state.fps);
    *progress = static_cast<double>(state.progress);
    *bass = static_cast<double>(state.audioData.bass);
    *mid = static_cast<double>(state.audioData.mid);
    *treb = static_cast<double>(state.audioData.treb);
    *bass_att = static_cast<double>(state.audioData.bassAtt);
    *mid_att = static_cast<double>(state.audioData.midAtt);
    *treb_att = static_cast<double>(state.audioData.trebAtt);

    for (int q = 0; q < QVarCount; q++)
    {
        *q_vars[q] = *presetPerFrameContext.q_vars[q];
    }

    for (int t = 0; t < TVarCount; t++)
    {
        *t_vars[t] = waveform.m_tValuesAfterInitCode[t];
    }

    *r = static_cast<double>(waveform.m_r);
    *g = static_cast<double>(waveform.m_g);
    *b = static_cast<double>(waveform.m_b);
    *a = static_cast<double>(waveform.m_a);
    *samples = static_cast<double>(waveform.m_samples);
}

void WaveformPerFrameContext::EvaluateInitCode(const std::string& perFrameInitCode,
                                               CustomWaveform& waveform)
{
    if (perFrameInitCode.empty())
    {
        return;
    }

    auto* initCode = projectm_eval_code_compile(perFrameCodeContext, perFrameInitCode.c_str());
    if (initCode == nullptr)
    {
        throw MilkdropCompileException("Could not compile custom wave " + std::to_string(waveform.m_index) + " per-frame init code");
    }

    projectm_eval_code_execute(initCode);
    projectm_eval_code_destroy(initCode);
}

void WaveformPerFrameContext::CompilePerFrameCode(const std::string& perFrameCode,
                                                  const CustomWaveform& waveform)
{
    if (perFrameCode.empty())
    {
        return;
    }

    perFrameCodeHandle = projectm_eval_code_compile(perFrameCodeContext, perFrameCode.c_str());
    if (perFrameCodeHandle == nullptr)
    {
        throw MilkdropCompileException("Could not compile custom wave " + std::to_string(waveform.m_index) + " per-frame code");
    }
}

void WaveformPerFrameContext::ExecutePerFrameCode()
{
    if (perFrameCodeHandle != nullptr)
    {
        projectm_eval_code_execute(perFrameCodeHandle);
    }
}
