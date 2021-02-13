#include "StaticGlShaders.h"
#include "projectM-opengl.h"
#include <cstring>


namespace {
// Variants of shaders for GLSL1.2
const std::string kPresetWarpVertexShaderGlsl120 = R"(
attribute vec2 vertex_position;
attribute vec4 vertex_color;
attribute vec2 vertex_texture;

uniform mat4 vertex_transformation;

varying vec4 frag_COLOR;
varying vec4 frag_TEXCOORD0;
varying vec2 frag_TEXCOORD1;

void main(){
    vec4 position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);
    gl_Position = position;
    frag_COLOR = vertex_color;
    frag_TEXCOORD0.xy = vertex_texture;
    frag_TEXCOORD0.zw = position.xy;
    frag_TEXCOORD1 = vec2(0.0, 0.0);
}
)";

const std::string kPresetCompVertexShaderGlsl120 = R"(
attribute vec2 vertex_position;
attribute vec4 vertex_color;
attribute vec2 vertex_texture;
attribute vec2 vertex_rad_ang;

varying vec4 frag_COLOR;
varying vec2 frag_TEXCOORD0;
varying vec2 frag_TEXCOORD1;

void main(){
    vec4 position = vec4(vertex_position, 0.0, 1.0);
    gl_Position = position;
    frag_COLOR = vertex_color;
    frag_TEXCOORD0 = vertex_texture;
    frag_TEXCOORD1 = vertex_rad_ang;
}
)";

const std::string kV2fC4fVertexShaderGlsl120 = R"(
attribute vec2 vertex_position;
attribute vec4 vertex_color;

uniform mat4 vertex_transformation;
uniform float vertex_point_size;

varying vec4 fragment_color;

void main(){
    gl_Position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);
    gl_PointSize = vertex_point_size;
    fragment_color = vertex_color;
}
)";

const std::string kV2fC4fFragmentShaderGlsl120 = R"(
varying vec4 fragment_color;

void main(){
    gl_FragColor = fragment_color;
}
)";

const std::string kV2fC4fT2fVertexShaderGlsl120 = R"(
attribute vec2 vertex_position;
attribute vec4 vertex_color;
attribute vec2 vertex_texture;

uniform mat4 vertex_transformation;

varying vec4 fragment_color;
varying vec2 fragment_texture;

void main(){
    gl_Position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);
    fragment_color = vertex_color;
    fragment_texture = vertex_texture;
}
)";

const std::string kV2fC4fT2fFragmentShaderGlsl120 = R"(
varying vec4 fragment_color;
varying vec2 fragment_texture;

uniform sampler2D texture_sampler;

varying vec4 color;

void main(){
    gl_FragColor = fragment_color * texture2D(texture_sampler,
                                              fragment_texture.st);
}
)";

const std::string kPresetShaderHeaderGlsl120 = R"(
#define  M_PI   3.14159265359
#define  M_PI_2 6.28318530718
#define  M_INV_PI_2  0.159154943091895

uniform float4   rand_frame;    // random float4, updated each frame
uniform float4   rand_preset;   // random float4, updated once per *preset*
uniform float4   _c0;           // .xy: multiplier to use on UV's to paste
                                // an image fullscreen, *aspect-aware*
                                // .zw = inverse.
uniform float4   _c1, _c2, _c3, _c4;
uniform float4   _c5;           // .xy = scale, bias for reading blur1
                                // .zw = scale, bias for reading blur2
uniform float4   _c6;           // .xy = scale, bias for reading blur3
                                // .zw = blur1_min, blur1_max
uniform float4   _c7;           // .xy ~= float2(1024,768)
                                // .zw ~= float2(1/1024.0, 1/768.0)
uniform float4   _c8;           // .xyzw ~= 0.5 + 0.5 * cos(
                                //   time * float4(~0.3, ~1.3, ~5, ~20))
