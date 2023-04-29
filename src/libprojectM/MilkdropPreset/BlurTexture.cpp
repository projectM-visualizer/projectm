#include "BlurTexture.hpp"

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "MilkdropStaticShaders.hpp"

#include <array>

BlurTexture::BlurTexture()
    : m_blurSampler(std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR))
{
    auto staticShaders = libprojectM::MilkdropPreset::MilkdropStaticShaders::Get();

    // Compile shader sources
    m_blur1Shader.CompileProgram(staticShaders->GetBlurVertexShader(),
                                 staticShaders->GetBlur1FragmentShader());
    m_blur2Shader.CompileProgram(staticShaders->GetBlurVertexShader(),
                                 staticShaders->GetBlur2FragmentShader());

    m_blurFramebuffer.CreateColorAttachment(0, 0);

    // Initialize Blur VAO/VBO with a single fullscreen quad.
    static constexpr std::array<float, 16> pointsBlur{
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0};

    glGenBuffers(1, &m_vboBlur);
    glGenVertexArrays(1, &m_vaoBlur);

    glBindVertexArray(m_vaoBlur);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboBlur);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointsBlur.size(), pointsBlur.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr); // Position at index 0 and 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(sizeof(float) * 2)); // Texture coord at index 2 and 3

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialize with empty textures.
    for (size_t i = 0; i < m_blurTextures.size(); i++)
    {
        std::string textureName;
        if (i % 2 == 1)
        {
            textureName = "blur" + std::to_string(i / 2 + 1);
        }

        m_blurTextures[i] = std::make_shared<Texture>(textureName, 0, GL_TEXTURE_2D, 0, 0, false);
    }
}

BlurTexture::~BlurTexture()
{
    glDeleteBuffers(1, &m_vboBlur);
    glDeleteVertexArrays(1, &m_vaoBlur);
}

void BlurTexture::SetRequiredBlurLevel(BlurTexture::BlurLevel level)
{
    m_blurLevel = std::max(level, m_blurLevel);
}

auto BlurTexture::GetDescriptorsForBlurLevel(BlurTexture::BlurLevel blurLevel) const -> std::vector<TextureSamplerDescriptor>
{
    std::vector<TextureSamplerDescriptor> descriptors;

    if (blurLevel == BlurLevel::Blur3)
    {
        descriptors.emplace_back(m_blurTextures[1], m_blurSampler, m_blurTextures[1]->Name(), std::string());
        descriptors.emplace_back(m_blurTextures[3], m_blurSampler, m_blurTextures[3]->Name(), std::string());
        descriptors.emplace_back(m_blurTextures[5], m_blurSampler, m_blurTextures[5]->Name(), std::string());
    }
    if (blurLevel == BlurLevel::Blur2)
    {
        descriptors.emplace_back(m_blurTextures[1], m_blurSampler, m_blurTextures[1]->Name(), std::string());
        descriptors.emplace_back(m_blurTextures[3], m_blurSampler, m_blurTextures[3]->Name(), std::string());
    }
    if (blurLevel == BlurLevel::Blur1)
    {
        descriptors.emplace_back(m_blurTextures[1], m_blurSampler, m_blurTextures[1]->Name(), std::string());
    }

    return descriptors;
}

