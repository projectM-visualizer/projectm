#pragma once

#include "RenderItem.hpp"

/**
 * @brief Renders a border around the screen.
 */
class Border : public RenderItem
{
public:

    /**
     * Constructor. Initializes the required OpenGL data structures.
     */
    Border();

    void InitVertexAttrib();

    /**
     * Draws the border.
     * @param context The render context data.
     */
    void Draw(RenderContext &context);

    float outer_size{ 0.0 }; //!< Outer border width
    float outer_r{ 0.0 }; //!< Outer border color, red channel.
    float outer_g{ 0.0 }; //!< Outer border color, green channel.
    float outer_b{ 0.0 }; //!< Outer border color, blue channel.
    float outer_a{ 0.0 }; //!< Outer border color, alpha channel.

    float inner_size{ 0.0 }; //!< Inner border width
    float inner_r{ 0.0 }; //!< Inner border color, red channel.
    float inner_g{ 0.0 }; //!< Inner border color, green channel.
    float inner_b{ 0.0 }; //!< Inner border color, blue channel.
    float inner_a{ 0.0 }; //!< Inner border color, alpha channel.
};