uniform float4   _c9;           // .xyzw ~= same, but using sin()
uniform float4   _c10;          // .xyzw ~= 0.5 + 0.5 * cos(
                                //   time * float4(~0.005, ~0.008, ~0.013,
                                //                 ~0.022))
uniform float4   _c11;          // .xyzw ~= same, but using sin()
uniform float4   _c12;          // .xyz = mip info for main image
                                // (.x=#across, .y=#down, .z=avg)
                                // .w = unused
uniform float4   _c13;          // .xy = blur2_min, blur2_max
                                // .zw = blur3_min, blur3_max
uniform float4   _qa;           // q vars bank 1 [q1-q4]
uniform float4   _qb;           // q vars bank 2 [q5-q8]
uniform float4   _qc;           // q vars ...
uniform float4   _qd;           // q vars
uniform float4   _qe;           // q vars
uniform float4   _qf;           // q vars
uniform float4   _qg;           // q vars
uniform float4   _qh;           // q vars bank 8 [q29-q32]

// note: in general, don't use the current time w/the *dynamic* rotations!

// four random, static rotations, randomized at preset load time.
// minor translation component (<1).
uniform float4x3 rot_s1;
uniform float4x3 rot_s2;
uniform float4x3 rot_s3;
uniform float4x3 rot_s4;

// four random, slowly changing rotations.
uniform float4x3 rot_d1;
uniform float4x3 rot_d2;
uniform float4x3 rot_d3;
uniform float4x3 rot_d4;

// faster-changing.
uniform float4x3 rot_f1;
uniform float4x3 rot_f2;
uniform float4x3 rot_f3;
uniform float4x3 rot_f4;

// very-fast-changing.
uniform float4x3 rot_vf1;
uniform float4x3 rot_vf2;
uniform float4x3 rot_vf3;
uniform float4x3 rot_vf4;

// ultra-fast-changing.
uniform float4x3 rot_uf1;
uniform float4x3 rot_uf2;
uniform float4x3 rot_uf3;
uniform float4x3 rot_uf4;

// Random every frame.
uniform float4x3 rot_rand1;
uniform float4x3 rot_rand2;
uniform float4x3 rot_rand3;
uniform float4x3 rot_rand4;

#define time     _c2.x
#define fps      _c2.y
#define frame    _c2.z
#define progress _c2.w
#define bass _c3.x
#define mid  _c3.y
#define treb _c3.z
#define vol  _c3.w
#define bass_att _c4.x
#define mid_att  _c4.y
#define treb_att _c4.z
#define vol_att  _c4.w
#define q1 _qa.x
#define q2 _qa.y
#define q3 _qa.z
#define q4 _qa.w
#define q5 _qb.x
#define q6 _qb.y
#define q7 _qb.z
#define q8 _qb.w
#define q9 _qc.x
#define q10 _qc.y
#define q11 _qc.z
#define q12 _qc.w
#define q13 _qd.x
#define q14 _qd.y
#define q15 _qd.z
#define q16 _qd.w
#define q17 _qe.x
#define q18 _qe.y
#define q19 _qe.z
#define q20 _qe.w
#define q21 _qf.x
#define q22 _qf.y
#define q23 _qf.z
#define q24 _qf.w
#define q25 _qg.x
#define q26 _qg.y
#define q27 _qg.z
#define q28 _qg.w
#define q29 _qh.x
#define q30 _qh.y
#define q31 _qh.z
#define q32 _qh.w

#define aspect   _c0

// .xy = (w,h); .zw = (1/(float)w, 1/(float)h)
#define texsize  _c7

#define roam_cos _c8
#define roam_sin _c9
#define slow_roam_cos _c10
#define slow_roam_sin _c11
#define mip_x   _c12.x
#define mip_y   _c12.y
#define mip_xy  _c12.xy
#define mip_avg _c12.z
#define blur1_min _c6.z
#define blur1_max _c6.w
#define blur2_min _c13.x
#define blur2_max _c13.y
#define blur3_min _c13.z
#define blur3_max _c13.w

