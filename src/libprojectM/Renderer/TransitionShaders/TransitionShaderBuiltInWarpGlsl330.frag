// Directional warp with added rotational twist.
// The image zooms and spins slightly as it transitions.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    vec3 imgOld = texture(iChannel0, uv).xyz;
    vec3 imgNew = texture(iChannel1, uv).xyz;

    // Randomize direction and twist amount; mid-range adds extra spin.
    float direction = mod(float(iRandStatic.x) * 0.01, 4.0);
    float twistAmount = mod(float(iRandStatic.y) * 0.001, 0.5) + 0.1;
    twistAmount *= 1.0 + 0.4 * clamp(iMidAtt, 0.0, 2.0);

    float progress = iProgressEased;

    // Choose warp axis based on random direction
    float coord;
    if (direction < 1.0) coord = uv.x;
    else if (direction < 2.0) coord = 1.0 - uv.x;
    else if (direction < 3.0) coord = uv.y;
    else coord = 1.0 - uv.y;

    // Blend factor
    float x = smoothstep(0.0, 1.0, (progress * 2.0 + coord - 1.0));

    // Apply twist: rotate UVs around center based on progress
    vec2 centeredUV = uv - 0.5;
    float rotation = (x - 0.5) * twistAmount;
    float cosR = cos(rotation);
    float sinR = sin(rotation);
    centeredUV = vec2(
        centeredUV.x * cosR - centeredUV.y * sinR,
        centeredUV.x * sinR + centeredUV.y * cosR
    );

    // Zoom warp
    vec2 warpedOld = centeredUV * (1.0 - x) + 0.5;
    vec2 warpedNew = centeredUV * x + 0.5;

    vec4 oldSample = texture(iChannel0, clamp(warpedOld, 0.0, 1.0));
    vec4 newSample = texture(iChannel1, clamp(warpedNew, 0.0, 1.0));

    fragColor = mix(oldSample, newSample, x);
}
