# Phase B3: Advanced Blending & Compositing

## Current Status (Verified)
- `BlendMode` enum for transitions: **Not present**
- GLSL blending helpers: **Not present**
- `iBlendMode` uniform: **Not present**
- Existing `BlendMode` class is only for OpenGL `glBlendFunc` (sprites/waveforms) — not used by transition shaders.

Transition shaders currently do all compositing manually in GLSL.

## Goal
Add a clean, extensible blending system so transition shaders can choose from multiple compositing modes (Alpha, Additive, Multiplicative, Screen, etc.) while keeping backward compatibility.

## Implementation Plan

### Step 1: Add TransitionBlendMode enum (C++)

**File:** `src/libprojectM/Renderer/PresetTransition.hpp`

Add after existing includes:

```cpp
enum class TransitionBlendMode {
    Alpha = 0,           // Default (mix)
    Additive = 1,
    Multiplicative = 2,
    Screen = 3,
    Masked = 4,          // Future: use a third mask texture
    Count
};
```

Add member:
```cpp
TransitionBlendMode m_blendMode = TransitionBlendMode::Alpha;
```

Add public methods:
```cpp
void SetBlendMode(TransitionBlendMode mode);
TransitionBlendMode GetBlendMode() const;
```

### Step 2: Implement in PresetTransition.cpp

In `Draw()` (after setting other uniforms):
```cpp
shader->setUniform("iBlendMode", static_cast<int>(m_blendMode));
```

Add the setter/getter implementations.

### Step 3: Add GLSL Blending Library

**File:** `src/libprojectM/Renderer/TransitionShaderHeaderGlsl330.frag`

Add at the top (after existing uniforms):

```glsl
uniform int iBlendMode;   // 0=Alpha, 1=Additive, 2=Multiplicative, 3=Screen

vec3 blend(vec3 a, vec3 b, float t) {
    if (iBlendMode == 0) {
        return mix(a, b, t);                    // Alpha
    } else if (iBlendMode == 1) {
        return a + b * t;                       // Additive
    } else if (iBlendMode == 2) {
        return a * mix(vec3(1.0), b, t);        // Multiplicative
    } else if (iBlendMode == 3) {
        return 1.0 - (1.0 - a) * (1.0 - b * t); // Screen
    }
    return mix(a, b, t); // fallback
}
```

### Step 4: Update at Least One Shader

Recommended first shader: `TransitionShaderBuiltInSimpleBlendGlsl330.frag`

Replace the final mix line with:
```glsl
vec3 finalColor = blend(oldColor, newColor, iProgressEased);
```

### Step 5: (Optional) Per-Shader Default

In `TransitionShaderManager.cpp`, add a `defaultBlendMode` field to `TransitionEntry` so each shader can declare its preferred mode.

## Success Criteria

- All 17 transitions still work with default (Alpha) mode
- At least SimpleBlend and one complex shader demonstrate a different mode
- No performance regression
- Clean build on desktop + Emscripten

## First Small Step

Implement Steps 1–3 only. Test with SimpleBlend shader in Alpha mode first, then switch to Additive to verify the new path works.
