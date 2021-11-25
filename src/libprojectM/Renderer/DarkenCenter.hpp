#pragma once

#include "RenderItem.hpp"

/**
 * @brief Darkens the screen center a bit on each frame.
 */
class DarkenCenter : public RenderItem
{
public:

    /**
     * Constructor. Initializes the required OpenGL data structures.
     */
    DarkenCenter();

    void InitVertexAttrib();

    /**
     * Applies the darkening area.
     * @param context The render context data.
     */
    void Draw(RenderContext &context);
};