#define sampler_FC_main sampler_fc_main
#define sampler_PC_main sampler_pc_main
#define sampler_FW_main sampler_fw_main
#define sampler_PW_main sampler_pw_main

#define GetMain(uv) (tex2D(sampler_main,uv).xyz)
#define GetPixel(uv) (tex2D(sampler_main,uv).xyz)
#define GetBlur1(uv) (tex2D(sampler_blur1,uv).xyz*_c5.x + _c5.y)
#define GetBlur2(uv) (tex2D(sampler_blur2,uv).xyz*_c5.z + _c5.w)
#define GetBlur3(uv) (tex2D(sampler_blur3,uv).xyz*_c6.x + _c6.y)

#define lum(x) (dot(x,float3(0.32,0.49,0.29)))
#define tex2d tex2D
#define tex3d tex3D
)";

const std::string kBlurVertexShaderGlsl120 = R"(
attribute vec2 vertex_position;
attribute vec2 vertex_texture;

varying vec2 fragment_texture;

void main(){
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragment_texture = vertex_texture;
}
)";

std::string kBlur1FragmentShaderGlsl120 = R"(
varying vec2 fragment_texture;

uniform sampler2D texture_sampler;
uniform vec4 _c0; // source texsize (.xy), and inverse (.zw)
uniform vec4 _c1; // w1..w4
uniform vec4 _c2; // d1..d4
uniform vec4 _c3; // scale, bias, w_div

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
        (texture2D(texture_sampler, uv2 + vec2(d1 * srctexsize.z, 0)).xyz +
         texture2D(texture_sampler, uv2 + vec2(-d1 * srctexsize.z, 0)).xyz) *
            w1 +
        (texture2D(texture_sampler, uv2 + vec2(d2 * srctexsize.z, 0)).xyz +
         texture2D(texture_sampler, uv2 + vec2(-d2 * srctexsize.z, 0)).xyz) *
            w2 +
        (texture2D(texture_sampler, uv2 + vec2(d3 * srctexsize.z, 0)).xyz +
         texture2D(texture_sampler, uv2 + vec2(-d3 * srctexsize.z, 0)).xyz) *
            w3 +
        (texture2D(texture_sampler, uv2 + vec2(d4 * srctexsize.z, 0)).xyz +
         texture2D(texture_sampler, uv2 + vec2(-d4 * srctexsize.z, 0)).xyz) *
            w4;

    blur.xyz *= w_div;

    blur.xyz = blur.xyz*fscale + fbias;

    gl_FragColor.xyz = blur;
    gl_FragColor.w   = 1.0;
}
)";

std::string kBlur2FragmentShaderGlsl120 = R"(
varying vec2 fragment_texture;

uniform sampler2D texture_sampler;
uniform vec4 _c0; // source texsize (.xy), and inverse (.zw)
uniform vec4 _c5; // w1,w2,d1,d2
uniform vec4 _c6; // w_div, edge_darken_c1, edge_darken_c2, edge_darken_c3

