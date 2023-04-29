precision mediump float;

in vec4 fragment_color;
in vec2 fragment_texture;

uniform sampler2D texture_sampler;

out vec4 color;

void main(){
    color = fragment_color * texture(texture_sampler, fragment_texture.st);
}
