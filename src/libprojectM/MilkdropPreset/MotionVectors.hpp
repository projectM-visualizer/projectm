#pragma once

#include "Renderer/RenderItem.hpp"

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
    void Draw(const RenderContext &context);

    float r{ 0.0 }; //!< Red color channel of the motion vectors (mv_r).
    float g{ 0.0 }; //!< Green color channel of the motion vectors (mv_g).
    float b{ 0.0 }; //!< Blue color channel of the motion vectors (mv_b).
    float a{ 0.0 }; //!< Alpha channel of the motion vectors (mv_a).
    float length{ 0.0 }; //!< Line length of the motion vectors (mv_l).
    float x_num{ 0.0 }; //!< Horizontal grid size (integer part of mv_x).
    float y_num{ 0.0 }; //!< Vertical grid size (integer part of mv_y).
    float x_offset{ 0.0 }; //!< Horizontal grid offset (mv_dx).
    float y_offset{ 0.0 }; //!< Vertical grid offset (mv_dy).

};
