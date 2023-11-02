// Circular blend with soft edge, inside-out or outside-in
// Source: https://www.shadertoy.com/view/NdGfzG
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
    float aspect = iResolution.y / iResolution.x;

    // Randomize direction and edge width
    float inOrOut = mod(float(iRandStatic.x) * .01, 2.);

    float progress;
    vec3 imgInner, imgOuter;
    if (inOrOut < 1.)
    {
        imgInner = texture(iChannel0, uv).xyz;
        imgOuter = texture(iChannel1, uv).xyz;
        progress = iProgressCosine;
    }
    else
    {
        imgOuter = texture(iChannel0, uv).xyz;
        imgInner = texture(iChannel1, uv).xyz;
        progress = 1. - iProgressCosine;
    }
    float blendWidth = mod(float(iRandStatic.y) * .001, .1) + .05;
    progress = progress * (1. + blendWidth) - blendWidth;

    // Blending
    vec2 center = vec2(.5);
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
        float v1=(r1-rad)/(rad2-rad);
        float v2 = 1.0 - v1;
        col = v1 * imgInner + v2 * imgOuter;
    }
    else
    {
        col = imgOuter;
    }

    // Output to screen
    fragColor = vec4(col, 1.0);
}
