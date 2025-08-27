#pragma once

#include "PresetState.hpp"

#include <projectm-eval.h>

namespace libprojectM {
namespace MilkdropPreset {


class PerFrameContext
{
public:

    PerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);


    ~PerFrameContext();


    void RegisterBuiltinVariables();


    void LoadStateVariables(PresetState& state);


    void EvaluateInitCode(PresetState& state);


    void CompilePerFrameCode(const std::string& perFrameCode);


    void ExecutePerFrameCode();

    projectm_eval_context* perFrameCodeContext{nullptr};
    projectm_eval_code* perFrameCodeHandle{nullptr};

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

}
}
