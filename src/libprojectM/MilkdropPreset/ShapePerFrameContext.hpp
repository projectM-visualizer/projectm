#pragma once

#include "PresetState.hpp"

class PerFrameContext;

class CustomShape;

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
     * @param inst The current shape isntance.
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
    double* time{};
    double* fps{};
    double* frame{};
    double* progress{};
    double* q_vars[QVarCount]{};
    double* t_vars[TVarCount]{};
    double* bass{};
    double* mid{};
    double* treb{};
    double* bass_att{};
    double* mid_att{};
    double* treb_att{};
    double* r{};
    double* g{};
    double* b{};
    double* a{};
    double* r2{};
    double* g2{};
    double* b2{};
    double* a2{};
    double* border_r{};
    double* border_g{};
    double* border_b{};
    double* border_a{};
    double* x{};
    double* y{};
    double* rad{};
    double* ang{};
    double* sides{};
    double* textured{};
    double* additive{};
    double* thick{};
    double* num_inst{};
    double* instance{};
    double* tex_zoom{};
    double* tex_ang{};
};
