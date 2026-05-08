// HeatWave — refraction distortion simulating shimmering hot air.
// 2-pass implementation:
//   Pass 0: base distortion + chromatic crossfade between presets.
//   Pass 1: layered heat shimmer, warm tint, and soft central haze.

float _hw_n2(vec2 p)
{
    return texture(sampler_noise_lq, p).r;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float p = iProgressEased;

    // Heat strength is a hump centered on the midpoint.
    float heat = sin(p * 3.14159265);

    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    float trebPump = clamp(iTrebAtt, 0.0, 2.0);

    if (iPass == 0)
    {
        // ================================================================
        // Pass 0 — Distortion & Chromatic Crossfade
        // ================================================================

        float speed = 1.5 + 1.5 * trebPump;
        float ampX  = 0.012 * heat * (1.0 + 0.6 * bassPump);
        float ampY  = 0.025 * heat * (1.0 + 0.6 * bassPump);

        // Stratified shimmer — stronger near the bottom, like rising heat.
        float strat = mix(0.4, 1.4, 1.0 - uv.y);

        // Two layered noise lookups for organic boiling motion.
        vec2 nUV1 = vec2(uv.x * 3.0, uv.y * 4.0 + iTime * speed);
        vec2 nUV2 = vec2(uv.x * 7.0 - iTime * speed * 0.7, uv.y * 9.0 + iTime * speed * 1.3);
        float n1 = _hw_n2(nUV1) - 0.5;
        float n2 = _hw_n2(nUV2) - 0.5;

        vec2 distort = vec2(
            (n1 + n2 * 0.5) * ampX,
            (sin(uv.x * 14.0 + iTime * speed * 1.7) * 0.5 + n1) * ampY
        ) * strat;

        // Chromatic refraction — split RGB along the heat normal.
        float chroma = 0.012 * heat * (1.0 + 0.5 * trebPump);
        vec2 nrm = normalize(distort + vec2(0.0001));

        vec2 uvOldR = clamp(uv + distort + nrm * chroma, 0.0, 1.0);
        vec2 uvOldG = clamp(uv + distort,                 0.0, 1.0);
        vec2 uvOldB = clamp(uv + distort - nrm * chroma, 0.0, 1.0);
        vec2 uvNewR = clamp(uv + distort * 0.5 + nrm * chroma * 0.5, 0.0, 1.0);
        vec2 uvNewG = clamp(uv + distort * 0.5,                       0.0, 1.0);
        vec2 uvNewB = clamp(uv + distort * 0.5 - nrm * chroma * 0.5, 0.0, 1.0);

        vec3 oldCol = vec3(
            texture(iChannel0, uvOldR).r,
            texture(iChannel0, uvOldG).g,
            texture(iChannel0, uvOldB).b
        );
        vec3 newCol = vec3(
            texture(iChannel1, uvNewR).r,
            texture(iChannel1, uvNewG).g,
            texture(iChannel1, uvNewB).b
        );

        // Crossfade with a noise-driven mask so the dissolve looks heat-burned.
        float mask = smoothstep(p - 0.25 - n1 * 0.15, p + 0.25 + n2 * 0.15, uv.y * 0.4 + 0.3 + p * 0.4);
        vec3 col = mix(oldCol, newCol, mask);

        fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
        return;
    }
    else
    {
        // ================================================================
        // Pass 1 — Heat Shimmer, Warm Tint, and Central Haze
        // ================================================================

        vec3 col = texture(iLastPassTex, uv).xyz;

        // Warm tint at peak heat.
        col += vec3(0.10, 0.05, 0.0) * heat * 0.4;

        // Secondary heat shimmer — sample at time-varying offsets for a
        // "boiling air" layer on top of the already-distorted image.
        float speed = 1.5 + 1.5 * trebPump;
        vec2 shimmerOff = vec2(
            sin(uv.y * 8.0 + iTime * speed * 1.2) * 0.004 * heat,
            cos(uv.x * 6.0 + iTime * speed * 0.9) * 0.004 * heat
        );
        vec3 shimmer = texture(iLastPassTex, uv + shimmerOff).xyz;
        col = mix(col, shimmer, 0.22 * heat);

        // Soft bright haze in the center at peak heat.
        float centerHaze = (1.0 - length(uv - 0.5) * 1.4) * heat * 0.12;
        col += vec3(1.0, 0.7, 0.35) * max(centerHaze, 0.0);

        // Subtle edge darkening (vignette) that strengthens with heat.
        float vig = 1.0 - dot(uv - 0.5, uv - 0.5) * 0.5 * heat;
        col *= 0.85 + 0.15 * vig;

        fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
        return;
    }
}
