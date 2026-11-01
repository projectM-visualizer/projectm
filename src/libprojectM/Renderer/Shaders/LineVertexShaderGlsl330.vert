precision highp float;
precision highp int;

// One instance per segment A-B of a line strip, drawn as a 4-vertex triangle strip. The points
// before and after the segment shape its ends: a miter where the strip continues, a flat end where
// the neighbour repeats the end point (see LineBatch). Positions are expanded in render pixels.
layout(location = 0) in vec2 point_previous;
layout(location = 1) in vec2 point_a;
layout(location = 2) in vec2 point_b;
layout(location = 3) in vec2 point_next;
layout(location = 4) in vec4 color_a;
layout(location = 5) in vec4 color_b;

uniform mat4 vertex_transformation;
uniform vec2 viewport_size;
uniform highp float half_width;
uniform vec2 pass_offset;        // vertex stage only: this pass's translation, in strip coordinates
uniform highp float antialias;   // 1: one-pixel anti-aliased edge; 0: hard edge (see the fragment shader)
uniform vec2 tie_bias;           // vertex stage only: render pixels added to every corner, see LineTieBias()

out vec4 fragment_color;
out float edge_distance;

vec2 ToPixels(vec2 position)
{
    // The pass offset moves the strip's points where MilkDrop moves them for its thick lines.
    return (vertex_transformation * vec4(position + pass_offset, 0.0, 1.0)).xy * 0.5 * viewport_size;
}

void main()
{
    vec2 a = ToPixels(point_a);
    vec2 b = ToPixels(point_b);
    vec2 direction = b - a;
    float segmentLength = length(direction);

    // A zero-length or non-finite segment draws nothing: all corners go outside the clip volume.
    if (!(segmentLength > 0.0001) || isinf(segmentLength))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        fragment_color = vec4(0.0);
        edge_distance = 0.0;
        return;
    }

    direction /= segmentLength;
    vec2 normal = vec2(-direction.y, direction.x);

    bool atA = gl_VertexID < 2;
    float side = (gl_VertexID % 2 == 0) ? -1.0 : 1.0;
    vec2 corner = atA ? a : b;
    vec2 neighbourDirection = atA ? corner - ToPixels(point_previous) : ToPixels(point_next) - corner;

    // Miter join: offset along the bisector, lengthened so both edges stay at the band's distance.
    // Turns sharper than about 120 degrees and strip ends keep the segment's own normal, which leaves
    // a gap or overlap of under a pixel at that corner. (0.49, not 0.5: an equilateral triangle's
    // corners, exactly 120 degrees, would otherwise miter or not depending on rounding.)
    vec2 offsetDirection = normal;
    float neighbourLength = length(neighbourDirection);
    if (neighbourLength > 0.0001 && !isinf(neighbourLength))
    {
        vec2 tangentSum = neighbourDirection / neighbourLength + direction;
        float tangentLength = length(tangentSum);
        if (tangentLength > 0.0001)
        {
            vec2 tangent = tangentSum / tangentLength;
            vec2 miter = vec2(-tangent.y, tangent.x);
            float cosine = dot(miter, normal);
            if (cosine > 0.49)
            {
                offsetDirection = miter / cosine;
            }
        }
    }

    float major = max(abs(direction.x), abs(direction.y)); // at least 0.707
    float bandHalfWidth = max(half_width, 0.0001);

    // Distances are measured along the line's minor axis, like GL lines: a 1 px line covers one
    // pixel per column (or row) at any angle, instead of 1 px perpendicular, which would cover up to
    // 1.41 times as many pixels on diagonals. A minor-axis distance d is d * major perpendicular to
    // the line.
    float extent;
    if (antialias > 0.5)
    {
        // The quad reaches one pixel beyond the band for the anti-aliased edge ramp.
        extent = bandHalfWidth + 1.0;
    }
    else
    {
        // Hard edge: the quad is the band itself, at least 1 px wide along the minor axis (thinner
        // lines fade by their width in the fragment shader). The rasterizer decides which pixels
        // are inside, so a pixel centre exactly on the band's edge (a line on a pixel border) is
        // drawn exactly once, whatever the precision of the varyings.
        extent = bandHalfWidth * max(1.0, 0.5 / max(half_width, 0.0001)) * major;
    }
    vec2 position = corner + offsetDirection * side * extent;
    // Pixel-border ties (a band edge exactly on a pixel centre) are left to the rasterizer; GL line
    // tie-breaking is implementation-defined, so this offset makes them land on the row the
    // platform's GL lines light (zero on desktop).
    position += tie_bias;
    gl_Position = vec4(position / (0.5 * viewport_size), 0.0, 1.0);

    // Along the minor axis, like half_width, where the anti-aliased ramp is centred. Only the
    // anti-aliased edge reads it.
    edge_distance = side * extent / major;
    fragment_color = atA ? color_a : color_b;
}
