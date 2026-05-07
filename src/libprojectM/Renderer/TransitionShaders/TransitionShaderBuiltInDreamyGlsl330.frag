// Dreamy — heavy radial blur + soft glow + desaturation midpoint. The image
// dissolves through a hazy, washed-out dream state at progress=0.5. Mid drives
// the haze breathing.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float p = iProgressEased;

    // Haze peaks at midpoint.
    float haze = sin(p * 3.14159265);
    float midPump = clamp(iMidAtt, 0.0, 2.0);
    float blurAmt = 0.025 * haze * (1.0 + 0.4 * midPump);

    // 12 spiral blur taps × 2 textures = 24 samples.
    const int TAPS = 12;
    vec3 oldBlur = vec3(0.0);
    vec3 newBlur = vec3(0.0);
    float wsum = 0.0;
    for (int i = 0; i < TAPS; i++)
    {
        float t = float(i) / float(TAPS);
        float a = t * 6.28318530718 * 1.5;
        float r = blurAmt * (0.3 + t);
        vec2 off = vec2(cos(a), sin(a)) * r;
        vec2 sUV = clamp(uv + off, 0.0, 1.0);
        float w = 1.0 - t;
        oldBlur += texture(iChannel0, sUV).xyz * w;
        newBlur += texture(iChannel1, sUV).xyz * w;
        wsum += w;
    }
    oldBlur /= max(wsum, 0.0001);
    newBlur /= max(wsum, 0.0001);

    // Crossfade with smooth ease.
    float dissolve = smoothstep(0.0, 1.0, p);
    vec3 col = mix(oldBlur, newBlur, dissolve);

    // Desaturate at midpoint, lift toward soft pastel.
    float lum = dot(col, vec3(0.299, 0.587, 0.114));
    col = mix(col, vec3(lum), haze * 0.55);
    col += vec3(0.10, 0.08, 0.12) * haze * 0.6;

    // Bloom-ish glow: highlight regions push past 1.0 then soft-clip.
    vec3 hi = max(col - 0.55, 0.0) * (1.5 + 0.5 * midPump);
    col += hi * haze * 0.6;

    // Soft vignette to deepen the dream feel.
    vec2 vUV = uv - 0.5;
    vUV.x *= iAspectX / iAspectY;
    float vig = 1.0 - dot(vUV, vUV) * (0.45 * haze);

    col = 1.0 - exp(-col * 1.05);                   // gentle filmic squash
    col *= vig;

    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