void BlurTexture::Update(const PresetState& presetState, const PerFrameContext& perFrameContext)
{
    if (m_blurLevel == BlurLevel::None)
    {
        return;
    }

    auto sourceTexture = presetState.mainTexture.lock();

    if (!sourceTexture ||
        sourceTexture->Width() == 0 ||
        sourceTexture->Height() == 0)
    {
        return;
    }

    AllocateTextures(*sourceTexture);

    unsigned int const passes = static_cast<int>(m_blurLevel) * 2;
    auto const blur1EdgeDarken = static_cast<float>(*perFrameContext.blur1_edge_darken);

    const std::array<float, 8> weights = {4.0f, 3.8f, 3.5f, 2.9f, 1.9f, 1.2f, 0.7f, 0.3f}; //<- user can specify these

    Values blurMin;
    Values blurMax;
    GetSafeBlurMinMaxValues(perFrameContext, blurMin, blurMax);

    std::array<float, 3> scale{};
    std::array<float, 3> bias{};

    // figure out the progressive scale & bias needed, at each step,
    // to go from one [min..max] range to the next.
    scale[0] = 1.0f / (blurMax[0] - blurMin[0]);
    bias[0] = -blurMin[0] * scale[0];
    float tempMin = (blurMin[1] - blurMin[0]) / (blurMax[0] - blurMin[0]);
    float tempMax = (blurMax[1] - blurMin[0]) / (blurMax[0] - blurMin[0]);
    scale[1] = 1.0f / (tempMax - tempMin);
    bias[1] = -tempMin * scale[1];
    tempMin = (blurMin[2] - blurMin[1]) / (blurMax[1] - blurMin[1]);
    tempMax = (blurMax[2] - blurMin[1]) / (blurMax[1] - blurMin[1]);
    scale[2] = 1.0f / (tempMax - tempMin);
    bias[2] = -tempMin * scale[2];

    // Remember previously bound framebuffer
    GLint origReadFramebuffer;
    GLint origDrawFramebuffer;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &origReadFramebuffer);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origDrawFramebuffer);

    m_blurFramebuffer.Bind(0);

    glBlendFunc(GL_ONE, GL_ZERO);
    glBindVertexArray(m_vaoBlur);

    for (unsigned int pass = 0; pass < passes; pass++)
    {
        if (m_blurTextures[pass]->TextureID() == 0)
        {
            continue;
        }

        // set pixel shader
        Shader* blurShader;
        if ((pass % 2) == 0)
        {
            blurShader = &m_blur1Shader;
        }
        else
        {
            blurShader = &m_blur2Shader;
        }
        blurShader->Bind();
        blurShader->SetUniformInt("texture_sampler", 0);

        glViewport(0, 0, m_blurTextures[pass]->Width(), m_blurTextures[pass]->Height());

        // hook up correct source texture - assume there is only one, at stage 0
        if (pass == 0)
        {
            sourceTexture->Bind(0);
        }
        else
        {
            m_blurTextures[pass - 1]->Bind(0);
        }
        m_blurSampler->Bind(0);

        float srcWidth = static_cast<float>((pass == 0) ? sourceTexture->Width() : m_blurTextures[pass - 1]->Width());
        float srcHeight = static_cast<float>((pass == 0) ? sourceTexture->Height() : m_blurTextures[pass - 1]->Height());

        float scaleNow = scale[pass / 2];
        float biasNow = bias[pass / 2];

        // set constants
        if (pass % 2 == 0)
        {
            // pass 1 (long horizontal pass)
            //-------------------------------------
            const float w1 = weights[0] + weights[1];
            const float w2 = weights[2] + weights[3];
            const float w3 = weights[4] + weights[5];
            const float w4 = weights[6] + weights[7];
            const float d1 = 0 + 2 * weights[1] / w1;
            const float d2 = 2 + 2 * weights[3] / w2;
            const float d3 = 4 + 2 * weights[5] / w3;
            const float d4 = 6 + 2 * weights[7] / w4;
            const float w_div = 0.5f / (w1 + w2 + w3 + w4);
            //-------------------------------------
            //float4 _c0; // source texsize (.xy), and inverse (.zw)
            //float4 _c1; // w1..w4
            //float4 _c2; // d1..d4
            //float4 _c3; // scale, bias, w_div, 0
            //-------------------------------------
            m_blur1Shader.SetUniformFloat4("_c0", {srcWidth, srcHeight, 1.0f / srcWidth, 1.0f / srcHeight});
            m_blur1Shader.SetUniformFloat4("_c1", {w1, w2, w3, w4});
            m_blur1Shader.SetUniformFloat4("_c2", {d1, d2, d3, d4});
            m_blur1Shader.SetUniformFloat4("_c3", {scaleNow, biasNow, w_div, 0.0});
        }
        else
        {
            // pass 2 (short vertical pass)
            //-------------------------------------
            const float w1 = weights[0] + weights[1] + weights[2] + weights[3];
            const float w2 = weights[4] + weights[5] + weights[6] + weights[7];
            const float d1 = 0 + 2 * ((weights[2] + weights[3]) / w1);
            const float d2 = 2 + 2 * ((weights[6] + weights[7]) / w2);
            const float w_div = 1.0f / ((w1 + w2) * 2);
            //-------------------------------------
            //float4 _c0; // source texsize (.xy), and inverse (.zw)
            //float4 _c5; // w1,w2,d1,d2
            //float4 _c6; // w_div, edge_darken_c1, edge_darken_c2, edge_darken_c3
            //-------------------------------------
            m_blur2Shader.SetUniformFloat4("_c0", {srcWidth, srcHeight, 1.0f / srcWidth, 1.0f / srcHeight});
            m_blur2Shader.SetUniformFloat4("_c5", {w1, w2, d1, d2});
            // note: only do this first time; if you do it many times,
            // then the super-blurred levels will have big black lines along the top & left sides.
            if (pass == 1)
            {
                // Darken edges
                m_blur2Shader.SetUniformFloat4("_c6", {w_div, (1 - blur1EdgeDarken), blur1EdgeDarken, 5.0f});
            }
            else
            {
                // Don't darken
                m_blur2Shader.SetUniformFloat4("_c6", {w_div, 1.0f, 0.0f, 5.0f});
            }
        }

        // Draw fullscreen quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Save to blur texture
        m_blurTextures[pass]->Bind(0);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_blurTextures[pass]->Width(), m_blurTextures[pass]->Height());
        m_blurTextures[pass]->Unbind(0);
    }

    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind previous framebuffer and reset viewport size
    glBindFramebuffer(GL_READ_FRAMEBUFFER, origReadFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origDrawFramebuffer);
    glViewport(0, 0, sourceTexture->Width(), sourceTexture->Height());

    Shader::Unbind();
}

