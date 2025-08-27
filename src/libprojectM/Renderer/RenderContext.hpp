
#pragma once

namespace libprojectM {
namespace Renderer {

class ShaderCache;
class TextureManager;


class RenderContext
{
public:
    float time{0.0f};
    int frame{0};
    float fps{0.0f};
    float progress{0.0f};
    float blendProgress{0.0f};
    int viewportSizeX{0};
    int viewportSizeY{0};
    float aspectX{1.0};
    float aspectY{1.0};
    float invAspectX{1.0};
    float invAspectY{1.0};

    int perPixelMeshX{64};
    int perPixelMeshY{48};

    TextureManager* textureManager{nullptr};
    ShaderCache* shaderCache{nullptr};
};

}
}
