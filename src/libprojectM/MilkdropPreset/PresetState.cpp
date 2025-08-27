#include "PresetState.hpp"

#include "MilkdropStaticShaders.hpp"
#include "PresetFileParser.hpp"

#include "Renderer/ShaderCache.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <random>

namespace libprojectM {
namespace MilkdropPreset {

const glm::mat4 PresetState::orthogonalProjection = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -40.0f, 40.0f);
const glm::mat4 PresetState::orthogonalProjectionFlipped = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -40.0f, 40.0f);

PresetState::PresetState()
    : globalMemory(projectm_eval_memory_buffer_create())
{
    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::uniform_int_distribution<> distrib(0, std::numeric_limits<int>::max());

    hueRandomOffsets[0] = static_cast<float>(distrib(randomGenerator) % 64841L) * 0.01f;
    hueRandomOffsets[1] = static_cast<float>(distrib(randomGenerator) % 53751L) * 0.01f;
    hueRandomOffsets[2] = static_cast<float>(distrib(randomGenerator) % 42661L) * 0.01f;
    hueRandomOffsets[3] = static_cast<float>(distrib(randomGenerator) % 31571L) * 0.01f;
}

PresetState::~PresetState()
{
    projectm_eval_memory_buffer_destroy(globalMemory);
}

