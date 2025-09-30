#include "MotionVectors.hpp"

#include "MilkdropStaticShaders.hpp"

#include <Renderer/ShaderCache.hpp>
#include <Renderer/TextureManager.hpp>

#include <algorithm>

namespace libprojectM {
namespace MilkdropPreset {

MotionVectors::MotionVectors(PresetState& presetState)
    : m_presetState(presetState)
    , m_motionVectorMesh(Renderer::VertexBufferUsage::StreamDraw)
{
    m_motionVectorMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::Lines);
}

void MotionVectors::Draw(const PerFrameContext& presetPerFrameContext, std::shared_ptr<Renderer::Texture> motionTexture)
{
    // Don't draw if invisible.
    if (*presetPerFrameContext.mv_a < 0.0001f)
    {
        return;
    }

    int countX = static_cast<int>(*presetPerFrameContext.mv_x);
    int countY = static_cast<int>(*presetPerFrameContext.mv_y);

    if (countX <= 0 || countY <= 0)
    {
        return;
    }

    float divertX = static_cast<float>(*presetPerFrameContext.mv_x) - static_cast<float>(countX);
    float divertY = static_cast<float>(*presetPerFrameContext.mv_y) - static_cast<float>(countY);

    if (countX > 64)
    {
        countX = 64;
        divertX = 0.0f;
    }
    if (countY > 48)
    {
        countY = 48;
        divertY = 0.0f;
    }

    auto const divertX2 = static_cast<float>(*presetPerFrameContext.mv_dx);
    auto const divertY2 = static_cast<float>(*presetPerFrameContext.mv_dy);

    // Clamp X/Y diversions to 0..1
    divertX = std::min(1.0f, std::max(0.0f, divertX));
    divertY = std::min(1.0f, std::max(0.0f, divertY));

    // Tweaked this a bit to ensure lines are always at least a bit more than 1px long.
    // Line smoothing makes some of them disappear otherwise.
    float const inverseWidth = 1.25f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
    float const inverseHeight = 1.25f / static_cast<float>(m_presetState.renderContext.viewportSizeY);
    float const minimumLength = sqrtf(inverseWidth * inverseWidth + inverseHeight * inverseHeight);

    m_motionVectorMesh.SetVertexCount(static_cast<std::size_t>(countX + 1) * 2); // countX + 1 lines for each grid row, 2 vertices each.

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shader = GetShader();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    shader->SetUniformFloat("length_multiplier", static_cast<float>(*presetPerFrameContext.mv_l));
    shader->SetUniformFloat("minimum_length", minimumLength);

    shader->SetUniformInt("warp_coordinates", 0);

    motionTexture->Bind(0, m_sampler);

    glVertexAttrib4f(1,
                     static_cast<float>(*presetPerFrameContext.mv_r),
                     static_cast<float>(*presetPerFrameContext.mv_g),
                     static_cast<float>(*presetPerFrameContext.mv_b),
                     static_cast<float>(*presetPerFrameContext.mv_a));

    glLineWidth(1);
#ifndef USE_GLES
    glEnable(GL_LINE_SMOOTH);
#endif

    auto& lineVertices = m_motionVectorMesh.Vertices();

    for (int y = 0; y < countY; y++)
    {
        float const posY = (static_cast<float>(y) + 0.25f) / (static_cast<float>(countY) + divertY + 0.25f - 1.0f) - divertY2;

        if (posY > 0.0001f && posY < 0.9999f)
        {
            int vertex = 0;
            for (int x = 0; x < countX; x++)
            {
                float const posX = (static_cast<float>(x) + 0.25f) / (static_cast<float>(countX) + divertX + 0.25f - 1.0f) + divertX2;

                if (posX > 0.0001f && posX < 0.9999f)
                {
                    lineVertices[vertex + 1] = lineVertices[vertex] = {posX, posY};

                    vertex += 2;
                }
            }

            // Draw a row of lines.
            m_motionVectorMesh.Update();
            m_motionVectorMesh.Draw();
        }
    }

    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();

#ifndef USE_GLES
    glDisable(GL_LINE_SMOOTH);
#endif

    glDisable(GL_BLEND);
}

std::shared_ptr<Renderer::Shader> MotionVectors::GetShader()
{
    auto shader = m_motionVectorShader.lock();
    if (!shader)
    {
        shader = m_presetState.renderContext.shaderCache->Get("milkdrop_motion_vectors");
    }
    if (!shader)
    {
        // First use, compile and cache.
        auto staticShaders = libprojectM::MilkdropPreset::MilkdropStaticShaders::Get();

        shader = std::make_shared<Renderer::Shader>();
        shader->CompileProgram(staticShaders->GetPresetMotionVectorsVertexShader(),
                               staticShaders->GetUntexturedDrawFragmentShader());

        m_presetState.renderContext.shaderCache->Insert("milkdrop_motion_vectors", shader);
    }

    m_motionVectorShader = shader;

    return shader;
}

} // namespace MilkdropPreset
} // namespace libprojectM
