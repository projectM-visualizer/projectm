# Starter: Advanced Blending Modes (Phase B3)

**Goal:** Add proper blending modes beyond basic additive (multiplicative, screen, masked, etc.)

---

## 1. Proposed BlendMode Enum (C++)

Add to `PresetTransition.hpp`:

```cpp
enum class BlendMode {
    Alpha = 0,        // Default (current behavior)
    Additive,         // Old + New
    Multiplicative,   // Old * New
    Screen,           // 1 - (1-Old)*(1-New)
    Masked,           // Use a mask texture
    Count
};
```

---

## 2. GLSL Blending Library (Add to Header)

```glsl
vec4 blendAlpha(vec4 a, vec4 b, float t) {
    return mix(a, b, t);
}

vec4 blendAdditive(vec4 a, vec4 b, float t) {
    return a + b * t;
}

vec4 blendMultiplicative(vec4 a, vec4 b, float t) {
    return a * (1.0 - t) + (a * b) * t;
}

vec4 blendScreen(vec4 a, vec4 b, float t) {
    vec4 screen = 1.0 - (1.0 - a) * (1.0 - b);
    return mix(a, screen, t);
}

vec4 blendMasked(vec4 a, vec4 b, float t, sampler2D mask) {
    float m = texture(mask, uv).r;
    return mix(a, b, t * m);
}
```

---

## 3. Usage in Shader (Example)

```glsl
vec4 oldColor = texture(iChannel0, uv);
vec4 newColor = texture(iChannel1, uv);

vec4 result;

if (blendMode == 0) result = blendAlpha(oldColor, newColor, iProgressEased);
else if (blendMode == 1) result = blendAdditive(oldColor, newColor, iProgressEased);
else if (blendMode == 2) result = blendMultiplicative(oldColor, newColor, iProgressEased);
else if (blendMode == 3) result = blendScreen(oldColor, newColor, iProgressEased);
// etc.

gl_FragColor = result;
```

---

## 4. First Steps for kimi

1. Add `BlendMode` enum to `PresetTransition.hpp`
2. Add the GLSL blending functions to `TransitionShaderHeaderGlsl330.frag`
3. Add `blendMode` uniform to `PresetTransition`
4. Update one transition (recommend starting with a simple one) to demonstrate a new mode
5. Register default blending mode per shader if needed

---

**Ready to hand to kimi.** Let me know if you want any adjustments to the design.