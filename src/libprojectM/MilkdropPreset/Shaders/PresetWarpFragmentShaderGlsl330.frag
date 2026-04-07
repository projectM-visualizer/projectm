in vec4 frag_COLOR;
in vec4 frag_TEXCOORD0;
in vec2 frag_TEXCOORD1;

uniform sampler2D texture_sampler;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 texCoords;

// Triangular-PDF dither: two hash-based uniform noises summed give a triangular
// distribution. Amplitude of 1/255 = 1 LSB in 8-bit, which whitens quantization
// noise and eliminates banding in the temporal feedback loop.
// Under HDR (16-bit float) this amplitude is scaled down to be negligible.
float tpdDither(vec2 fc) {
    float r1 = fract(sin(dot(fc, vec2(12.9898, 78.233))) * 43758.5453);
    float r2 = fract(sin(dot(fc, vec2(93.989,  67.345))) * 12345.678);
#ifdef PROJECTM_HDR_RENDERING
    return (r1 + r2 - 1.0) * (1.0 / 65535.0);
#else
    return (r1 + r2 - 1.0) * (1.0 / 255.0);
#endif
}

#ifdef PROJECTM_HDR_RENDERING
// Reinhard luminance tone-mapping: maps unbounded HDR values into [0,1] while
// preserving relative brightness. Operates on linear light before gamma encoding.
vec3 reinhardTonemap(vec3 c) {
    float lum = dot(c, vec3(0.2126, 0.7152, 0.0722));
    float lumMapped = lum / (1.0 + lum);
    return c * (lumMapped / max(lum, 0.0001));
}

// Piecewise sRGB gamma encode (IEC 61966-2-1).
// Converts linear light to display-referred sRGB for SDR output.
vec3 linearToSRGB(vec3 c) {
    vec3 lo = c * 12.92;
    vec3 hi = 1.055 * pow(clamp(c, 0.0, 1.0), vec3(1.0 / 2.4)) - 0.055;
    return mix(lo, hi, step(0.0031308, c));
}
#endif

void main() {
    // Main image
    color = frag_COLOR * texture(texture_sampler, frag_TEXCOORD0.xy);

#ifdef PROJECTM_HDR_RENDERING
    // HDR path: tone-map from unbounded linear light → [0,1], then gamma-encode.
    color.rgb = linearToSRGB(reinhardTonemap(color.rgb));

#ifdef PROJECTM_HDR_P3
    // BT.709 primaries → Display-P3 (D65 white point) color matrix.
    // Applied after sRGB gamma encode so the display interprets the signal as P3.
    // Source: ICC color science, column-major mat3 (GLSL convention).
    const mat3 bt709_to_p3 = mat3(
         0.8225,  0.0331,  0.0171,
         0.1774,  0.9669,  0.0724,
         0.0003,  0.0003,  0.9108);
    color.rgb = clamp(bt709_to_p3 * color.rgb, 0.0, 1.0);
#endif
#endif

    // Apply dither after tone-mapping, before quantization
    color.rgb += tpdDither(gl_FragCoord.xy);

    // Motion vector grid u/v coords for the next frame
    texCoords = frag_TEXCOORD0.xy;
}
