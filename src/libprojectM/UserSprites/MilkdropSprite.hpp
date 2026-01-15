#pragma once

#include "UserSprites/Sprite.hpp"

#include <Renderer/Mesh.hpp>
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

    void Init(const std::string& spriteData, const Renderer::RenderContext& renderContext) override;

    void Draw(const Audio::FrameAudioData& audioData,
              const Renderer::RenderContext& renderContext,
              uint32_t outputFramebufferObject,
              PresetList presets) override;

    auto Done() const -> bool override;

private:
    /**
     * @brief Context for the init and per-frame code.
     */
    struct CodeContext {
        CodeContext();

        ~CodeContext();

        /**
         * @brief Registers the state variables in the expression evaluator context.
         */
        void RegisterBuiltinVariables();

        /**
         * @brief Compiles and runs the init code of the sprite once, if any.
         * Also sets up a the default values of the output variables.
         * @param initCode The initialization code.
         */
        void RunInitCode(const std::string& initCode, const Renderer::RenderContext& renderContext);

        /**
         * @brief Runs the per-frame update code for the sprite.
         * @param audioData The frame audio data structure.
         * @param renderContext The frame rendering context data.
         */
        void RunPerFrameCode(const Audio::FrameAudioData& audioData,
                             const Renderer::RenderContext& renderContext);

        projectm_eval_context* spriteCodeContext{nullptr}; //!< The code runtime context, holds memory buffers and variables.
        projectm_eval_code* perFrameCodeHandle{nullptr};   //!< The compiled per-frame code handle.

        // Input variables
        PRJM_EVAL_F* time{};     //!< Time passed since program start. Also available in init code.
        PRJM_EVAL_F* frame{};    //!< Total frames rendered so far. Also available in init code.
        PRJM_EVAL_F* fps{};      //!< Current (or, if not available, target) frames per second value.
        PRJM_EVAL_F* progress{}; //!< Preset blending progress (only if blending).
        PRJM_EVAL_F* bass{};     //!< Bass frequency loudness, median of 1.0, range of ~0.7 to ~1.3 in most cases.
        PRJM_EVAL_F* mid{};      //!< Middle frequency loudness, median of 1.0, range of ~0.7 to ~1.3 in most cases.
        PRJM_EVAL_F* treb{};     //!< Treble frequency loudness, median of 1.0, range of ~0.7 to ~1.3 in most cases.
        PRJM_EVAL_F* bass_att{}; //!< More attenuated/smoothed value of bass.
        PRJM_EVAL_F* mid_att{};  //!< More attenuated/smoothed value of mid.
        PRJM_EVAL_F* treb_att{}; //!< More attenuated/smoothed value of treb.

        // Output variables
        PRJM_EVAL_F* done{};      //!< If this becomes non-zero, the sprite is deleted. Default: 0.0
        PRJM_EVAL_F* burn{};      //!< If non-zero, the sprite will be "burned" into currently rendered presets when done is also true, effectively "dissolving" the sprite in the preset. Default: 1.0
        PRJM_EVAL_F* x{};         //!< Sprite x position (position of the image center). Range from -1000 to 1000. Default: 0.5
        PRJM_EVAL_F* y{};         //!< Sprite y position (position of the image center). Range from -1000 to 1000. Default: 0.5
        PRJM_EVAL_F* sx{};        //!< Sprite x scaling factor. Range from -1000 to 1000. Default: 1.0
        PRJM_EVAL_F* sy{};        //!< Sprite y scaling factor. Range from -1000 to 1000. Default: 1.0
        PRJM_EVAL_F* rot{};       //!< Sprite rotation in radians (2*PI equals one full rotation). Default: 0.0
        PRJM_EVAL_F* flipx{};     //!< If flag is non-zero, the sprite is flipped on the x axis. Default: 0.0
        PRJM_EVAL_F* flipy{};     //!< If flag is non-zero, the sprite is flipped on the y axis. Default: 0.0
        PRJM_EVAL_F* repeatx{};   //!< Repeat count of the image on the sprite quad on the x axis. Fractional values allowed. Range from 0.01 to 100.0. Default: 1.0
        PRJM_EVAL_F* repeaty{};   //!< Repeat count of the image on the sprite quad on the y axis. Fractional values allowed. Range from 0.01 to 100.0. Default: 1.0
        PRJM_EVAL_F* blendmode{}; //!< Image blending mode. 0 = Alpha blending (default), 1 = Decal mode (no transparency), 2 = Additive blending, 3 = Source color blending, 4 = Color key blending. Default: 0
        PRJM_EVAL_F* r{};         //!< Modulation color used in some blending modes. Default: 1.0
        PRJM_EVAL_F* g{};         //!< Modulation color used in some blending modes. Default: 1.0
        PRJM_EVAL_F* b{};         //!< Modulation color used in some blending modes. Default: 1.0
        PRJM_EVAL_F* a{};         //!< Modulation color used in some blending modes. Default: 1.0
    };

    CodeContext m_codeContext;                         //!< Sprite init and per-frame code.
    std::shared_ptr<Renderer::Texture> m_texture;      //!< The sprite image, loaded via a name like other textures in Milkdrop presets.
    Renderer::Sampler m_sampler{GL_REPEAT, GL_LINEAR}; //!< Texture sampler settings
    std::weak_ptr<Renderer::Shader> m_spriteShader;    //!< The shader used to draw the user sprite.
    Renderer::Mesh m_mesh;                             //!< A simple quad.
    bool m_spriteDone{false};                          //!< If true, the sprite will be removed from the list.
};

} // namespace UserSprites
} // namespace libprojectM
