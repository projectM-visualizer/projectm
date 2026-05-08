// Multi-pass test transition — proof of concept for 2-pass rendering.
// Pass 0: simple eased crossfade.
// Pass 1: warm color tint + vignette applied on top.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressEased;

    if (iPass == 0)
    {
        // Pass 0 — basic crossfade output stored in intermediate FBO.
        vec3 oldImg = texture(iChannel0, uv).xyz;
        vec3 newImg = texture(iChannel1, uv).xyz;
        fragColor = vec4(mix(oldImg, newImg, progress), 1.0);
    }
    else
    {
        // Pass 1 — sample the pass 0 result and add a warm tint + vignette.
        vec3 col = texture(iLastPassTex, uv).xyz;

        // Warm tint that intensifies at the midpoint.
        float tintStrength = sin(progress * 3.14159265) * 0.15;
        col += vec3(0.12, 0.06, 0.02) * tintStrength;

        // Soft vignette (stronger at edges).
        vec2 vigUV = uv - 0.5;
        vigUV.x *= iAspectX / iAspectY;
        float vig = 1.0 - dot(vigUV, vigUV) * 0.35;
        col *= vig;

        fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
    }
}
