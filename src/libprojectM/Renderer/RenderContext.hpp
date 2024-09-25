/**
* @file RenderContext.hpp
* @brief A class holding per-frame render values for use in preset rendering.
*/
#pragma once

namespace libprojectM {
namespace Renderer {

class TextureManager;

enum class RenderContextTextureFilter
{
    Nearest = 0,
    Linear = 1,
};

/**
 * @brief Holds all global data of the current rendering context, which can change from frame to frame.
 */
class RenderContext
{
public:
    float time{0.0f};      //!< Time since the preset started, in seconds.
    int frame{0};          //!< Frames rendered so far.
    float fps{0.0f};       //!< Frames per second.
    float progress{0.0f};  //!< Preset progress.
    int viewportSizeX{0};  //!< Horizontal viewport size in pixels
    int viewportSizeY{0};  //!< Vertical viewport size in pixels
    float aspectX{1.0};    //!< X aspect ratio.
    float aspectY{1.0};    //!< Y aspect ratio.
    float invAspectX{1.0}; //!< Inverse X aspect ratio.
    float invAspectY{1.0}; //!< Inverse Y aspect ratio.

    int perPixelMeshX{64}; //!< Per-pixel/per-vertex mesh X resolution.
    int perPixelMeshY{48}; //!< Per-pixel/per-vertex mesh Y resolution.

    int targetX{0};        //!< Target framebuffer viewport x.
    int targetY{0};        //!< Target framebuffer viewport y.
    int targetWidth{0};    //!< Target framebuffer viewport width.
    int targetHeight{0};   //!< Target framebuffer viewport height.
    RenderContextTextureFilter targetTextureFilter{RenderContextTextureFilter::Nearest}; //!< Target texture filtering when drawing to framebuffer.

    TextureManager* textureManager{nullptr}; //!< Holds all loaded textures for shader access.
};

} // namespace Renderer
} // namespace libprojectM
