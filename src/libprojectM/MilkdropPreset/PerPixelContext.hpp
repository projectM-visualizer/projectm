#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <projectm-eval.h>

namespace libprojectM {
namespace MilkdropPreset {

class PerPixelContext
{
public:

    PerPixelContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100]);


    ~PerPixelContext();


    void RegisterBuiltinVariables();


    void LoadStateReadOnlyVariables(PresetState& state, PerFrameContext& perFrameState);


    void LoadPerFrameQVariables(PresetState& state, PerFrameContext& perFrameState);


    void CompilePerPixelCode(const std::string& perPixelCode);


    void ExecutePerPixelCode();

    projectm_eval_context* perPixelCodeContext{nullptr};
    projectm_eval_code* perPixelCodeHandle{nullptr};

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

}
}
