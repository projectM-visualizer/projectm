# Project-M GLSL Transition Shader System Overview (Updated 2026-05-07)

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

## 2. Current Uniforms (as of Phase 2)

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
| `iPass` (future)         | Multi-pass support      | Current pass number (planned) |
| `iLastPassTex` (future)  | Multi-pass support      | Previous pass texture (planned) |

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

## 4. Texture Binding (Updated in Phase 2)

In `PresetTransition::Draw()`:
- Explicit `glActiveTexture(GL_TEXTURE0/1)`
- `GL_LINEAR` filtering set on both textures
- Optional anisotropic filtering on desktop
- Proper unbind + reset to `GL_TEXTURE0`

---

## 5. Important Notes for Future Work

- **Header is critical** — All new uniforms and helpers must be added to `TransitionShaderHeaderGlsl330.frag`
- **Backward compatibility** — Old macros (`iProgressCosine`, `iProgressBicubic`) are still available
- **Emscripten safety** — All new features must be guarded where necessary
- **Multi-pass (upcoming)** — Will need `iPass` + `iLastPassTex` + framebuffer management

---

**Last Updated:** After Phase 2 completion (Easing + Filtering + Polish)