void main(){
    // SHORT VERTICAL PASS 2:
    #define srctexsize _c0
    #define w1 _c5.x
    #define w2 _c5.y
    #define d1 _c5.z
    #define d2 _c5.w
    #define edge_darken_c1 _c6.y
    #define edge_darken_c2 _c6.z
    #define edge_darken_c3 _c6.w
    #define w_div   _c6.x

    // note: if you just take one sample at exactly uv.xy, you get an avg of 4
    // pixels.

    // + moves blur UP, LEFT by TWO-pixel increments! (since texture is 1/2 the
    // size of blur1_ps)
    vec2 uv2 = fragment_texture.xy + srctexsize.zw*vec2(0,0);

    vec3 blur =
        (texture2D(texture_sampler, uv2 + vec2(0, d1 * srctexsize.w)).xyz +
         texture2D(texture_sampler, uv2 + vec2(0, -d1 * srctexsize.w)).xyz) *
            w1 +
        (texture2D(texture_sampler, uv2 + vec2(0, d2 * srctexsize.w)).xyz +
         texture2D(texture_sampler, uv2 + vec2(0, -d2 * srctexsize.w)).xyz) *
            w2;
    blur.xyz *= w_div;

    // tone it down at the edges (only happens on 1st X pass!)
    float t = min(min(fragment_texture.x, fragment_texture.y),
                  1.0 - max(fragment_texture.x, fragment_texture.y));
    t = sqrt(t);
    t = edge_darken_c1 + edge_darken_c2 * clamp(t * edge_darken_c3, 0.0, 1.0);
    blur.xyz *= t;

    gl_FragColor.xyz = blur;
    gl_FragColor.w = 1.0;
}
)";

// Variants of shaders for GLSL3.3
const std::string kPresetWarpVertexShaderGlsl330 = R"(
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec2 vertex_texture;

uniform mat4 vertex_transformation;

out vec4 frag_COLOR;
out vec4 frag_TEXCOORD0;
out vec2 frag_TEXCOORD1;

void main(){
    vec4 position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);
    gl_Position = position;
    frag_COLOR = vertex_color;
    frag_TEXCOORD0.xy = vertex_texture;
    frag_TEXCOORD0.zw = position.xy;
    frag_TEXCOORD1 = vec2(0.0, 0.0);
}
)";

const std::string kPresetCompVertexShaderGlsl330 = R"(
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
)";

const std::string kV2fC4fVertexShaderGlsl330 = R"(
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;

uniform mat4 vertex_transformation;
uniform float vertex_point_size;

out vec4 fragment_color;

void main(){
    gl_Position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);
    gl_PointSize = vertex_point_size;
    fragment_color = vertex_color;
}
)";

const std::string kV2fC4fFragmentShaderGlsl330 = R"(
precision mediump float;

in vec4 fragment_color;
out vec4 color;

void main(){
    color = fragment_color;
}
)";

const std::string kV2fC4fT2fVertexShaderGlsl330 = R"(
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
)";

const std::string kV2fC4fT2fFragmentShaderGlsl330 = R"(
precision mediump float;

in vec4 fragment_color;
in vec2 fragment_texture;

uniform sampler2D texture_sampler;

out vec4 color;

void main(){
    color = fragment_color * texture(texture_sampler, fragment_texture.st);
}
)";

const std::string kPresetShaderHeaderGlsl330 = R"(
#define  M_PI   3.14159265359
#define  M_PI_2 6.28318530718
#define  M_INV_PI_2  0.159154943091895

uniform float4   rand_frame;    // random float4, updated each frame
uniform float4   rand_preset;   // random float4, updated once per *preset*
uniform float4   _c0;           // .xy: multiplier to use on UV's to paste
                                // an image fullscreen, *aspect-aware*
                                // .zw = inverse.
uniform float4   _c1, _c2, _c3, _c4;
uniform float4   _c5;           // .xy = scale, bias for reading blur1
                                // .zw = scale, bias for reading blur2
uniform float4   _c6;           // .xy = scale, bias for reading blur3
                                // .zw = blur1_min, blur1_max
uniform float4   _c7;           // .xy ~= float2(1024,768)
                                // .zw ~= float2(1/1024.0, 1/768.0)
uniform float4   _c8;           // .xyzw ~= 0.5 + 0.5 * cos(
                                //   time * float4(~0.3, ~1.3, ~5, ~20))
uniform float4   _c9;           // .xyzw ~= same, but using sin()
uniform float4   _c10;          // .xyzw ~= 0.5 + 0.5 * cos(
                                //   time * float4(~0.005, ~0.008, ~0.013,
                                //                 ~0.022))
