// Directional motion blur wipe — directional blur that sweeps across the screen.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressCosine;

    // Randomize angle and blur intensity; bass amplifies the blur length.
    float angle = radians(mod(float(iRandStatic.x), 360.0));
    float blurAmount = mod(float(iRandStatic.y) * 0.001, 0.08) + 0.02;
    blurAmount *= 1.0 + 0.5 * clamp(iBassAtt, 0.0, 2.0);

    vec2 direction = vec2(cos(angle), sin(angle));

    // Wipe position moves across the screen
    float wipePos = dot(uv - 0.5, direction) + 0.5;
    float wipeWidth = 0.3;
    float wipeMask = smoothstep(progress - wipeWidth, progress, wipePos);

    // Motion blur samples
    int samples = 16;
    vec3 oldColor = vec3(0.0);
    vec3 newColor = vec3(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < samples; i++)
    {
        float t = float(i) / float(samples - 1);
        float weight = 1.0 - abs(t - 0.5) * 2.0;
        vec2 offset = direction * (t - 0.5) * blurAmount;

        vec2 sampUV = clamp(uv + offset, 0.0, 1.0);
        oldColor += texture(iChannel0, sampUV).xyz * weight;
        newColor += texture(iChannel1, sampUV).xyz * weight;
        totalWeight += weight;
    }

    oldColor /= totalWeight;
    newColor /= totalWeight;

    fragColor = vec4(mix(oldColor, newColor, wipeMask), 1.0);
}
