/*
 * VideoEcho.cpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#include "VideoEcho.hpp"
#include "Renderer/ShaderEngine.hpp"
#include <glm/gtc/type_ptr.hpp>

VideoEcho::VideoEcho(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void VideoEcho::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr); // Positions

    glDisableVertexAttribArray(1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(sizeof(float) * 2)); // Textures
}

void VideoEcho::Draw(PerFrameContext& perFrameContext)
{
    int orientation = static_cast<int>(m_presetState.videoEchoOrientation) % 4;
    int flipX;
    int flipY;
    switch (orientation)
    {
        case 0:
            flipX = 1;
            flipY = 1;
            break;
        case 1:
            flipX = -1;
            flipY = 1;
            break;
        case 2:
            flipX = 1;
            flipY = -1;
            break;
        case 3:
            flipX = -1;
            flipY = -1;
            break;
        default:
            flipX = 1;
            flipY = 1;
            break;
    }

    float buffer_data[8][2] = {
        {-0.5f * flipX, -0.5f * flipY},
        {0.0, 1.0},

        {-0.5f * flipX, 0.5f * flipY},
        {0.0, 0.0},

        {0.5f * flipX, 0.5f * flipY},
        {1.0, 0.0},

        {0.5f * flipX, -0.5f * flipY},
        {1.0, 1.0}};

    glm::mat4 mat_first_translation = glm::mat4(1.0);
    mat_first_translation[3][0] = -0.5;
    mat_first_translation[3][1] = -0.5;

    glm::mat4 mat_scale = glm::mat4(1.0);
    mat_scale[0][0] = 1.0f / static_cast<float>(m_presetState.videoEchoZoom);
    mat_scale[1][1] = 1.0f / static_cast<float>(m_presetState.videoEchoZoom);

    glm::mat4 mat_second_translation = glm::mat4(1.0);
    mat_second_translation[3][0] = 0.5;
    mat_second_translation[3][1] = 0.5;

    for (int i = 1; i < 8; i += 2)
    {
        glm::vec4 texture = glm::vec4(buffer_data[i][0], buffer_data[i][1], 0, 1);
        texture = mat_first_translation * texture;
        texture = mat_scale * texture;
        texture = mat_second_translation * texture;

        buffer_data[i][0] = texture[0];
        buffer_data[i][1] = texture[1];
    }


    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, buffer_data, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_presetState.texturedShader.Bind();
    m_presetState.texturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);
    m_presetState.texturedShader.SetUniformInt("texture_sampler", 0);

    //Now Blend the Video Echo
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, static_cast<float>(m_presetState.videoEchoAlpha));

    glBindVertexArray(m_vaoID);

    //draw video echo
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
