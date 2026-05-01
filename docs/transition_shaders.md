# projectM Transition Shaders

This document describes the architecture, authoring interface, and built-in effects for projectM's **preset transition shaders** — the GPU-powered visual effects that blend two presets during a soft cut.

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Effect Authoring Interface](#effect-authoring-interface)
- [Available Uniforms and Samplers](#available-uniforms-and-samplers)
- [Built-in Effects](#built-in-effects)
- [Authoring a New Effect](#authoring-a-new-effect)
- [Tips and Techniques](#tips-and-techniques)
- [Future Enhancements](#future-enhancements)

---

## Architecture Overview

Transition shaders are **Shadertoy-style GLSL fragment shaders** that blend the output of two presets into a single frame. The pipeline works as follows:

### Build-Time Embedding

All transition shader source files live in:

```
src/libprojectM/Renderer/TransitionShaders/
```

During the CMake configuration phase, `GenerateShaderResourcesScript.cmake` reads each `.frag` and `.vert` file and embeds it as a `static std::string` constant inside a generated header:

```cpp
build/src/libprojectM/Renderer/BuiltInTransitionsResources.hpp
```

This means shaders are **compiled into the binary** — there is no runtime filesystem loading for built-in effects.

### Runtime Compilation

When `ProjectM` initializes, it creates a `TransitionShaderManager`. The manager's constructor concatenates four pieces of source code into a complete fragment shader for each effect:

1. **Version header** — `#version 330` (or `#version 300 es` for GLES)
2. **`TransitionShaderHeaderGlsl330.frag`** — Uniform declarations and `#define` macros
3. **Effect body** — The individual `.frag` file for the effect (e.g., `TransitionShaderBuiltInCircleGlsl330.frag`)
4. **`TransitionShaderMainGlsl330.frag`** — Entry point `main()` that calls `mainImage()` and applies triangular-PDF dither

The complete shader is compiled and linked with a shared pass-through vertex shader (`TransitionVertexShaderGlsl330.vert`). All compiled shaders are stored in a `std::vector<std::shared_ptr<Shader>>`.

### Per-Frame Rendering

When a **soft cut** is triggered (`ProjectM::StartPresetTransition()`):

1. The old preset is rendered to a texture (`iChannel0`).
2. The new preset is rendered to a texture (`iChannel1`).
3. A `PresetTransition` object is created with a **randomly selected** transition shader.
4. Each frame, `PresetTransition::Draw()`:
   - Binds the transition shader
   - Sets uniforms (progress, time, audio data, random values, resolution)
   - Binds `iChannel0` and `iChannel1`
   - Binds 10 noise textures
   - Draws a fullscreen quad

The transition ends when the elapsed time exceeds `m_softCutDuration` (default 3.0 seconds).

---

## Effect Authoring Interface

To write a transition effect, you only need to implement one function:

```glsl
void mainImage(out vec4 fragColor, in vec2 fragCoord)
```

- `fragCoord` — Pixel coordinates in screen space (`(0,0)` to `(iResolution.x, iResolution.y)`)
- `fragColor` — The output RGBA color you must write

You do **not** write `main()` — that is provided by `TransitionShaderMainGlsl330.frag`.

### Minimal Example

```glsl
// Simple cosine crossfade
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    vec3 oldImg = texture(iChannel0, uv).xyz;
    vec3 newImg = texture(iChannel1, uv).xyz;
    fragColor = vec4(mix(oldImg, newImg, iProgressCosine), 1.0);
}
```

---

## Available Uniforms and Samplers

### Progress Curves

| Name | Type | Description |
|------|------|-------------|
| `iProgressLinear` | `float` | Linear 0.0 → 1.0 over the transition duration |
| `iProgressCosine` | `float` | Smooth cosine ease 0.0 → 1.0 |
| `iProgressBicubic` | `float` | Smooth bicubic ease 0.0 → 1.0 (slower start/end) |
| `iTransitionDuration` | `float` | Total transition duration in seconds |

> **Tip:** Use `iProgressCosine` for most blends — it looks smoother than linear. Use `iProgressBicubic` for very dramatic effects that need to linger at the extremes.

### Time

| Name | Type | Description |
|------|------|-------------|
| `iTime` | `float` | Seconds since transition started |
| `iTimeDelta` | `float` | Seconds since last frame |

### Audio Reactivity

| Name | Type | Description |
|------|------|-------------|
| `iBass` | `float` | Instantaneous bass level |
| `iMid` | `float` | Instantaneous mid level |
| `iTreb` | `float` | Instantaneous treble level |
| `iBassAtt` | `float` | Attenuated (smoothed) bass level |
| `iMidAtt` | `float` | Attenuated mid level |
| `iTrebAtt` | `float` | Attenuated treble level |

> **Tip:** Audio values are typically in the 0.0–2.0 range. Use them to modulate intensity, speed, or distortion amount.

### Randomness

| Name | Type | Description |
|------|------|-------------|
| `iRandStatic` | `ivec4` | Four random integers fixed for the entire transition |
| `iRandFrame` | `ivec4` | Four random integers that change every frame |

> **Tip:** Use `iRandStatic` to randomize direction, angle, or grid size. Use `iRandFrame` for animated noise, glitch jitter, or sparkles.

### Textures

| Name | Type | Description |
|------|------|-------------|
| `iChannel0` | `sampler2D` | The old (fading out) preset output |
| `iChannel1` | `sampler2D` | The new (fading in) preset output |

### Noise Samplers

These are the same noise textures used by Milkdrop presets:

| Name | Type | Description |
|------|------|-------------|
| `sampler_noise_lq` | `sampler2D` | Low-quality 2D noise (smooth) |
| `sampler_pw_noise_lq` | `sampler2D` | Low-quality 2D noise (nearest/neighbor) |
| `sampler_noise_mq` | `sampler2D` | Medium-quality 2D noise |
| `sampler_pw_noise_mq` | `sampler2D` | Medium-quality 2D noise (nearest) |
| `sampler_noise_hq` | `sampler2D` | High-quality 2D noise |
| `sampler_pw_noise_hq` | `sampler2D` | High-quality 2D noise (nearest) |
| `sampler_noisevol_lq` | `sampler3D` | Low-quality 3D noise volume |
| `sampler_pw_noisevol_lq` | `sampler3D` | Low-quality 3D noise volume (nearest) |
| `sampler_noisevol_hq` | `sampler3D` | High-quality 3D noise volume |
| `sampler_pw_noisevol_hq` | `sampler3D` | High-quality 3D noise volume (nearest) |

Convenience macros are also defined:

```glsl
#define iNoiseLQ            sampler_noise_lq
#define iNoiseLQNearest     sampler_pw_noise_lq
// ... etc
```

> **Note (v18+):** The `iBass`, `iMid`, `iTreb`, `iBassAtt`, `iMidAtt`, `iTrebAtt`, and `iNoise*` macros previously expanded with a trailing semicolon, making them only usable as standalone statements. They are now safe in any expression context (`float x = iBass * 2.0;` works as expected).

### Screen Properties

| Name | Type | Description |
|------|------|-------------|
| `iResolution` | `vec3` | Viewport size in pixels (`xy`) and `0.0` (`z`) |

---

## Built-in Effects

As of the latest version, projectM includes **18 built-in transition effects**.

> **Audio reactivity.** Most effects respond to `iBass`, `iMid`, or `iTreb` (and their attenuated variants) — bass typically drives intensity/zoom, mid drives rotation/spin, and treble drives chromatic and color cycling. See each effect for specifics.

### Geometric Wipes

| Effect | Description |
|--------|-------------|
| **Circle** | Iris wipe with soft edge and feathered chromatic aberration. Randomly opens from center outward or collapses inward. |
| **Sweep** | Angular wipe with randomized angle. Supports dual-edge mode where two wipe fronts converge from opposite sides. |
| **TileFlip** | Grid of tiles that flip like cards. Grid size, flip direction, and per-tile timing are randomized. |
| **SliceSwipe** | 8–22 slices slide off-screen with staggered, hash-randomized per-slice delay and direction. Bass adds a faint sheen across the active slices. |
| **PageCurl** | 3D-ish page-turn with curl shadow, leading-edge highlight, and a darker desaturated paper-back. Mid lightly modulates curl radius. |
| **CubeRotate** | Perspective rotation about X or Y. The old preset rotates away as the new face rolls in, with cosine-based face shading. |

### Distortion Effects

| Effect | Description |
|--------|-------------|
| **Warp** | Directional zoom squeeze with rotational twist. Mid amplifies the twist amount. |
| **WaterDrop** | Expanding ripple distortion from a random center. Bass increases ripple speed and amplitude. |
| **Kaleidoscope** | True reflective N-way symmetry (2 / 3 / 6 / 8) with animated rotation and a radial reveal. Mid drives rotation; bass drives radial zoom-breath. |
| **HeatWave** | Layered noise-based refraction with chromatic split and stratified rising-air falloff. Treble drives shimmer rate; bass amps distortion. |

### Noise / Dissolve Effects

| Effect | Description |
|--------|-------------|
| **Plasma** | Turbulent fractal value-noise dissolve with a color-cycled molten edge. Bass pumps the turbulence; treble cycles the edge hue. |
| **Pixelate** | Progressive pixelation that resolves to sharp pixels of the new preset. Bass kicks momentarily blow up block size during the early phase. |
| **Glitch** | RGB channel split + horizontal scanline displacement. Bass amplifies glitch intensity. |

### Blur Effects

| Effect | Description |
|--------|-------------|
| **ZoomBlur** | Radial zoom blur with chromatic aberration, randomized focal point (50% off-center), tent-weighted samples, and exponential dissolve. Bass amps blur; treble pumps chroma. |
| **MotionBlur** | Directional 16-tap motion blur wipe at a random angle. Bass amplifies blur length. |
| **Dreamy** | Heavy spiral blur, midpoint desaturation, soft pastel lift, filmic highlight bloom, and vignette. Mid drives the haze breathing. |
| **MosaicZoom** | Tiles fly outward from their own centers with per-tile delay and treble-driven rotation jitter. The new preset bleeds in beneath, with drop shadows under each tile. |

### Blends

| Effect | Description |
|--------|-------------|
| **SimpleBlend** | Cosine crossfade with vignette darkening and contrast boost at the midpoint for added punch. |

---

## Authoring a New Effect

### Step 1: Create the Shader File

Create a new file in:

```
src/libprojectM/Renderer/TransitionShaders/TransitionShaderBuiltIn<Name>Glsl330.frag
```

The file must implement `mainImage(out vec4 fragColor, in vec2 fragCoord)`.

### Step 2: Register in CMake

Add the file to `TRANSITION_SHADER_FILES` in:

```
src/libprojectM/Renderer/CMakeLists.txt
```

Alphabetical order is preferred.

### Step 3: Register in C++

Add a `CompileTransitionShader(kTransitionShaderBuiltIn<Name>Glsl330)` call to the initializer list in:

```
src/libprojectM/Renderer/TransitionShaderManager.cpp
```

Again, alphabetical order is preferred.

### Step 4: Build and Test

```bash
cmake --build . --parallel
```

If a shader fails to compile, the `TransitionShaderManager` silently skips it (returns an empty `shared_ptr`). You can check for compilation errors by temporarily adding logging or by inspecting the return value.

---

## Tips and Techniques

### Using Progress Curves

Different progress curves create different emotional feels:

| Curve | Feel | Best For |
|-------|------|----------|
| `iProgressLinear` | Mechanical, even | Progress bars, technical wipes |
| `iProgressCosine` | Smooth, natural | Most blends and fades |
| `iProgressBicubic` | Languid, dramatic | Cinematic reveals, slow burns |

Example — make an effect accelerate at the end:

```glsl
float fastProgress = iProgressCosine * iProgressCosine;
```

### Using Audio Reactivity

Make a distortion effect thump with the bass:

```glsl
float intensity = 0.1 + iBass * 0.2;
```

Or make a wipe speed up on loud beats:

```glsl
float audioBoost = 1.0 + iBassAtt * 0.5;
float progress = iProgressCosine * audioBoost;
progress = clamp(progress, 0.0, 1.0);
```

### Using Noise for Organic Dissolves

Sample 2D noise to create an organic mask:

```glsl
float noise = texture(sampler_noise_lq, uv * 2.0).r;
float mask = smoothstep(iProgressCosine + 0.1, iProgressCosine - 0.1, noise);
```

For animated noise, offset the UVs with time or `iRandFrame`:

```glsl
vec2 noiseUV = uv * 2.0 + vec2(float(iRandFrame.x) * 0.01);
float noise = texture(sampler_noise_lq, noiseUV).r;
```

### Randomization Best Practices

- Use `iRandStatic` for parameters that must stay consistent during the transition (direction, angle, grid size).
- Use `iRandFrame` for per-frame randomness (glitch jitter, sparkle positions).
- Convert integers to usable ranges with `mod()` and multiplication:

```glsl
float angle = mod(float(iRandStatic.x), 360.0);
float direction = (mod(float(iRandStatic.y) * 0.01, 2.0) < 1.0) ? 1.0 : -1.0;
```

### Aspect Ratio Correction

Always correct for non-square viewports when doing circular or radial effects:

```glsl
float aspect = iResolution.y / iResolution.x;
vec2 correctedUV = vec2((uv.x - 0.5) / aspect + 0.5, uv.y);
```

---

## Future Enhancements

The current transition system is intentionally simple. Possible future improvements include:

- **Runtime Custom Shader Loading** — Allow frontends to load `.frag` files from a user-configurable directory at runtime.
- **Weighted / Categorized Selection** — Group effects by type (wipe, blur, dissolve) and allow apps to prefer certain categories.
- **Per-Effect Duration Override** — Let individual effects specify their own ideal duration (e.g., a fast glitch vs. a slow zoom blur).
- **Transition Metadata** — A small JSON or comment-based format for effect names, descriptions, and author credits.
- **C API Exposure** — Functions like `projectm_add_transition_path()` or `projectm_set_transition_category_weights()`.

---

## License

Transition shaders are part of libprojectM and are licensed under the same terms as the project: **LGPL-2.1-only**.
