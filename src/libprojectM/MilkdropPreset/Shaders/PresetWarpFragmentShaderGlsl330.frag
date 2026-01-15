precision mediump float;

in vec4 frag_COLOR;
in vec4 frag_TEXCOORD0;
in vec2 frag_TEXCOORD1;

uniform sampler2D texture_sampler;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 texCoords;

void main() {
    // Main image
    color = frag_COLOR * texture(texture_sampler, frag_TEXCOORD0.xy);
    // Motion vector grid u/v coords for the next frame
    texCoords = frag_TEXCOORD0.xy;
}
