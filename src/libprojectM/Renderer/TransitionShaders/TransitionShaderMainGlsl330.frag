
// Triangular-PDF dither for the transition blend output.
// Whitens quantization noise at the final display write.
float _prjm_tpd_dither(vec2 fc) {
    float r1 = fract(sin(dot(fc, vec2(12.9898, 78.233))) * 43758.5453);
    float r2 = fract(sin(dot(fc, vec2(93.989,  67.345))) * 12345.678);
#ifdef PROJECTM_HDR_RENDERING
    return (r1 + r2 - 1.0) * (1.0 / 65535.0);
#else
    return (r1 + r2 - 1.0) * (1.0 / 255.0);
#endif
}

void main() {
    _prjm_transition_out = vec4(1.0, 1.0, 1.0, 1.0);

    vec4 _user_out_color = vec4(1.0, 1.0, 1.0, 1.0);

    mainImage(_user_out_color, gl_FragCoord.xy);

    _prjm_transition_out = vec4(_user_out_color.xyz + _prjm_tpd_dither(gl_FragCoord.xy), 1.0);
}