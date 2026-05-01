// Water drop / ripple transition — expanding circular distortion from a random center.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressCosine;

    // Random center point
    vec2 center = vec2(
        mod(float(iRandStatic.x) * 0.001, 0.6) + 0.2,
        mod(float(iRandStatic.y) * 0.001, 0.6) + 0.2
    );

    // Distance from ripple center
    vec2 delta = uv - center;
    delta.x *= iResolution.x / iResolution.y;
    float dist = length(delta);

    // Ripple parameters; bass adds extra wave amplitude.
    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    float rippleSpeed = 3.0 + 1.5 * bassPump;
    float rippleWidth = 0.15;
    float ripple = sin(dist * 20.0 - progress * rippleSpeed * 10.0) * 0.5 + 0.5;
    ripple *= exp(-dist * 3.0);
    ripple *= smoothstep(progress + rippleWidth, progress, dist);

    // Distort UVs based on ripple; safe normalization.
    vec2 distortDir = (dist > 1e-5) ? (delta / dist) : vec2(0.0);
    vec2 distort = distortDir * ripple * (0.05 + 0.025 * bassPump);
    vec2 distortedUV = clamp(uv + distort, 0.0, 1.0);

    // Sample presets
    vec3 oldImg = texture(iChannel0, distortedUV).xyz;
    vec3 newImg = texture(iChannel1, uv).xyz;

    // Ripple acts as the transition mask
    float mask = smoothstep(progress - 0.05, progress + 0.05, dist - ripple * 0.1);

    fragColor = vec4(mix(oldImg, newImg, mask), 1.0);
}