uniform float4   _c11;          // .xyzw ~= same, but using sin()
uniform float4   _c12;          // .xyz = mip info for main image
                                // (.x=#across, .y=#down, .z=avg)
                                // .w = unused
uniform float4   _c13;          // .xy = blur2_min, blur2_max
                                // .zw = blur3_min, blur3_max
uniform float4   _qa;           // q vars bank 1 [q1-q4]
uniform float4   _qb;           // q vars bank 2 [q5-q8]
uniform float4   _qc;           // q vars ...
uniform float4   _qd;           // q vars
uniform float4   _qe;           // q vars
uniform float4   _qf;           // q vars
uniform float4   _qg;           // q vars
uniform float4   _qh;           // q vars bank 8 [q29-q32]

// note: in general, don't use the current time w/the *dynamic* rotations!

// four random, static rotations, randomized at preset load time.
// minor translation component (<1).
uniform float4x3 rot_s1;
uniform float4x3 rot_s2;
uniform float4x3 rot_s3;
uniform float4x3 rot_s4;

// four random, slowly changing rotations.
uniform float4x3 rot_d1;
uniform float4x3 rot_d2;
uniform float4x3 rot_d3;
uniform float4x3 rot_d4;

// faster-changing.
uniform float4x3 rot_f1;
uniform float4x3 rot_f2;
uniform float4x3 rot_f3;
uniform float4x3 rot_f4;

// very-fast-changing.
uniform float4x3 rot_vf1;
uniform float4x3 rot_vf2;
uniform float4x3 rot_vf3;
uniform float4x3 rot_vf4;

// ultra-fast-changing.
uniform float4x3 rot_uf1;
uniform float4x3 rot_uf2;
uniform float4x3 rot_uf3;
uniform float4x3 rot_uf4;

// Random every frame.
uniform float4x3 rot_rand1;
uniform float4x3 rot_rand2;
uniform float4x3 rot_rand3;
uniform float4x3 rot_rand4;

#define time     _c2.x
#define fps      _c2.y
#define frame    _c2.z
#define progress _c2.w
#define bass _c3.x
#define mid  _c3.y
#define treb _c3.z
#define vol  _c3.w
#define bass_att _c4.x
#define mid_att  _c4.y
#define treb_att _c4.z
#define vol_att  _c4.w
#define q1 _qa.x
#define q2 _qa.y
#define q3 _qa.z
#define q4 _qa.w
#define q5 _qb.x
#define q6 _qb.y
#define q7 _qb.z
#define q8 _qb.w
#define q9 _qc.x
#define q10 _qc.y
#define q11 _qc.z
#define q12 _qc.w
#define q13 _qd.x
#define q14 _qd.y
#define q15 _qd.z
#define q16 _qd.w
#define q17 _qe.x
#define q18 _qe.y
#define q19 _qe.z
#define q20 _qe.w
#define q21 _qf.x
#define q22 _qf.y
#define q23 _qf.z
#define q24 _qf.w
#define q25 _qg.x
#define q26 _qg.y
#define q27 _qg.z
#define q28 _qg.w
#define q29 _qh.x
#define q30 _qh.y
#define q31 _qh.z
#define q32 _qh.w

#define aspect   _c0

// .xy = (w,h); .zw = (1/(float)w, 1/(float)h)
#define texsize  _c7

#define roam_cos _c8
#define roam_sin _c9
#define slow_roam_cos _c10
#define slow_roam_sin _c11
#define mip_x   _c12.x
#define mip_y   _c12.y
#define mip_xy  _c12.xy
#define mip_avg _c12.z
#define blur1_min _c6.z
#define blur1_max _c6.w
#define blur2_min _c13.x
#define blur2_max _c13.y
#define blur3_min _c13.z
#define blur3_max _c13.w

#define sampler_FC_main sampler_fc_main
#define sampler_PC_main sampler_pc_main
#define sampler_FW_main sampler_fw_main
#define sampler_PW_main sampler_pw_main

