# Project-M GLSL Transition Shader System Overview (Updated 2026-05-08)

## 1. How Shaders Are Managed

**Main Class:** `TransitionShaderManager`
- Located in: `src/libprojectM/Renderer/TransitionShaderManager.cpp`
- Currently registers **17 built-in transition shaders**
- Uses `CompileTransitionShader()` to wrap each shader body with:
  - Version + precision header (desktop vs GLES)
  - Common header (`TransitionShaderHeaderGlsl330.frag`)
  - Main entry point wrapper

**Key Files:**
- `TransitionShaderManager.cpp` — registration & compilation
- `TransitionShaderHeaderGlsl330.frag` — shared uniforms + helper functions
- Individual shaders: `TransitionShaderBuiltIn*Glsl330.frag`

---

## 2. Current Uniforms (as of Phase B2)

| Uniform                  | Source                  | Purpose |
|--------------------------|-------------------------|---------|
| `iChannel0`              | Old preset              | Previous preset output |
| `iChannel1`              | New preset              | Current preset output |
| `durationParams`         | `PresetTransition`      | x = linear progress, w = easing type |
| `iProgressEased`         | Macro in header         | Eased progress (recommended) |
| `iAspectX / iAspectY`    | `PresetTransition`      | Aspect ratio correction |
| `iInvAspectX / iInvAspectY` | `PresetTransition`   | Inverse aspect ratios |
| `iResolution`            | Render context          | Screen resolution |
| `iTime`                  | Render context          | Global time |
| `iRandStatic / iRandFrame` | Per-transition        | Random values |
| `iPass`                  | Multi-pass support      | Current pass number (0 = first, 1 = second) |
| `iLastPassTex`           | Multi-pass support      | Previous pass texture (valid in pass 1+) |

---

## 3. Easing System (Added in Phase 2)

- `durationParams.w` now carries the **easing type**
- GLSL helper: `_prjm_getEasedProgress(t, easingType)`
- All 17 shaders now default to `iProgressEased`
- Supported modes:
  - 0 = Linear
  - 1 = Smoothstep (default)
  - 2 = Ease-in
  - 3 = Ease-out

---

## 4. Multi-Pass System (Added in Phase B2)

### C++ API

`PresetTransition` exposes explicit pass control:

```cpp
// Before Draw(), set how many passes the shader needs.
presetTransition.SetPassCount(2);  // 1 = single-pass (default), 2 = multi-pass

// During Draw(), these are called automatically, but can also be used manually:
presetTransition.BeginPass(0, viewportWidth, viewportHeight);  // Bind intermediate FBO
// ... draw pass 0 ...
presetTransition.EndPass();                                     // Restore original FBO

presetTransition.BeginPass(1, viewportWidth, viewportHeight);  // Bind original FBO
// ... bind iLastPassTex, draw pass 1 ...
presetTransition.EndPass();

// Query state:
int currentPass = presetTransition.GetCurrentPass();            // -1 = none active
auto pass0Tex = presetTransition.GetPassTexture(0);             // intermediate result
```

### GLSL Usage

In a 2-pass transition shader:

```glsl
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;

    if (iPass == 0)
    {
        // Pass 0: compute geometry, base shading, crossfade.
        // Write to the intermediate framebuffer.
        fragColor = vec4(..., 1.0);
    }
    else
    {
        // Pass 1: sample pass 0 result and add post-processing.
        vec3 col = texture(iLastPassTex, uv).xyz;
        // ... add glow, shadows, color grading ...
        fragColor = vec4(col, 1.0);
    }
}
```

### Pass Count Registration

`TransitionShaderManager` tracks per-shader pass counts. When adding a new multi-pass transition, update the constructor:

```cpp
// In TransitionShaderManager::TransitionShaderManager()
candidates.emplace_back(Candidate{kTransitionShaderBuiltInMyEffectGlsl330, 2});
```

### Currently Multi-Pass Transitions

| Transition | Passes | Pass 0 | Pass 1 |
|------------|--------|--------|--------|
| **PageCurl** | 2 | Geometry + base shading (old page, curl ribbon, new page) | Edge glow, highlight, drop shadow |
| **HeatWave** | 2 | Distortion + chromatic crossfade | Heat shimmer, warm tint, central haze |
| **MultiPassTest** | 2 | Basic crossfade | Warm tint + vignette |

---

## 5. Texture Binding (Updated in Phase 2)

In `PresetTransition::Draw()`:
- Explicit `glActiveTexture(GL_TEXTURE0/1)`
- `GL_LINEAR` filtering set on both textures
- Optional anisotropic filtering on desktop
- Proper unbind + reset to `GL_TEXTURE0`
- For multi-pass pass 1: `iLastPassTex` is bound to the next available texture unit after noise textures

---

## 6. Important Notes for Future Work

- **Header is critical** — All new uniforms and helpers must be added to `TransitionShaderHeaderGlsl330.frag`
- **Backward compatibility** — Old macros (`iProgressCosine`, `iProgressBicubic`) are still available
- **Emscripten safety** — All new features must be guarded where necessary
- **Single-pass shaders** — Setting `iPass = 0` in single-pass mode means existing shaders work unchanged
- **FBO safety** — Pass 0 renders to an intermediate FBO; pass 1 samples it. The original draw FBO is restored between passes so sampling is safe.

---

**Last Updated:** After Phase B2 completion (Multi-pass + 2-pass PageCurl/HeatWave)
