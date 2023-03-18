#pragma once

#include "RenderContext.hpp"

/**
 * @brief Base class for render meshes.
 * Also defines a few standard vertex attribute structures for use with the shaders.
 */
class RenderItem
{
public:
    /**
     * A simple 2D point.
     */
    struct Point {
        Point() = default;
        Point(float pointX, float pointY)
            : x(pointX)
            , y(pointY){};

        float x{}; //!< Vertex X coordinate.
        float y{}; //!< Vertex Y coordinate.
    };

    /**
     * Struct for use as a vertex attribute in the untextured shader.
     */
    struct ColoredPoint {
        float x{}; //!< Vertex X coordinate.
        float y{}; //!< Vertex Y coordinate.
        float r{}; //!< Vertex red color value.
        float g{}; //!< Vertex green color value.
        float b{}; //!< Vertex blue color value.
        float a{}; //!< Vertex alpha value.
    };

    /**
     * Struct for use as a vertex attribute in the textured shader.
     */
    struct TexturedPoint {
        float x{}; //!< Vertex X coordinate.
        float y{}; //!< Vertex Y coordinate.
        float r{}; //!< Vertex red color value.
        float g{}; //!< Vertex green color value.
        float b{}; //!< Vertex blue color value.
        float a{}; //!< Vertex alpha value.
        float u{}; //!< Texture X coordinate.
        float v{}; //!< Texture Y coordinate.
    };

    /**
     * Default constructor.
     */
    RenderItem() = default;

    /**
     * Destructor.
     */
    virtual ~RenderItem();

    /**
     * Sets up the vertex attribute pointers for rendering this item.
     */
    virtual void InitVertexAttrib() = 0;

protected:
    /**
     * @brief Initializes the OpenGL objects in the base class.
     *
     * Must be called in the constructor of derived classes.
     */
    void Init();

    GLuint m_vboID{0}; //!< This RenderItem's vertex buffer object ID
    GLuint m_vaoID{0}; //!< This RenderItem's vertex array object ID
};
