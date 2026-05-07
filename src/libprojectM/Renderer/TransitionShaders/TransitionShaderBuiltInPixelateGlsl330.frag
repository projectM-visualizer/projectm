// Progressive pixelation/despixelation transition.
// Uses iProgressEased to drive block size from large (old preset) to 1x1 (new preset).

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;

    // Randomize max block size and aspect correction
    float maxBlock = mod(float(iRandStatic.x) * 0.01, 64.0) + 32.0;
    float progress = iProgressEased;

    // Block size shrinks as progress increases; bass kicks momentarily blow it up.
    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    float blockSize = mix(maxBlock, 1.0, progress);
    blockSize = max(1.0, blockSize * (1.0 + 0.6 * bassPump * (1.0 - progress)));

    // Quantize UVs to block grid
    vec2 grid = iResolution.xy / blockSize;
    vec2 pixelUV = floor(uv * grid) / grid;

    // Sample old preset with pixelated UVs, new preset with original UVs
    vec3 oldPixel = texture(iChannel0, pixelUV).xyz;
    vec3 newPixel = texture(iChannel1, uv).xyz;

    // Crossfade between pixelated old and sharp new
    fragColor = vec4(mix(oldPixel, newPixel, progress), 1.0);
}
