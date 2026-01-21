#include "MilkdropStaticShaders.hpp"

#include <Renderer/OpenGL.h>

#include <cstring>
#include <stdexcept>

namespace libprojectM {
namespace MilkdropPreset {

static std::string kBlur1FragmentShaderGlsl330 = R"(
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

static std::string kBlur2FragmentShaderGlsl330 = R"(
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
})";

static std::string kBlurVertexShaderGlsl330 = R"(
precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 2) in vec2 vertex_texture;

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
)";

static std::string kPresetCompVertexShaderGlsl330 = R"(
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
)";

static std::string kPresetMotionVectorsVertexShaderGlsl330 = R"(
precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;

uniform mat4 vertex_transformation;
uniform float length_multiplier;
uniform float minimum_length;

uniform sampler2D warp_coordinates;

out vec4 fragment_color;

void main() {
    // Input positions are given in texture coordinates (0...1), not the usual
    // screen coordinates.
    vec2 pos = vertex_position;

    if (gl_VertexID % 2 == 1)
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
)";

static std::string kPresetShaderHeaderGlsl330 = R"(
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

static std::string kPresetWarpFragmentShaderGlsl330 = R"(
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
)";

static std::string kPresetWarpVertexShaderGlsl330 = R"(
precision mediump float;

#define pos vertex_position
#define radius rad_ang.x
#define angle rad_ang.y
#define zoom transforms.x
#define zoomExp transforms.y
#define rot transforms.z
#define warp transforms.w

#define aspectX aspect.x
#define aspectY aspect.y
#define invAspectX aspect.z
#define invAspectY aspect.w

layout(location = 0) in vec2 vertex_position;
layout(location = 3) in vec2 rad_ang;
layout(location = 4) in vec4 transforms;
layout(location = 5) in vec2 warp_center;
layout(location = 6) in vec2 warp_distance;
layout(location = 7) in vec2 stretch;

uniform mat4 vertex_transformation;
uniform vec4 aspect;
uniform float warpTime;
uniform float warpScaleInverse;
uniform vec4 warpFactors;
uniform vec2 texelOffset;
uniform float decay;

out vec4 frag_COLOR;
out vec4 frag_TEXCOORD0;
out vec2 frag_TEXCOORD1;

void main() {
    gl_Position = vertex_transformation * vec4(pos, 0.0, 1.0);

    float zoom2 = pow(zoom, pow(zoomExp, radius * 2.0 - 1.0));
    float zoom2Inverse = 1.0 / zoom2;

    // Initial texture coordinates, with built-in zoom factor
    float u = pos.x * aspectX * 0.5 * zoom2Inverse + 0.5;
    float v = pos.y * aspectY * 0.5 * zoom2Inverse + 0.5;

    // original UV coordinates
    vec2 uv_original = vec2(pos.x * 0.5 + 0.5 + texelOffset.x,
                            pos.y * 0.5 + 0.5 + texelOffset.y);

    // Stretch on X, Y
    u = (u - warp_center.x) / stretch.x + warp_center.x;
    v = (v - warp_center.y) / stretch.y + warp_center.y;

    // Warping
    u += warp * 0.0035 * sin(warpTime * 0.333 + warpScaleInverse * (pos.x * warpFactors.x - pos.y * warpFactors.w));
    v += warp * 0.0035 * cos(warpTime * 0.375 - warpScaleInverse * (pos.x * warpFactors.z + pos.y * warpFactors.y));
    u += warp * 0.0035 * cos(warpTime * 0.753 - warpScaleInverse * (pos.x * warpFactors.y - pos.y * warpFactors.z));
    v += warp * 0.0035 * sin(warpTime * 0.825 + warpScaleInverse * (pos.x * warpFactors.x + pos.y * warpFactors.w));

    // Rotation
    float u2 = u - warp_center.x;
    float v2 = v - warp_center.y;

    float cosRotation = cos(rot);
    float sinRotation = sin(rot);
    u = u2 * cosRotation - v2 * sinRotation + warp_center.x;
    v = u2 * sinRotation + v2 * cosRotation + warp_center.y;

    // Translation
    u -= warp_distance.x;
    v -= warp_distance.y;

    // Undo aspect ratio fix
    u = (u - 0.5) * invAspectX + 0.5;
    v = (v - 0.5) * invAspectY + 0.5;

    // Final half-texel translation
    u += texelOffset.x;
    v += texelOffset.y;

    frag_COLOR = vec4(decay, decay, decay, 1.0);
    frag_TEXCOORD0.xy = vec2(u, v);
    frag_TEXCOORD0.zw = uv_original;
    frag_TEXCOORD1 = rad_ang;
}
)";

static std::string kTexturedDrawFragmentShaderGlsl330 = R"(
precision mediump float;

in vec4 fragment_color;
in vec2 fragment_texture;

uniform sampler2D texture_sampler;

out vec4 color;

void main(){
    color = fragment_color * texture(texture_sampler, fragment_texture.st);
}
)";

static std::string kTexturedDrawVertexShaderGlsl330 = R"(
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
)";

static std::string kUntexturedDrawFragmentShaderGlsl330 = R"(
precision mediump float;

in vec4 fragment_color;

out vec4 color;

void main(){
    color = fragment_color;
}
)";

static std::string kUntexturedDrawVertexShaderGlsl330 = R"(
precision mediump float;

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



MilkdropStaticShaders::MilkdropStaticShaders(bool useGLES)
    : m_useGLES(useGLES)
{
    if (m_useGLES)
    {
        // If GLES is enabled, use the embedded specification language variant.
        m_versionHeader = "#version 300 es";
        m_GLSLGeneratorVersion = M4::GLSLGenerator::Version::Version_300_ES;
    }
    else
    {
        m_versionHeader = "#version 330";
        m_GLSLGeneratorVersion = M4::GLSLGenerator::Version::Version_330;
    }
}

std::string MilkdropStaticShaders::AddVersionHeader(std::string shader_text)
{
    return m_versionHeader + "\n" + shader_text;
}

#define DECLARE_SHADER_ACCESSOR(name)              \
    std::string MilkdropStaticShaders::Get##name()       \
    {                                              \
        return AddVersionHeader(k##name##Glsl330); \
    }

#define DECLARE_SHADER_ACCESSOR_NO_HEADER(name) \
    std::string MilkdropStaticShaders::Get##name()    \
    {                                           \
        return k##name##Glsl330;                \
    }

DECLARE_SHADER_ACCESSOR(Blur1FragmentShader);
DECLARE_SHADER_ACCESSOR(Blur2FragmentShader);
DECLARE_SHADER_ACCESSOR(BlurVertexShader);
DECLARE_SHADER_ACCESSOR(PresetCompVertexShader);
DECLARE_SHADER_ACCESSOR(PresetMotionVectorsVertexShader);
DECLARE_SHADER_ACCESSOR_NO_HEADER(PresetShaderHeader);
DECLARE_SHADER_ACCESSOR(PresetWarpFragmentShader);
DECLARE_SHADER_ACCESSOR(PresetWarpVertexShader);
DECLARE_SHADER_ACCESSOR(TexturedDrawFragmentShader);
DECLARE_SHADER_ACCESSOR(TexturedDrawVertexShader);
DECLARE_SHADER_ACCESSOR(UntexturedDrawFragmentShader);
DECLARE_SHADER_ACCESSOR(UntexturedDrawVertexShader);


} // namespace MilkdropPreset
} // namespace libprojectM
