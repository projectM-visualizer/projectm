// SliceSwipe — vertical or horizontal slices that slide off-screen with staggered
// per-slice timing, revealing the new preset behind. Random direction mix per
// slice so it feels organic rather than mechanical.

float _ss_hash(float n) { return fract(sin(n * 78.233) * 43758.5453); }

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float p = iProgressBicubic;

    // Random orientation and slice count.
    bool vertical = mod(float(iRandStatic.x) * 0.01, 2.0) < 1.0;
    float slices  = floor(mod(float(iRandStatic.y) * 0.01, 14.0)) + 8.0;   // 8..22

    // Per-slice index along the swipe axis.
    float axis  = vertical ? uv.x : uv.y;
    float other = vertical ? uv.y : uv.x;
    float idx   = floor(axis * slices);

    // Per-slice deterministic delay [0, 0.5] and direction sign.
    float seed  = idx + float(iRandStatic.z) * 0.001;
    float delay = _ss_hash(seed) * 0.5;
    float sgn   = (_ss_hash(seed + 17.0) > 0.5) ? 1.0 : -1.0;

    // Local progress for this slice.
    float pSlice = clamp((p - delay) / max(1.0 - delay, 0.0001), 0.0, 1.0);
    pSlice = smoothstep(0.0, 1.0, pSlice);

    // Slice-local UV: shift the slice "off" by pSlice in `other` direction.
    float localOther = other - sgn * pSlice;

    // If the slice has slid out of [0,1], pixel here is the new preset.
    bool gone = (localOther < 0.0) || (localOther > 1.0);

    vec3 col;
    if (gone)
    {
        col = texture(iChannel1, uv).xyz;
    }
    else
    {
        vec2 sliceUV = vertical ? vec2(uv.x, localOther) : vec2(localOther, uv.y);
        sliceUV = clamp(sliceUV, 0.0, 1.0);
        vec3 oldCol = texture(iChannel0, sliceUV).xyz;
        vec3 newCol = texture(iChannel1, uv).xyz;

        // Fade in the new preset slightly under the slice as it travels —
        // adds depth so the slice isn't a flat plane.
        float underFade = 0.20 * pSlice;
        col = mix(oldCol, newCol, underFade);

        // Trailing-edge highlight where the slice meets the next slice.
        float edge = vertical ? abs(uv.x - idx / slices) : abs(uv.y - idx / slices);
        edge = 1.0 - smoothstep(0.0, 0.004, edge);
        col += vec3(edge) * 0.15 * pSlice;
    }

    // Subtle audio modulation: bass thumps add a brief darkening sheen across
    // the active slices.
    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    col *= 1.0 - 0.08 * bassPump * (1.0 - p) * sin(p * 3.14159265);

    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
