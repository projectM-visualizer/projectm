precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_texture;

uniform int flipVertical;

out vec2 fragment_texture;

void main(){
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragment_texture = vertex_texture;

    // Vertically flip main texture, but not the already blurred ones.
    if (flipVertical == 1)
    {
        fragment_texture.y = 1.0 - fragment_texture.y;
    }
}
