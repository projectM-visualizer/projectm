#pragma once

#include "RenderContext.hpp"

/**
 * Base class of all renderable items in the projectM render queue.
 */
class RenderItem
{
public:
    using floatPair = float[2]; //!< A pair of floats (AKA vec2).
    using floatTriple = float[3]; //!< A triple of floats (AKA vec3).
    using floatQuad =  float[4]; //!< A quad of floats (AKA vec4).

    using List = std::vector<RenderItem*>; //!< A list of RenderItems instances.

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

    /**
     * Renders the current item.
     * @param context The render context data.
     */
    virtual void Draw(RenderContext &context) = 0;

    float masterAlpha{ 1.0 }; //!< Alpha value for the whole RenderItem.

protected:
    /**
     * @brief Initializes the OpenGL objects in the base class.
     *
     * Must be called in the constructor of derived classes.
     */
    void Init();

    GLuint m_vboID{ 0 }; //!< This RenderItem's vertex buffer object ID
    GLuint m_vaoID{ 0 }; //!< This RenderItem's vertex array object ID
};