void PresetState::Initialize(::libprojectM::PresetFileParser& parsedFile)
{


    decay = parsedFile.GetFloat("fDecay", decay);
    gammaAdj = parsedFile.GetFloat("fGammaAdj", gammaAdj);
    videoEchoZoom = parsedFile.GetFloat("fVideoEchoZoom", videoEchoZoom);
    videoEchoAlpha = parsedFile.GetFloat("fVideoEchoAlpha", videoEchoAlpha);
    videoEchoOrientation = parsedFile.GetInt("nVideoEchoOrientation", videoEchoOrientation);
    redBlueStereo = parsedFile.GetBool("bRedBlueStereo", redBlueStereo);
    brighten = parsedFile.GetBool("bBrighten", brighten);
    darken = parsedFile.GetBool("bDarken", darken);
    solarize = parsedFile.GetBool("bSolarize", solarize);
    invert = parsedFile.GetBool("bInvert", invert);
    shader = parsedFile.GetFloat("fShader", shader);
    blur1Min = parsedFile.GetFloat("b1n", blur1Min);
    blur2Min = parsedFile.GetFloat("b2n", blur2Min);
    blur3Min = parsedFile.GetFloat("b3n", blur3Min);
    blur1Max = parsedFile.GetFloat("b1x", blur1Max);
    blur2Max = parsedFile.GetFloat("b2x", blur2Max);
    blur3Max = parsedFile.GetFloat("b3x", blur3Max);
    blur1EdgeDarken = parsedFile.GetFloat("b1ed", blur1EdgeDarken);


    waveMode = parsedFile.GetInt("nWaveMode", waveMode);
    additiveWaves = parsedFile.GetBool("bAdditiveWaves", additiveWaves);
    waveDots = parsedFile.GetBool("bWaveDots", waveDots);
    waveThick = parsedFile.GetBool("bWaveThick", waveThick);
    modWaveAlphaByvolume = parsedFile.GetBool("bModWaveAlphaByVolume", modWaveAlphaByvolume);
    maximizeWaveColor = parsedFile.GetBool("bMaximizeWaveColor", maximizeWaveColor);
    waveAlpha = parsedFile.GetFloat("fWaveAlpha", waveAlpha);
    waveScale = parsedFile.GetFloat("fWaveScale", waveScale);
    waveSmoothing = parsedFile.GetFloat("fWaveSmoothing", waveSmoothing);
    waveParam = parsedFile.GetFloat("fWaveParam", waveParam);
    modWaveAlphaStart = parsedFile.GetFloat("fModWaveAlphaStart", modWaveAlphaStart);
    modWaveAlphaEnd = parsedFile.GetFloat("fModWaveAlphaEnd", modWaveAlphaEnd);
    waveR = parsedFile.GetFloat("wave_r", waveR);
    waveG = parsedFile.GetFloat("wave_g", waveG);
    waveB = parsedFile.GetFloat("wave_b", waveB);
    waveX = parsedFile.GetFloat("wave_x", waveX);
    waveY = parsedFile.GetFloat("wave_y", waveY);
    mvX = parsedFile.GetFloat("nMotionVectorsX", mvX);
    mvY = parsedFile.GetFloat("nMotionVectorsY", mvY);
    mvDX = parsedFile.GetFloat("mv_dx", mvDX);
    mvDY = parsedFile.GetFloat("mv_dy", mvDY);
    mvL = parsedFile.GetFloat("mv_l", mvL);
    mvR = parsedFile.GetFloat("mv_r", mvR);
    mvG = parsedFile.GetFloat("mv_g", mvG);
    mvB = parsedFile.GetFloat("mv_b", mvB);
    mvA = parsedFile.GetBool("bMotionVectorsOn", false) ? 1.0f : 0.0f;
    mvA = parsedFile.GetFloat("mv_a", mvA);


    zoom = parsedFile.GetFloat("zoom", zoom);
    rot = parsedFile.GetFloat("rot", rot);
    rotCX = parsedFile.GetFloat("cx", rotCX);
    rotCY = parsedFile.GetFloat("cy", rotCY);
    xPush = parsedFile.GetFloat("dx", xPush);
    yPush = parsedFile.GetFloat("dy", yPush);
    warpAmount = parsedFile.GetFloat("warp", warpAmount);
    stretchX = parsedFile.GetFloat("sx", stretchX);
    stretchY = parsedFile.GetFloat("sy", stretchY);
    texWrap = parsedFile.GetBool("bTexWrap", texWrap);
    darkenCenter = parsedFile.GetBool("bDarkenCenter", darkenCenter);
    warpAnimSpeed = parsedFile.GetFloat("fWarpAnimSpeed", warpAnimSpeed);
    warpScale = parsedFile.GetFloat("fWarpScale", warpScale);
    zoomExponent = parsedFile.GetFloat("fZoomExponent", zoomExponent);


    outerBorderSize = parsedFile.GetFloat("ob_size", outerBorderSize);
    outerBorderR = parsedFile.GetFloat("ob_r", outerBorderR);
    outerBorderG = parsedFile.GetFloat("ob_g", outerBorderG);
    outerBorderB = parsedFile.GetFloat("ob_b", outerBorderB);
    outerBorderA = parsedFile.GetFloat("ob_a", outerBorderA);
    innerBorderSize = parsedFile.GetFloat("ib_size", innerBorderSize);
    innerBorderR = parsedFile.GetFloat("ib_r", innerBorderR);
    innerBorderG = parsedFile.GetFloat("ib_g", innerBorderG);
    innerBorderB = parsedFile.GetFloat("ib_b", innerBorderB);
    innerBorderA = parsedFile.GetFloat("ib_a", innerBorderA);


    presetVersion = parsedFile.GetInt("MILKDROP_PRESET_VERSION", presetVersion);
    if (presetVersion < 200)
    {

        warpShaderVersion = 0;
        compositeShaderVersion = 0;
    }
    else if (presetVersion == 200)
    {

        warpShaderVersion = parsedFile.GetInt("PSVERSION", warpShaderVersion);
        compositeShaderVersion = parsedFile.GetInt("PSVERSION", compositeShaderVersion);
    }
    else
    {
        warpShaderVersion = parsedFile.GetInt("PSVERSION_WARP", warpShaderVersion);
        compositeShaderVersion = parsedFile.GetInt("PSVERSION_COMP", compositeShaderVersion);
    }


    perFrameInitCode = parsedFile.GetCode("per_frame_init_");
    perFrameCode = parsedFile.GetCode("per_frame_");
    perPixelCode = parsedFile.GetCode("per_pixel_");


    for (int i = 0; i < CustomWaveformCount; i++)
    {
        std::string const wavePrefix = "wave_" + std::to_string(i) + "_";
        customWaveInitCode[i] = parsedFile.GetCode(wavePrefix + "init");
        customWavePerFrameCode[i] = parsedFile.GetCode(wavePrefix + "per_frame");
        customWavePerPointCode[i] = parsedFile.GetCode(wavePrefix + "per_point");
    }


    for (int i = 0; i < CustomShapeCount; i++)
    {
        std::string const shapePrefix = "shape_" + std::to_string(i) + "_";
        customShapeInitCode[i] = parsedFile.GetCode(shapePrefix + "init");
        customShapePerFrameCode[i] = parsedFile.GetCode(shapePrefix + "per_frame");
    }


    warpShader = parsedFile.GetCode("warp_");
    compositeShader = parsedFile.GetCode("comp_");
}

void PresetState::LoadShaders()
{
    auto staticShaders = libprojectM::MilkdropPreset::MilkdropStaticShaders::Get();

    auto untexturedShaderShared = renderContext.shaderCache->Get("milkdrop_generic_untextured");
    if (!untexturedShaderShared)
    {
        untexturedShaderShared = std::make_shared<Renderer::Shader>();
        untexturedShaderShared->CompileProgram(staticShaders->GetUntexturedDrawVertexShader(),
                                               staticShaders->GetUntexturedDrawFragmentShader());
        renderContext.shaderCache->Insert("milkdrop_generic_untextured", untexturedShaderShared);
    }
    untexturedShader = untexturedShaderShared;

    auto texturedShaderShared = renderContext.shaderCache->Get("milkdrop_generic_textured");
    if (!texturedShaderShared)
    {
        texturedShaderShared = std::make_shared<Renderer::Shader>();
        texturedShaderShared->CompileProgram(staticShaders->GetTexturedDrawVertexShader(),
                                             staticShaders->GetTexturedDrawFragmentShader());
        renderContext.shaderCache->Insert("milkdrop_generic_textured", texturedShaderShared);
    }
    texturedShader = texturedShaderShared;
}

}
}
