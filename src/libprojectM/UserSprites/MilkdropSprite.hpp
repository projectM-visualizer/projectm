#pragma once

#include "UserSprites/Sprite.hpp"

#include <Renderer/Texture.hpp>
#include <Renderer/TextureSamplerDescriptor.hpp>

#include <projectm-eval.h>

namespace libprojectM {
namespace UserSprites {

class MilkdropSprite : public Sprite
{
public:
    MilkdropSprite();

    ~MilkdropSprite() override = default;

    void InitVertexAttrib() override;

    void Init(const std::string& spriteData, const Renderer::RenderContext& renderContext) override;

    void Draw(const Audio::FrameAudioData& audioData,
              const Renderer::RenderContext& renderContext,
              uint32_t outputFramebufferObject,
              PresetList presets) override;

    auto Done() const -> bool override;

private:
    using Quad = std::array<TexturedPoint, 4>;


    struct CodeContext {
        CodeContext();

        ~CodeContext();


        void RegisterBuiltinVariables();


        void RunInitCode(const std::string& initCode, const Renderer::RenderContext& renderContext);


        void RunPerFrameCode(const Audio::FrameAudioData& audioData,
                             const Renderer::RenderContext& renderContext);

        projectm_eval_context* spriteCodeContext{nullptr};
        projectm_eval_code* perFrameCodeHandle{nullptr};


        PRJM_EVAL_F* time{};
        PRJM_EVAL_F* frame{};
        PRJM_EVAL_F* fps{};
        PRJM_EVAL_F* progress{};
        PRJM_EVAL_F* bass{};
        PRJM_EVAL_F* mid{};
        PRJM_EVAL_F* treb{};
        PRJM_EVAL_F* bass_att{};
        PRJM_EVAL_F* mid_att{};
        PRJM_EVAL_F* treb_att{};


        PRJM_EVAL_F* done{};
        PRJM_EVAL_F* burn{};
        PRJM_EVAL_F* x{};
        PRJM_EVAL_F* y{};
        PRJM_EVAL_F* sx{};
        PRJM_EVAL_F* sy{};
        PRJM_EVAL_F* rot{};
        PRJM_EVAL_F* flipx{};
        PRJM_EVAL_F* flipy{};
        PRJM_EVAL_F* repeatx{};
        PRJM_EVAL_F* repeaty{};
        PRJM_EVAL_F* blendmode{};
        PRJM_EVAL_F* r{};
        PRJM_EVAL_F* g{};
        PRJM_EVAL_F* b{};
        PRJM_EVAL_F* a{};
    };

    CodeContext m_codeContext;
    std::shared_ptr<Renderer::Texture> m_texture;
    Renderer::Sampler m_sampler{GL_REPEAT, GL_LINEAR};
    std::weak_ptr<Renderer::Shader> m_spriteShader;
    bool m_spriteDone{false};
};

}
}
