// Kaleidoscope — true reflective N-way symmetry (2/3/6/8) with animated rotation,
// audio-reactive zoom, and a smooth radial reveal. Mid drives rotation, bass drives zoom.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float aspect = iResolution.x / max(iResolution.y, 1.0);

    // Aspect-corrected centered coords so reflections look uniform on widescreen.
    vec2 c = uv - 0.5;
    c.x *= aspect;

    // Pick from {2, 3, 6, 8}-way symmetry per transition.
    int symPick = int(mod(float(iRandStatic.x), 4.0));
    float segments = symPick == 0 ? 2.0
                   : symPick == 1 ? 3.0
                   : symPick == 2 ? 6.0
                   : 8.0;
    float rotDir = (mod(float(iRandStatic.y) * 0.01, 2.0) < 1.0) ? 1.0 : -1.0;

    float p = iProgressCosine;

    // Polar.
    float angle  = atan(c.y, c.x);
    float radius = length(c);

    // True mirrored kaleidoscope: fold into [0, segAngle/2], reflect, then mirror back.
    float segAngle = 6.28318530718 / segments;
    float a = mod(angle, segAngle);
    a = abs(a - segAngle * 0.5);

    // Animate rotation with time + audio.
    float midPump = clamp(iMidAtt, 0.0, 2.0);
    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    float rotation = (iTime * 0.4 + p * 1.57079632) * rotDir + midPump * 0.6 * rotDir;

    // Audio-reactive zoom — radius breathes with bass.
    float zoom = 1.0 - 0.18 * bassPump * (1.0 - p);
    float r = radius * zoom;

    // Reconstruct sample UVs (undo aspect on x).
    vec2 kUV = vec2(cos(a + rotation), sin(a + rotation)) * r;
    kUV.x /= aspect;
    kUV += 0.5;

    // Sample old preset through kaleidoscope, new preset normally.
    vec3 oldImg = texture(iChannel0, clamp(kUV, 0.0, 1.0)).xyz;
    vec3 newImg = texture(iChannel1, uv).xyz;

    // Radial reveal — center opens first; smoothstep edge prevents banding.
    float revealRadius = p * 1.2;
    float revealEdge   = 0.18 + 0.10 * (1.0 - p);
    float radialMask   = 1.0 - smoothstep(revealRadius - revealEdge, revealRadius, radius);

    // Subtle highlight along the reveal seam.
    float seam = smoothstep(0.06, 0.0, abs(radius - revealRadius));
    vec3 col = mix(oldImg, newImg, radialMask);
    col += vec3(0.4, 0.6, 1.0) * seam * 0.25 * (0.4 + 0.6 * midPump);

    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
