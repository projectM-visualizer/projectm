#pragma once

#include "RenderItem.hpp"

/**
 * @brief Draws a flexible motion vector field.
 *
 * This is broken right now, as it only renders a relatively static 1px point grid, with no apparent motion trails.
 * Milkdrop renders this as lines with trails.
 *
 * @todo Reverse-engineer the original Milkdrop code and reimplement it properly.
 *       https://github.com/projectM-visualizer/milkdrop2/blob/f05b0d811a87a17c4624170c26c93bac39b05bde/src/vis_milk2/milkdropfs.cpp#L1239
 */
class MotionVectors : public RenderItem
{
public:

    /**
     * Constructor. Initializes the required OpenGL data structures.
     */
    MotionVectors();

    void InitVertexAttrib();

    /**
     * Redners the motion vectors.
     * @param context The render context data.
     */
    void Draw(RenderContext &context);

    float r{ 0.0 }; //!< Red color channel of the motion vectors.
    float g{ 0.0 }; //!< Green color channel of the motion vectors.
    float b{ 0.0 }; //!< Blue color channel of the motion vectors.
    float a{ 0.0 }; //!< Alpha channel of the motion vectors.
    float length{ 0.0 }; //!< Line length of the motion vectors.
    float x_num{ 0.0 }; //!< Horizontal grid size.
    float y_num{ 0.0 }; //!< Vertical grid size.
    float x_offset{ 0.0 }; //!< Horizontal grid offset.
    float y_offset{ 0.0 }; //!< Vertical grid offset.

};
