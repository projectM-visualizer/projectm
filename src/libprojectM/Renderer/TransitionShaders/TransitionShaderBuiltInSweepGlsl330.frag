// Side-to-side sweep with random angle and transition zone width
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
    vec3 imgOld = texture(iChannel0, uv).xyz;
    vec3 imgNew = texture(iChannel1, uv).xyz;

    // Blending
    float currentAngle = mod(float(iRandStatic.x), 360.);
    float blendWidth = mod(float(iRandStatic.y), .25) + .05;

    uv -= .5;
    float angle = radians(90.0) - radians(currentAngle) + atan(uv.y, uv.x);

    float len = length(uv) / sqrt(2.);
    uv = vec2(cos(angle) * len, sin(angle) * len) + .5;

    vec3 col = mix(imgOld, imgNew, smoothstep(prog, prog + blendWidth, (uv.x / (1.0 + 2. * blendWidth)) + blendWidth));

    // Output to screen
    fragColor = vec4(col, 1.0);
}