#define GetMain(uv) (tex2D(sampler_main,uv).xyz)
#define GetPixel(uv) (tex2D(sampler_main,uv).xyz)
#define GetBlur1(uv) (tex2D(sampler_blur1,uv).xyz*_c5.x + _c5.y)
#define GetBlur2(uv) (tex2D(sampler_blur2,uv).xyz*_c5.z + _c5.w)
#define GetBlur3(uv) (tex2D(sampler_blur3,uv).xyz*_c6.x + _c6.y)

#define lum(x) (dot(x,float3(0.32,0.49,0.29)))
#define tex2d tex2D
#define tex3d tex3D
)";

const std::string kBlurVertexShaderGlsl330 = R"(
layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_texture;

out vec2 fragment_texture;

void main(){
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragment_texture = vertex_texture;
}
)";

std::string kBlur1FragmentShaderGlsl330 = R"(
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
)";

std::string kBlur2FragmentShaderGlsl330 = R"(
precision mediump float;

in vec2 fragment_texture;

uniform sampler2D texture_sampler;
uniform vec4 _c0; // source texsize (.xy), and inverse (.zw)
uniform vec4 _c5; // w1,w2,d1,d2
uniform vec4 _c6; // w_div, edge_darken_c1, edge_darken_c2, edge_darken_c3

out vec4 color;

void main(){
    // SHORT VERTICAL PASS 2:
    #define srctexsize _c0
    #define w1 _c5.x
    #define w2 _c5.y
    #define d1 _c5.z
    #define d2 _c5.w
    #define edge_darken_c1 _c6.y
    #define edge_darken_c2 _c6.z
    #define edge_darken_c3 _c6.w
    #define w_div   _c6.x

    // note: if you just take one sample at exactly uv.xy, you get an avg of 4
    // pixels.

    // + moves blur UP, LEFT by TWO-pixel increments! (since texture is 1/2 the
    // size of blur1_ps)
    vec2 uv2 = fragment_texture.xy + srctexsize.zw*vec2(0,0);

    vec3 blur =
        (texture(texture_sampler, uv2 + vec2(0, d1 * srctexsize.w)).xyz +
        texture(texture_sampler, uv2 + vec2(0, -d1 * srctexsize.w)).xyz) *
            w1 +
        (texture(texture_sampler, uv2 + vec2(0, d2 * srctexsize.w)).xyz +
        texture(texture_sampler, uv2 + vec2(0, -d2 * srctexsize.w)).xyz) *
            w2;
    blur.xyz *= w_div;

    // tone it down at the edges (only happens on 1st X pass!)
    float t = min(min(fragment_texture.x, fragment_texture.y),
                  1.0 - max(fragment_texture.x, fragment_texture.y));
    t = sqrt(t);
    t = edge_darken_c1 + edge_darken_c2 * clamp(t * edge_darken_c3, 0.0, 1.0);
    blur.xyz *= t;

    color.xyz = blur;
    color.w = 1.0;
}
)";

}  // namespace

StaticGlShaders::StaticGlShaders(bool use_gles) : use_gles_(use_gles) {
    version_ = QueryGlslVersion();

    if (use_gles_) {
        // If GLES is specified, override the version header.
        version_header_ = "#version 300 es";
        glsl_generator_version_ = M4::GLSLGenerator::Version::Version_300_ES;
    } else {
        if (version_.major < 3) {
            // For all GLSL versions less than 3.*, use the GLSL1.2 shaders.
            version_header_ = "#version 120";
            glsl_generator_version_ = M4::GLSLGenerator::Version::Version_120;
        } else {
            version_header_ = "#version 330";
            glsl_generator_version_ = M4::GLSLGenerator::Version::Version_330;
        }
    }
}

