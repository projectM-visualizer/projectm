precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in vec2 vertex_rad_ang;

out vec4 frag_COLOR;
out vec2 frag_TEXCOORD0;
out vec2 frag_TEXCOORD1;

void main(){
    vec4 position = vec4(vertex_position, 0.0, 1.0);
    gl_Position = position;
    frag_COLOR = vertex_color;
    frag_TEXCOORD0 = vertex_texture;
    frag_TEXCOORD1 = vertex_rad_ang;
}
