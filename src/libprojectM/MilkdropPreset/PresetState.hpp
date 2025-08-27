
#pragma once

#include "Constants.hpp"

#include "BlurTexture.hpp"

#include <Audio/FrameAudioData.hpp>

#include <Renderer/RenderContext.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/TextureSamplerDescriptor.hpp>

#include <projectm-eval.h>

#include <string>

namespace libprojectM {

class PresetFileParser;

namespace MilkdropPreset {


using BlendableFloat = float;


class PresetState
{
public:
    PresetState();

    ~PresetState();


    void Initialize(::libprojectM::PresetFileParser& parsedFile);


    void LoadShaders();

    BlendableFloat gammaAdj{2.0f};
    BlendableFloat videoEchoZoom{2.0f};
    BlendableFloat videoEchoAlpha{0.0f};
    BlendableFloat videoEchoAlphaOld{0.0f};
    int videoEchoOrientation{0};
    int videoEchoOrientationOld{0};

    BlendableFloat decay{0.98f};

    int waveMode{0};
    int oldWaveMode{-1};
    bool additiveWaves{false};
    BlendableFloat waveAlpha{0.8f};
    BlendableFloat waveScale{1.0f};
    BlendableFloat waveSmoothing{0.75f};
    bool waveDots{false};
    bool waveThick{false};
    BlendableFloat waveParam{0.0f};
    bool modWaveAlphaByvolume{false};
    BlendableFloat modWaveAlphaStart{0.75f};
    BlendableFloat modWaveAlphaEnd{0.95f};
    float warpAnimSpeed{1.0f};
    BlendableFloat warpScale{1.0f};
    BlendableFloat zoomExponent{1.0f};
    BlendableFloat shader{0.0f};
    bool maximizeWaveColor{true};
    bool texWrap{true};
    bool darkenCenter{false};
    bool redBlueStereo{false};
    bool brighten{false};
    bool darken{false};
    bool solarize{false};
    bool invert{false};

    BlendableFloat zoom{1.0f};
    BlendableFloat rot{0.0f};
    BlendableFloat rotCX{0.5f};
    BlendableFloat rotCY{0.5f};
    BlendableFloat xPush{0.0f};
    BlendableFloat yPush{0.0f};
    BlendableFloat warpAmount{1.0f};
    BlendableFloat stretchX{1.0f};
    BlendableFloat stretchY{1.0f};
    BlendableFloat waveR{1.0f};
    BlendableFloat waveG{1.0f};
    BlendableFloat waveB{1.0f};
    BlendableFloat waveX{0.5f};
    BlendableFloat waveY{0.5f};
    BlendableFloat outerBorderSize{0.01f};
    BlendableFloat outerBorderR{0.0f};
    BlendableFloat outerBorderG{0.0f};
    BlendableFloat outerBorderB{0.0f};
    BlendableFloat outerBorderA{0.0f};
    BlendableFloat innerBorderSize{0.01f};
    BlendableFloat innerBorderR{0.25f};
    BlendableFloat innerBorderG{0.25f};
    BlendableFloat innerBorderB{0.25f};
    BlendableFloat innerBorderA{0.0f};
    BlendableFloat mvX{12.0f};
    BlendableFloat mvY{9.0f};
    BlendableFloat mvDX{0.0f};
    BlendableFloat mvDY{0.0f};
    BlendableFloat mvL{0.9f};
    BlendableFloat mvR{1.0f};
    BlendableFloat mvG{1.0f};
    BlendableFloat mvB{1.0f};
    BlendableFloat mvA{0.0f};
    BlendableFloat blur1Min{0.0f};
    BlendableFloat blur2Min{0.0f};
    BlendableFloat blur3Min{0.0f};
    BlendableFloat blur1Max{1.0f};
    BlendableFloat blur2Max{1.0f};
    BlendableFloat blur3Max{1.0f};
    BlendableFloat blur1EdgeDarken{0.25f};

    int presetVersion{100};
    int warpShaderVersion{2};
    int compositeShaderVersion{2};

    std::array<float, 4> hueRandomOffsets;

    projectm_eval_mem_buffer globalMemory{nullptr};
    double globalRegisters[100]{};
    std::array<double, QVarCount> frameQVariables{};

    libprojectM::Audio::FrameAudioData audioData;
    Renderer::RenderContext renderContext;

    std::string perFrameInitCode;
    std::string perFrameCode;
    std::string perPixelCode;

    std::array<std::string, CustomWaveformCount> customWaveInitCode;
    std::array<std::string, CustomWaveformCount> customWavePerFrameCode;
    std::array<std::string, CustomWaveformCount> customWavePerPointCode;

    std::array<std::string, CustomShapeCount> customShapeInitCode;
    std::array<std::string, CustomShapeCount> customShapePerFrameCode;

    std::string warpShader;
    std::string compositeShader;

    std::weak_ptr<Renderer::Shader> untexturedShader;
    std::weak_ptr<Renderer::Shader> texturedShader;

    std::weak_ptr<Renderer::Texture> mainTexture;
    BlurTexture blurTexture;

    std::map<int, Renderer::TextureSamplerDescriptor> randomTextureDescriptors;

    static const glm::mat4 orthogonalProjection;
    static const glm::mat4 orthogonalProjectionFlipped;
};

}
}
