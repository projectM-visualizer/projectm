#include "UserSprites/MilkdropSprite.hpp"

#include "SpriteShaders.hpp"

#include <Preset.hpp>
#include <PresetFileParser.hpp>

#include <Renderer/ShaderCache.hpp>
#include <Renderer/TextureManager.hpp>

#include <Utils.hpp>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <locale>
#include <sstream>
#include <string>

#define REG_VAR(var) \
    var = projectm_eval_context_register_variable(spriteCodeContext, #var);

namespace libprojectM {
namespace UserSprites {

MilkdropSprite::MilkdropSprite()
{
    RenderItem::Init();
}

void MilkdropSprite::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    // Color (index 1) is passed as a 4-float constant vertex attribute.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, u))); // Texture coordinate
}

void MilkdropSprite::Init(const std::string& spriteData, const Renderer::RenderContext& renderContext)
{
    PresetFileParser parser;
    std::stringstream spriteDataStream(spriteData);
    if (!parser.Read(spriteDataStream))
    {
        throw SpriteException("Error reading sprite data.");
    }

    // Load/compile shader
    auto spriteShader = renderContext.shaderCache->Get("milkdrop_user_sprite");
    if (!spriteShader)
    {
        // ToDo: Better handle this in the shader class to reduce duplicate code.
#ifdef USE_GLES
        // GLES also requires a precision specifier for variables and 3D samplers
        constexpr char versionHeader[] = "#version 300 es\n\nprecision mediump float;\nprecision mediump sampler3D;\n";
#else
        constexpr char versionHeader[] = "#version 330\n\n";
#endif

        spriteShader = std::make_shared<Renderer::Shader>();
        spriteShader->CompileProgram(static_cast<const char*>(versionHeader) + kMilkdropSpriteVertexGlsl330,
                                     static_cast<const char*>(versionHeader) + kMilkdropSpriteFragmentGlsl330);
        renderContext.shaderCache->Insert("milkdrop_user_sprite", spriteShader);
    }

    m_spriteShader = spriteShader;

    // We ignore the color key value here because OpenGL doesn't support those (DirectX has a specific texture
    // render state for this, which replaces the given color value with transparent texels).
    // Since sprites are user-supplied, we can just make blend mode 4 based on the texture's alpha channel,
    // which gives way better results and users can easily convert any image to PNG.

    // Load and compile code
    auto initCode = parser.GetCode("init_");
    m_codeContext.RunInitCode(initCode, renderContext);

    auto perFrameCode = parser.GetCode("code_");
    if (!perFrameCode.empty())
    {
        m_codeContext.perFrameCodeHandle = projectm_eval_code_compile(m_codeContext.spriteCodeContext, perFrameCode.c_str());
        if (!m_codeContext.perFrameCodeHandle)
        {
            int errorLine{};
            int errorColumn{};
            auto* errorMessage = projectm_eval_get_error(m_codeContext.spriteCodeContext, &errorLine, &errorColumn);

            throw SpriteException("Error compiling sprite per-frame code:" + std::string(errorMessage) + " (Line " + std::to_string(errorLine) + ", column " + std::to_string(errorColumn) + ")");
        }
    }

    auto imageName = Utils::ToLower(parser.GetString("img", ""));

    // Store texture as a shared_ptr to make sure TextureManager doesn't delete it.
    std::locale const loc;
    if (imageName.length() >= 6 &&
        imageName.substr(0, 4) == "rand" && std::isdigit(imageName.at(4), loc) && std::isdigit(imageName.at(5), loc))
    {
        m_texture = renderContext.textureManager->GetRandomTexture(imageName).Texture();
    }
    else
    {
        m_texture = renderContext.textureManager->GetTexture(imageName).Texture();
    }
}

void MilkdropSprite::Draw(const Audio::FrameAudioData& audioData,
                          const Renderer::RenderContext& renderContext,
                          uint32_t outputFramebufferObject,
                          PresetList presets)
{
    auto spriteShader = m_spriteShader.lock();
    assert(spriteShader.get());

    m_codeContext.RunPerFrameCode(audioData, renderContext);

    m_spriteDone = *m_codeContext.done != 0.0;
    bool burnIn = *m_codeContext.burn != 0.0;

    Quad vertices{};

    // Get values from expression code and clamp them where necessary.
    float x = std::min(1000.0f, std::max(-1000.0f, static_cast<float>(*m_codeContext.x) * 2.0f - 1.0f));
    float y = std::min(1000.0f, std::max(-1000.0f, static_cast<float>(*m_codeContext.y) * 2.0f - 1.0f));
    float sx = std::min(1000.0f, std::max(-1000.0f, static_cast<float>(*m_codeContext.sx)));
    float sy = std::min(1000.0f, std::max(-1000.0f, static_cast<float>(*m_codeContext.sy)));
    float rot = static_cast<float>(*m_codeContext.rot);
    int flipx = (*m_codeContext.flipx == 0.0) ? 0 : 1; // Comparing float to 0.0 isn't actually a good idea...
    int flipy = (*m_codeContext.flipy == 0.0) ? 0 : 1;
    float repeatx = std::min(100.0f, std::max(0.01f, static_cast<float>(*m_codeContext.repeatx)));
    float repeaty = std::min(100.0f, std::max(0.01f, static_cast<float>(*m_codeContext.repeaty)));

    int blendMode = std::min(4, std::max(0, (static_cast<int>(*m_codeContext.blendmode))));
    float r = std::min(1.0f, std::max(0.0f, (static_cast<float>(*m_codeContext.r))));
    float g = std::min(1.0f, std::max(0.0f, (static_cast<float>(*m_codeContext.g))));
    float b = std::min(1.0f, std::max(0.0f, (static_cast<float>(*m_codeContext.b))));
    float a = std::min(1.0f, std::max(0.0f, (static_cast<float>(*m_codeContext.a))));

    // ToDo: Move all translations to vertex shader
    vertices[0 + flipx].x = -sx;
    vertices[1 - flipx].x = sx;
    vertices[2 + flipx].x = -sx;
    vertices[3 - flipx].x = sx;
    vertices[0 + flipy * 2].y = -sy;
    vertices[1 + flipy * 2].y = -sy;
    vertices[2 - flipy * 2].y = sy;
    vertices[3 - flipy * 2].y = sy;

    // First aspect ratio: adjust for non-1:1 images
    {
        auto aspect = m_texture->Height() / static_cast<float>(m_texture->Width());

        if (aspect < 1.0f)
        {
            // Landscape image
            for (auto& vertex : vertices)
            {
                vertex.y *= aspect;
            }
        }
        else
        {
            // Portrait image
            for (auto& vertex : vertices)
            {
                vertex.x /= aspect;
            }
        }
    }

    // 2D rotation
    {
        auto cos_rot = std::cos(rot);
        auto sin_rot = std::sin(rot);

        for (auto& vertex : vertices)
        {
            float rotX = vertex.x * cos_rot - vertex.y * sin_rot;
            float rotY = vertex.x * sin_rot + vertex.y * cos_rot;
            vertex.x = rotX;
            vertex.y = rotY;
        }
    }

    // Translation
    for (auto& vertex : vertices)
    {
        vertex.x += x;
        vertex.y += y;
    }

    // Second aspect ratio: normalize to width of screen
    {
        float aspect = renderContext.viewportSizeX / static_cast<float>(renderContext.viewportSizeY);

        if (aspect > 1.0)
        {
            for (auto& vertex : vertices)
            {
                vertex.y *= aspect;
            }
        }
        else
        {
            for (auto& vertex : vertices)
            {
                vertex.x /= aspect;
            }
        }
    }

    // Third aspect ratio: adjust for burn-in
    // -> Not required in projectM, as we always render at viewport size, not a fixed 4:3 ratio

    // Set u,v coords
    {
        float dtu = 0.5f;
        float dtv = 0.5f;

        vertices[0].u = -dtu;
        vertices[1].u = dtu;
        vertices[2].u = -dtu;
        vertices[3].u = dtu;
        vertices[0].v = dtv;
        vertices[1].v = dtv;
        vertices[2].v = -dtv;
        vertices[3].v = -dtv;

        for (auto& vertex : vertices)
        {
            vertex.u = (vertex.u - 0.0f) * repeatx + 0.5f;
            vertex.v = (vertex.v - 0.0f) * repeaty + 0.5f;
        }
    }

    spriteShader->Bind();

    spriteShader->SetUniformInt("blend_mode", blendMode);
    spriteShader->SetUniformInt("texture_sampler", 0);

    m_texture->Bind(0);
    m_sampler.Bind(0);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttrib4f(1, r, g, b, a);

    switch (blendMode)
    {
        case 0:
        default:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case 1:
            glDisable(GL_BLEND);
            break;
        case 2:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case 3:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
            break;
        case 4:
            // Milkdrop actually changed color keying to using texture alpha. The color key is ignored.
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }

    // Draw to current output buffer
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (burnIn)
    {
        // Also draw into all active preset main textures for next-frame burn-in effect
        for (const auto preset : presets)
        {
            if (!preset.get())
            {
                continue;
            }

            preset.get()->BindFramebuffer();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        // Reset to original FBO
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(outputFramebufferObject));
    }

    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    m_texture->Unbind(0);
    Renderer::Shader::Unbind();
}

auto MilkdropSprite::Done() const -> bool
{
    return m_spriteDone;
}

MilkdropSprite::CodeContext::CodeContext()
    : spriteCodeContext(projectm_eval_context_create(nullptr, nullptr))
{
}

MilkdropSprite::CodeContext::~CodeContext()
{
    projectm_eval_context_destroy(spriteCodeContext);

    spriteCodeContext = nullptr;
    perFrameCodeHandle = nullptr;
}

void MilkdropSprite::CodeContext::RegisterBuiltinVariables()
{
    projectm_eval_context_reset_variables(spriteCodeContext);

    // Input variables
    REG_VAR(time);
    REG_VAR(frame);
    REG_VAR(fps);
    REG_VAR(progress);
    REG_VAR(bass);
    REG_VAR(mid);
    REG_VAR(treb);
    REG_VAR(bass_att);
    REG_VAR(mid_att);
    REG_VAR(treb_att);

    // Output variables
    REG_VAR(done);
    REG_VAR(burn);
    REG_VAR(x);
    REG_VAR(y);
    REG_VAR(sx);
    REG_VAR(sy);
    REG_VAR(rot);
    REG_VAR(flipx);
    REG_VAR(flipy);
    REG_VAR(repeatx);
    REG_VAR(repeaty);
    REG_VAR(blendmode);
    REG_VAR(r);
    REG_VAR(g);
    REG_VAR(b);
    REG_VAR(a);
}

void MilkdropSprite::CodeContext::RunInitCode(const std::string& initCode, const Renderer::RenderContext& renderContext)
{
    RegisterBuiltinVariables();

    // Set default values of output variables:
    // (by not setting these every frame, we allow the values to persist from frame-to-frame.)
    *x = 0.5;
    *y = 0.5;
    *sx = 1.0;
    *sy = 1.0;
    *repeatx = 1.0;
    *repeaty = 1.0;
    *rot = 0.0;
    *flipx = 0.0;
    *flipy = 0.0;
    *r = 1.0;
    *g = 1.0;
    *b = 1.0;
    *a = 1.0;
    *blendmode = 0.0;
    *done = 0.0;
    *burn = 1.0;

    if (initCode.empty())
    {
        return;
    }

    // Only time and frame values are passed to the init code.
    *time = renderContext.time;
    *frame = renderContext.frame;

    auto* initCodeHandle = projectm_eval_code_compile(spriteCodeContext, initCode.c_str());
    if (initCodeHandle == nullptr)
    {
        int errorLine{};
        int errorColumn{};
        const auto* errorMessage = projectm_eval_get_error(spriteCodeContext, &errorLine, &errorColumn);

        throw SpriteException("Error compiling sprite init code:" + std::string(errorMessage) + " (Line " + std::to_string(errorLine) + ", column " + std::to_string(errorColumn) + ")");
    }

    projectm_eval_code_execute(initCodeHandle);
    projectm_eval_code_destroy(initCodeHandle);
}

void MilkdropSprite::CodeContext::RunPerFrameCode(const Audio::FrameAudioData& audioData, const Renderer::RenderContext& renderContext)
{
    // If there's no per-frame code, e.g. with static sprites, just skip it.
    if (perFrameCodeHandle == nullptr)
    {
        return;
    }

    // Fill in input variables
    *time = renderContext.time;
    *frame = renderContext.frame;
    *fps = renderContext.fps;
    *progress = renderContext.blendProgress;
    *bass = audioData.bass;
    *mid = audioData.mid;
    *treb = audioData.treb;
    *bass_att = audioData.bassAtt;
    *mid_att = audioData.midAtt;
    *treb_att = audioData.trebAtt;

    // Run the code
    projectm_eval_code_execute(perFrameCodeHandle);
}

} // namespace UserSprites
} // namespace libprojectM
