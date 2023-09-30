precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in int vertex_index;

uniform mat4 vertex_transformation;
uniform float length_multiplier;
uniform float minimum_length;

uniform sampler2D warp_coordinates;

out vec4 fragment_color;

void main() {
    // Input positions are given in texture coordinates (0...1), not the usual
    // screen coordinates.
    vec2 pos = vertex_position;

    if (vertex_index % 2 == 1)
    {
        // Reverse propagation using the u/v texture written in the previous frame.
        // Milkdrop's original code did a simple bilinear interpolation, but here it was already
        // done by the fragment shader during the warp mesh drawing. We just need to look up the
        // motion vector coordinate.
        // We simply invert the y coordinate because it's easier than flipping the u/v texture.
        vec2 oldUV = texture(warp_coordinates, vec2(pos.x, 1.0 - pos.y)).xy;

        // Enforce minimum trail length
        vec2 dist = oldUV - pos;
        dist *= length_multiplier;
        float len = length(dist);
        if (len > minimum_length)
        {}
        else if (len > 0.00000001f)
        {
            len = minimum_length / len;
            dist *= len;
        }
        else
        {
            dist = vec2(minimum_length);
        }

        pos += dist;
    }

    // Transform positions from 0...1 to -1...1 in each direction.
    pos = pos * 2.0 - 1.0;

    // Flip final Y, as we draw this top-down, which is bottom-up in OpenGL.
    pos.y = -pos.y;

    // Now we've got the usual coordinates, apply our orthogonal transformation.
    gl_Position = vertex_transformation * vec4(pos, 0.0, 1.0);
    fragment_color = vertex_color;
}
