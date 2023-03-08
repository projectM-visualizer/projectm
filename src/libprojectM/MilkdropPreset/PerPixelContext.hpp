#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <projectm-eval.h>

class PerPixelContext
{
public:
    /**
     * @brief Constructor. Creates a new per-frame state object.
     * @param gmegabuf The global memory buffer to use in the code context.
     * @param globalRegisters The global variables to use in the code context.
     */
    PerPixelContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    /**
     * @brief Destructor.
     */
    ~PerPixelContext();

    /**
     * @brief Registers the state variables in the expression evaluator context.
     */
    void RegisterBuiltinVariables();

    /**
     * @brief Loads the current state read-only values into the expression evaluator variables.
     *
     * All non-RO variables are loaded once per vertex. There's no real read-only flag, which means
     * preset authors may use this fact to change these values from vertex to vertex. Even if this
     * is an undocumented feature, we should do the same as some presets may depend on it.
     *
     * @param state The preset state container.
     */
    void LoadStateReadOnlyVariables(PresetState& state, PerFrameContext& perFrameState);

    /**
     * @brief Copies the current per-frame Q variable values into the per-pixel state.
     * @param state The preset state container.
     * @param perFrameState The per-frame execution context.
     */
    void LoadPerFrameQVariables(PresetState& state, PerFrameContext& perFrameState);

    /**
     * @brief Compiles the per-pixel code and stores the code handle in the class.
     * @throws MilkdropCompileException Thrown if the per-pixel code couldn't be compiled.
     * @param perPixelCode The code to compile.
     */
    void CompilePerPixelCode(const std::string& perPixelCode);

    /**
     * @brief Executes the per-pixel code with the current state.
     */
    void ExecutePerPixelCode();

    projectm_eval_context* perPixelCodeContext{nullptr}; //!< The code runtime context, holds memory buffers and variables.
    projectm_eval_code* perPixelCodeHandle{nullptr};     //!< The compiled per-pixel code handle.

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
    PRJM_EVAL_F* x{};
    PRJM_EVAL_F* y{};
    PRJM_EVAL_F* rad{};
    PRJM_EVAL_F* ang{};
    PRJM_EVAL_F* frame{};
    PRJM_EVAL_F* q_vars[QVarCount]{};
    PRJM_EVAL_F* progress{};
    PRJM_EVAL_F* meshx{};
    PRJM_EVAL_F* meshy{};
    PRJM_EVAL_F* pixelsx{};
    PRJM_EVAL_F* pixelsy{};
    PRJM_EVAL_F* aspectx{};
    PRJM_EVAL_F* aspecty{};
};
