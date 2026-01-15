precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec2 vertex_texture;

uniform mat4 vertex_transformation;

out vec4 fragment_color;
out vec2 fragment_texture;

void main(){
    gl_Position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);
    fragment_color = vertex_color;
    fragment_texture = vertex_texture;
}
