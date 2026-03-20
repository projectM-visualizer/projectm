# Complete Guide to Creating .milk Preset Files for AI Agents

This guide provides everything needed to programmatically generate, understand, and modify projectM .milk preset files.

## Table of Contents

1. [File Format Basics](#file-format-basics)
2. [Core Parameters](#core-parameters)
3. [Expression Code System](#expression-code-system)
4. [Per-Frame Equations](#per-frame-equations)
5. [Per-Pixel/Vertex Equations](#per-pixelvertex-equations)
6. [Custom Waves](#custom-waves)
7. [Custom Shapes](#custom-shapes)
8. [Shader System (Warp & Composite)](#shader-system-warp--composite)
9. [Code Generation Patterns](#code-generation-patterns)
10. [Example Presets](#example-presets)

---

## File Format Basics

### Parsing Rules

The .milk file format is a simple INI-like structure:

- **Line splitting:** Lines are split on the first space or `=` character
- **Key normalization:** Keys are lowercased; values retain their case
- **Duplicate handling:** Only the first occurrence of a key is kept; duplicates are silently ignored
- **Comments:** Lines with no delimiter (space or `=`) and `[section]` headers are ignored
- **File size limit:** Maximum 1 MB (0x100000 bytes)
- **Null byte handling:** A null byte (`\0`) causes entire parse to fail
- **Code line markers:** Lines beginning with backtick (`` ` ``) have the backtick stripped (used for shader lines)

### Code Assembly

Numbered code keys are assembled by index:

- `per_frame_1`, `per_frame_2`, `per_frame_3`, ... are concatenated into one code string
- **Gap detection:** If a number is missing, parsing stops (e.g., missing `per_frame_3` stops at `per_frame_2`)
- This applies to: `per_frame_*`, `per_frame_init_*`, `per_pixel_*`, `warp_*`, `comp_*`, and wave/shape code

### Basic Structure

```
[preset00]
MILKDROP_PRESET_VERSION=200
fGammaAdj=2.0
fDecay=0.98

per_frame_1=zoom=0.95+0.05*sin(time);
per_frame_2=rot=rot+0.001;

per_pixel_1=zoom=zoom*(1.0-rad*0.1);
```

---

## Core Parameters

### General Post-Processing

| Key | Default | Type | Purpose |
|-----|---------|------|---------|
| `fDecay` | 0.98 | float | Screen decay/persistence |
| `fGammaAdj` | 2.0 | float | Gamma correction |
| `fVideoEchoZoom` | 2.0 | float | Echo effect zoom |
| `fVideoEchoAlpha` | 0.0 | float | Echo effect opacity |
| `nVideoEchoOrientation` | 0 | int | Echo orientation (0-1) |
| `bRedBlueStereo` | false | bool | Stereo 3D mode |
| `bBrighten` | false | bool | Post-process brighten |
| `bDarken` | false | bool | Post-process darken |
| `bSolarize` | false | bool | Solarize effect |
| `bInvert` | false | bool | Color inversion |
| `fShader` | 0.0 | float | Shader blend factor |

### Motion & Transformation

| Key | Default | Purpose |
|-----|---------|---------|
| `zoom` | 1.0 | Overall zoom level |
| `rot` | 0.0 | Rotation amount |
| `cx`, `cy` | 0.5 | Rotation center point |
| `dx`, `dy` | 0.0 | Translation push |
| `sx`, `sy` | 1.0 | Stretch X/Y |
| `warp` | 1.0 | Warp/distortion amount |
| `bTexWrap` | true | Enable texture wrapping |
| `fWarpAnimSpeed` | 1.0 | Warp animation speed |
| `fWarpScale` | 1.0 | Warp effect scale |
| `fZoomExponent` | 1.0 | Zoom exponent (non-linear) |
| `bDarkenCenter` | false | Darken center of screen |

### Built-in Waveform

| Key | Default | Purpose |
|-----|---------|---------|
| `nWaveMode` | 0 | Wave display mode (0-7+) |
| `fWaveAlpha` | 0.8 | Wave opacity |
| `fWaveScale` | 1.0 | Wave size |
| `fWaveSmoothing` | 0.75 | Wave smoothing (0-1) |
| `fWaveParam` | 0.0 | Wave-mode-specific parameter |
| `bAdditiveWaves` | false | Additive wave blending |
| `bWaveDots` | false | Draw as dots instead of line |
| `bWaveThick` | false | Draw with thick lines |
| `bModWaveAlphaByVolume` | false | Modulate alpha by volume |
| `bMaximizeWaveColor` | true | Maximize wave brightness |
| `wave_r`, `wave_g`, `wave_b` | 1.0 | Wave color (RGB) |
| `wave_x`, `wave_y` | 0.5 | Wave position (centered) |
| `fModWaveAlphaStart` | 0.75 | Volume modulation start |
| `fModWaveAlphaEnd` | 0.95 | Volume modulation end |

### Blur Textures

| Key | Default | Purpose |
|-----|---------|---------|
| `b1n` | 0.0 | Blur1 minimum |
| `b1x` | 1.0 | Blur1 maximum |
| `b2n` | 0.0 | Blur2 minimum |
| `b2x` | 1.0 | Blur2 maximum |
| `b3n` | 0.0 | Blur3 minimum |
| `b3x` | 1.0 | Blur3 maximum |
| `b1ed` | 0.25 | Blur1 edge darkening |

### Borders (Outer & Inner)

**Outer Border (`ob_*`):**
| Key | Default |
|-----|---------|
| `ob_size` | 0.01 |
| `ob_r`, `ob_g`, `ob_b`, `ob_a` | 0.0 |

**Inner Border (`ib_*`):**
| Key | Default |
|-----|---------|
| `ib_size` | 0.01 |
| `ib_r`, `ib_g`, `ib_b` | 0.25 |
| `ib_a` | 0.0 |

### Motion Vectors

| Key | Default | Purpose |
|-----|---------|---------|
| `nMotionVectorsX` | 12.0 | Grid width |
| `nMotionVectorsY` | 9.0 | Grid height |
| `mv_dx`, `mv_dy` | 0.0 | Offset |
| `mv_l` | 0.9 | Length decay |
| `mv_r`, `mv_g`, `mv_b`, `mv_a` | varies | Color & visibility |

### Version Tags

| Key | Purpose |
|-----|---------|
| `MILKDROP_PRESET_VERSION` | 100=MD1.x, 200=MD2.0, 201+=MD2.x (required) |
| `PSVERSION` | Shader version for MD2.0 |
| `PSVERSION_WARP` | Warp shader version (MD2.x) |
| `PSVERSION_COMP` | Composite shader version (MD2.x) |

---

## Expression Code System

### Available Contexts

Expression code runs in these contexts (in order per frame):

1. **per_frame_init** — Runs once on preset load; sets up initial state
2. **per_frame** — Runs at start of each frame before rendering
3. **per_pixel** — Runs per warp mesh vertex (can modulate per-vertex transforms)
4. **Custom wave init/per_frame/per_point** — Per-waveform custom code
5. **Custom shape init/per_frame** — Per-shape custom code
6. **Shaders (warp/composite)** — HLSL code for GPU effects

### Code Assembly

All code keys numbered sequentially with **no gaps**:

```
per_frame_1=zoom=1.0;
per_frame_2=rot=time*0.1;
per_frame_3=cx=0.5+0.1*sin(time);
```

If `per_frame_2` is missing, parsing stops after `per_frame_1`.

### Variable Communication via Q Variables

The primary inter-context channel: **32 Q variables** (`q1` through `q32`)

- **Set in:** per_frame_init and per_frame
- **Read in:** per_pixel, custom waves, custom shapes, shaders
- **Scope:** Copied from per_frame into other contexts each frame

Example:

```
per_frame_init=SPEED=1.0;
per_frame_1=q1=sin(time*SPEED);
          =q2=cos(time*SPEED);
per_pixel_1=zoom=1.0+q1*0.1;
            zoom=zoom+q2*rad*0.05;
```

### Global Persistent State

**Registers (reg00–reg99):** 100 double-precision values shared across all contexts, persist across frames:

```
per_frame_init=reg00=0;
per_frame_1=reg00=reg00+1;
```

**Megabuffer:** Global memory via `gmegabuf` / `megabuf(index)`:

```
per_pixel_1=gmegabuf[x*100+y] = rad;
```

### Wave/Shape Local State

**T variables (t1–t8):** Local to each custom wave/shape, persist across init → per_frame → per_point:

```
wave_0_init_1=t1=0;
              t2=1;
wave_0_per_frame_1=t1=t1+0.01;
wave_0_per_point_1=x=0.5+t1*sin(sample*2*pi);
```

---

## Per-Frame Equations

### Execution Timing

Runs once per frame **before** per-pixel, waveforms, and shapes. Updates preset-level state.

### Available Variables

All common time/audio variables plus preset parameters:

| Variable | Type | Purpose |
|----------|------|---------|
| `time` | float | Elapsed time in seconds |
| `fps` | float | Current frames per second |
| `frame` | int | Frame counter (increments each frame) |
| `progress` | float | Preset blend progress (0-1 when transitioning) |
| `bass`, `mid`, `treb` | float | Current beat detection levels |
| `bass_att`, `mid_att`, `treb_att` | float | Attenuated (smoothed) beat levels |
| `decay` | float | Decay amount (writable) |
| `gamma` | float | Gamma adjustment |
| `zoom`, `rot`, `warp` | float | Transform parameters (writable) |
| `cx`, `cy` | float | Rotation center (writable) |
| `dx`, `dy` | float | Translation push (writable) |
| `sx`, `sy` | float | Stretch amounts (writable) |
| `wave_*` | float | Wave color/position/mode (writable) |
| `ob_*`, `ib_*` | float | Border parameters (writable) |
| `mv_*` | float | Motion vector parameters (writable) |
| `blur*_min`, `blur*_max` | float | Blur ranges (writable) |
| `q1` through `q32` | float | Communication with other contexts (writable) |

### Example: Breathing Zoom

```
per_frame_init=MIN=0.9; MAX=1.1;
per_frame_1=zoom=MIN+0.5*(MAX-MIN)*(1.0+sin(time*2.0))/2.0;
```

### Example: Reactive Colors to Bass

```
per_frame_1=q1=clamp(bass,0,1);
            wave_r=1.0;
            wave_g=1.0-q1*0.5;
            wave_b=q1*0.7;
```

### Example: Rotating Warp Center

```
per_frame_init=ROT_SPEED=0.5;
per_frame_1=cx=0.5+0.2*cos(time*ROT_SPEED);
            cy=0.5+0.2*sin(time*ROT_SPEED);
```

---

## Per-Pixel/Vertex Equations

### Purpose

Runs once per warp mesh vertex. Allows per-location transformations to create mesh distortion effects.

### Key Difference from Per-Frame

- Variables are applied **per vertex** instead of globally
- Access to `x`, `y`, `rad`, `ang` (vertex position in 0-1 normalized space)
- Most transform variables are **writable** to override per-vertex

### Available Variables

All per-frame variables PLUS:

| Variable | Type | Purpose |
|----------|------|---------|
| `x`, `y` | float | Vertex position (0-1) |
| `rad` | float | Radial distance from center |
| `ang` | float | Angle in radians (0-2π) |
| `zoom` | float | Writable: per-vertex zoom |
| `zoomexp` | float | Writable: zoom exponent |
| `rot` | float | Writable: per-vertex rotation |
| `warp` | float | Writable: per-vertex warp |
| `cx`, `cy`, `dx`, `dy`, `sx`, `sy` | float | All writable per-vertex |
| `meshx`, `meshy` | int | Mesh grid dimensions |
| `pixelsx`, `pixelsy` | int | Viewport dimensions |
| `aspectx`, `aspecty` | float | Aspect ratio scaling |

**Note:** `aspectx`/`aspecty` in per-pixel context is the actual aspect ratio (unlike per-frame where they're inverted).

### Example: Radial Zoom (Tunnel Effect)

```
per_pixel_1=zoom=1.0-rad*0.3;
```

### Example: Circular Warp

```
per_pixel_1=warp=0.5+0.5*sin(rad*10);
```

### Example: Distance-Based Rotation

```
per_pixel_1=rot=rot+ang;
```

### Example: Vignette Darkness

```
per_pixel_1=decay=decay*(1.0-rad*0.5);
```

---

## Custom Waves

### Overview

4 custom waveforms (indices 0–3) render audio as animated lines or dots. Each has:
- Static parameters loaded from preset
- Per-frame code (runs once per frame)
- Per-point code (runs per sample point)

### Static Parameters

Loaded from `wavecode_N_*` keys (N = 0–3):

| Key | Default | Type | Purpose |
|-----|---------|------|---------|
| `wavecode_N_enabled` | false | bool | Enable this waveform |
| `wavecode_N_samples` | 512 | int | Number of sample points to render |
| `wavecode_N_sep` | 0 | float | Stereo channel separation |
| `wavecode_N_bSpectrum` | false | bool | Use FFT spectrum vs PCM waveform |
| `wavecode_N_bUseDots` | false | bool | Draw as dots instead of line |
| `wavecode_N_bDrawThick` | false | bool | Draw thick lines |
| `wavecode_N_bAdditive` | false | bool | Additive blend mode |
| `wavecode_N_scaling` | 1.0 | float | Scale amplitude |
| `wavecode_N_smoothing` | 0.5 | float | Smoothing (0-1) |
| `wavecode_N_r`, `_g`, `_b`, `_a` | 1.0 | float | Base color (RGBA) |

### Per-Frame Code

Keys: `wave_N_per_frame_1`, `wave_N_per_frame_2`, ...

Runs once per frame **before** per-point code. Can modify wave parameters and T variables.

**Available variables:**

| Variable | Type | Purpose |
|----------|------|---------|
| `time`, `fps`, `frame`, `progress` | float/int | Timing info |
| `q1` through `q32` | float | From preset per_frame |
| `t1` through `t8` | float | Local T variables (writable, persist to per_point) |
| `bass`, `mid`, `treb`, `bass_att`, `mid_att`, `treb_att` | float | Audio data |
| `r`, `g`, `b`, `a` | float | Wave color (writable, affects all points) |
| `samples` | int | Number of points to render (writable) |

### Per-Point Code

Keys: `wave_N_per_point_1`, `wave_N_per_point_2`, ...

Runs once per sample point. Computes rendering position and color for each point.

**Available variables:**

| Variable | Type | Purpose |
|----------|------|---------|
| `sample` | float | Normalized sample index (0-1) |
| `value1` | float | Left channel audio value |
| `value2` | float | Right channel audio value |
| `x` | float | Output X position (writable; default: 0.5 + value1) |
| `y` | float | Output Y position (writable; default: 0.5 + value2) |
| `r`, `g`, `b`, `a` | float | Output point color (writable, inherits from per_frame) |
| `t1` through `t8` | float | Local T variables (readable) |
| `bass`, `mid`, `treb`, `bass_att`, `mid_att`, `treb_att` | float | Audio (read-only) |
| `time`, `fps`, `frame`, `progress` | float/int | Timing (read-only) |
| `q1` through `q32` | float | From preset (read-only) |

### Example: Spectrum Analyzer

```
wavecode_0_enabled=1
wavecode_0_bSpectrum=1
wavecode_0_samples=256
wavecode_0_r=1.0
wavecode_0_g=0.0
wavecode_0_b=1.0
wavecode_0_bAdditive=1

wave_0_per_frame_1=a=0.5+0.4*mid;

wave_0_per_point_1=x=sample;
                  =y=0.5+value1*0.3;
                  =r=sample;
                  =b=1.0-sample;
```

### Example: Circular Waveform

```
wavecode_1_enabled=1
wavecode_1_scaling=0.4
wavecode_1_bAdditive=1

wave_1_per_frame_1=t1=time*0.3;

wave_1_per_point_1=ang=sample*2*pi+t1;
                  =r=cos(ang)*0.5+0.5;
                  =g=sin(ang)*0.5+0.5;
                  =x=0.5+0.3*cos(ang)*value1;
                  =y=0.5+0.3*sin(ang)*value1;
```

---

## Custom Shapes

### Overview

4 custom shapes (indices 0–3). Each shape is a polygon that can be animated and textured.

### Static Parameters

Loaded from `shapecode_N_*` keys (N = 0–3):

| Key | Default | Type | Purpose |
|-----|---------|------|---------|
| `shapecode_N_enabled` | false | bool | Enable shape |
| `shapecode_N_sides` | 4 | int | Number of sides (3-100) |
| `shapecode_N_additive` | false | bool | Additive blending |
| `shapecode_N_thickOutline` | false | bool | Thick border |
| `shapecode_N_textured` | false | bool | Apply texture |
| `shapecode_N_num_inst` | 1 | int | Number of instances |
| `shapecode_N_x`, `_y` | 0.5 | float | Center position |
| `shapecode_N_rad` | 0.1 | float | Radius (1.0 = screen size) |
| `shapecode_N_ang` | 0.0 | float | Rotation angle |
| `shapecode_N_tex_ang` | 0.0 | float | Texture rotation |
| `shapecode_N_tex_zoom` | 1.0 | float | Texture zoom |
| `shapecode_N_r`, `_g`, `_b`, `_a` | varies | float | Center fill color (RGBA) |
| `shapecode_N_r2`, `_g2`, `_b2`, `_a2` | varies | float | Edge fill color (RGBA) |
| `shapecode_N_border_r`, `_g`, `_b`, `_a` | 1.0 | float | Border color |
| `shapecode_N_image` | "" | string | Texture filename (optional) |

### Per-Frame Code

Keys: `shape_N_per_frame_1`, `shape_N_per_frame_2`, ...

Runs once per frame. Modulates shape appearance.

**Available variables:**

| Variable | Type | Purpose |
|----------|------|---------|
| `time`, `fps`, `frame`, `progress` | float/int | Timing |
| `q1` through `q32` | float | From preset per_frame |
| `t1` through `t8` | float | Local T variables (writable) |
| `bass`, `mid`, `treb`, `bass_att`, `mid_att`, `treb_att` | float | Audio |
| `x`, `y` | float | Position (writable) |
| `rad`, `ang` | float | Radius and angle (writable) |
| `tex_ang`, `tex_zoom` | float | Texture transform (writable) |
| `sides` | int | Number of sides (writable) |
| `textured` | bool | Texture flag (writable) |
| `additive` | bool | Additive flag (writable) |
| `thick` | bool | Thick outline flag (writable) |
| `num_inst`, `instance` | int | Total instances and current index |
| `r`, `g`, `b`, `a` | float | Center fill color (writable) |
| `r2`, `g2`, `b2`, `a2` | float | Edge fill color (writable) |
| `border_r`, `border_g`, `border_b`, `border_a` | float | Border color (writable) |

### Example: Pulsing Star

```
shapecode_0_enabled=1
shapecode_0_sides=5
shapecode_0_r=1.0
shapecode_0_g=1.0
shapecode_0_b=0.0
shapecode_0_a=0.8

shape_0_per_frame_1=rad=0.2+0.1*sin(time*2);
                   =ang=time;
                   =r=1.0-bass*0.3;
```

### Example: Multiple Rotating Shapes

```
shapecode_1_enabled=1
shapecode_1_sides=6
shapecode_1_num_inst=3
shapecode_1_rad=0.15

shape_1_per_frame_1=ANG_SPEED=1.0;
                   =instance_offset=instance*2*pi/num_inst;
                   =ang=time*ANG_SPEED+instance_offset;
                   =x=0.5+0.25*cos(ang);
                   =y=0.5+0.25*sin(ang);
                   =r=0.3+0.7*sin(time+instance);
```

---

## Shader System (Warp & Composite)

### Overview

Two optional HLSL shader stages:
1. **Warp:** Distorts the rendered frame (outputs modified UV coordinates)
2. **Composite:** Post-processes the final image

### File Format

Shaders are provided as **backtick-prefixed lines** in the .milk file:

```
`code line 1
`code line 2
`code line 3
```

The backticks are stripped during parsing; the resulting text is assembled sequentially.

### Code Assembly

- **Warp shader:** `warp_1`, `warp_2`, ... (concatenated)
- **Composite shader:** `comp_1`, `comp_2`, ... (concatenated)

Each line should be prefixed with backtick in the .milk file:

```
warp_1=`float rad = length(uv - 0.5);
warp_2=`float ang = atan(uv.y - 0.5, uv.x - 0.5);
warp_3=`ret = tex2D(sampler_main, uv + rad * 0.1 * sin(ang)).xyz;
```

### Shader Preprocessing

The engine wraps your code in a function signature:

**Warp:**
```hlsl
void PS(float4 _vDiffuse : COLOR,
        float4 _uv : TEXCOORD0,          // xy = current UV, zw = original UV
        float2 _rad_ang : TEXCOORD1,
        out float4 _return_value : COLOR0,
        out float4 _mv_tex_coords : COLOR1)
{
    float3 ret = 0;
    _mv_tex_coords.xy = _uv.xy;           // <-- warp-specific

    // Your code here

    _return_value = float4(ret.xyz, 1.0);
}
```

**Composite:**
```hlsl
void PS(float4 _vDiffuse : COLOR,
        float2 _uv : TEXCOORD0,
        float2 _rad_ang : TEXCOORD1,
        out float4 _return_value : COLOR)
{
    float3 ret = 0;

    // Your code here

    _return_value = float4(ret.xyz, 1.0);
}
```

### Built-in Aliases

These are automatically #define'd:

| Alias | Maps To | Purpose |
|-------|---------|---------|
| `uv` | `_uv.xy` | Current/target UV coordinates |
| `uv_orig` | `_uv.zw` (warp) or `_uv.xy` (comp) | Original UV (for reference) |
| `rad` | `_rad_ang.x` | Radial distance from center |
| `ang` | `_rad_ang.y` | Angle from center |
| `hue_shader` | `_vDiffuse.xyz` | Input hue (composite only) |

### Uniform Constants

Available as float4 vectors in shader code:

| Uniform | Contents | Purpose |
|---------|----------|---------|
| `rand_frame` | 4 random floats | Change every frame |
| `rand_preset` | 4 random floats | Constant for preset lifetime |
| `_c0` | `(aspectX, aspectY, 1/aspectX, 1/aspectY)` | Aspect ratios |
| `_c2` | `(time, fps, frame, progress)` | Timing |
| `_c3` | `(bass, mid, treb, vol)` | Current audio |
| `_c4` | `(bass_att, mid_att, treb_att, vol_att)` | Smoothed audio |
| `_c5` | `(blur1Max-Min, blur1Min, blur2Max-Min, blur2Min)` | Blur ranges |
| `_c7` | `(viewportW, viewportH, 1/W, 1/H)` | Viewport dimensions |
| `_c8`, `_c9` | 4 fast cos/sin oscillators | Fast animation (varies with time) |
| `_c10`, `_c11` | 4 slow cos/sin oscillators | Slow animation |
| `_qa` through `_qh` | q1–q32 packed as float4 | Preset Q variables |
| `rot_s1`–`rot_s4` | Rotation matrices (slow) | Stable 3D rotation |
| `rot_d1`–`rot_d4` | Rotation matrices (medium) | Medium speed 3D |
| `rot_f1`–`rot_f4` | Rotation matrices (fast) | Fast 3D rotation |
| `rot_vf1`–`rot_vf4` | Rotation matrices (very fast) | Very fast 3D |
| `rot_rand1`–`rot_rand4` | Random rotation matrices | Per-frame random |

### Texture Samplers

Built-in and user textures:

| Sampler | Purpose |
|---------|---------|
| `sampler_main` | Current frame render |
| `sampler_blur1`, `sampler_blur2`, `sampler_blur3` | Blur passes |
| `sampler_fw_noise_lq` | Low-quality noise texture |
| `sampler_fw_noisevol_lq` | Noise with volume modulation |
| `sampler_randXX` (00–15) | Random texture slots (persistent per preset) |
| `sampler_<custom_name>` | User texture (loaded by filename) |

Texture dimensions via `texsize_<name>` float4: `(width, height, 1/width, 1/height)`

### Example: Simple Warp (Radial Distortion)

```
warp_1=`float d = length(uv - 0.5) * 0.5;
warp_2=`float a = atan(uv.y - 0.5, uv.x - 0.5);
warp_3=`uv += d * sin(a + _c2.x) * 0.1;
warp_4=`ret = tex2D(sampler_main, uv).xyz;
```

### Example: Composite with Bass Reaction

```
comp_1=`float bass = _c3.x;
comp_2=`float glow = bass * 0.3;
comp_3=`ret = hue_shader * (1.0 + glow);
comp_4=`ret = clamp(ret, 0.0, 1.0);
```

### Example: Kaleidoscope via Polar Mapping

```
warp_1=`float2 c = uv - 0.5;
warp_2=`float r = length(c);
warp_3=`float a = atan(c.y, c.x);
warp_4=`a = fmod(a * 6.0, 6.283185307);
warp_5=`float2 new_uv = 0.5 + r * float2(cos(a), sin(a));
warp_6=`ret = tex2D(sampler_main, new_uv).xyz;
```

---

## Code Generation Patterns

### Pattern 1: Simple Animation Loop

```python
def generate_breathing_effect():
    return {
        'fDecay': 0.98,
        'per_frame_1': 'zoom = 0.9 + 0.1 * sin(time * 2);',
        'per_frame_2': 'rot = time * 0.5;',
    }
```

### Pattern 2: Audio-Reactive Variables

```python
def generate_bass_reactive():
    return {
        'per_frame_init': 'MIN_ZOOM = 0.8; MAX_ZOOM = 1.2; BASS_SENS = 0.5;',
        'per_frame_1': 'q1 = clamp(bass, 0, 1);',
        'per_frame_2': 'zoom = MIN_ZOOM + (MAX_ZOOM - MIN_ZOOM) * q1 * BASS_SENS;',
    }
```

### Pattern 3: Numbered Code Assembly

```python
def add_per_frame_code(lines):
    """Converts list of code lines to numbered keys"""
    result = {}
    for i, line in enumerate(lines, 1):
        result[f'per_frame_{i}'] = line
    return result

# Usage:
code_lines = [
    'zoom = 1.0 + 0.1 * sin(time);',
    'rot = rot + 0.01;',
    'wave_r = 1.0 - mid * 0.5;',
]
preset = add_per_frame_code(code_lines)
```

### Pattern 4: Custom Wave Generation

```python
def generate_spectrum_wave():
    return {
        'wavecode_0_enabled': 1,
        'wavecode_0_bSpectrum': 1,
        'wavecode_0_samples': 512,
        'wavecode_0_bAdditive': 1,
        'wavecode_0_r': 1.0,
        'wavecode_0_g': 0.0,
        'wavecode_0_b': 1.0,
        'wavecode_0_a': 0.7,
        'wave_0_per_frame_1': 'a = 0.5 + 0.4 * bass;',
        'wave_0_per_point_1': 'x = sample;',
        'wave_0_per_point_2': 'y = 0.5 + value1 * 0.3;',
    }
```

### Pattern 5: Multi-Instance Shapes

```python
def generate_rotating_shapes(num_shapes=3):
    result = {
        'shapecode_0_enabled': 1,
        'shapecode_0_sides': 6,
        'shapecode_0_num_inst': num_shapes,
        'shapecode_0_rad': 0.15,
        'shapecode_0_r': 1.0,
        'shapecode_0_g': 0.5,
    }

    frame_code = [
        f'ANG_SPEED = 1.0;',
        f'inst_ang = time * ANG_SPEED + instance * 2 * pi / {num_shapes};',
        f'x = 0.5 + 0.25 * cos(inst_ang);',
        f'y = 0.5 + 0.25 * sin(inst_ang);',
    ]

    for i, line in enumerate(frame_code, 1):
        result[f'shape_0_per_frame_{i}'] = line

    return result
```

### Pattern 6: Building a Complete Preset

```python
def build_complete_preset(name, params, per_frame_code, custom_wave=None):
    """Assemble all components into a .milk-compatible dict"""

    preset = {
        'MILKDROP_PRESET_VERSION': 200,
    }

    # Add basic parameters
    preset.update(params)

    # Add per_frame code (ensure no gaps)
    for i, line in enumerate(per_frame_code, 1):
        preset[f'per_frame_{i}'] = line

    # Add optional custom wave
    if custom_wave:
        preset.update(custom_wave)

    return preset

# Usage:
my_preset = build_complete_preset(
    name='MyPreset',
    params={
        'fDecay': 0.96,
        'fGammaAdj': 2.2,
        'zoom': 1.0,
    },
    per_frame_code=[
        'zoom = zoom * 0.99;',
        'rot = rot + mid * 0.01;',
    ],
)
```

---

## Example Presets

### Minimal Preset

```ini
[preset00]
MILKDROP_PRESET_VERSION=200
fDecay=0.98
fGammaAdj=2.0
zoom=1.0
```

### Breathing Zoom with Wave

```ini
[preset00]
MILKDROP_PRESET_VERSION=200
fDecay=0.98
fGammaAdj=2.0

per_frame_init=MIN=0.8; MAX=1.2;
per_frame_1=zoom = MIN + 0.5 * (MAX - MIN) * (1.0 + sin(time)) / 2.0;

wavecode_0_enabled=1
wavecode_0_bSpectrum=0
wavecode_0_samples=256
wavecode_0_r=1.0
wavecode_0_g=0.5
wavecode_0_b=1.0

wave_0_per_frame_1=a = 0.3 + 0.5 * bass;
wave_0_per_point_1=x = sample;
wave_0_per_point_2=y = 0.5 + value1 * 0.2;
```

### Bass-Reactive Shape

```ini
[preset00]
MILKDROP_PRESET_VERSION=200
fDecay=0.95

per_frame_1=q1 = clamp(bass, 0, 1);
per_frame_2=q2 = clamp(mid, 0, 1);

shapecode_0_enabled=1
shapecode_0_sides=5
shapecode_0_x=0.5
shapecode_0_y=0.5
shapecode_0_r=1.0
shapecode_0_g=0.0
shapecode_0_b=0.0
shapecode_0_a=0.8

shape_0_per_frame_1=rad = 0.1 + q1 * 0.2;
shape_0_per_frame_2=ang = time + q2 * pi;
shape_0_per_frame_3=r = q1;
shape_0_per_frame_4=g = q2 * 0.5;
```

### Warp Shader Example

```ini
[preset00]
MILKDROP_PRESET_VERSION=200
fDecay=0.97
PSVERSION_WARP=3

per_frame_1=q1 = sin(time * 0.5);

warp_1=`float d = length(uv - 0.5);
warp_2=`float a = atan(uv.y - 0.5, uv.x - 0.5);
warp_3=`uv += 0.1 * sin(a * 3.0 + _c2.x) * d;
warp_4=`ret = tex2D(sampler_main, uv).xyz;
```

---

## Reference Resources

- **Test presets:** `/home/user/Project-M/presets/tests/` — Real-world examples
  - `001-line.milk` — Minimal structure
  - `101-per_frame.milk` — Per-frame equations
  - `110-per_pixel.milk` — Per-pixel warp
  - `250-wavecode.milk` — Custom waves
  - `260-compshader-noise_lq.milk` — Composite shaders
  - `300-beatdetect-bassmidtreb.milk` — Audio reactivity

- **Source code:**
  - Parser: `/home/user/Project-M/src/libprojectM/MilkdropPreset/PresetFileParser.cpp`
  - Execution: `/home/user/Project-M/src/libprojectM/MilkdropPreset/` (per-frame, per-pixel, wave, shape contexts)
  - Shaders: `/home/user/Project-M/src/libprojectM/MilkdropPreset/MilkdropPreset.cpp` (warp/composite assembly)

---

## Key Takeaways for AI Agents

1. **Code is numbered sequentially** — Use `per_frame_1`, `per_frame_2`, etc. with no gaps
2. **Q variables are the bridge** — Set in per_frame, read in per_pixel/waves/shapes
3. **T variables are local** — Each wave/shape has its own `t1`–`t8`, persistent across init/per_frame/per_point
4. **Shaders need backticks** — In .milk files, shader lines start with backtick; the parser strips them
5. **Audio data is immediate** — `bass`, `mid`, `treb`, `bass_att`, etc. are available in every context
6. **Warp modulates per-vertex** — `per_pixel` code runs once per mesh vertex, allowing per-location transforms
7. **Custom waves use FFT or PCM** — Set `wavecode_N_bSpectrum` to choose spectrum (FFT) vs waveform (PCM)
8. **Shapes are instanced** — Set `num_inst` to draw multiple copies; loop with `instance` index

