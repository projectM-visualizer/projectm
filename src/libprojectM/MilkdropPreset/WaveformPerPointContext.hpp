#pragma once

#include "PresetState.hpp"

class CustomWaveform;

class PerFrameContext;

class WaveformPerPointContext
{
public:
    /**
     * @brief Constructor. Creates a new waveform per-point state object.
     * @param gmegabuf The global memory buffer to use in the code context.
     * @param globalRegisters The global variables to use in the code context.
     */
    WaveformPerPointContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    ~WaveformPerPointContext();

    /**
     * @brief Registers the state variables in the expression evaluator context.
     */
    void RegisterBuiltinVariables();

    /**
     * @brief Loads the current read-only state values into the expression evaluator variables.
     * This is done once per frame/waveform for a subset of the per-point variables, specifically
     * those marked as "read-only". All other variables are assigned/reset for each rendered point.
     * @param presetPerFrameContext The preset per-frame context.
     */
    void LoadReadOnlyStateVariables(const PerFrameContext& presetPerFrameContext);

    /**
     * @brief Compiles the per-point code and stores the code handle in the class.
     * @param perPointCode The code to compile.
     * @param waveform The waveform this context belongs to.
     */
    void CompilePerPointCode(const std::string& perPointCode, const CustomWaveform& waveform);

    /**
     * @brief Executes the per-point code with the current state.
     */
    void ExecutePerPointCode();

    projectm_eval_context* perPointCodeContext{nullptr}; //!< The code runtime context, holds memory buffers and variables.
    projectm_eval_code* perPointCodeHandle{nullptr}; //!< The compiled waveform per-point code handle.

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
