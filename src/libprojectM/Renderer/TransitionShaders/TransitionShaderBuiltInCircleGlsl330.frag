// Circular blend with soft edge and feathered chromatic aberration.
// RGB channels sample at slightly different radii inside the transition band.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float aspect = iAspectY / iAspectX;

    // Randomize direction and edge width
    float inOrOut = mod(float(iRandStatic.x) * 0.01, 2.0);

    float progress;
    vec3 imgInner, imgOuter;
    if (inOrOut < 1.0)
    {
        imgInner = texture(iChannel0, uv).xyz;
        imgOuter = texture(iChannel1, uv).xyz;
        progress = iProgressEased;
    }
    else
    {
        imgOuter = texture(iChannel0, uv).xyz;
        imgInner = texture(iChannel1, uv).xyz;
        progress = 1.0 - iProgressEased;
    }

    float blendWidth = mod(float(iRandStatic.y) * 0.001, 0.1) + 0.05;
    float chromaOffset = mod(float(iRandStatic.z) * 0.001, 0.02) + 0.005;
    progress = progress * (1.0 + blendWidth) - blendWidth;

    // Blending
    vec2 center = vec2(0.5);
    float rad = sqrt(center.x / aspect * center.x / aspect + center.y * center.y) * progress;
    float rad2 = rad + blendWidth;
    float r1 = sqrt((uv.x - center.x) / aspect * (uv.x - center.x) / aspect + (uv.y - center.y) * (uv.y - center.y));

    vec3 col;
    if (r1 > rad2)
    {
        col = imgInner;
    }
    else if (r1 > rad)
    {
        float v1 = (r1 - rad) / (rad2 - rad);
        float v2 = 1.0 - v1;

        // Chromatic aberration: sample R and B at slightly different radii
        float rOffset = r1 + chromaOffset * v2;
        float bOffset = r1 - chromaOffset * v2;

        vec2 rUV = center + (uv - center) * (rOffset / max(r1, 0.0001));
        vec2 bUV = center + (uv - center) * (bOffset / max(r1, 0.0001));

        float innerR = texture(inOrOut < 1.0 ? iChannel0 : iChannel1, clamp(rUV, 0.0, 1.0)).r;
        float innerB = texture(inOrOut < 1.0 ? iChannel0 : iChannel1, clamp(bUV, 0.0, 1.0)).b;
        float innerG = texture(inOrOut < 1.0 ? iChannel0 : iChannel1, uv).g;

        float outerR = texture(inOrOut < 1.0 ? iChannel1 : iChannel0, clamp(rUV, 0.0, 1.0)).r;
        float outerB = texture(inOrOut < 1.0 ? iChannel1 : iChannel0, clamp(bUV, 0.0, 1.0)).b;
        float outerG = texture(inOrOut < 1.0 ? iChannel1 : iChannel0, uv).g;

        vec3 chromaInner = vec3(innerR, innerG, innerB);
        vec3 chromaOuter = vec3(outerR, outerG, outerB);

        col = v1 * chromaInner + v2 * chromaOuter;
    }
    else
    {
        col = imgOuter;
    }

    // Output to screen
    fragColor = vec4(col, 1.0);
}
