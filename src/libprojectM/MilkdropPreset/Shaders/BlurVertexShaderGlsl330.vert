precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_texture;

out vec2 fragment_texture;

void main(){
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragment_texture = vertex_texture;
}
