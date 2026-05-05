# Creative Brief: milk011 — "Nebula Core"

## 1. Concept Title
**Nebula Core** — A deep-space audio-visualizer featuring a multi-octave fbm nebula, an SDF ringed gas-giant, a dense orbital particle ring, and composite bloom with a shifting cyan/purple/magenta palette.

## 2. Visual Description
A slowly rotating nebula fills the screen with layered, cloud-like noise in cyan and purple hues. At dead-center sits a pulsing ringed planet with subtle atmospheric banding and a glowing rim. Hundreds of bright particles orbit the planet in a tilted ring, twinkling on the off-beats. A soft lens-flare bloom bleeds from the planet and the brightest particles, while a dark vignette pulls the eye toward the center. The entire palette breathes with the music: bass shakes the planet, mids shift the nebula color field, and treble triggers particle flares.

## 3. Color Palette
| Element | Base Color | Audio Modulation |
|---|---|---|
| Nebula Deep | `rgb(5, 0, 20)` | Darkened by `1.0 - bass_att * 0.2` |
| Nebula Mid | `rgb(0, 150, 230)` cyan | Blended toward purple via `mid_att` |
| Nebula Bright | `rgb(230, 0, 140)` magenta | Peaks in high fbm regions + `treb_att` |
| Planet Body | `rgb(230, 180, 100)` warm gold | Banding modulated by `sin(uv * freq + time)` |
| Planet Rim | `rgb(100, 200, 255)` ice blue | Rim-light intensity tied to `bass_att` |
| Planet Ring | `rgb(80, 200, 255)` cyan | Alpha pulses with `vol` |
| Particles | `rgb(200, 240, 255)` white-cyan | Twinkle driven by `treb_att` |
| Bloom / Flare | Additive white-cyan | Composite `GetBlur1/2/3` masked by luminance |

## 4. Audio Reactivity Strategy
| Channel | Target | Mechanism |
|---|---|---|
| `bass_att` | Planet pulse & rim glow | Scales planet radius `0.12 * (1.0 + 0.15 * bass_att)` |
| `bass_att` | Global brightness boost | `q10 = 0.9 + 0.3 * bass_att` applied to final pixel |
| `mid_att` | Nebula palette shift | `q7 = mid_att` drives `lerp` between cyan and purple |
| `mid_att` | Warp swirl speed | Increases spiral rotation rate in warp shader |
| `treb_att` | Particle twinkle intensity | Multiplies per-pixel sparkle amplitude `q12 = treb_att * 2.0` |
| `treb_att` | Asteroid shape alpha | `shapecode` additive alpha `0.4 + treb_att * 0.6` |
| `vol` | Ring visibility | `q5 = (bass + mid + treb) * 0.1` modulates ring opacity |

## 5. Component Plan

### A. per_frame (HLSL backticks)
- **Init**: Set `decay = 0.98`.
- **Bridge vars** (q1–q12):
  - `q1 = time`
  - `q2 = bass_att`
  - `q3 = mid_att`
  - `q4 = treb_att`
  - `q5 = (bass + mid + treb) * 0.1`
  - `q6 = 1.0 + 0.15 * bass_att` (planet pulse)
  - `q7 = mid_att`
  - `q8 = time * 5.0` (twinkle timer)
  - `q9 = time * 0.3` (ring rotation)
  - `q10 = 0.9 + 0.3 * bass_att` (global boost)
  - `q11 = time * 0.02` (nebula drift X)
  - `q12 = time * 0.03` (nebula drift Y)

### B. per_pixel (HLSL backticks) — Main Render Pass
This is the heavy lifter. It runs fullscreen and outputs directly to `ret`.

**Layers (drawn back-to-front):**
1. **Nebula Background** — 4-octave fbm using `hash()` and value noise. Color-mapped through cyan→purple→magenta.
2. **Planet Body** — SDF circle with atmospheric banding and rim lighting.
3. **Planet Ring** — 2D torus SDF with tilt simulation via `sin(angle)` modulation.
4. **Orbital Particles** — High-density per-pixel sparkle ring using angle/radial quantization and hash.

