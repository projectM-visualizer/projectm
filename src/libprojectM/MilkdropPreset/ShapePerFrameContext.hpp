#pragma once

#include "PresetState.hpp"

namespace libprojectM {
namespace MilkdropPreset {

class PerFrameContext;
class CustomShape;


class ShapePerFrameContext
{
public:

    ShapePerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);

    ~ShapePerFrameContext();


    void RegisterBuiltinVariables();


    void LoadStateVariables(const PresetState& state,
                            CustomShape& shape,
                            int inst);


    void EvaluateInitCode(const std::string& perFrameInitCode, const CustomShape& shape);


    void CompilePerFrameCode(const std::string& perFrameCode, const CustomShape& shape);


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

}
}
