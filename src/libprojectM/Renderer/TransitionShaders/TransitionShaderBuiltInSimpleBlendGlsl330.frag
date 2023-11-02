// Simple crossfade effect from old to new
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;

    // Blending
    fragColor = vec4(mix(texture(iChannel0, uv).xyz,
                         texture(iChannel1, uv).xyz,
                         iProgressCosine), 1.0);
}
