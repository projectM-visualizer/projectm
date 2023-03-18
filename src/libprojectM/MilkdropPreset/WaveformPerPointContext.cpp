#include "WaveformPerPointContext.hpp"

#include "CustomWaveform.hpp"
#include "MilkdropPresetExceptions.hpp"
#include "PerFrameContext.hpp"

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif

#define REG_VAR(var) \
    var = projectm_eval_context_register_variable(perPointCodeContext, #var);

WaveformPerPointContext::WaveformPerPointContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100])
    : perPointCodeContext(projectm_eval_context_create(gmegabuf, globalRegisters))
{
}

WaveformPerPointContext::~WaveformPerPointContext()
{
    if (perPointCodeHandle != nullptr)
    {
        projectm_eval_code_destroy(perPointCodeHandle);
    }

    if (perPointCodeContext != nullptr)
    {
        projectm_eval_context_destroy(perPointCodeContext);
    }
}

void WaveformPerPointContext::RegisterBuiltinVariables()
{
    projectm_eval_context_reset_variables(perPointCodeContext);

    REG_VAR(time);
    REG_VAR(fps);
    REG_VAR(frame);
    REG_VAR(progress);

    for (int q = 0; q < QVarCount; q++)
    {
        std::string const qvar = "q" + std::to_string(q + 1);
        q_vars[q] = projectm_eval_context_register_variable(perPointCodeContext, qvar.c_str());
    }

    for (int t = 0; t < TVarCount; t++)
    {
        std::string const tvar = "t" + std::to_string(t + 1);
        t_vars[t] = projectm_eval_context_register_variable(perPointCodeContext, tvar.c_str());
    }

    REG_VAR(bass);
    REG_VAR(mid);
    REG_VAR(treb);
    REG_VAR(bass_att);
    REG_VAR(mid_att);
    REG_VAR(treb_att);
    REG_VAR(sample);
    REG_VAR(value1);
    REG_VAR(value2);
    REG_VAR(x);
    REG_VAR(y);
    REG_VAR(r);
    REG_VAR(g);
    REG_VAR(b);
    REG_VAR(a);
}

void WaveformPerPointContext::LoadReadOnlyStateVariables(const PerFrameContext& presetPerFrameContext)
{
    *time = *presetPerFrameContext.time;
    *frame = *presetPerFrameContext.frame;
    *fps = *presetPerFrameContext.fps;
    *progress = *presetPerFrameContext.progress;
    *bass = *presetPerFrameContext.bass;
    *mid = *presetPerFrameContext.mid;
    *treb = *presetPerFrameContext.treb;
    *bass_att = *presetPerFrameContext.bass_att;
    *mid_att = *presetPerFrameContext.mid_att;
    *treb_att = *presetPerFrameContext.treb_att;
}

void WaveformPerPointContext::CompilePerPointCode(const std::string& perPointCode,
                                                  const CustomWaveform& waveform)
{
    if (perPointCode.empty())
    {
        return;
    }

    perPointCodeHandle = projectm_eval_code_compile(perPointCodeContext, perPointCode.c_str());
    if (perPointCodeHandle == nullptr)
    {
#ifdef MILKDROP_PRESET_DEBUG
        int line;
        int col;
        auto* errmsg = projectm_eval_get_error(perPointCodeContext, &line, &col);
        if (errmsg)
        {
            std::cerr << "[Preset] Could not compile custom wave " << waveform.m_index << " per-point code: " << errmsg << "(L" << line << " C" << col << ")" << std::endl;
        }
#endif
        throw MilkdropCompileException("Could not compile custom wave " + std::to_string(waveform.m_index) + " per-point code");
    }
}

void WaveformPerPointContext::ExecutePerPointCode()
{
    if (perPointCodeHandle != nullptr)
    {
        projectm_eval_code_execute(perPointCodeHandle);
    }
}