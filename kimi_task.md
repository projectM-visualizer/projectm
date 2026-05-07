# Kimi Task: Finish & Polish Dual-Pipeline Preset Transition System

**Repo:** github.com/ford442/Project-M  
**Branch:** main  
**Date:** 2026-05-07  
**Context:** Dual-FBO / transition work started in PR #46 (WIP). Most of the heavy lifting is already done.

---

## Current Status (Accurate Summary from Investigation)

Your dual-FBO / transition system is **already mostly complete** and follows a clean **Milkdrop-style split architecture** (actually better than a single monolithic `TransitionState`):

| Area                    | Status     | Details |
|-------------------------|------------|---------|
| Dual texture binding    | ✅ Fully working | `iChannel0` = old preset, `iChannel1` = new preset |
| Progress uniforms       | ✅ Excellent | `durationParams.x/y/z/w` (linear + smoothed versions) |
| Extra parameters        | ✅ Very good | `iRandStatic`, `iRandFrame`, beat data, `iResolution`, `iTime` |
| Aspect ratio            | ⚠️ Partial | Only via `iResolution`. No dedicated `iAspectX`/`iAspectY` |
| Ownership model         | ✅ Milkdrop-compatible | `PresetTransition` + `MilkdropPreset` (dual `m_framebuffer[2]`) |
| Transition shaders      | ✅ Expanded | 17 shaders registered (all your new ones included) |

**Architecture highlights:**
- `MilkdropPreset` owns the dual framebuffers (`m_framebuffer[0]` / `m_framebuffer[1]`) and handles the old/new swap.
- `PresetTransition` owns the blending logic and the transition shader.
- This is very close to real Milkdrop — excellent design.

---

## Goal

Make the system **production-ready** with:
- Proper aspect-ratio correction in all transition shaders
- Full consistency across all 17 shaders
- Better robustness and documentation
- Clean Emscripten/WASM support

---

## Step-by-Step Tasks for kimi-cli

### Step 1: Add Dedicated Aspect Uniforms (Recommended)

In `PresetTransition::Draw(...)`, also pass these uniforms:

```cpp
shader->setUniform("iAspectX",     renderContext.aspectX);
shader->setUniform("iAspectY",     renderContext.aspectY);
shader->setUniform("iInvAspectX",  renderContext.invAspectX);
shader->setUniform("iInvAspectY",  renderContext.invAspectY);
```

### Step 2: Update the Shared Transition Shader Header

Add these lines to the common header used by all transition shaders:

```glsl
uniform float iAspectX;
uniform float iAspectY;
uniform float iInvAspectX;
uniform float iInvAspectY;
```

**File likely involved:** `src/libprojectM/Renderer/TransitionShaderHeader.glsl` (or wherever the common header is defined).

### Step 3: Audit & Fix All 17 Transition Shaders

Ensure **every** shader uses consistent naming:

- `iChannel0` and `iChannel1` (not `texture0`/`texture1`)
- `durationParams.x` (or `.y`/`.z`) for progress
- Optionally use the new `iAspect*` uniforms for correct geometry

**Priority shaders to fix first (most geometry-sensitive):**
1. `TransitionShaderBuiltInPageCurlGlsl330.frag`
2. `TransitionShaderBuiltInGlitchGlsl330.frag`
3. `TransitionShaderBuiltInHeatWaveGlsl330.frag`
4. `TransitionShaderBuiltInKaleidoscopeGlsl330.frag`
5. `TransitionShaderBuiltInWaterDropGlsl330.frag`
6. `TransitionShaderBuiltInMosaicZoomGlsl330.frag`
7. `TransitionShaderBuiltInPixelateGlsl330.frag`

**All other shaders** should also be audited for consistency.

### Step 4: Add Simple Validation / Fallback

In `PresetTransition`, add a safety check:

```cpp
if (!iChannel0 || !iChannel1) {
    // fallback to SimpleBlend or log warning
}
```

### Step 5: Emscripten / WASM Notes

- Confirm FBOs use `GL_RGBA` + `GL_UNSIGNED_BYTE`
- Ensure texture units 0 and 1 are correctly activated before binding
- Test rapid preset switching (the recent framebuffer detach fix should help here)

### Step 6: Final Polish & Documentation

- Add clear comments in:
  - `PresetTransition.cpp`
  - `MilkdropPreset.cpp` (explaining dual `m_framebuffer[2]` ownership and swap logic)
- Consider exposing a public API to let users choose a specific transition (or force random)

---

## Files kimi-cli Will Likely Need to Touch

| File | Purpose |
|------|---------|
| `src/libprojectM/Renderer/PresetTransition.cpp` | Main blending + uniform passing logic |
| `src/libprojectM/Renderer/MilkdropPreset.cpp` | Dual framebuffer ownership and swap |
| `src/libprojectM/Renderer/TransitionShaderHeader.glsl` (or equivalent) | Shared uniforms |
| All 17 `TransitionShaderBuiltIn*Glsl330.frag` files | Consistency fixes |
| `src/libprojectM/Renderer/CMakeLists.txt` | If any new headers are added |

---

## Success Criteria

- All 17 transition shaders use consistent `iChannel0`/`iChannel1` + `durationParams`
- PageCurl, Glitch, HeatWave, etc. look correct at non-square aspect ratios
- No crashes or visual glitches during transitions (including rapid switching)
- System works cleanly on both desktop and Emscripten/WASM builds
- Code is well-commented and easy to maintain

---

**Ready when you are.**  
Start with **Step 1 + Step 2** (aspect uniforms) — they are the highest-impact missing piece.

Let me know when you're done with a step and I'll review or give the next one.