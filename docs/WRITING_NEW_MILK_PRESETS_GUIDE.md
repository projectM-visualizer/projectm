# ProjectM Milkdrop Preset Guide — 2026 Edition

> **Target runtime:** projectM 4.1.x  
> **Preset format:** MilkDrop 2.x (`.milk`)  
> **Shader language:** HLSL 9 subset (transpiled to GLSL at runtime)  
> **Last updated:** 2026-04-30

**TL;DR —** Copy the [5-minute preset](#1️⃣-quick-start-beginner) at the bottom of this page, load it in projectM, and you’re rendering. For deeper dives, jump to [Advanced Topics](#8️⃣-advanced-topics).

---

## 📑 Table of Contents

1. [Quick-Start (Beginner)](#1️⃣-quick-start-beginner)
2. [Core Parameters & Versioning](#2️⃣-core-parameters--versioning)
3. [Expression Code System](#3️⃣-expression-code-system)
4. [Shader System (Warp & Composite)](#4️⃣-shader-system-warp--composite)
5. [Custom Waves & Shapes](#5️⃣-custom-waves--shapes)
6. [Performance & Debugging Tips](#6️⃣-performance--debugging-tips)
7. [Tooling & Workflow](#7️⃣-tooling--workflow)
8. [Advanced Topics](#8️⃣-advanced-topics)
9. [Common Pitfalls Checklist](#9️⃣-common-pitfalls-checklist)
10. [Appendix — Glossary & Quick Reference](#10️⃣-appendix--glossary--quick-reference)

---

## 1️⃣ Quick-Start (Beginner)

**[Beginner]**

### Your First Preset in 5 Steps

1. **Create a file** named `my_first.milk`.
2. **Paste the header:**

```ini
[preset00]
MILKDROP_PRESET_VERSION=200
PSVERSION_WARP=3
PSVERSION_COMP=3
fDecay=0.98
fGammaAdj=2.0
zoom=1.0
```

3. **Add a breathing zoom:**

```ini
per_frame_1=zoom=0.9+0.2*sin(time*1.5);
```

4. **Add a tiny warp shader:**

```ini
warp_1=`float d = length(uv-0.5);
warp_2=`float a = atan2(uv.y-0.5, uv.x-0.5);
warp_3=`float amp = 0.12 * sin(time);
warp_4=`uv += amp * sin(6.0*a + time) * d;
warp_5=`ret = tex2D(sampler_main, uv).xyz;
```

5. **Test it:** Load the file in the **projectM SDL test UI** (`projectM-Test-UI`) or **NestDrop**. You should see a pulsing tunnel.

> **Tip:** Press `F5` in the SDL test UI to reload the preset instantly after editing.

### What Each Line Does

| Line | Purpose |
|------|---------|
| `MILKDROP_PRESET_VERSION=200` | Tells the parser this is a MilkDrop 2.x preset |
| `PSVERSION_WARP=3` / `PSVERSION_COMP=3` | Enables modern shader features |
| `fDecay` / `fGammaAdj` | Screen persistence and gamma |
| `per_frame_1=` | Runs once per frame on the CPU. Cheap. |
| `warp_1=` … `warp_5=` | GPU shader lines. Each starts with a backtick `` ` ``. |

---

## 2️⃣ Core Parameters & Versioning

**[Beginner]**

> **TL;DR**
> - Use `MILKDROP_PRESET_VERSION=200` for all new presets.
> - Set `PSVERSION_WARP=3` and `PSVERSION_COMP=3` to unlock modern shaders.
> - The whole file must be < 1 MB.

### Version Tags

| Tag | Value | Meaning |
|-----|-------|---------|
| `MILKDROP_PRESET_VERSION` | `200` | MilkDrop 2.0 baseline (use this) |
| `PSVERSION_WARP` | `3` | Modern warp shader features |
| `PSVERSION_COMP` | `3` | Modern composite shader features |

### Essential Parameters

| Category | Key | Default | Purpose |
|----------|-----|---------|---------|
| Post-processing | `fDecay` | `0.98` | Screen persistence |
| | `fGammaAdj` | `2.0` | Gamma correction |
| | `bBrighten`, `bDarken`, `bSolarize`, `bInvert` | `0` | Boolean effects |
| Motion | `zoom` | `1.0` | Overall zoom |
| | `rot` | `0.0` | Rotation |
| | `warp` | `1.0` | Distortion amount |
| | `cx`, `cy` | `0.5` | Rotation center |
| | `dx`, `dy` | `0.0` | Translation push |
| | `sx`, `sy` | `1.0` | Stretch X/Y |
| Waveform | `nWaveMode` | `0` | Built-in wave mode (0–7) |
| | `fWaveAlpha` | `0.8` | Wave opacity |
| | `wave_r`, `wave_g`, `wave_b` | `1.0` | Wave color |
| Blur | `b1n`, `b2n`, `b3n` | `0.0` | Blur minima |
| | `b1x`, `b2x`, `b3x` | `1.0` | Blur maxima |
| Motion vectors | `nMotionVectorsX`, `nMotionVectorsY` | `12.0`, `9.0` | Grid size |

> **Note:** See the [Appendix](#10️⃣-appendix--glossary--quick-reference) for the complete parameter table.

---

## 3️⃣ Expression Code System

**[Beginner → Intermediate]**

> **TL;DR**
> - `per_frame_*` → `q1`–`q32` → everything else.
> - `t1`–`t8` = local memory per wave/shape.
> - `reg00`–`reg99` + `megabuf` = global persistent memory.
> - Never write to a variable that is also read in the same expression; use temporaries.

### Execution Contexts

| Context | When | Typical Use |
|---------|------|-------------|
| `per_frame_init` | Once on preset load | Initialize Q-vars, registers |
| `per_frame_*` | Every frame, before rendering | Global transforms, audio reactivity, Q-vars |
| `per_pixel_*` | Per warp-mesh vertex | Vertex-local distortion |
| `wave_N_per_frame_*` | Per frame, per custom wave | Change wave color, sample count |
| `wave_N_per_point_*` | Per sample point | Compute X/Y positions |
| `shape_N_per_frame_*` | Per frame, per custom shape | Animate rotation, size, position |
| `warp_*` / `comp_*` | GPU pixel shader | Pixel-level effects |

### Code Assembly Rules

Numbered keys are concatenated by index. **A missing number stops parsing cold.**

```ini
per_frame_1=zoom=1.0;
per_frame_2=rot=time*0.1;
per_frame_3=cx=0.5+0.1*sin(time);
```

Applies to all numbered sequences: `per_frame_*`, `per_frame_init_*`, `per_pixel_*`, `warp_*`, `comp_*`, and all wave/shape code.

### The Three Data Paths

```
per_frame_*          (CPU, once per frame)
    │
    ├──► q1–q32 ──────► per_pixel, waves, shapes, shaders
    │
    ├──► reg00–reg99 ─► global persistent state (all contexts)
    │
    └──► megabuf ─────► global memory array (all contexts)

waves/shapes:
    └──► t1–t8 ───────► local state (init → per_frame → per_point)
```

### Q-Variables (The Bridge)

**32 Q variables** (`q1` through `q32`):
- **Set in:** `per_frame_init` and `per_frame`
- **Read in:** `per_pixel`, custom waves, custom shapes, shaders
- **Scope:** Copied from `per_frame` into other contexts each frame

Example:

```ini
per_frame_init=SPEED=1.0;
per_frame_1=q1=sin(time*SPEED);
          =q2=cos(time*SPEED);
per_pixel_1=zoom=1.0+q1*0.1;
            zoom=zoom+q2*rad*0.05;
```

### T-Variables (Local State)

**T variables** (`t1` through `t8`): local to each custom wave/shape, persisting across `init` → `per_frame` → `per_point`.

```ini
wave_0_init_1=t1=0;
              t2=1;
wave_0_per_frame_1=t1=t1+0.01;
wave_0_per_point_1=x=0.5+t1*sin(sample*2*pi);
```

### Registers & Megabuf

**Registers (`reg00`–`reg99`):** 100 double-precision values shared across all contexts, persisting across frames.

```ini
per_frame_init=reg00=0;
per_frame_1=reg00=reg00+1;
```

**Megabuffer:** Global memory via `gmegabuf` / `megabuf(index)`.

```ini
per_pixel_1=gmegabuf[x*100+y] = rad;
```

### Order-of-Evaluation Safety

Never write to a variable that is also used as an input in the same expression. Use a temporary:

```ini
; DANGEROUS — undefined behavior
per_frame_1=zoom = zoom + 0.01;

; SAFE
per_frame_1=z = zoom + 0.01;
            zoom = z;
```

---

## 4️⃣ Shader System (Warp & Composite)

**[Intermediate → Advanced]**

> **TL;DR**
> - Source language is **HLSL 9 subset**. projectM transpiles it to GLSL at runtime.
> - Lines start with `` ` `` (backtick). No gaps in numbering.
> - The engine wraps your code in a `void PS(...)` function. You write the body.
> - Precompute constants in `per_frame_*` + q-vars. Keep shaders minimal.

### 4.1 How the Parser Assembles Shader Code

| Step | What the parser does | What you must provide |
|------|----------------------|----------------------|
| Backtick marker | Strips the leading `` ` `` and concatenates lines in numeric order | Every shader line must start with `` ` ``. No spaces before it. |
| Numbered assembly | Stops at the first missing index | Never skip a number. To disable a line, delete the key or rename it (e.g., `warp_1_disabled`). |
| Wrapping | Inserts your code into a pre-generated `void PS(...)` wrapper | Do not duplicate the wrapper. Do not redeclare `ret`. |
| Uniform mapping | Auto-creates `#define` aliases (`uv`, `rad`, `ang`, `time`, etc.) | Use the friendly names directly. |
| Size limit | Whole file ≤ 1 MiB | Keep shaders short; typical warp shader is 8–15 lines. |

### 4.2 Built-in Aliases & Uniforms

**Aliases (auto-#define'd):**

| Alias | Maps To | Context |
|-------|---------|---------|
| `uv` | `_uv.xy` | Both |
| `uv_orig` | `_uv.zw` (warp) / `_uv.xy` (comp) | Both |
| `rad` | `_rad_ang.x` | Both |
| `ang` | `_rad_ang.y` | Both |
| `hue_shader` | `_vDiffuse.xyz` | Composite only |

**Uniform `float4` vectors:**

| Uniform | Contents |
|---------|----------|
| `rand_frame` | 4 random floats (per frame) |
| `rand_preset` | 4 random floats (per preset lifetime) |
| `_c0` | `(aspectX, aspectY, invAspectX, invAspectY)` |
| `_c2` | `(time, fps, frame, progress)` |
| `_c3` | `(bass, mid, treb, vol)` |
| `_c4` | `(bass_att, mid_att, treb_att, vol_att)` |
| `_c5` | `(blur1_range, blur1_min, blur2_range, blur2_min)` |
| `_c7` | `(viewportW, viewportH, 1/W, 1/H)` |
| `_c8`, `_c9` | Fast oscillators |
| `_c10`, `_c11` | Slow oscillators |
| `_qa`–`_qh` | `q1`–`q32` packed as `float4` |
| `rot_s1`–`rot_s4` | Slow rotation matrices |
| `rot_d1`–`rot_d4` | Medium rotation matrices |
| `rot_f1`–`rot_f4` | Fast rotation matrices |
| `rot_vf1`–`rot_vf4` | Very fast rotation matrices |
| `rot_rand1`–`rot_rand4` | Per-frame random rotation matrices |

### 4.3 What the Engine Supports (HLSL-9 Subset)

| Feature | Supported | Not Supported |
|---------|-----------|---------------|
| Types | `float`, `float2`, `float3`, `float4` | `int`, `bool`, `double`, structs with semantics |
| Math | `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `pow`, `exp`, `log`, `sqrt`, `rsqrt`, `abs`, `sign`, `floor`, `ceil`, `frac`, `min`, `max`, `clamp`, `saturate`, `lerp`, `smoothstep`, `dot`, `cross`, `normalize`, `length`, `distance`, `fmod`, `mad` | `round`, `trunc`, `asinh`, `acosh`, `atanh` |
| Texture | `tex2D`, `tex2Dproj` | `textureLod`, `textureGrad`, `Texture2D`, `SamplerState` |
| Pre-processor | Simple `#define` (macro substitution) | `#include`, `#if`, `#ifdef`, `#pragma`, `#undef` |
| Functions | Static helper functions (before `PS` body) | Overloading, templates, recursion |
| Loops | `for`, `while` with bounded/compile-time-resolvable iteration | Dynamic unbounded loops |
| Branching | `if`/`else` (shallow only) | `switch`, deep per-pixel branching |
| Output | `ret` (color) + `_mv_tex_coords` (warp UV) | Multiple render targets, depth output |

> **Bottom line:** Write plain HLSL-9 that could compile with legacy `fxc /T ps_2_0`. Anything more modern will be rejected.

### 4.4 Helper Library (Copy-Paste)

Paste these at the top of your shader block (e.g., `warp_1` or `comp_1`):

```hlsl
float3 hsv2rgb(float3 c)
{
    float4 K = float4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
}

float rand(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}
```

Because the parser concatenates numbered lines as-is, you can place the helper in `warp_1` and your effect code in `warp_2` onwards.

### 4.5 Modern Shader Patterns

**Pattern: SDF Circle Warp**

```ini
warp_1=`float2 p = (uv - 0.5) * aspect.xy;
warp_2=`float d = length(p) - 0.3;
warp_3=`float2 offset = normalize(p + 0.0001) * smoothstep(0.0, 0.1, d) * 0.05;
warp_4=`uv += offset;
warp_5=`ret = tex2D(sampler_main, uv).xyz;
```

**Pattern: Multi-Pass Glow (Composite)**

```ini
comp_1=`float3 blur = GetBlur1(uv) + GetBlur2(uv) + GetBlur3(uv);
comp_2=`float glow = _c3.x * 0.3;
comp_3=`ret = hue_shader + blur * glow;
comp_4=`ret = saturate(ret);
```

**Pattern: Kaleidoscope Symmetry**

```ini
warp_1=`float2 c = (uv - 0.5) * aspect.xy;
warp_2=`float r = length(c);
warp_3=`float a = atan(c.y, c.x);
warp_4=`a = fmod(a * 6.0, 6.283185307);
warp_5=`float2 new_uv = 0.5 + r * float2(cos(a), sin(a)) * aspect.zw;
warp_6=`ret = tex2D(sampler_main, new_uv).xyz;
```

**Pattern: Noise-Driven Displacement**

```ini
warp_1=`float2 nuv = uv * 3.0 + _c2.x * 0.15;
warp_2=`float3 n = tex2D(sampler_fw_noisevol_lq, nuv).xyz;
warp_3=`float2 disp = (n.xy - 0.5) * 0.03 * _c3.x;
warp_4=`uv += disp;
warp_5=`ret = tex2D(sampler_main, uv).xyz;
```

**Pattern: Reactive Hue Shift (Composite)**

```ini
comp_1=`float angle = _c3.x * 0.5;
comp_2=`float3x3 rot = float3x3(
comp_3=`  cos(angle), -sin(angle), 0,
comp_4=`  sin(angle),  cos(angle), 0,
comp_5=`  0,          0,           1);
comp_6=`ret = mul(rot, hue_shader);
comp_7=`ret = saturate(ret);
```

### 4.6 Performance Rules Checklist

- [ ] **Offload to per-frame + q-vars.** Any value constant across pixels belongs in `per_frame_*`.
- [ ] **No `if` / branching in shaders.** Use `saturate()`, `step()`, `lerp()`, `clamp()`.
- [ ] **Limit `sin`/`cos`/`atan2` in shaders.** Precompute in per-frame and pass via q-vars.
- [ ] **Sample near 1:1 scale.** Extreme zooms thrash the texture cache.
- [ ] **Textures ≤256×256.** Save user textures as JPG 95% quality.
- [ ] **Aspect ratio:** Use `aspect.xy` / `aspect.zw` in shaders. In per-pixel, use `aspectx`/`aspecty`.
- [ ] **Prefer `half` where full `float` isn't needed.**
- [ ] **Always `saturate(ret)` at the end.** Clamp to `[0,1]` for consistent output.
- [ ] **Use `GetBlur1/2/3(uv)`** for blurred sampling.
- [ ] **Keep shaders short.** Warp for motion; composite for final post-processing.
- [ ] **Noise:** Prefer `_lq` / `_lite` variants + `rand_frame` / `rand_preset`.

### 4.7 GLSL / WGSL → HLSL Porting Guide

There is no one-click converter. Use structured manual translation.

#### Step-by-Step Workflow

1. **Identify target:** Motion/persistence → **Warp shader.** Final post-processing → **Composite shader.**
2. **Translate syntax** using the table below.
3. **Adapt structure:** Wrap in `PS(...)` signature. Output via `ret = computed_color.xyz;`.
4. **Map inputs:** `iTime` → `time` / `_c2.x`; `iResolution` → `_c7` / `texsize_main`; audio → `_c3`/`_c4` or q-vars.
5. **Test in projectM.** Iterate.

#### Syntax Mapping Table

| GLSL / WGSL | MilkDrop HLSL | Notes |
|-------------|---------------|-------|
| `vec2` / `vec3` / `vec4` | `float2` / `float3` / `float4` | |
| `texture2D(s, uv)` | `tex2D(sampler_xxx, uv)` | Declare sampler at top |
| `mix(a, b, t)` | `lerp(a, b, t)` | |
| `fract(x)` | `frac(x)` | |
| `mod(x, y)` | `fmod(x, y)` | |
| `clamp(x, 0, 1)` | `saturate(x)` | Preferred for `[0,1]` |
| `iTime` | `time` / `_c2.x` | |
| `iResolution` | `_c7.xy` / `texsize_main.xy` | |
| `fragCoord / iResolution` | `uv` | Already normalized |
| `mainImage(out, coord)` | Compute color, assign to `ret` | Inside `PS(...)` wrapper |
| `smoothstep` | `smoothstep` | Identical |
| `pow`, `abs`, `sin`, `cos` | `pow`, `abs`, `sin`, `cos` | Same |

#### Common Pitfalls

| Problem | Cause | Fix |
|---------|-------|-----|
| Black screen | Missing `ret` assignment | Always write to `ret` |
| Shader won't compile | GLSL syntax left in HLSL | Check `vec*` → `float*`, `mix` → `lerp` |
| Stretched distortion | Wrong aspect handling | Use `aspect.xy` in shaders; `aspectx`/`aspecty` in per-pixel |
| No audio reactivity | Using hardcoded uniforms | Map to `_c3`, `_c4`, or q-variables |
| Gaps in shader | Missing line number | Ensure `warp_N` / `comp_N` are sequential |

#### WGSL Note

WGSL is not directly supported. Transpile WGSL → GLSL first using **Naga** or **Tint**, then apply the mapping above.

#### Pro Tip

Generating **native MilkDrop HLSL directly** (using the patterns in this guide) is almost always more reliable than porting arbitrary ShaderToy code.

#### Full Conversion Example

**Original GLSL:**
```glsl
#version 300 es
precision mediump float;
uniform float iTime;
uniform vec2 iResolution;
uniform sampler2D iChannel0;
out vec4 fragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution;
    float d = length(uv - 0.5);
    float a = atan(uv.y-0.5, uv.x-0.5);
    float amp = 0.2 * sin(iTime*2.0);
    uv += amp * sin(6.0*a + iTime) * d;
    fragColor = texture(iChannel0, uv);
}
```

**Converted to .milk warp shader:**
```ini
warp_1=`float2 uv = _uv.xy;
warp_2=`float d = length(uv - 0.5);
warp_3=`float a = atan2(uv.y-0.5, uv.x-0.5);
warp_4=`float amp = 0.2 * sin(time*2.0);
warp_5=`uv += amp * sin(6.0*a + time) * d;
warp_6=`ret = tex2D(sampler_main, uv).xyz;
```

> **Note:** We added a sixth line because the wrapper already declares `float3 ret = 0;`. The final assignment writes the color.


---

## 5️⃣ Custom Waves & Shapes

**[Intermediate → Advanced]**

> **TL;DR**
> - 4 wave slots (0–3) and 4 shape slots (0–3).
> - Waves have `init` → `per_frame` → `per_point` lifecycle.
> - Shapes have `init` → `per_frame` lifecycle.
> - Use `t1`–`t8` for local state and `instance`/`num_inst` for multi-instance shapes.

### 5.1 Custom Waves — Full Lifecycle Example: Spiral Spectrum

This example uses **all three stages** of a custom wave and demonstrates `t1`–`t8` persistence.

```ini
; --- static parameters -------------------------------------------------
wavecode_0_enabled=1
wavecode_0_bSpectrum=1          ; use FFT spectrum
wavecode_0_samples=256
wavecode_0_scaling=0.6
wavecode_0_bAdditive=1
wavecode_0_r=0.9
wavecode_0_g=0.2
wavecode_0_b=0.5

; --- init (run once) ---------------------------------------------------
wave_0_init_1=t1=0;             ; rotation offset
wave_0_init_2=t2=0;             ; radius growth
wave_0_init_3=t3=1.0;           ; scale factor

; --- per-frame ---------------------------------------------------------
wave_0_per_frame_1=t1=t1+0.02;              ; spin
wave_0_per_frame_2=t2=clamp(t2+0.001,0,0.4); ; slowly expand
wave_0_per_frame_3=t3=0.5+bass*0.5;          ; bass-reactive scale

; --- per-point (run for each sample) -----------------------------------
wave_0_per_point_1=ang = sample*2*pi + t1;
wave_0_per_point_2=r = 0.2 + t2*sample;
wave_0_per_point_3=x = 0.5 + r*cos(ang);
wave_0_per_point_4=y = 0.5 + r*sin(ang);
wave_0_per_point_5=r = value1 * t3;          ; amplitude from spectrum
wave_0_per_point_6=g = 1.0 - value1;
```

### 5.2 Custom Shapes — Multi-Instance Rotating Hexagons

```ini
shapecode_0_enabled=1
shapecode_0_sides=6
shapecode_0_num_inst=4
shapecode_0_rad=0.12
shapecode_0_r=0.8
shapecode_0_g=0.6
shapecode_0_b=0.2
shapecode_0_a=0.9

shape_0_per_frame_1=ANG_SPEED=0.7;
shape_0_per_frame_2=inst_ang = time*ANG_SPEED + instance*2*pi/num_inst;
shape_0_per_frame_3=ang = inst_ang;
shape_0_per_frame_4=x = 0.5 + 0.3*cos(inst_ang);
shape_0_per_frame_5=y = 0.5 + 0.3*sin(inst_ang);
shape_0_per_frame_6=r = 0.12 + 0.04*sin(time+instance);
```

### 5.3 Recipe Boxes

**Recipe: Spectrum Analyzer with Per-Point Color Mapping**

```ini
wavecode_1_enabled=1
wavecode_1_bSpectrum=1
wavecode_1_samples=128
wavecode_1_bAdditive=1

wave_1_per_point_1=x=sample;
wave_1_per_point_2=y=0.5+value1*0.25;
wave_1_per_point_3=r=bass;
wave_1_per_point_4=g=mid;
wave_1_per_point_5=b=treb;
```

**Recipe: Bass-Reactive Circular Waveform**

```ini
wavecode_2_enabled=1
wavecode_2_scaling=0.4
wavecode_2_bAdditive=1

wave_2_per_frame_1=t1=time*0.3;
wave_2_per_frame_2=t2=0.25+bass*0.15;

wave_2_per_point_1=ang=sample*2*pi+t1;
wave_2_per_point_2=r=cos(ang)*0.5+0.5;
wave_2_per_point_3=g=sin(ang)*0.5+0.5;
wave_2_per_point_4=x=0.5+t2*cos(ang)*value1;
wave_2_per_point_5=y=0.5+t2*sin(ang)*value1;
```

**Recipe: Textured Shape Reacting to Bass**

```ini
shapecode_1_enabled=1
shapecode_1_sides=4
shapecode_1_textured=1
shapecode_1_image=mytexture.jpg
shapecode_1_rad=0.2

shape_1_per_frame_1=rad=0.15+bass*0.15;
shape_1_per_frame_2=tex_zoom=1.0+mid*0.5;
shape_1_per_frame_3=tex_ang=time*0.2;
shape_1_per_frame_4=r=1.0;
shape_1_per_frame_5=g=1.0-bass*0.5;
shape_1_per_frame_6=b=1.0-mid*0.5;
```

### 5.4 Advanced: Persistent Trails & Echo Effects

Use `reg00+` to store previous frame state:

**Wave trail:**
```ini
wavecode_3_enabled=1
wavecode_3_bSpectrum=0
wavecode_3_samples=100
wavecode_3_bAdditive=1
wavecode_3_a=0.3

wave_3_per_frame_1=t1=reg10;
wave_3_per_frame_2=reg10=time;

wave_3_per_point_1=x=0.5+value1*0.4*sin(t1+sample*6.28);
wave_3_per_point_2=y=0.5+value2*0.4*cos(t1+sample*6.28);
```

**Shape echo:**
```ini
shapecode_2_enabled=1
shapecode_2_sides=3
shapecode_2_num_inst=3
shapecode_2_rad=0.08
shapecode_2_a=0.4

shape_2_per_frame_1=t1=reg20+instance;
shape_2_per_frame_2=reg20=time*0.5;
shape_2_per_frame_3=x=0.5+0.3*cos(t1+instance);
shape_2_per_frame_4=y=0.5+0.3*sin(t1+instance);
shape_2_per_frame_5=a=0.6-instance*0.15;
```

[← Back to Table of Contents](#-table-of-contents)

---

## 6️⃣ Performance & Debugging Tips

**[All Levels]**

> **TL;DR**
> - Cache `sin(time)` in a Q-var if used more than once per frame.
> - Use `saturate()` instead of `clamp()` for `[0,1]` — it's one GPU instruction.
> - Avoid dynamic loops > 8 iterations in shaders.
> - Visual debug: `ret = float4(q1,q1,q1,1);` or use the `monitor` variable.
> - Profile by disabling shaders/waves/shapes one at a time.

### Fast vs. Slow HLSL Functions

| Fast (cheap) | Slow (expensive) | Avoid in shaders |
|--------------|------------------|------------------|
| `saturate`, `lerp`, `step` | `sin`, `cos`, `atan2` | Precompute in per-frame |
| `frac`, `fmod` | `pow` with large exponents | Cache in Q-var |
| `dot`, `length` | `tex2D` inside loops | Sample once, reuse |
| `abs`, `min`, `max` | Branching (`if`/`else`) | Use math tricks instead |

### Cache-Once-Per-Frame Pattern

```ini
; BAD — sin(time) computed twice per pixel in shader
; warp_1=`float x = sin(time) * 0.1;

; GOOD — compute once in per-frame, pass via q-var
per_frame_1=q1=sin(time);
warp_1=`float x = q1 * 0.1;
```

### Visual Debug Techniques

| Technique | How |
|-----------|-----|
| Show Q-var value on screen | `ret = float4(q1, q1, q1, 1.0);` in composite shader |
| In-app variable monitor | `per_frame_1=monitor=q1;` |
| Check audio levels | `ret = float4(bass, mid, treb, 1.0);` |
| Verify UV space | `ret = float4(uv, 0.0, 1.0);` |

### Profiling by Component Disable

If a preset is slow, disable components one at a time:

1. Remove all `warp_*` lines → GPU warp bottleneck?
2. Remove all `comp_*` lines → GPU composite bottleneck?
3. Set all `wavecode_*_enabled=0` → Custom wave bottleneck?
4. Set all `shapecode_*_enabled=0` → Custom shape bottleneck?
5. Simplify `per_pixel_*` → Per-vertex math bottleneck?

On modern desktop GPUs, shader complexity is usually the culprit. On mobile/WebGL, texture sampling and branching matter more.

[← Back to Table of Contents](#-table-of-contents)

---

## 7️⃣ Tooling & Workflow

**[All Levels]**

### Recommended Editors

- **VS Code** with any INI syntax highlighter works well.
- **Sublime Text**, **Vim**, **Emacs** — `.milk` is plain text.
- No dedicated MilkDrop IDE exists, but plain-text editing is sufficient.

### Testing & Preview

| Tool | Platform | What it does |
|------|----------|--------------|
| **projectM SDL test UI** | Linux, macOS, Windows | `projectM-Test-UI` binary included in builds. Loads presets from a directory. Press `F5` to reload. |
| **NestDrop** | Windows | Modern MilkDrop-compatible renderer with excellent error feedback and fast iteration. |
| **WebGL builds** | Browser | projectM can compile to WASM/Emscripten for browser testing. |

### Directory Layout for Preset Packs

```
my_preset_pack/
├── presets/
│   ├── 001-hello.milk
│   ├── 002-tunnel.milk
│   └── ...
├── textures/
│   ├── mytexture.jpg
│   └── noise.png
└── README.md
```

### Git Workflow

Presets are small text files — perfect for Git:

```bash
git init my_preset_pack
git add presets/
git commit -m "Add initial preset collection"
```

### Validation Checklist (Manual)

Before sharing a preset:

- [ ] File size < 1 MB
- [ ] No null bytes (`\0`)
- [ ] All numbered sequences are gapless
- [ ] Every shader line starts with `` ` ``
- [ ] `ret` is assigned in every shader path
- [ ] All used Q-variables are set in `per_frame_*`
- [ ] `saturate(ret)` at the end of composite shaders
- [ ] Tested in projectM or NestDrop

[← Back to Table of Contents](#-table-of-contents)

---

## 8️⃣ Advanced Topics

**[Advanced]**

### 8.1 Programmatic Preset Generation

Presets are text. Generate them with code.

#### Python: Complete Preset Builder

```python
def add_numbered_code(base_key, lines):
    return {f'{base_key}_{i}': line for i, line in enumerate(lines, 1)}


def add_shader(shader_type, lines):
    """shader_type: 'warp' or 'comp'."""
    return {f'{shader_type}_{i}': f'`{line}' for i, line in enumerate(lines, 1)}


def build_preset(params, per_frame_lines, per_pixel_lines=None,
                 custom_wave=None, custom_shape=None,
                 warp_shader=None, comp_shader=None):
    preset = {
        'MILKDROP_PRESET_VERSION': 200,
        'PSVERSION_WARP': 3,
        'PSVERSION_COMP': 3,
    }
    preset.update(params)
    preset.update(add_numbered_code('per_frame', per_frame_lines))

    if per_pixel_lines:
        preset.update(add_numbered_code('per_pixel', per_pixel_lines))
    if custom_wave:
        preset.update(custom_wave)
    if custom_shape:
        preset.update(custom_shape)
    if warp_shader:
        preset.update(add_shader('warp', warp_shader))
    if comp_shader:
        preset.update(add_shader('comp', comp_shader))

    return preset


def serialize_preset(preset):
    lines = ['[preset00]']
    priority = ['MILKDROP_PRESET_VERSION', 'PSVERSION_WARP', 'PSVERSION_COMP']
    for key in priority:
        if key in preset:
            lines.append(f'{key}={preset[key]}')
    for key in sorted(preset.keys()):
        if key not in priority:
            lines.append(f'{key}={preset[key]}')
    return '\n'.join(lines)
```

#### Python: Dynamic Wave & Shape Generators

```python
def generate_spectrum_wave(index=0, samples=256, color=(1.0, 0.0, 1.0)):
    r, g, b = color
    return {
        f'wavecode_{index}_enabled': 1,
        f'wavecode_{index}_bSpectrum': 1,
        f'wavecode_{index}_samples': samples,
        f'wavecode_{index}_bAdditive': 1,
        f'wavecode_{index}_r': r,
        f'wavecode_{index}_g': g,
        f'wavecode_{index}_b': b,
        f'wave_{index}_per_frame_1': 'a=0.5+0.4*mid;',
        f'wave_{index}_per_point_1': 'x=sample;',
        f'wave_{index}_per_point_2': 'y=0.5+value1*0.3;',
        f'wave_{index}_per_point_3': f'r=sample*{r};',
        f'wave_{index}_per_point_4': f'b=(1.0-sample)*{b};',
    }


def generate_multi_instance_shape(index=0, sides=6, num_inst=5, color=(1.0, 0.5, 0.0)):
    r, g, b = color
    shape = {
        f'shapecode_{index}_enabled': 1,
        f'shapecode_{index}_sides': sides,
        f'shapecode_{index}_num_inst': num_inst,
        f'shapecode_{index}_rad': 0.1,
        f'shapecode_{index}_r': r,
        f'shapecode_{index}_g': g,
        f'shapecode_{index}_b': b,
    }
    frame_lines = [
        'ANG_SPEED=1.0;',
        f'instance_offset=instance*2*pi/{num_inst};',
        'ang=time*ANG_SPEED+instance_offset;',
        'x=0.5+0.3*cos(ang);',
        'y=0.5+0.3*sin(ang);',
        'r=0.3+0.7*sin(time+instance);',
        'g=0.3+0.7*cos(time+instance*2);',
    ]
    shape.update(add_numbered_code(f'shape_{index}_per_frame', frame_lines))
    return shape
```

#### Python: SDF Warp Shader Generator

```python
def generate_sdf_warp_shader(shape='circle', strength=0.05):
    lines = [
        'float2 p = (uv - 0.5) * aspect.xy;',
    ]
    if shape == 'circle':
        lines.append('float d = length(p) - 0.3;')
    elif shape == 'box':
        lines.append('float d = length(max(abs(p)-0.2, 0.0));')
    lines.extend([
        f'float2 offset = normalize(p + 0.0001) * smoothstep(0.0, 0.1, d) * {strength};',
        'uv += offset;',
        'ret = tex2D(sampler_main, uv).xyz;',
    ])
    return lines
```

#### Rust: Minimal Serializer

```rust
use std::collections::BTreeMap;

fn serialize_preset(preset: &BTreeMap<&str, &str>) -> String {
    let mut lines = vec!["[preset00]".to_string()];
    for (k, v) in preset {
        lines.push(format!("{}={}", k, v));
    }
    lines.join("\n")
}

// Usage:
// let mut preset = BTreeMap::new();
// preset.insert("MILKDROP_PRESET_VERSION", "200");
// println!("{}", serialize_preset(&preset));
```

#### JavaScript: Minimal Serializer

```javascript
function serializePreset(preset) {
    const lines = ['[preset00]'];
    for (const [k, v] of Object.entries(preset)) {
        lines.push(`${k}=${v}`);
    }
    return lines.join('\n');
}
```

#### Template-Engine Approach

For repetitive code generation, use **Jinja2** (Python) or **Handlebars** (JS):

```python
from jinja2 import Template

tmpl = Template("""
[preset00]
MILKDROP_PRESET_VERSION=200
{% for i in range(n_waves) %}
wavecode_{{ i }}_enabled=1
wavecode_{{ i }}_bSpectrum=1
wavecode_{{ i }}_samples={{ samples }}
{% endfor %}
""")
print(tmpl.render(n_waves=4, samples=256))
```

### 8.2 Cross-Tool Compatibility Notes

| Feature | projectM 4.x | Native MilkDrop 2 | NestDrop | Butterchurn |
|---------|--------------|-------------------|----------|-------------|
| `MILKDROP_PRESET_VERSION=200` | ✅ | ✅ | ✅ | ✅ |
| Warp shaders (`PSVERSION_WARP=3`) | ✅ | ✅ | ✅ | ⚠️ limited |
| Composite shaders (`PSVERSION_COMP=3`) | ✅ | ✅ | ✅ | ⚠️ limited |
| Custom waves/shapes | ✅ | ✅ | ✅ | ❌ |
| `GetBlur1/2/3` | ✅ | ✅ | ✅ | ❌ |
| `distance`, `ldexp`, `faceforward` | ✅ (4.1+) | ⚠️ depends | ⚠️ depends | ❌ |
| `#define` macros | ✅ | ✅ | ✅ | ⚠️ limited |

Always test in **projectM** as the primary target. If you need native MilkDrop parity, avoid recently added intrinsics.

### 8.3 Multi-Preset Blending

The `progress` variable (0–1) indicates how far along a preset transition is. You can read it in any context to create blend-aware effects:

```ini
per_frame_1=blend_factor = 1.0 - abs(progress - 0.5) * 2.0;
```

[← Back to Table of Contents](#-table-of-contents)


---

## 9️⃣ Common Pitfalls Checklist

**[All Levels]**

> **TL;DR** — If your preset is broken, run through this table before anything else.

### The 10 Most Frequent Errors

| # | Pitfall | Symptom | Fix |
|---|---------|---------|-----|
| 1 | **Missing numeric index** (e.g., `warp_1` then `warp_3`) | Shader stops early → visual output looks broken or incomplete | Ensure continuous sequence from 1 to N with no gaps |
| 2 | **Backtick omitted** on shader line | Line parsed as normal key/value → silently ignored → shader incomplete | Every shader line must start with `` ` `` (no leading spaces) |
| 3 | **Using a variable name that collides with a built-in** (e.g., redeclaring `uv_orig`) | Compile error: "redefinition of built-in variable" | Do not redeclare aliases; use them directly |
| 4 | **Calling a function that returns `float4` but assigning to `float3`** | HLSL compile error | Match types exactly; use `.xyz` swizzle if needed |
| 5 | **Using an undefined Q-variable** (`if (q5 > 0.5)` when q5 was never set) | q5 stays at default 0 → branch never taken or behaves unpredictably | Initialize the Q-variable in `per_frame` or `per_frame_init` |
| 6 | **Texture sampler not declared** (`sampler_mytex` without a corresponding image) | Runtime error: "sampler not bound" or black output | Add `shapecode_N_image=mytex.png` or reference a built-in sampler |
| 7 | **File size > 1 MiB** | Parser aborts → preset fails to load entirely | Keep shaders short; factor complex logic into helper functions |
| 8 | **Null byte (`\0`) inside the file** | Parse aborts → preset fails to load | Ensure file is plain UTF-8 text with no embedded nulls |
| 9 | **Division by zero or `pow(x, -1.0)` with x near zero** | GPU NaN → whole preset freezes or shows black | Guard with `if (abs(x) > 1e-6)` or use `1.0/x` with care |
| 10 | **Forgetting `saturate(ret)` at the end of composite shaders** | Colors may clip unpredictably on some drivers | Always clamp final output: `ret = saturate(ret);` |

### Additional Gotchas

**Dynamic loops in shaders:**
The HLSL-9 subset allows `for` and `while`, but only if the iteration count is bounded or resolvable at compile time. Unbounded loops will cause the GPU driver to abort the shader.

```hlsl
; SAFE — bounded loop
for (int i = 0; i < 4; i++) { ... }

; DANGEROUS — dynamic termination
for (int i = 0; i < bass * 100.0; i++) { ... }
```

**Precision jitter:**
Relying on `float` precision for high-frequency oscillators (e.g., `sin(time * 1000.0)`) can cause visual jitter. Use the built-in fast oscillators (`_c8`, `_c9`) or rotation matrices for smoother results.

**Mouse input:**
There is no mouse input in projectM. If porting ShaderToy code that uses `iMouse`, replace it with a Q-variable or a time-based condition.

[← Back to Table of Contents](#-table-of-contents)

---

## 10️⃣ Appendix — Glossary & Quick Reference

**[Reference]**

### Glossary

| Term | Definition |
|------|------------|
| **Q-variables** (`q1`–`q32`) | 32 float slots written in `per_frame` (or `init`) and readable everywhere else |
| **Registers** (`reg00`–`reg99`) | Persistent double-precision storage shared across all contexts |
| **T-variables** (`t1`–`t8`) | Local per-wave/shape storage surviving from `init` → `per_frame` → `per_point` |
| **Warp shader** | Distorts UV coordinates before the frame is composited |
| **Composite shader** | Post-processes the final image (colour grading, bloom, etc.) |
| **PSVERSION_WARP / COMP** | Integer selecting shader model (`0` = none, `3` = HLSL 9) |
| **progress** | `0`–`1` blend factor during preset transitions |
| **aspectX / aspectY** | Inverse aspect ratios (`1/width`, `1/height`) — useful for correcting stretch |
| **rand_frame / rand_preset** | Four-float random vectors that change per frame or per preset lifetime |
| **megabuf / gmegabuf** | Global memory array accessible from all contexts |

### Quick Reference: Shader Uniforms

| Uniform | Data | Typical Use |
|---------|------|-------------|
| `_c0` | Aspect ratios | `aspect.xy` for UV scaling |
| `_c2` | Timing | `time`, `fps`, `frame` |
| `_c3` | Raw audio | `bass`, `mid`, `treb`, `vol` |
| `_c4` | Smoothed audio | `bass_att`, `mid_att`, `treb_att` |
| `_c5` | Blur ranges | `GetBlur1/2` scaling |
| `_c7` | Viewport | `viewportW`, `viewportH` |
| `_qa`–`_qh` | Q variables | `q1`–`q32` packed as `float4` |
| `rand_frame` | Random | Per-frame noise |
| `rand_preset` | Random | Persistent random seeds |
| `rot_*` | Matrices | 3D rotation helpers |

### Quick Reference: Samplers

| Sampler | Purpose |
|---------|---------|
| `sampler_main` | Current frame render |
| `sampler_blur1`, `sampler_blur2`, `sampler_blur3` | Blur passes |
| `sampler_fw_noise_lq` | Low-quality noise |
| `sampler_fw_noisevol_lq` | Volume-modulated noise |
| `sampler_rand00`–`sampler_rand15` | Preset-persistent random textures |
| `sampler_<custom>` | User-loaded image (via `shapecode_N_image=`) |

### Quick Reference: GLSL → HLSL Mapping

| GLSL | HLSL |
|------|------|
| `vec2` / `vec3` / `vec4` | `float2` / `float3` / `float4` |
| `texture2D(s, uv)` | `tex2D(sampler_xxx, uv)` |
| `mix(a, b, t)` | `lerp(a, b, t)` |
| `fract(x)` | `frac(x)` |
| `mod(x, y)` | `fmod(x, y)` |
| `clamp(x, 0, 1)` | `saturate(x)` |
| `iTime` | `time` / `_c2.x` |
| `iResolution` | `_c7.xy` / `texsize_main.xy` |
| `mainImage(...)` | Write to `ret` inside `PS(...)` |

### Quick Reference: Complete Parameter Table

| Category | Key | Default | Type |
|----------|-----|---------|------|
| Post-processing | `fDecay` | `0.98` | float |
| | `fGammaAdj` | `2.0` | float |
| | `fVideoEchoZoom` | `2.0` | float |
| | `fVideoEchoAlpha` | `0.0` | float |
| | `nVideoEchoOrientation` | `0` | int |
| | `bRedBlueStereo` | `0` | bool |
| | `bBrighten` | `0` | bool |
| | `bDarken` | `0` | bool |
| | `bSolarize` | `0` | bool |
| | `bInvert` | `0` | bool |
| | `fShader` | `0.0` | float |
| Motion | `zoom` | `1.0` | float |
| | `rot` | `0.0` | float |
| | `cx`, `cy` | `0.5` | float |
| | `dx`, `dy` | `0.0` | float |
| | `sx`, `sy` | `1.0` | float |
| | `warp` | `1.0` | float |
| | `bTexWrap` | `1` | bool |
| | `fWarpAnimSpeed` | `1.0` | float |
| | `fWarpScale` | `1.0` | float |
| | `fZoomExponent` | `1.0` | float |
| | `bDarkenCenter` | `0` | bool |
| Waveform | `nWaveMode` | `0` | int |
| | `fWaveAlpha` | `0.8` | float |
| | `fWaveScale` | `1.0` | float |
| | `fWaveSmoothing` | `0.75` | float |
| | `fWaveParam` | `0.0` | float |
| | `bAdditiveWaves` | `0` | bool |
| | `bWaveDots` | `0` | bool |
| | `bWaveThick` | `0` | bool |
| | `bModWaveAlphaByVolume` | `0` | bool |
| | `bMaximizeWaveColor` | `1` | bool |
| | `wave_r`, `wave_g`, `wave_b` | `1.0` | float |
| | `wave_x`, `wave_y` | `0.5` | float |
| Blur | `b1n`, `b2n`, `b3n` | `0.0` | float |
| | `b1x`, `b2x`, `b3x` | `1.0` | float |
| | `b1ed` | `0.25` | float |
| Borders | `ob_size` | `0.01` | float |
| | `ob_r`, `ob_g`, `ob_b`, `ob_a` | `0.0` | float |
| | `ib_size` | `0.01` | float |
| | `ib_r`, `ib_g`, `ib_b` | `0.25` | float |
| | `ib_a` | `0.0` | float |
| Motion vectors | `nMotionVectorsX` | `12.0` | float |
| | `nMotionVectorsY` | `9.0` | float |
| | `mv_dx`, `mv_dy` | `0.0` | float |
| | `mv_l` | `0.9` | float |
| | `mv_r`, `mv_g`, `mv_b`, `mv_a` | varies | float |

### Resources & Further Reading

- **projectM GitHub:** https://github.com/projectM-visualizer/projectm — Source code, releases, issues.
- **NestDrop:** https://nestdrop.net — Modern renderer with excellent tutorials.
- **MilkDrop Preset Authoring Guide (Geisswerks):** The canonical reference for the original format.
- **Community preset packs:** Search Reddit / Discord for "Cream of the Crop" and "MegaPack".
- **Conversion tools:** milkdrop-preset-converter (community), NestDrop import tools.

### Historical Footnote: MilkDrop 1.x

MilkDrop 1.x used `MILKDROP_PRESET_VERSION=100`. It lacked warp/composite shaders, custom waves/shapes, and many modern parameters. projectM 4.x still parses `100` presets for backward compatibility, but **all new authoring should target `200`**. This footnote exists only for reference; no new presets should use MD1.x features.

---

> **End of guide.** Happy preset authoring! 🎨🔮  
> [← Back to Table of Contents](#-table-of-contents)
