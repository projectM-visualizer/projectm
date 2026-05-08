// Enhanced crossfade with subtle vignette darkening at the midpoint.
// Adds atmosphere to the otherwise plain linear/cosine blend.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressEased;

    // Sample both presets
    vec4 oldColor = texture(iChannel0, uv);
    vec4 newColor = texture(iChannel1, uv);

    // Apply selected blending mode
    vec4 result;
    if (iBlendMode == 0)      result = blendAlpha(oldColor, newColor, progress);
    else if (iBlendMode == 1) result = blendAdditive(oldColor, newColor, progress);
    else if (iBlendMode == 2) result = blendMultiplicative(oldColor, newColor, progress);
    else if (iBlendMode == 3) result = blendScreen(oldColor, newColor, progress);
    else                      result = blendAlpha(oldColor, newColor, progress);

    vec3 col = result.xyz;

    // Vignette darkening strongest at progress = 0.5
    float vignetteStrength = sin(progress * 3.14159265) * 0.25;
    vec2 vignetteUV = uv - 0.5;
    vignetteUV.x *= iAspectX / iAspectY;
    float vignette = 1.0 - dot(vignetteUV, vignetteUV) * vignetteStrength;
    col *= vignette;

    // Slight contrast boost at midpoint for punch
    float contrast = 1.0 + sin(progress * 3.14159265) * 0.1;
    col = (col - 0.5) * contrast + 0.5;

    fragColor = vec4(col, 1.0);
}
