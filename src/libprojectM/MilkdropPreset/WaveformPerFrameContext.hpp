#pragma once

#include "PresetState.hpp"

class PerFrameContext;

class CustomWaveform;

class WaveformPerFrameContext
{
public:
    /**
     * @brief Constructor. Creates a new per-frame state object.
     * @param gmegabuf The global memory buffer to use in the code context.
     * @param globalRegisters The global variables to use in the code context.
     */
    WaveformPerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    ~WaveformPerFrameContext();

    /**
     * @brief Registers the state variables in the expression evaluator context.
     */
    void RegisterBuiltinVariables();

    /**
     * @brief Loads the current state values into the expression evaluator variables.
     * @param state The preset state container.
     * @param presetPerFrameContext The preset per-frame context (for q vars).
     * @param waveform The waveform this context belongs to.
     */
    void LoadStateVariables(PresetState& state, const PerFrameContext& presetPerFrameContext, CustomWaveform& waveform);

    /**
     * @brief Compiles and runs the preset init code.
     * @throws MilkdropCompileException Thrown if the custom wave init code couldn't be compiled.
     * @param perFrameInitCode The init code.
     * @param waveform The waveform this context belongs to.
     */
    void EvaluateInitCode(const std::string& perFrameInitCode, CustomWaveform& waveform);

    /**
     * @brief Compiles the per-frame code and stores the code handle in the class.
     * @throws MilkdropCompileException Thrown if the custom wave per-frame code couldn't be compiled.
     * @param perFrameCode The code to compile.
     * @param waveform The waveform this context belongs to.
     */
    void CompilePerFrameCode(const std::string& perFrameCode, const CustomWaveform& waveform);

    /**
     * @brief Executes the per-frame code with the current state.
     */
    void ExecutePerFrameCode();

    projectm_eval_context* perFrameCodeContext{nullptr}; //!< The code runtime context, holds memory buffers and variables.
    projectm_eval_code* perFrameCodeHandle{nullptr}; //!< The compiled per-frame code handle.

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
