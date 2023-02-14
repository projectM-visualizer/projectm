#pragma once

#include "PresetState.hpp"

#include <projectm-eval.h>

/**
 * @brief Hold the per-frame state variables and code execution context.
 *
 * This is the first code evaluated on each frame. The init code is also the first code
 * executed after a preset has been loaded. The result of the per-frame code in form of the
 * "q" variables is then passed to other execution contexts, e.g. per-pixel (AKA per-vertex)
 * and custom shapes and waveforms.
 *
 * Since global memory and registers are shared between all contexts, these values don't
 * need to be passed individually.
 */
class PerFrameContext
{
public:
    /**
     * @brief Constructor. Creates a new per-frame state object.
     * @param gmegabuf The global memory buffer to use in the code context.
     * @param globalRegisters The global variables to use in the code context.
     */
    PerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    /**
     * @brief Destructor.
     */
    ~PerFrameContext();

    /**
     * @brief Registers the state variables in the expression evaluator context.
     */
    void RegisterBuiltinVariables();

    /**
     * @brief Loads the current state values into the expression evaluator variables.
     * @param state The preset state container.
     */
    void LoadStateVariables(PresetState& state);

    /**
     * @brief Compiles and runs the preset init code.
     * @throws MilkdropCompileException Thrown if the per-frame init code couldn't be compiled.
     * @param perFrameInitCode The init code.
     */
    void EvaluateInitCode(PresetState& state);

    /**
     * @brief Compiles the per-frame code and stores the code handle in the class.
     * @throws MilkdropCompileException Thrown if the per-frame code couldn't be compiled.
     * @param perFrameCode The code to compile.
     */
    void CompilePerFrameCode(const std::string& perFrameCode);

    /**
     * @brief Executes the per-frame code with the current state.
     */
    void ExecutePerFrameCode();

    projectm_eval_context* perFrameCodeContext{nullptr}; //!< The code runtime context, holds memory buffers and variables.
    projectm_eval_code* perFrameCodeHandle{nullptr}; //!< The compiled per-frame code handle.

    PRJM_EVAL_F* zoom{};
    PRJM_EVAL_F* zoomexp{};
    PRJM_EVAL_F* rot{};
    PRJM_EVAL_F* warp{};
    PRJM_EVAL_F* cx{};
    PRJM_EVAL_F* cy{};
    PRJM_EVAL_F* dx{};
    PRJM_EVAL_F* dy{};
    PRJM_EVAL_F* sx{};
    PRJM_EVAL_F* sy{};
    PRJM_EVAL_F* time{};
    PRJM_EVAL_F* fps{};
    PRJM_EVAL_F* bass{};
    PRJM_EVAL_F* mid{};
    PRJM_EVAL_F* treb{};
    PRJM_EVAL_F* bass_att{};
    PRJM_EVAL_F* mid_att{};
    PRJM_EVAL_F* treb_att{};
    PRJM_EVAL_F* wave_a{};
    PRJM_EVAL_F* wave_r{};
    PRJM_EVAL_F* wave_g{};
    PRJM_EVAL_F* wave_b{};
    PRJM_EVAL_F* wave_x{};
    PRJM_EVAL_F* wave_y{};
    PRJM_EVAL_F* wave_mystery{};
    PRJM_EVAL_F* wave_mode{};
    PRJM_EVAL_F* decay{};
    PRJM_EVAL_F* frame{};
    PRJM_EVAL_F* q_vars[QVarCount]{};
    PRJM_EVAL_F* progress{};
    PRJM_EVAL_F* ob_size{};
    PRJM_EVAL_F* ob_r{};
    PRJM_EVAL_F* ob_g{};
    PRJM_EVAL_F* ob_b{};
    PRJM_EVAL_F* ob_a{};
    PRJM_EVAL_F* ib_size{};
    PRJM_EVAL_F* ib_r{};
    PRJM_EVAL_F* ib_g{};
    PRJM_EVAL_F* ib_b{};
    PRJM_EVAL_F* ib_a{};
    PRJM_EVAL_F* mv_x{};
    PRJM_EVAL_F* mv_y{};
    PRJM_EVAL_F* mv_dx{};
    PRJM_EVAL_F* mv_dy{};
    PRJM_EVAL_F* mv_l{};
    PRJM_EVAL_F* mv_r{};
    PRJM_EVAL_F* mv_g{};
    PRJM_EVAL_F* mv_b{};
    PRJM_EVAL_F* mv_a{};
    PRJM_EVAL_F* echo_zoom{};
    PRJM_EVAL_F* echo_alpha{};
    PRJM_EVAL_F* echo_orient{};
    PRJM_EVAL_F* wave_usedots{};
    PRJM_EVAL_F* wave_thick{};
    PRJM_EVAL_F* wave_additive{};
    PRJM_EVAL_F* wave_brighten{};
    PRJM_EVAL_F* darken_center{};
    PRJM_EVAL_F* gamma{};
    PRJM_EVAL_F* wrap{};
    PRJM_EVAL_F* invert{};
    PRJM_EVAL_F* brighten{};
    PRJM_EVAL_F* darken{};
    PRJM_EVAL_F* solarize{};
    PRJM_EVAL_F* meshx{};
    PRJM_EVAL_F* meshy{};
    PRJM_EVAL_F* pixelsx{};
    PRJM_EVAL_F* pixelsy{};
    PRJM_EVAL_F* aspectx{};
    PRJM_EVAL_F* aspecty{};
    PRJM_EVAL_F* blur1_min{};
    PRJM_EVAL_F* blur2_min{};
    PRJM_EVAL_F* blur3_min{};
    PRJM_EVAL_F* blur1_max{};
    PRJM_EVAL_F* blur2_max{};
    PRJM_EVAL_F* blur3_max{};
    PRJM_EVAL_F* blur1_edge_darken{};

    PRJM_EVAL_F q_values_after_init_code[QVarCount]{};
};
