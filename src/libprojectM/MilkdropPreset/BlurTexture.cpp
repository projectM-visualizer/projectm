#include "BlurTexture.hpp"

#include <Renderer/StaticGlShaders.hpp>

#include <array>

BlurTexture::BlurTexture()
{
    // Compile shader sources
    m_blur1Shader.CompileProgram(StaticGlShaders::Get()->GetBlurVertexShader(),
                                 StaticGlShaders::Get()->GetBlur1FragmentShader());
    m_blur2Shader.CompileProgram(StaticGlShaders::Get()->GetBlurVertexShader(),
                                 StaticGlShaders::Get()->GetBlur2FragmentShader());

    // Initialize Blur VAO/VBO with a single fullscreen quad
    static constexpr std::array<float, 16> pointsBlur{
        -1.0, -1.0, 0.0, 1.0,
        1.0, -1.0, 1.0, 1.0,
        -1.0, 1.0, 0.0, 0.0,
        1.0, 1.0, 1.0, 0.0};

    glGenBuffers(1, &m_vboBlur);
    glGenVertexArrays(1, &m_vaoBlur);

    glBindVertexArray(m_vaoBlur);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboBlur);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 2, pointsBlur.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr); // Positions

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(sizeof(float) * 2)); // Textures

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

BlurTexture::~BlurTexture()
{
    glDeleteBuffers(1, &m_vboBlur);
    glDeleteVertexArrays(1, &m_vaoBlur);
}

void BlurTexture::UpdateFromTexture(const BlurParameters& parameters, const Texture& sourceTexture)
{
    if (parameters.level == BlurLevel::None ||
        sourceTexture.width == 0 ||
        sourceTexture.height == 0)
    {
        return;
    }

    AllocateTextures(sourceTexture);

    unsigned int passes = static_cast<int>(parameters.level) * 2;

    const std::array<float, 8> weights = {4.0f, 3.8f, 3.5f, 2.9f, 1.9f, 1.2f, 0.7f, 0.3f}; //<- user can specify these
    std::array<float, 3> blurMin{parameters.blur1Min, parameters.blur2Min, parameters.blur3Min};
    std::array<float, 3> blurMax{parameters.blur1Max, parameters.blur2Max, parameters.blur3Max};

    // check that precision isn't wasted in later blur passes [...min-max gap can't grow!]
    // also, if min-max are close to each other, push them apart:
    const float minDist = 0.1f;
    if (blurMax[0] - blurMin[0] < minDist)
    {
        float const avg = (blurMin[0] + blurMax[0]) * 0.5f;
        blurMin[0] = avg - minDist * 0.5f;
        blurMax[0] = avg - minDist * 0.5f;
    }
    blurMax[1] = std::min(blurMax[0], blurMax[1]);
    blurMin[1] = std::max(blurMin[0], blurMin[1]);
    if (blurMax[1] - blurMin[1] < minDist)
    {
        float const avg = (blurMin[1] + blurMax[1]) * 0.5f;
        blurMin[1] = avg - minDist * 0.5f;
        blurMax[1] = avg - minDist * 0.5f;
    }
    blurMax[2] = std::min(blurMax[1], blurMax[2]);
    blurMin[2] = std::max(blurMin[1], blurMin[2]);
    if (blurMax[2] - blurMin[2] < minDist)
    {
        float const avg = (blurMin[2] + blurMax[2]) * 0.5f;
        blurMin[2] = avg - minDist * 0.5f;
        blurMax[2] = avg - minDist * 0.5f;
    }

    std::array<float, 3> scale{};
    std::array<float, 3> bias{};

    // figure out the progressive scale & bias needed, at each step,
    // to go from one [min..max] range to the next.
    float tempMin;
    float tempMax;
    scale[0] = 1.0f / (blurMax[0] - blurMin[0]);
    bias[0] = -blurMin[0] * scale[0];
    tempMin = (blurMin[1] - blurMin[0]) / (blurMax[0] - blurMin[0]);
    tempMax = (blurMax[1] - blurMin[0]) / (blurMax[0] - blurMin[0]);
    scale[1] = 1.0f / (tempMax - tempMin);
    bias[1] = -tempMin * scale[1];
    tempMin = (blurMin[2] - blurMin[1]) / (blurMax[1] - blurMin[1]);
    tempMax = (blurMax[2] - blurMin[1]) / (blurMax[1] - blurMin[1]);
    scale[2] = 1.0f / (tempMax - tempMin);
    bias[2] = -tempMin * scale[2];

    glBlendFunc(GL_ONE, GL_ZERO);
    glBindVertexArray(m_vaoBlur);

    for (unsigned int pass = 0; pass < passes; pass++)
    {
        // set pixel shader
        if ((pass % 2) == 0)
        {
            m_blur1Shader.Bind();
            m_blur1Shader.SetUniformInt("texture_sampler", 0);
        }
        else
        {
            m_blur2Shader.Bind();
            m_blur2Shader.SetUniformInt("texture_sampler", 0);
        }

        glViewport(0, 0, m_blurTextures[pass]->width, m_blurTextures[pass]->height);

        // hook up correct source texture - assume there is only one, at stage 0
        if (pass == 0)
        {
            sourceTexture.Bind(0);
        }
        else
        {
            m_blurTextures[pass - 1]->Bind(0);
        }

        float srcWidth = static_cast<float>((pass == 0) ? sourceTexture.width : m_blurTextures[pass - 1]->width);
        float srcHeight = static_cast<float>((pass == 0) ? sourceTexture.height : m_blurTextures[pass - 1]->height);

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
                m_blur2Shader.SetUniformFloat4("_c6", {w_div, (1 - parameters.blur1EdgeDarken), parameters.blur1EdgeDarken, 5.0f}); //darken edges
            else
                m_blur2Shader.SetUniformFloat4("_c6", {w_div, 1.0f, 0.0f, 5.0f}); // don't darken
        }

        // draw fullscreen quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // save to blur texture
        m_blurTextures[pass]->Bind(0);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_blurTextures[pass]->width, m_blurTextures[pass]->height);
        m_blurTextures[pass]->Unbind(0);
    }

    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader::Unbind();
}

void BlurTexture::AllocateTextures(const Texture& sourceTexture)
{
    if (m_blurTextures[0] != nullptr &&
        sourceTexture.width > 0 &&
        sourceTexture.height > 0 &&
        sourceTexture.width == m_sourceTextureWidth &&
        sourceTexture.height == m_sourceTextureHeight)
    {
        // Size unchanged, return.
        return;
    }

    int width = sourceTexture.width;
    int height = sourceTexture.height;
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

        std::string textureName = "blur" + std::to_string(i / 2 + 1) + ((i % 2) ? "" : "doNOTuseME");
        auto textureBlur = std::make_unique<Texture>(textureName, width2, height2, false);
        textureBlur->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);

        // This will automatically replace any old texture.
        m_blurTextures[i] = std::move(textureBlur);
    }

    m_sourceTextureWidth = sourceTexture.width;
    m_sourceTextureHeight = sourceTexture.height;
}
