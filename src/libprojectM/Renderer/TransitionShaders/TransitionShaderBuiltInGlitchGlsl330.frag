// Glitch transition — RGB channel split + horizontal displacement driven by noise and randomness.

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressEased;

    // Randomize glitch intensity and frequency
    float intensity = mod(float(iRandStatic.x) * 0.001, 0.3) + 0.1;
    float freq = mod(float(iRandStatic.y) * 0.01, 20.0) + 10.0;

    // Animated noise using frame random for jitter
    float noise = hash(vec2(floor(uv.y * freq), float(iRandFrame.x)));
    float noise2 = hash(vec2(floor(uv.y * freq * 0.5), float(iRandFrame.y)));

    // Glitch is strongest in the middle of the transition; bass kicks add punch.
    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    float glitchStrength = intensity * sin(progress * 3.14159265) * (1.0 + 0.6 * bassPump);

    // Horizontal displacement
    float shift = (noise - 0.5) * glitchStrength;
    float shiftR = shift * (1.0 + noise2 * 0.5);
    float shiftB = shift * (1.0 - noise2 * 0.5);

    vec2 uvR = clamp(uv + vec2(shiftR, 0.0), 0.0, 1.0);
    vec2 uvG = clamp(uv + vec2(shift,  0.0), 0.0, 1.0);
    vec2 uvB = clamp(uv + vec2(shiftB, 0.0), 0.0, 1.0);

    float r = texture(iChannel0, uvR).r * (1.0 - progress) + texture(iChannel1, uvR).r * progress;
    float g = texture(iChannel0, uvG).g * (1.0 - progress) + texture(iChannel1, uvG).g * progress;
    float b = texture(iChannel0, uvB).b * (1.0 - progress) + texture(iChannel1, uvB).b * progress;

    // Scanline darkening
    float scanline = sin(uv.y * 800.0) * 0.04 * glitchStrength;

    fragColor = vec4(r - scanline, g - scanline, b - scanline, 1.0);
}