### C. shapecode_0 — Asteroid Belt (Instanced Shapes)
- `num_inst = 24`
- `sides = 5` (irregular asteroids)
- `additive = 1`
- Per-instance orbit using `instance` index, offset from per-pixel particle ring by `+0.04` radius.
- Color: cyan-white, alpha tied to `treb_att`.

### D. warp (HLSL backticks) — Subtle Spiral Distortion
- Applies a gentle barrel-spiral twist to the previous frame.
- Strength kept low (`0.1`) so it functions as background motion rather than dominant distortion.
- Uses `sampler_fw_main` (architect to verify exact sampler name for this engine).

### E. composite (HLSL backticks) — Bloom, Vignette, Grade
- Samples `GetBlur1`, `GetBlur2`, `GetBlur3`.
- Computes luminance mask; adds weighted blur as additive bloom.
- Applies corner vignette `saturate(1.0 - dot(uv-0.5, uv-0.5) * 1.2)`.
- Final saturation boost: `ret = pow(ret, float3(0.9, 0.95, 1.1))` to push cyans and magentas.

## 6. Key Math Formulas / Pseudocode

### 6.1 Value Noise (for fbm)
```hlsl
float hash(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float vnoise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    f = f * f * (3.0 - 2.0 * f); // smoothstep cubic
    float a = hash(i);
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}
```

### 6.2 FBM (4 octaves)
```hlsl
float fbm(float2 p)
{
    float v = 0.0;
    float a = 0.5;
    for (int i = 0; i < 4; i++)
    {
        v += a * vnoise(p);
        p = p * 2.1 + float2(1.7, 9.2); // offset breaks grid symmetry
        a *= 0.5;
    }
    return v;
}
```

### 6.3 Nebula Color Mapping
```hlsl
float n = fbm(uv * 2.0 + float2(q11, q12));
float3 cyan    = float3(0.0, 0.6, 0.9);
float3 purple  = float3(0.5, 0.1, 0.8);
float3 magenta = float3(0.9, 0.0, 0.5);

float3 col = lerp(cyan, purple, saturate(n * 2.0 - 0.2 + q7 * 0.3));
col = lerp(col, magenta, saturate(n * 1.5 - 0.5 + q4 * 0.2));
col *= 0.15 + 0.85 * n; // contrast
```

### 6.4 Ringed Planet SDF
```hlsl
float2 pc = uv - float2(0.5, 0.5);
float r = length(pc);
float planetRadius = 0.12 * q6;
float planet = smoothstep(planetRadius, planetRadius - 0.008, r);

// Banding
float band = sin(pc.y * 30.0 + q1 * 0.5) * 0.5 + 0.5;
float3 pCol = lerp(float3(0.9, 0.7, 0.4), float3(0.8, 0.5, 0.2), band);

// Rim
float rim = pow(1.0 - saturate(r / planetRadius), 3.0);
pCol += float3(0.3, 0.5, 0.8) * rim * q2;

// Ring (tilted torus in 2D)
float angle = atan2(pc.y, pc.x);
float tilt = 0.35 * abs(sin(angle));
float ringRad = 0.22 * q6;
float ringThick = 0.022;
float ringDist = abs(r - ringRad * (1.0 - tilt)) - ringThick * (1.0 - tilt);
float ring = smoothstep(0.0, -0.005, ringDist);

// Ring occultation (planet blocks back half)
float occ = smoothstep(planetRadius + 0.015, planetRadius - 0.005, r);
ring *= lerp(1.0, 0.15, occ); // back of ring fades

float3 ringCol = float3(0.4, 0.8, 1.0) * (0.6 + 0.4 * q5);
```

