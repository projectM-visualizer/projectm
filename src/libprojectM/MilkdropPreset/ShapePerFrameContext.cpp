#include "ShapePerFrameContext.hpp"

#include "CustomShape.hpp"
#include "MilkdropPresetExceptions.hpp"
#include "PerFrameContext.hpp"

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif

#define REG_VAR(var) \
    var = projectm_eval_context_register_variable(perFrameCodeContext, #var);

ShapePerFrameContext::ShapePerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100])
    : perFrameCodeContext(projectm_eval_context_create(gmegabuf, globalRegisters))
{
}

ShapePerFrameContext::~ShapePerFrameContext()
{
    if (perFrameCodeHandle != nullptr)
    {
        projectm_eval_code_destroy(perFrameCodeHandle);
    }

    if (perFrameCodeContext != nullptr)
    {
        projectm_eval_context_destroy(perFrameCodeContext);
    }
}

void ShapePerFrameContext::RegisterBuiltinVariables()
{
    projectm_eval_context_reset_variables(perFrameCodeContext);

    REG_VAR(time);
    REG_VAR(fps);
    REG_VAR(frame);
    REG_VAR(progress);

    for (int q = 0; q < QVarCount; q++)
    {
        std::string qvar = "q" + std::to_string(q + 1);
        q_vars[q] = projectm_eval_context_register_variable(perFrameCodeContext, qvar.c_str());
    }

    for (int t = 0; t < TVarCount; t++)
    {
        std::string tvar = "t" + std::to_string(t + 1);
        t_vars[t] = projectm_eval_context_register_variable(perFrameCodeContext, tvar.c_str());
    }

    REG_VAR(bass);
    REG_VAR(mid);
    REG_VAR(treb);
    REG_VAR(bass_att);
    REG_VAR(mid_att);
    REG_VAR(treb_att);
    REG_VAR(x);
    REG_VAR(y);
    REG_VAR(rad);
    REG_VAR(ang);
    REG_VAR(tex_ang);
    REG_VAR(tex_zoom);
    REG_VAR(sides);
    REG_VAR(textured);
    REG_VAR(instance);
    REG_VAR(num_inst);
    REG_VAR(additive);
    REG_VAR(thick);
    REG_VAR(r);
    REG_VAR(g);
    REG_VAR(b);
    REG_VAR(a);
    REG_VAR(r2);
    REG_VAR(g2);
    REG_VAR(b2);
    REG_VAR(a2);
    REG_VAR(border_r);
    REG_VAR(border_g);
    REG_VAR(border_b);
    REG_VAR(border_a);
}
void ShapePerFrameContext::LoadStateVariables(const PresetState& state,
                                              CustomShape& shape,
                                              int inst)
{
    *time = static_cast<double>(state.renderContext.time);
    *frame = static_cast<double>(state.renderContext.frame);
    *fps = static_cast<double>(state.renderContext.fps);
    *progress = static_cast<double>(state.renderContext.progress);
    *bass = static_cast<double>(state.audioData.bass);
    *mid = static_cast<double>(state.audioData.mid);
    *treb = static_cast<double>(state.audioData.treb);
    *bass_att = static_cast<double>(state.audioData.bassAtt);
    *mid_att = static_cast<double>(state.audioData.midAtt);
    *treb_att = static_cast<double>(state.audioData.trebAtt);

    for (int q = 0; q < QVarCount; q++)
    {
        *q_vars[q] = state.frameQVariables[q];
    }

    for (int t = 0; t < TVarCount; t++)
    {
        *t_vars[t] = shape.m_tValuesAfterInitCode[t];
    }

    *x = static_cast<double>(shape.m_x);
    *y = static_cast<double>(shape.m_y);
    *rad = static_cast<double>(shape.m_radius);
    *ang = static_cast<double>(shape.m_angle);
    *tex_zoom = static_cast<double>(shape.m_tex_zoom);
    *tex_ang = static_cast<double>(shape.m_tex_ang);
    *sides = static_cast<double>(shape.m_sides);
    *additive = static_cast<double>(shape.m_additive);
    *textured = static_cast<double>(shape.m_textured);
    *num_inst = static_cast<double>(shape.m_instances);
    *instance = static_cast<double>(inst);
    *thick = static_cast<double>(shape.m_thickOutline);
    *r = static_cast<double>(shape.m_r);
    *g = static_cast<double>(shape.m_g);
    *b = static_cast<double>(shape.m_b);
    *a = static_cast<double>(shape.m_a);
    *r2 = static_cast<double>(shape.m_r2);
    *g2 = static_cast<double>(shape.m_g2);
    *b2 = static_cast<double>(shape.m_b2);
    *a2 = static_cast<double>(shape.m_a2);
    *border_r = static_cast<double>(shape.m_border_r);
    *border_g = static_cast<double>(shape.m_border_g);
    *border_b = static_cast<double>(shape.m_border_b);
    *border_a = static_cast<double>(shape.m_border_a);
}

void ShapePerFrameContext::EvaluateInitCode(const std::string& perFrameInitCode,
                                            const CustomShape& shape)
{
    if (perFrameInitCode.empty())
    {
        return;
    }

    auto* initCode = projectm_eval_code_compile(perFrameCodeContext, perFrameInitCode.c_str());
    if (initCode == nullptr)
    {
#ifdef MILKDROP_PRESET_DEBUG
        int line;
        int col;
        auto* errmsg = projectm_eval_get_error(perFrameCodeContext, &line, &col);
        if (errmsg)
        {
            std::cerr << "[Preset] Could not compile custom shape " << shape.m_index << " per-frame INIT code: " << errmsg << "(L" << line << " C" << col << ")" << std::endl;
        }
#endif
        throw MilkdropCompileException("Could not compile custom shape " + std::to_string(shape.m_index) + " per-frame init code");
    }

    projectm_eval_code_execute(initCode);
    projectm_eval_code_destroy(initCode);
}

void ShapePerFrameContext::CompilePerFrameCode(const std::string& perFrameCode,
                                               const CustomShape& shape)
{
    if (perFrameCode.empty())
    {
        return;
    }

    perFrameCodeHandle = projectm_eval_code_compile(perFrameCodeContext, perFrameCode.c_str());
    if (perFrameCodeHandle == nullptr)
    {
#ifdef MILKDROP_PRESET_DEBUG
        int line;
        int col;
        auto* errmsg = projectm_eval_get_error(perFrameCodeContext, &line, &col);
        if (errmsg)
        {
            std::cerr << "[Preset] Could not compile custom shape " << shape.m_index << " per-frame code: " << errmsg << "(L" << line << " C" << col << ")" << std::endl;
        }
#endif
        throw MilkdropCompileException("Could not compile custom shape " + std::to_string(shape.m_index) + " per-frame code");
    }
}


void ShapePerFrameContext::ExecutePerFrameCode()
{
    if (perFrameCodeHandle != nullptr)
    {
        projectm_eval_code_execute(perFrameCodeHandle);
    }
}
