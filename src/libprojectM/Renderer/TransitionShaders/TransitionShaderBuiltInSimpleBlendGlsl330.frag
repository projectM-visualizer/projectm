// Simple crossfade effect from old to new
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
    vec3 imgOld = texture(iChannel0, uv).xyz;
    vec3 imgNew = texture(iChannel1, uv).xyz;

    // Blending
    vec3 col = vec3((1.0 - iProgressCosine) * imgOld + iProgressCosine * imgNew);

    // Output to screen
    fragColor = vec4(col, 1.0);
}
