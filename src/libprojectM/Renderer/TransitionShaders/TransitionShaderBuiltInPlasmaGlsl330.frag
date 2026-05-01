// Plasma — turbulent fractal value-noise dissolve with audio-reactive distortion
// and a color-cycled molten edge. Bass pumps the turbulence; treble drives the cycle.

float _plasma_hash(vec2 p)
{
    float seed = float(iRandStatic.x) * 0.001 + 0.137;
    return fract(abs(sin(p.x * 0.018 + p.y * 0.3077) * (10.0 + seed * 50.0)));
}

float _plasma_value(vec2 uv, float scale)
{
    vec2 luv  = fract(uv * scale);
    vec2 luvs = smoothstep(0.0, 1.0, luv);
    vec2 id   = floor(uv * scale);
    float bl = _plasma_hash(id + vec2(0.0, 0.0));
    float br = _plasma_hash(id + vec2(1.0, 0.0));
    float tl = _plasma_hash(id + vec2(0.0, 1.0));
    float tr = _plasma_hash(id + vec2(1.0, 1.0));
    float b = mix(bl, br, luvs.x);
    float t = mix(tl, tr, luvs.x);
    return mix(b, t, luvs.y) * 2.0 - 1.0;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    vec2 nuv = uv;
    nuv.y /= max(iResolution.x / iResolution.y, 0.0001);

    // Audio-reactive turbulent advection of the noise field.
    float bassPump  = clamp(iBassAtt, 0.0, 2.0);
    float trebFlick = clamp(iTrebAtt, 0.0, 2.0);
    nuv += vec2(
        sin(nuv.y * 6.283 + iTime * 1.7) * 0.05 * bassPump,
        cos(nuv.x * 5.131 + iTime * 1.3) * 0.05 * bassPump
    );

    float scale = mod(float(iRandStatic.y) * 0.01, 7.0) * 4.0 + 4.0;

    float fractValue = 0.0;
    float amp = 1.0;
    float ampSum = 0.0;
    for (int i = 0; i < 12; i++)
    {
        fractValue += _plasma_value(nuv, float(i + 1) * scale) * amp;
        ampSum += amp;
        amp *= 0.5;
    }
    fractValue = fractValue / max(ampSum, 0.0001) * 0.5 + 0.5;

    float band = 0.18;
    float p    = iProgressCosine;
    float cutoff = smoothstep(p + band, p - band, fractValue);
    float edge   = smoothstep(p + band, p, fractValue) -
                   smoothstep(p, p - band, fractValue);
    edge = clamp(edge, 0.0, 1.0);

    vec2 sUV = clamp(uv, 0.0, 1.0);
    vec3 imgOld = texture(iChannel0, sUV).xyz;
    vec3 imgNew = texture(iChannel1, sUV).xyz;
    vec3 col = mix(imgOld, imgNew, cutoff);

    // Color-cycling molten edge — hue rotates with time + treble + per-transition seed.
    float hue = fract(iTime * 0.5 + trebFlick * 0.4 + float(iRandStatic.z) * 0.001);
    vec3 edgeCol = 0.5 + 0.5 * cos(6.28318 * (hue + vec3(0.0, 0.33, 0.67)));
    col = mix(col, edgeCol, edge * (0.55 + 0.35 * bassPump));

    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
