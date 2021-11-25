#pragma once

#include "TextureManager.hpp"

#include <glm/mat4x4.hpp>

class BeatDetect;

/**
 * Holds data of the current rendering context (associated with a single preset).
 */
class RenderContext
{
public:
    float time{ 0.0 }; //!< Time since the preset started, in seconds.
    int texsize{ 512 }; //!< Size of the internal render texture.
    float aspectRatio{ 1.0 }; //!< X aspect ratio.
    bool aspectCorrect{ false }; //!< Aspect ratio correction.
    BeatDetect *beatDetect{ nullptr }; //!< Beat detection class to retrieve beat-related values.
    TextureManager *textureManager{ nullptr }; //!< Holds all loaded textures for shader access.
    GLuint programID_v2f_c4f{ 0 }; //!< Vertex shader program ID (no texture coordinates)
    GLuint programID_v2f_c4f_t2f{ 0 }; //!< Vertex shader program ID (with texture coordinates)
    GLint uniform_v2f_c4f_vertex_transformation{ 0 }; //!< Uniform location of the vertex shader transformation parameter (untextured output)
    GLint uniform_v2f_c4f_vertex_point_size{ 0 }; //!< Uniform location of the vertex shader point size parameter (untextured output)
    GLint uniform_v2f_c4f_t2f_vertex_transformation{ 0 }; //!< Uniform location of the vertex shader transformation parameter (textured output)
    GLint uniform_v2f_c4f_t2f_frag_texture_sampler{ 0 }; //!< Uniform location of the vertex shader transformation parameter (textured output)
    glm::mat4 mat_ortho{ 0.0 }; //!< Orthographic projection matrix.
};
