// PageCurl — fakes a 3D page-turn by rolling a diagonal seam across the screen.
// Polished 2-pass implementation:
//   Pass 0: geometry + base shading (old page, curl ribbon, new page).
//   Pass 1: aspect-correct glow, audio-reactive highlight, softer shadow,
//           subtle noise, and early-out for pixels far from the curl.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float aspect = iAspectX / iAspectY;

    // Random seam angle from a small set so it always feels deliberate.
    int dirPick = int(mod(float(iRandStatic.x), 4.0));
    vec2 dir;
    if (dirPick == 0)      dir = normalize(vec2( 1.0,  0.6));
    else if (dirPick == 1) dir = normalize(vec2(-1.0,  0.6));
    else if (dirPick == 2) dir = normalize(vec2( 1.0, -0.6));
    else                   dir = normalize(vec2(-1.0, -0.6));

    float p = iProgressEased;

    // Coord along the curl axis, normalized so the seam sweeps from -1..+1.
    vec2 c = uv - 0.5;
    c.x *= aspect;
    float along = dot(c, dir);
    float seam  = mix(-1.2 * 0.5 * (aspect + 1.0), 1.2 * 0.5 * (aspect + 1.0), p);
    float d     = along - seam;

    // Curl radius — audio-reactive so fast beats make a tighter curl.
    float curlRad = 0.18 + 0.05 * clamp(iMidAtt, 0.0, 2.0);

    if (iPass == 0)
    {
        // ================================================================
        // Pass 0 — Geometry & Base Shading
        // ================================================================

        if (d > 0.0)
        {
            // Untouched old page.
            fragColor = vec4(texture(iChannel0, uv).xyz, 1.0);
            return;
        }
        else if (d > -curlRad)
        {
            // The curl: parameterize by angle = pi*(1 - t), where t = -d/curlRad in [0,1].
            float t = -d / curlRad;
            float angle = 3.14159265 * t;

            // Project the curl back along the seam direction.
            float alongCurl = seam - sin(angle) * curlRad;
            vec2 cCurl = c + dir * (alongCurl - along);
            cCurl.x /= aspect;
            vec2 oldUV = clamp(cCurl + 0.5, 0.0, 1.0);
            vec3 oldCol = texture(iChannel0, oldUV).xyz;

            // Backface shading: darker, desaturated version of the old page.
            vec3 backTint = vec3(dot(oldCol, vec3(0.299, 0.587, 0.114))) * 0.55
                          + vec3(0.10, 0.08, 0.06);

            // Lighting: Lambert-ish term.
            float lambert = abs(cos(angle));
            float frontFace = step(0.5, 1.0 - t);
            vec3 paper = mix(backTint, oldCol, frontFace);
            paper *= 0.55 + 0.45 * lambert;

            fragColor = vec4(clamp(paper, 0.0, 1.0), 1.0);
            return;
        }
        else
        {
            // Revealed new page (raw — shadow added in pass 1).
            vec3 newCol = texture(iChannel1, uv).xyz;
            fragColor = vec4(newCol, 1.0);
            return;
        }
    }
    else
    {
        // ================================================================
        // Pass 1 — Lighting, Edge Glow, and Drop Shadow
        // ================================================================

        vec3 col = texture(iLastPassTex, uv).xyz;
        float distFromCurl = abs(d);

        // --- Early out: pixels far from the curl only need the shadow ---
        if (distFromCurl > curlRad + 0.2)
        {
            if (d < -curlRad)
            {
                float shadow = smoothstep(curlRad + 0.18, curlRad, -d) * 0.35;
                col *= (1.0 - shadow);
            }
            fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
            return;
        }

        // Seam direction in UV space (aspect-correct for isotropic offsets).
        vec2 dirUV = normalize(vec2(dir.x / aspect, dir.y));

        // --- Audio-reactive edge highlight ---
        float audioBoost = 1.0 + 0.35 * clamp(iBassAtt + iTrebAtt, 0.0, 2.0);
        float edgeHi = smoothstep(0.12, 0.0, distFromCurl) * 0.65 * audioBoost;
        col += vec3(1.0, 0.93, 0.80) * edgeHi;

        // --- Subtle per-frame noise on the highlight for organic feel ---
        float noise = fract(sin(dot(uv + float(iRandFrame.x) * 0.01,
                                     vec2(12.9898, 78.233))) * 43758.5453);
        col += vec3(1.0, 0.95, 0.85) * noise * edgeHi * 0.12;

        // --- Soft luminous glow by sampling neighbors along the seam ---
        // Wider kernel (7 taps) for a creamier highlight bloom.
        vec3 glow = vec3(0.0);
        for (int i = -3; i <= 3; i++)
        {
            vec2 off = dirUV * float(i) * 0.004;
            glow += texture(iLastPassTex, uv + off).xyz;
        }
        glow /= 7.0;

        // Blend glow most strongly right at the curl seam.
        float edgeMask = smoothstep(curlRad + 0.08, 0.0, distFromCurl);
        col = mix(col, glow * 1.1, edgeMask * 0.3);

        // --- Subtle chromatic aberration near the curl edge ---
        if (edgeMask > 0.01)
        {
            vec2 abOff = dirUV * 0.0025 * edgeMask;
            float r = texture(iLastPassTex, uv + abOff).r;
            float b = texture(iLastPassTex, uv - abOff).b;
            col.r = mix(col.r, r, 0.4);
            col.b = mix(col.b, b, 0.4);
        }

        // --- Drop shadow on the new page just below the curl ---
        if (d < -curlRad)
        {
            float shadow = smoothstep(curlRad + 0.18, curlRad, -d) * 0.35;
            col *= (1.0 - shadow);
        }

        fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
        return;
    }
}
