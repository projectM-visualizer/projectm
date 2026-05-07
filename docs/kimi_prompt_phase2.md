You are an expert C++ / OpenGL developer on Project-M.

### Current Status
Phase 1 (Aspect uniforms + dual texture robustness) is complete and building cleanly.

### Goal: Phase 2 – Milkdrop-Style Timing + Quality

We now want the transitions to feel more natural and high-quality, like classic Milkdrop.

---

## Tasks for This Session

### Task 1: Add Easing Support

Add support for different easing curves controlled by `durationParams`.

**Recommended implementation:**

1. Add a new uniform or reuse `durationParams.w` (or add a new one) to select easing type.
2. Create a small helper function in GLSL (in the header or per-shader) like:

```glsl
float ease(float t, int type) {
    if (type == 0) return t;                    // Linear
    if (type == 1) return t * t * (3.0 - 2.0 * t); // Smoothstep
    if (type == 2) return t * t;                // Ease-in
    if (type == 3) return 1.0 - (1.0 - t) * (1.0 - t); // Ease-out
    return t;
}
```

3. Use the eased value for `progress` inside the transition shaders.

**Files to touch:**
- `TransitionShaderHeaderGlsl330.frag` (add the ease function)
- `PresetTransition.cpp` (pass easing type via uniform)
- Optionally expose easing choice in the transition system

### Task 2: Improve Texture Filtering

Ensure high-quality sampling during transitions:

- Set `GL_TEXTURE_MIN_FILTER` and `GL_TEXTURE_MAG_FILTER` to `GL_LINEAR` (or `GL_LINEAR_MIPMAP_LINEAR` if mipmaps are generated) for both `iChannel0` and `iChannel1`.
- Add anisotropic filtering if the platform supports it (desktop only).
- Add a comment explaining the filtering strategy.

**Files:**
- `PresetTransition.cpp` (when binding textures)
- Possibly in `MilkdropPreset.cpp` when creating the framebuffers

### Task 3: Make Progress Feel Natural

- Default to using eased progress instead of raw linear progress.
- Consider exposing a global or per-transition "easing strength" parameter.

---

## Rules

- Work in small, testable steps.
- Provide exact diffs where possible.
- Prioritize Emscripten compatibility (avoid features that break WebGL).
- Keep changes minimal and clean.

**Start with Task 1 (Easing Support).**

Report back when Task 1 is done, then we’ll do Task 2.