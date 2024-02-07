#pragma once

#include "PresetState.hpp"

namespace libprojectM {
namespace MilkdropPreset {

class PerFrameContext;
class CustomShape;

/**
 * @class ShapePerFrameContext
 * @brief Contains the per-frame execution context and code for shapes. 
 **/
class ShapePerFrameContext
{
public:
    /**
     * @brief Constructor. Creates a new per-frame state object.
     * @param gmegabuf The global memory buffer to use in the code context.
     * @param globalRegisters The global variables to use in the code context.
     */
    ShapePerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    ~ShapePerFrameContext();

    /**
     * @brief Registers the state variables in the expression evaluator context.
     */
    void RegisterBuiltinVariables();

    /**
     * @brief Loads the current state values into the expression evaluator variables.
     * @param state The preset state container.
     * @param shape The shape this context belongs to.
     * @param inst The current shape instance.
     */
    void LoadStateVariables(const PresetState& state,
                            CustomShape& shape,
                            int inst);

    /**
     * @brief Compiles and runs the preset init code.
     * @throws MilkdropCompileException Thrown if one of the custom shape init code couldn't be compiled.
     * @param perFrameInitCode The init code.
     * @param shape The shape this context belongs to.
     */
    void EvaluateInitCode(const std::string& perFrameInitCode, const CustomShape& shape);

    /**
     * @brief Compiles the per-frame code and stores the code handle in the class.
     * @throws MilkdropCompileException Thrown if one of the per-frame code couldn't be compiled.
     * @param perFrameCode The code to compile.
     * @param shape The shape this context belongs to.
     */
    void CompilePerFrameCode(const std::string& perFrameCode, const CustomShape& shape);

    /**
     * @brief Executes the per-frame code with the current state.
     */
    void ExecutePerFrameCode();

    projectm_eval_context* perFrameCodeContext{nullptr}; //!< The code runtime context, holds memory buffers and variables.
    projectm_eval_code* perFrameCodeHandle{nullptr};     //!< The compiled per-frame code handle.

    // Expression variable pointers.
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
    PRJM_EVAL_F* r2{};
    PRJM_EVAL_F* g2{};
    PRJM_EVAL_F* b2{};
    PRJM_EVAL_F* a2{};
    PRJM_EVAL_F* border_r{};
    PRJM_EVAL_F* border_g{};
    PRJM_EVAL_F* border_b{};
    PRJM_EVAL_F* border_a{};
    PRJM_EVAL_F* x{};
    PRJM_EVAL_F* y{};
    PRJM_EVAL_F* rad{};
    PRJM_EVAL_F* ang{};
    PRJM_EVAL_F* sides{};
    PRJM_EVAL_F* textured{};
    PRJM_EVAL_F* additive{};
    PRJM_EVAL_F* thick{};
    PRJM_EVAL_F* num_inst{};
    PRJM_EVAL_F* instance{};
    PRJM_EVAL_F* tex_zoom{};
    PRJM_EVAL_F* tex_ang{};
};

} // namespace MilkdropPreset
} // namespace libprojectM
