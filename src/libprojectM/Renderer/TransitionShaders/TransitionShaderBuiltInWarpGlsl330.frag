// Horizontal/vertical warp effect
// Source: https://www.shadertoy.com/view/ssj3Dh
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
    vec3 imgOld = texture(iChannel0, uv).xyz;
    vec3 imgNew = texture(iChannel1, uv).xyz;

    // Randomize direction
    float direction = mod(float(iRandStatic.x) * .01, 4.);
    float coord;
    if (direction < 1.) coord = uv.x;
    else if (direction < 2.) coord = 1. - uv.x;
    else if (direction < 3.) coord = uv.y;
    else coord = 1. - uv.y;

    // Blending
    float x = smoothstep(.0,1.0,(iProgressCosine * 2.0 + coord - 1.0));
    fragColor = mix(texture(iChannel0, (uv - .5) * (1. - x) + .5), texture(iChannel1, (uv - .5) * x + .5), x);
}
