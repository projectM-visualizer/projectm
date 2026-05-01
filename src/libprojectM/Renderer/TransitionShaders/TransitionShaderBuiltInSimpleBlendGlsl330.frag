// Enhanced crossfade with subtle vignette darkening at the midpoint.
// Adds atmosphere to the otherwise plain linear/cosine blend.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressCosine;

    // Sample both presets
    vec3 oldImg = texture(iChannel0, uv).xyz;
    vec3 newImg = texture(iChannel1, uv).xyz;

    // Basic cosine crossfade
    vec3 col = mix(oldImg, newImg, progress);

    // Vignette darkening strongest at progress = 0.5
    float vignetteStrength = sin(progress * 3.14159265) * 0.25;
    vec2 vignetteUV = uv - 0.5;
    vignetteUV.x *= iResolution.x / iResolution.y;
    float vignette = 1.0 - dot(vignetteUV, vignetteUV) * vignetteStrength;
    col *= vignette;

    // Slight contrast boost at midpoint for punch
    float contrast = 1.0 + sin(progress * 3.14159265) * 0.1;
    col = (col - 0.5) * contrast + 0.5;

    fragColor = vec4(col, 1.0);
}