void BlurTexture::Bind(GLint& unit, Shader& shader) const
{
    for (size_t i = 0; i < static_cast<size_t>(m_blurLevel) * 2; i++)
    {
        if (i % 2 == 1)
        {
            m_blurTextures[i]->Bind(unit, m_blurSampler);
            shader.SetUniformInt(std::string("sampler_blur" + std::to_string(i / 2 + 1)).c_str(), unit);
            unit++;
        }
    }
}

void BlurTexture::GetSafeBlurMinMaxValues(const PerFrameContext& perFrameContext,
                                          Values& blurMin,
                                          Values& blurMax)
{
    blurMin[0] = static_cast<float>(*perFrameContext.blur1_min);
    blurMin[1] = static_cast<float>(*perFrameContext.blur2_min);
    blurMin[2] = static_cast<float>(*perFrameContext.blur3_min);
    blurMax[0] = static_cast<float>(*perFrameContext.blur1_max);
    blurMax[1] = static_cast<float>(*perFrameContext.blur2_max);
    blurMax[2] = static_cast<float>(*perFrameContext.blur3_max);

    // check that precision isn't wasted in later blur passes [...min-max gap can't grow!]
    // also, if min-max are close to each other, push them apart:
    const float fMinDist = 0.1f;
    if (blurMax[0] - blurMin[0] < fMinDist)
    {
        float avg = (blurMin[0] + blurMax[0]) * 0.5f;
        blurMin[0] = avg - fMinDist * 0.5f;
        blurMax[0] = avg - fMinDist * 0.5f;
    }
    blurMax[1] = std::min(blurMax[0], blurMax[1]);
    blurMin[1] = std::max(blurMin[0], blurMin[1]);
    if (blurMax[1] - blurMin[1] < fMinDist)
    {
        float avg = (blurMin[1] + blurMax[1]) * 0.5f;
        blurMin[1] = avg - fMinDist * 0.5f;
        blurMax[1] = avg - fMinDist * 0.5f;
    }
    blurMax[2] = std::min(blurMax[1], blurMax[2]);
    blurMin[2] = std::max(blurMin[1], blurMin[2]);
    if (blurMax[2] - blurMin[2] < fMinDist)
    {
        float avg = (blurMin[2] + blurMax[2]) * 0.5f;
        blurMin[2] = avg - fMinDist * 0.5f;
        blurMax[2] = avg - fMinDist * 0.5f;
    }
}

void BlurTexture::AllocateTextures(const Texture& sourceTexture)
{
    int width = sourceTexture.Width();
    int height = sourceTexture.Height();

    if (m_blurTextures[0] != nullptr &&
        width > 0 &&
        height > 0 &&
        width == m_sourceTextureWidth &&
        height == m_sourceTextureHeight)
    {
        // Size unchanged, return.
        return;
    }

    for (size_t i = 0; i < m_blurTextures.size(); i++)
    {
        // main VS = 1024
        // blur0 = 512
        // blur1 = 256  <-  user sees this as "blur1"
        // blur2 = 128
        // blur3 = 128  <-  user sees this as "blur2"
        // blur4 =  64
        // blur5 =  64  <-  user sees this as "blur3"
        if (!(i & 1) || (i < 2))
        {
            width = std::max(16, width / 2);
            height = std::max(16, height / 2);
        }
        int width2 = ((width + 3) / 16) * 16;
        int height2 = ((height + 3) / 4) * 4;

        if (i == 0)
        {
            // Only use as much space as needed to render the blur textures.
            m_blurFramebuffer.SetSize(width2, height2);
        }

        std::string textureName;
        if (i % 2 == 1)
        {
            textureName = "blur" + std::to_string(i / 2 + 1);
        }

        // This will automatically replace any old texture.
        m_blurTextures[i] = std::make_shared<Texture>(textureName, width2, height2, false);
    }

    m_sourceTextureWidth = sourceTexture.Width();
    m_sourceTextureHeight = sourceTexture.Height();
}
