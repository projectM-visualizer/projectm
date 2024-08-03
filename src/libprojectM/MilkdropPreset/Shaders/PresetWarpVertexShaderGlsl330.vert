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
layout(location = 1) in vec2 rad_ang;
layout(location = 2) in vec4 transforms;
layout(location = 3) in vec2 center2;
layout(location = 4) in vec2 distance2;
layout(location = 5) in vec2 stretch;

uniform mat4 vertex_transformation;
uniform vec4 aspect;
uniform float warpTime;
uniform float warpScaleInverse;
uniform vec4 warpFactors;
uniform vec2 texelOffset;
uniform float decay;

out vec4 frag_color;
out vec4 frag_texcoord;
out vec2 frag_texcoord1;

void main(){
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
    u = (u - center2.x) / stretch.x + center2.x;
    v = (v - center2.y) / stretch.y + center2.y;

    // Warping
    u += warp * 0.0035 * sin(warpTime * 0.333 + warpScaleInverse * (pos.x * warpFactors.x - pos.y * warpFactors.w));
    v += warp * 0.0035 * cos(warpTime * 0.375 - warpScaleInverse * (pos.x * warpFactors.z + pos.y * warpFactors.y));
    u += warp * 0.0035 * cos(warpTime * 0.753 - warpScaleInverse * (pos.x * warpFactors.y - pos.y * warpFactors.z));
    v += warp * 0.0035 * sin(warpTime * 0.825 + warpScaleInverse * (pos.x * warpFactors.x + pos.y * warpFactors.w));

    // Rotation
    float u2 = u - center2.x;
    float v2 = v - center2.y;

    float cosRotation = cos(rot);
    float sinRotation = sin(rot);
    u = u2 * cosRotation - v2 * sinRotation + center2.x;
    v = u2 * sinRotation + v2 * cosRotation + center2.y;

    // Translation
    u -= distance2.x;
    v -= distance2.y;

    // Undo aspect ratio fix
    u = (u - 0.5) * invAspectX + 0.5;
    v = (v - 0.5) * invAspectY + 0.5;

    // Final half-texel translation
    u += texelOffset.x;
    v += texelOffset.y;

    frag_color = vec4(decay, decay, decay, 1.0);
    frag_texcoord.xy = vec2(u, v);
    frag_texcoord.zw = uv_original;
    frag_texcoord1 = rad_ang;
}