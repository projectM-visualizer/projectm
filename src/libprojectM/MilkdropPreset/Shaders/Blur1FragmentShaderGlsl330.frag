precision mediump float;

in vec2 fragment_texture;

uniform sampler2D texture_sampler;
uniform vec4 _c0; // source texsize (.xy), and inverse (.zw)
uniform vec4 _c1; // w1..w4
uniform vec4 _c2; // d1..d4
uniform vec4 _c3; // scale, bias, w_div

out vec4 color;

void main(){
    // LONG HORIZ. PASS 1:
    #define srctexsize _c0
    #define w1 _c1.x
    #define w2 _c1.y
    #define w3 _c1.z
    #define w4 _c1.w
    #define d1 _c2.x
    #define d2 _c2.y
    #define d3 _c2.z
    #define d4 _c2.w
    #define fscale _c3.x
    #define fbias  _c3.y
    #define w_div  _c3.z

    // note: if you just take one sample at exactly uv.xy, you get an avg of 4
    // pixels.

    // + moves blur UP, LEFT by 1-pixel increments
    vec2 uv2 = fragment_texture.xy + srctexsize.zw*vec2(1.0,1.0);

    vec3 blur =
    (texture(texture_sampler, uv2 + vec2(d1 * srctexsize.z, 0)).xyz +
    texture(texture_sampler, uv2 + vec2(-d1 * srctexsize.z, 0)).xyz) *
    w1 +
    (texture(texture_sampler, uv2 + vec2(d2 * srctexsize.z, 0)).xyz +
    texture(texture_sampler, uv2 + vec2(-d2 * srctexsize.z, 0)).xyz) *
    w2 +
    (texture(texture_sampler, uv2 + vec2(d3 * srctexsize.z, 0)).xyz +
    texture(texture_sampler, uv2 + vec2(-d3 * srctexsize.z, 0)).xyz) *
    w3 +
    (texture(texture_sampler, uv2 + vec2(d4 * srctexsize.z, 0)).xyz +
    texture(texture_sampler, uv2 + vec2(-d4 * srctexsize.z, 0)).xyz) *
    w4;

    blur.xyz *= w_div;

    blur.xyz = blur.xyz*fscale + fbias;

    color.xyz = blur;
    color.w   = 1.0;
}
