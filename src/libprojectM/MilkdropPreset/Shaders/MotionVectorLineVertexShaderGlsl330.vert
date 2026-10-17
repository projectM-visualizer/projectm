precision highp float;
precision highp int;

// One instance per motion vector: a flat-ended quad from the grid point to where the u/v map says
// the image came from, as in PresetMotionVectorsVertexShaderGlsl330.vert. Used with the line
// fragment shader.
layout(location = 0) in vec2 vector_start;
layout(location = 1) in vec4 vector_color;

uniform mat4 vertex_transformation;
uniform vec2 viewport_size;
uniform highp float half_width;
uniform highp float antialias; // 1: one-pixel anti-aliased edge; 0: hard edge (see the line fragment shader)
uniform vec2 tie_bias;           // vertex stage only: render pixels added to every corner, see LineTieBias()
uniform float length_multiplier;
uniform float minimum_length;
uniform sampler2D warp_coordinates;

out vec4 fragment_color;
out float edge_distance;

vec2 ToPixels(vec2 texturePosition)
{
    // Texture coordinates (0...1, top-down) to the usual -1...1, then to render pixels.
    vec2 position = texturePosition * 2.0 - 1.0;
    position.y = -position.y;
    return (vertex_transformation * vec4(position, 0.0, 1.0)).xy * 0.5 * viewport_size;
}

void main()
{
    vec2 start = vector_start;
    vec2 oldUV = texture(warp_coordinates, vec2(start.x, 1.0 - start.y)).xy;

    // Enforce the minimum trail length, as the GL-line shader does.
    vec2 dist = (oldUV - start) * length_multiplier;
    float len = length(dist);
    if (len > minimum_length)
    {
    }
    else if (len > 0.00000001)
    {
        dist *= minimum_length / len;
    }
    else
    {
        dist = vec2(minimum_length);
    }

    vec2 a = ToPixels(start);
    vec2 b = ToPixels(start + dist);
    vec2 direction = b - a;
    float segmentLength = length(direction);

    // A zero-length or non-finite vector draws nothing: all corners go outside the clip volume.
    if (!(segmentLength > 0.0001) || isinf(segmentLength))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        fragment_color = vec4(0.0);
        edge_distance = 0.0;
        return;
    }
    direction /= segmentLength;
    vec2 normal = vec2(-direction.y, direction.x);

    float side = (gl_VertexID % 2 == 0) ? -1.0 : 1.0;

    // Distances are measured along the line's minor axis, like GL lines (see the line vertex shader).
    float major = max(abs(direction.x), abs(direction.y));
    // Anti-aliased: one pixel beyond the line for the edge ramp. Hard edge: the band itself, at
    // least 1 px wide along the minor axis, so the rasterizer resolves pixel-border ties.
    float extent = antialias > 0.5 ? half_width + 1.0 : max(half_width, 0.5) * major;
    vec2 position = (gl_VertexID < 2 ? a : b) + normal * side * extent;
    // Pixel-border ties (a band edge exactly on a pixel centre) are left to the rasterizer; GL line
    // tie-breaking is implementation-defined, so this offset makes them land on the row the
    // platform's GL lines light (zero on desktop).
    position += tie_bias;
    gl_Position = vec4(position / (0.5 * viewport_size), 0.0, 1.0);

    edge_distance = side * extent / major;
    fragment_color = vector_color;
}
