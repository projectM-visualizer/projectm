precision mediump float;

in vec4 frag_color;
in vec4 frag_texcoord;
in vec2 frag_texcoord1;

uniform sampler2D texture_sampler;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 texCoords;

void main() {
    // Main image
    color = frag_color * texture(texture_sampler, frag_texcoord.xy);
    // Motion vector grid u/v coords for the next frame
    texCoords = frag_texcoord.xy;
}