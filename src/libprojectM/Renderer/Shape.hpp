#pragma once

#include "RenderItem.hpp"

/**
 * @brief Renders a custom shape with or without a texture.
 *
 * The class creates two sets of VBO/VAO as it's only known later (in the Draw() call) whether the shape is textured
 * or not.
 */
class Shape : public RenderItem
{
public:
    Shape();

    ~Shape() override;

    void InitVertexAttrib();

    virtual void Draw(RenderContext& context);

    std::string imageUrl; //!< Texture filename to be rendered on this shape

    int sides{ 4 }; //!< Number of sides (vertices)
    bool thickOutline{ false }; //!< If true, the shape is rendered with a thick line, otherwise a single-pixel line.
    bool enabled{ false }; //!< If false, the shape isn't drawn.
    bool additive{ false }; //!< Flag that specifies whether the shape should be drawn additive.
    bool textured{ false }; //!< If true, the shape will be rendered with the given texture.

    float tex_zoom{ 1.0 }; //!< Texture zoom value.
    float tex_ang{ 0.0 }; //!< Texture rotation angle.

    float x{ 0.5 }; //!< The shape x position.
    float y{ 0.5 }; //!< The shape y position.
    float radius{ 1.0 }; //!< The shape radius (1.0 fills the whole screen).
    float ang{ 0.0 };  //!< The shape rotation.

    float r{ 0.0 }; ///!< Red color value.
    float g{ 0.0 }; ///!< Green color value.
    float b{ 0.0 }; ///!< Blue color value.
    float a{ 0.0 }; ///!< Alpha color value.

    float r2{ 0.0 }; //!< Red color value.
    float g2{ 0.0 }; //!< Green color value.
    float b2{ 0.0 }; //!< Blue color value.
    float a2{ 0.0 }; //!< Alpha color value.

    float border_r{ 0.0 }; //!< Red color value.
    float border_g{ 0.0 }; //!< Green color value.
    float border_b{ 0.0 }; //!< Blue color value.
    float border_a{ 0.0 }; //!< Alpha color value.

private:

    /**
     * Vertex shader inputs for each shape vertex.
     */
    struct ShapeVertexShaderData
    {
        float point_x; //!< The vertex x coordinate.
        float point_y; //!< The vertex y coordinate.
        float color_r; //!< The vertex color, red channel.
        float color_g; //!< The vertex color, green channel.
        float color_b; //!< The vertex color, blue channel.
        float color_a; //!< The vertex color, alpha channel.
        float tex_x; //!< The vertex x (u) texture coordinate.
        float tex_y; //!< The vertex y (v) texture coordinate.
    };

    struct ShapeVertex
    {
        float x{ .0f }; //!< The vertex X coordinate.
        float y{ .0f }; //!< The vertex Y coordinate.
    };

    GLuint m_vboID_texture{ 0 }; //!< Vertex buffer object ID for a textured shape.
    GLuint m_vaoID_texture{ 0 }; //!< Vertex array object ID for a textured shape.

    GLuint m_vboID_not_texture{ 0 }; //!< Vertex buffer object ID for an untextured shape.
    GLuint m_vaoID_not_texture{ 0 }; //!< Vertex array object ID for an untextured shape.
};
