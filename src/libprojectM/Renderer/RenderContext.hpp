#pragma once

#include "projectM-opengl.h"

#include <glm/mat4x4.hpp>

class TextureManager;

/**
 * @brief Holds all global data of the current rendering context, which can change from frame to frame.
 */
class RenderContext
{
public:
    float time{0.0f};                                   //!< Time since the preset started, in seconds.
    int frame{0};                                       //!< Frames rendered so far.
    float fps{0.0f};                                    //!< Frames per second.
    float progress{0.0f};                               //!< Preset progress.
    int texsize{512};                                   //!< Size of the internal render texture.
    int viewportSizeX{0};                               //!< Horizontal viewport size in pixels
    int viewportSizeY{0};                               //!< Vertical viewport size in pixels
    float aspectX{1.0};                                 //!< X aspect ratio.
    float aspectY{1.0};                                 //!< Y aspect ratio.
    float invAspectX{1.0};                              //!< Inverse X aspect ratio.
    float invAspectY{1.0};                              //!< Inverse Y aspect ratio.

    TextureManager* textureManager{nullptr};            //!< Holds all loaded textures for shader access.

    glm::mat4 mat_ortho{0.0};                           //!< Orthographic projection matrix.
};
