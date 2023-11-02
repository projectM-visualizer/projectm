// Fractal, plasma-like transition effect with random scales and noise seeds.
// Source: https://www.shadertoy.com/view/MstBzf

float sinNoise(vec2 uv)
{
    return fract(abs(sin(uv.x * 180.0 + uv.y * 3077.0) * (float(iRandStatic.x) * .001)));
}

float valueNoise(vec2 uv, float scale)
{
    vec2 luv = fract(uv * scale);
    vec2 luvs = smoothstep(0.0, 1.0, fract(uv * scale));
    vec2 id = floor(uv * scale);
    float tl = sinNoise(id + vec2(0.0, 1.0));
    float tr = sinNoise(id + vec2(1.0, 1.0));
    float t = mix(tl, tr, luvs.x);

    float bl = sinNoise(id + vec2(0.0, 0.0));
    float br = sinNoise(id + vec2(1.0, 0.0));
    float b = mix(bl, br, luvs.x);

    return mix(b, t, luvs.y) * 2.0 - 1.0;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    vec3 imgOld = texture(iChannel0, uv).xyz;
    vec3 imgNew = texture(iChannel1, uv).xyz;

    uv.y /= iResolution.x/iResolution.y;

    float sinN = sinNoise(uv);

    float scale = mod(float(iRandStatic.y) * .01, 7.) * 4. + 4.;

    float fractValue = 0.;
    float amp = 1.;
    for(int i = 0; i < 16; i++)
    {
        fractValue += valueNoise(uv, float(i + 1) * scale) * amp;
        amp *= .5;
    }

    fractValue *= .25;
    fractValue += .5;

    float cutoff = smoothstep(iProgressCosine + .1, iProgressCosine - .1, fractValue);
    vec3 col = mix(imgOld, imgNew, cutoff);

    // Output to screen
    fragColor = vec4(col, 1.);
}