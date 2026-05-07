// Dual-edge angular sweep with random angle and transition zone width.
// Enhanced to optionally use two wipe fronts instead of one.

float atan_y_over_x(float y, float x)
{
    if (x > 0.0) return atan(y / x);
    else if (x < 0.0 && y >= 0.0) return atan(y / x) + 3.14159265;
    else if (x < 0.0 && y < 0.0) return atan(y / x) - 3.14159265;
    else return 1.57079632 * sign(y);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    vec3 imgOld = texture(iChannel0, uv).xyz;
    vec3 imgNew = texture(iChannel1, uv).xyz;

    // Randomize angle, blend width, and dual-edge mode
    float currentAngle = mod(float(iRandStatic.x), 360.0);
    float blendWidth = mod(float(iRandStatic.y) * 0.001, 0.3) + 0.1;
    bool dualEdge = mod(float(iRandStatic.z) * 0.01, 2.0) < 1.0;

    uv -= 0.5;
    float angle = radians(90.0) - radians(currentAngle) + atan_y_over_x(uv.y, uv.x);

    float len = length(uv) / sqrt(2.0);
    uv = vec2(cos(angle) * len, sin(angle) * len) + 0.5;

    float progress = iProgressEased;

    float mask;
    if (dualEdge)
    {
        // Two opposing wipe fronts converging from left and right
        float leftWipe = smoothstep(progress, progress + blendWidth, (uv.x / (1.0 + 2.0 * blendWidth)) + blendWidth);
        float rightWipe = 1.0 - smoothstep(1.0 - progress - blendWidth, 1.0 - progress, (uv.x / (1.0 + 2.0 * blendWidth)) + blendWidth);
        mask = leftWipe * rightWipe;
    }
    else
    {
        // Single wipe front
        mask = smoothstep(progress, progress + blendWidth, (uv.x / (1.0 + 2.0 * blendWidth)) + blendWidth);
    }

    vec3 col = mix(imgNew, imgOld, mask);

    // Output to screen
    fragColor = vec4(col, 1.0);
}
