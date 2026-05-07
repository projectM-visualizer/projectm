// ZoomBlur — radial zoom blur with chromatic aberration and a randomized focal point.
// 10 zoom iterations × 3 chroma channels on the fading-out preset, plus a single
// sharp sample of the incoming preset (≈31 texture samples). Bass amps the blur;
// treble pumps the chroma.

const float PI = 3.14159265359;

float _zb_easeInOutSine(float t) { return 0.5 - 0.5 * cos(PI * clamp(t, 0.0, 1.0)); }

float _zb_easeInOutExpo(float t)
{
    t = clamp(t, 0.0, 1.0);
    if (t == 0.0) return 0.0;
    if (t == 1.0) return 1.0;
    if (t < 0.5)  return 0.5 * pow(2.0, 20.0 * t - 10.0);
    return 1.0 - 0.5 * pow(2.0, -20.0 * t + 10.0);
}

float _zb_rand(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float p = iProgressEased;

    // Randomized focal point: 50% chance to pick a non-center point.
    bool offCenter = mod(float(iRandStatic.w) * 0.01, 2.0) < 1.0;
    vec2 focus = vec2(0.5);
    if (offCenter)
    {
        focus = vec2(
            mod(float(iRandStatic.x) * 0.001, 0.6) + 0.2,
            mod(float(iRandStatic.y) * 0.001, 0.6) + 0.2
        );
    }

    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    float trebPump = clamp(iTrebAtt, 0.0, 2.0);

    // Strength peaks at the midpoint, bass adds punch.
    float baseStrength = 0.28 + 0.15 * bassPump;
    float strength = _zb_easeInOutSine(1.0 - abs(p * 2.0 - 1.0)) * baseStrength;

    float dissolve = _zb_easeInOutExpo(p);

    float chroma = 0.020 + 0.030 * trebPump;
    vec2 toFocus = focus - uv;

    // Hash-jitter masks the fixed sample count.
    float offset = _zb_rand(uv) * 0.5;

    const int SAMPLES = 10;
    vec3 oldCol = vec3(0.0);
    float total = 0.0;
    for (int i = 0; i < SAMPLES; i++)
    {
        float percent = (float(i) + offset) / float(SAMPLES);
        float weight  = percent - percent * percent;        // tent

        float pR = percent * (1.0 + chroma);
        float pG = percent;
        float pB = percent * (1.0 - chroma);

        vec2 uvR = clamp(uv + toFocus * pR * strength, 0.0, 1.0);
        vec2 uvG = clamp(uv + toFocus * pG * strength, 0.0, 1.0);
        vec2 uvB = clamp(uv + toFocus * pB * strength, 0.0, 1.0);

        oldCol += vec3(
            texture(iChannel0, uvR).r,
            texture(iChannel0, uvG).g,
            texture(iChannel0, uvB).b
        ) * weight;
        total += weight;
    }
    oldCol /= max(total, 0.0001);

    vec3 newCol = texture(iChannel1, clamp(uv, 0.0, 1.0)).xyz;

    vec3 col = mix(oldCol, newCol, dissolve);
    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
