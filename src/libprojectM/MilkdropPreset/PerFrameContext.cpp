#include "PerFrameContext.hpp"

#include "MilkdropPresetExceptions.hpp"

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif

#define REG_VAR(var) \
    var = projectm_eval_context_register_variable(perFrameCodeContext, #var);

PerFrameContext::PerFrameContext(projectm_eval_mem_buffer gmegabuf, PRJM_EVAL_F (*globalRegisters)[100])
    : perFrameCodeContext(projectm_eval_context_create(gmegabuf, globalRegisters))
{
}

PerFrameContext::~PerFrameContext()
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

void PerFrameContext::RegisterBuiltinVariables()
{
    projectm_eval_context_reset_variables(perFrameCodeContext);

    REG_VAR(zoom);
    REG_VAR(zoomexp);
    REG_VAR(rot);
    REG_VAR(warp);
    REG_VAR(cx);
    REG_VAR(cy);
    REG_VAR(dx);
    REG_VAR(dy);
    REG_VAR(sx);
    REG_VAR(sy);
    REG_VAR(time);
    REG_VAR(fps);
    REG_VAR(bass);
    REG_VAR(mid);
    REG_VAR(treb);
    REG_VAR(bass_att);
    REG_VAR(mid_att);
    REG_VAR(treb_att);
    REG_VAR(frame);
    REG_VAR(decay);
    REG_VAR(wave_a);
    REG_VAR(wave_r);
    REG_VAR(wave_g);
    REG_VAR(wave_b);
    REG_VAR(wave_x);
    REG_VAR(wave_y);
    REG_VAR(wave_mystery);
    REG_VAR(wave_mode);
    for (int q = 0; q < QVarCount; q++)
    {
        std::string qvar = "q" + std::to_string(q + 1);
        q_vars[q] = projectm_eval_context_register_variable(perFrameCodeContext, qvar.c_str());
    }
    REG_VAR(progress);
    REG_VAR(ob_size);
    REG_VAR(ob_r);
    REG_VAR(ob_g);
    REG_VAR(ob_b);
    REG_VAR(ob_a);
    REG_VAR(ib_size);
    REG_VAR(ib_r);
    REG_VAR(ib_g);
    REG_VAR(ib_b);
    REG_VAR(ib_a);
    REG_VAR(mv_x);
    REG_VAR(mv_y);
    REG_VAR(mv_dx);
    REG_VAR(mv_dy);
    REG_VAR(mv_l);
    REG_VAR(mv_r);
    REG_VAR(mv_g);
    REG_VAR(mv_b);
    REG_VAR(mv_a);
    REG_VAR(echo_zoom);
    REG_VAR(echo_alpha);
    REG_VAR(echo_orient);
    REG_VAR(wave_usedots);
    REG_VAR(wave_thick);
    REG_VAR(wave_additive);
    REG_VAR(wave_brighten);
    REG_VAR(darken_center);
    REG_VAR(gamma);
    REG_VAR(wrap);
    REG_VAR(invert);
    REG_VAR(brighten);
    REG_VAR(darken);
    REG_VAR(solarize);
    REG_VAR(meshx);
    REG_VAR(meshy);
    REG_VAR(pixelsx);
    REG_VAR(pixelsy);
    REG_VAR(aspectx);
    REG_VAR(aspecty);
    REG_VAR(blur1_min);
    REG_VAR(blur2_min);
    REG_VAR(blur3_min);
    REG_VAR(blur1_max);
    REG_VAR(blur2_max);
    REG_VAR(blur3_max);
    REG_VAR(blur1_edge_darken);
}

void PerFrameContext::EvaluateInitCode(PresetState& state)
{
    if (state.perFrameInitCode.empty())
    {
        return;
    }

    auto* initCode = projectm_eval_code_compile(perFrameCodeContext, state.perFrameInitCode.c_str());
    if (initCode == nullptr)
    {
#ifdef MILKDROP_PRESET_DEBUG
        int line;
        int col;
        auto* errmsg = projectm_eval_get_error(perFrameCodeContext, &line, &col);
        if (errmsg)
        {
            std::cerr << "[Preset] Could not compile per-frame INIT code: " << errmsg << "(L" << line << " C" << col << ")" << std::endl;
        }
#endif
        throw MilkdropCompileException("Could not compile per-frame init code");
    }

    projectm_eval_code_execute(initCode);
    projectm_eval_code_destroy(initCode);

    for (int q = 0; q < QVarCount; q++)
    {
        q_values_after_init_code[q] = *q_vars[q];
        state.frameQVariables[q] = *q_vars[q];
    }
}

void PerFrameContext::LoadStateVariables(PresetState& state)
{
    *zoom = static_cast<PRJM_EVAL_F>(state.zoom);
    *zoomexp = static_cast<PRJM_EVAL_F>(state.zoomExponent);
    *rot = static_cast<PRJM_EVAL_F>(state.rot);
    *warp = static_cast<PRJM_EVAL_F>(state.warpAmount);
    *cx = static_cast<PRJM_EVAL_F>(state.rotCX);
    *cy = static_cast<PRJM_EVAL_F>(state.rotCY);
    *dx = static_cast<PRJM_EVAL_F>(state.xPush);
    *dy = static_cast<PRJM_EVAL_F>(state.yPush);
    *sx = static_cast<PRJM_EVAL_F>(state.stretchX);
    *sy = static_cast<PRJM_EVAL_F>(state.stretchY);
    *time = static_cast<PRJM_EVAL_F>(state.renderContext.time);
    *fps = static_cast<PRJM_EVAL_F>(state.renderContext.fps);
    *bass = static_cast<PRJM_EVAL_F>(state.audioData.bass);
    *mid = static_cast<PRJM_EVAL_F>(state.audioData.mid);
    *treb = static_cast<PRJM_EVAL_F>(state.audioData.treb);
    *bass_att = static_cast<PRJM_EVAL_F>(state.audioData.bassAtt);
    *mid_att = static_cast<PRJM_EVAL_F>(state.audioData.midAtt);
    *treb_att = static_cast<PRJM_EVAL_F>(state.audioData.trebAtt);
    *frame = static_cast<PRJM_EVAL_F>(state.renderContext.frame);
    for (int q = 0; q < QVarCount; q++)
    {
        *q_vars[q] = q_values_after_init_code[q];
    }
    *progress = static_cast<PRJM_EVAL_F>(state.progress);
    *decay = static_cast<PRJM_EVAL_F>(state.decay);
    *wave_a = static_cast<PRJM_EVAL_F>(state.waveAlpha);
    *wave_r = static_cast<PRJM_EVAL_F>(state.waveR);
    *wave_g = static_cast<PRJM_EVAL_F>(state.waveG);
    *wave_b = static_cast<PRJM_EVAL_F>(state.waveB);
    *wave_x = static_cast<PRJM_EVAL_F>(state.waveX);
    *wave_y = static_cast<PRJM_EVAL_F>(state.waveY);
    *wave_mystery = static_cast<PRJM_EVAL_F>(state.waveParam);
    *wave_mode = static_cast<PRJM_EVAL_F>(state.waveMode);
    *ob_size = static_cast<PRJM_EVAL_F>(state.outerBorderSize);
    *ob_r = static_cast<PRJM_EVAL_F>(state.outerBorderR);
    *ob_g = static_cast<PRJM_EVAL_F>(state.outerBorderG);
    *ob_b = static_cast<PRJM_EVAL_F>(state.outerBorderB);
    *ob_a = static_cast<PRJM_EVAL_F>(state.outerBorderA);
    *ib_size = static_cast<PRJM_EVAL_F>(state.innerBorderSize);
    *ib_r = static_cast<PRJM_EVAL_F>(state.innerBorderR);
    *ib_g = static_cast<PRJM_EVAL_F>(state.innerBorderG);
    *ib_b = static_cast<PRJM_EVAL_F>(state.innerBorderB);
    *ib_a = static_cast<PRJM_EVAL_F>(state.innerBorderA);
    *mv_x = static_cast<PRJM_EVAL_F>(state.mvX);
    *mv_y = static_cast<PRJM_EVAL_F>(state.mvY);
    *mv_dx = static_cast<PRJM_EVAL_F>(state.mvDX);
    *mv_dy = static_cast<PRJM_EVAL_F>(state.mvDY);
    *mv_l = static_cast<PRJM_EVAL_F>(state.mvL);
    *mv_r = static_cast<PRJM_EVAL_F>(state.mvR);
    *mv_g = static_cast<PRJM_EVAL_F>(state.mvG);
    *mv_b = static_cast<PRJM_EVAL_F>(state.mvB);
    *mv_a = static_cast<PRJM_EVAL_F>(state.mvA);
    *echo_zoom = static_cast<PRJM_EVAL_F>(state.videoEchoZoom);
    *echo_alpha = static_cast<PRJM_EVAL_F>(state.videoEchoAlpha);
    *echo_orient = static_cast<PRJM_EVAL_F>(state.videoEchoOrientation);
    *wave_usedots = static_cast<PRJM_EVAL_F>(state.waveDots);
    *wave_thick = static_cast<PRJM_EVAL_F>(state.waveThick);
    *wave_additive = static_cast<PRJM_EVAL_F>(state.additiveWaves);
    *wave_brighten = static_cast<PRJM_EVAL_F>(state.maximizeWaveColor);
    *darken_center = static_cast<PRJM_EVAL_F>(state.darkenCenter);
    *gamma = static_cast<PRJM_EVAL_F>(state.gammaAdj);
    *wrap = static_cast<PRJM_EVAL_F>(state.texWrap);
    *invert = static_cast<PRJM_EVAL_F>(state.invert);
    *brighten = static_cast<PRJM_EVAL_F>(state.brighten);
    *darken = static_cast<PRJM_EVAL_F>(state.darken);
    *solarize = static_cast<PRJM_EVAL_F>(state.solarize);
    *meshx = static_cast<PRJM_EVAL_F>(state.renderContext.perPixelMeshX);
    *meshy = static_cast<PRJM_EVAL_F>(state.renderContext.perPixelMeshY);
    *pixelsx = static_cast<PRJM_EVAL_F>(state.renderContext.viewportSizeX);
    *pixelsy = static_cast<PRJM_EVAL_F>(state.renderContext.viewportSizeY);
    *aspectx = static_cast<PRJM_EVAL_F>(state.renderContext.invAspectX);
    *aspecty = static_cast<PRJM_EVAL_F>(state.renderContext.invAspectY);
    *blur1_min = static_cast<PRJM_EVAL_F>(state.blur1Min);
    *blur2_min = static_cast<PRJM_EVAL_F>(state.blur2Min);
    *blur3_min = static_cast<PRJM_EVAL_F>(state.blur3Min);
    *blur1_max = static_cast<PRJM_EVAL_F>(state.blur1Max);
    *blur2_max = static_cast<PRJM_EVAL_F>(state.blur2Max);
    *blur3_max = static_cast<PRJM_EVAL_F>(state.blur3Max);
    *blur1_edge_darken = static_cast<PRJM_EVAL_F>(state.blur1EdgeDarken);
}

void PerFrameContext::CompilePerFrameCode(const std::string& perFrameCode)
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
            std::cerr << "[Preset] Could not compile per-frame code: " << errmsg << "(L" << line << " C" << col << ")" << std::endl;
        }
#endif
        throw MilkdropCompileException("Could not compile per-frame code");
    }
}

void PerFrameContext::ExecutePerFrameCode()
{
    if (perFrameCodeHandle != nullptr)
    {
        projectm_eval_code_execute(perFrameCodeHandle);
    }
}