StaticGlShaders::GlslVersion StaticGlShaders::QueryGlslVersion() {
    /* In the linux desktop environment with --use-gles configured, the parsing of the GL_SHADING_LANGUAGE_VERSION
    string comes back as "OpenGL ES GLSL ES 3"
    And I think this was supposed to be parsing somethign like
    "3.10 etc etc"
    This caused exceptions to be raised in the std::stoi section;
    
    So - The parsing will look for <anything> <number> ['.' <number>] [<anything else>]
    and will default to 3.0 for the version in case of errors    
    */
    
    int major = 3;  /* 3.0 is default */
    int minor = 0;

    std::string glsl_version_string = reinterpret_cast<const char *>(
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    size_t version_len = glsl_version_string.length();
    /* make a c version of the string and do the conversion to integers manually just for this case */
    if (version_len) { // find the number
      size_t position = 0; 
      char *cstr = new char [version_len+1];

      strcpy(cstr,glsl_version_string.c_str()); 
      
      /* scan the anything before the number */
      while (position<version_len) {
        char ch = cstr[position];
	if ((ch >= '0') && (ch <= '9')) {
          break;
	  }
	position++;
	}
	
      /* scan the first number */
      {
       	int possible_major = 0;
        while (position<version_len) {
          char ch = cstr[position];
     	  if ((ch >= '0') && (ch <= '9')) {
	    possible_major = (possible_major * 10) + ch - '0';
	    }
	  else if (ch == '.') { /* got the minor */
	    int possible_minor = 0;
	    position++;
            while (position<version_len) {
	      ch = cstr[position];
     	      if ((ch >= '0') && (ch <= '9')) {
	        possible_minor = (possible_minor * 10) + ch - '0';
		}
	      else break;
	      position++;
	      }  /* while scanning the minor version */
	    if (possible_major) { /* set the minor version only if the major number is valid */
	      minor = possible_minor;
	      }
	    break; // We scanned it
	    }
	  else { /* not a number or period */
	    break;
	    }
	  position++;
	  } /* while scanning the major number */
        if (possible_major) {
	  major = possible_major;
	  }
	} /* scanning block */
      delete[] cstr;
      } /* if there is a string to parse */
    
    return GlslVersion{major, minor};
}

std::string StaticGlShaders::AddVersionHeader(std::string shader_text) {
    return version_header_ + "\n" + shader_text;
}

#define DECLARE_SHADER_ACCESSOR(name)                  \
    std::string StaticGlShaders::Get##name() {         \
        if (use_gles_) {                               \
            return AddVersionHeader(k##name##Glsl330); \
        }                                              \
        if (version_.major < 3) {                      \
            return AddVersionHeader(k##name##Glsl120); \
        }                                              \
        return AddVersionHeader(k##name##Glsl330);     \
    }

#define DECLARE_SHADER_ACCESSOR_NO_HEADER(name) \
    std::string StaticGlShaders::Get##name() {  \
        if (use_gles_) {                        \
            return k##name##Glsl330;            \
        }                                       \
        if (version_.major < 3) {               \
            return k##name##Glsl120;            \
        }                                       \
        return k##name##Glsl330;                \
    }

DECLARE_SHADER_ACCESSOR(PresetWarpVertexShader);
DECLARE_SHADER_ACCESSOR(PresetCompVertexShader);
DECLARE_SHADER_ACCESSOR(V2fC4fVertexShader);
DECLARE_SHADER_ACCESSOR(V2fC4fFragmentShader);
DECLARE_SHADER_ACCESSOR(V2fC4fT2fVertexShader);
DECLARE_SHADER_ACCESSOR(V2fC4fT2fFragmentShader);
DECLARE_SHADER_ACCESSOR(BlurVertexShader);
DECLARE_SHADER_ACCESSOR(Blur1FragmentShader);
DECLARE_SHADER_ACCESSOR(Blur2FragmentShader);
DECLARE_SHADER_ACCESSOR_NO_HEADER(PresetShaderHeader);