### 6.5 Orbital Particle Ring (per-pixel)
```hlsl
float orbitAngle = angle + q9;
float targetR = 0.32 + 0.02 * sin(orbitAngle * 3.0);
float radialDiff = abs(r - targetR);

float particleID = floor((orbitAngle + q1 * 0.4) * 50.0);
float h = frac(sin(particleID * 12.9898) * 43758.5453);
float presence = step(0.75, h);
float sz = 0.008 + h * 0.012;
float bright = presence * smoothstep(sz, 0.0, radialDiff);
float twinkle = 0.5 + 0.5 * sin(q8 + h * 50.0);
bright *= twinkle * (0.6 + q12);

float3 partCol = float3(0.8, 0.95, 1.0);
```

### 6.6 Composite Bloom
```hlsl
float3 base = tex2D(sampler_fc_main, uv).xyz; // verify sampler name
float3 b1 = GetBlur1(uv);
float3 b2 = GetBlur2(uv);
float3 b3 = GetBlur3(uv);
float lum = dot(base, float3(0.299, 0.587, 0.114));
float mask = smoothstep(0.25, 0.7, lum);
float3 bloom = (b1 * 0.5 + b2 * 0.35 + b3 * 0.15) * mask * 1.4;
ret = base + bloom;
// vignette
float2 vc = uv - 0.5;
ret *= saturate(1.0 - dot(vc, vc) * 1.3);
```

### 6.7 Warp (Optional)
```hlsl
float2 d = uv - 0.5;
float r = length(d);
float theta = atan2(d.y, d.x) + r * r * 0.2 + q1 * 0.03 + q3 * 0.1;
float2 wuv = float2(r * cos(theta), r * sin(theta)) + 0.5;
ret = tex2D(sampler_fw_main, wuv); // verify sampler name
```

### 6.8 shapecode_0 Asteroid Belt
```ini
shapecode_0_enabled=1
shapecode_0_sides=5
shapecode_0_additive=1
shapecode_0_num_inst=24
shapecode_0_r=0.5
shapecode_0_g=0.9
shapecode_0_b=1.0
shapecode_0_a=0.5
```
```hlsl
// shape_0_per_frame (backtick or milk syntax)
angle = (instance / 24.0) * 6.283 + q9 + instance * 0.1;
rad orbitR = 0.36 + 0.03 * sin(angle * 2.0);
x = 0.5 + cos(angle) * orbitR;
y = 0.5 + sin(angle) * orbitR;
rad = 0.012 + q4 * 0.008;
a = 0.4 + q4 * 0.6;
```

## 7. Performance Considerations
- **FBM cost**: 4 octaves × ~15 instructions = ~60 ALU ops per pixel. Keep the `hash()` function to a single `frac(sin(dot()))` — do not use slower multi-hash variants.
- **Branchless design**: Use `lerp`, `step`, and `smoothstep` instead of `if` statements in per_pixel HLSL. The GPU handles this much better.
- **Composite blur**: `GetBlur1/2/3` are precomputed mip-blur samplers; sampling them is cheap, but do not sample more than once per blur level.
- **Shape count**: `num_inst=24` is well within safe bounds. Do not exceed 50 instanced shapes if targeting 60 fps on integrated GPUs.
- **Particle ring**: The per-pixel particle math uses only `floor`, `frac`, `sin`, and `smoothstep` — all fast. The `50.0` angular density gives ~78 particles visible; adjust `50.0` down to `30.0` if fill-rate is limited.
- **Warp shader**: If the engine runs warp + per_pixel + composite sequentially, omit the warp shader on low-end devices; the nebula fbm already provides motion.
- **Precision**: All constants can stay `float`. No `double` needed.

## 8. File Name & Meta
```ini
[preset00]
fRating=5.0
fGammaAdj=1.0
fDecay=0.950
fWaveAlpha=0.001
fShader=0.0
zoom=1.0
rot=0.0
cx=0.5
cy=0.5
warp=0.010
sx=1.0
sy=1.0
ob_size=0.01
ib_size=0.01
mv_a=0.0
```

---
**Target file:** `custom_milk_fixed/milk011.milk`
**Engine target:** glsl.1ink.us custom Milkdrop (HLSL per_pixel, q-bridge, loop/draw_shape, shapecode_N, warp/comp backtick shaders)
