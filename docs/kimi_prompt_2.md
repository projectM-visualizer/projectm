You are an expert C++ / OpenGL developer working on Project-M (Milkdrop-compatible visualizer).

### Goal: Milkdrop Blending Parity (Realistic Path)

We want our preset transitions to feel as smooth, clean, and high-quality as the original Milkdrop (which uses DirectX render targets very effectively).

We will take a **phased, realistic approach** (Option A) instead of trying to match everything at once.

---

### Current State (already working)
- Dual FBO system exists (`iChannel0` = old, `iChannel1` = new)
- Basic blending via `PresetTransition` + `MilkdropPreset`
- 17 transition shaders registered

---

## Phase 1: Make Current Dual-FBO System Milkdrop-Like (Clean & Robust)

### Task 1.1: Improve Dual Texture Handling
Review and clean up the core dual texture logic in:
- `src/libprojectM/Renderer/PresetTransition.cpp`
- `src/libprojectM/Renderer/MilkdropPreset.cpp`

**Specific improvements:**
- Add clear, Milkdrop-style comments explaining the dual framebuffer ownership and old/new swap logic.
- Ensure correct texture unit activation (`GL_TEXTURE0` + `GL_TEXTURE1`).
- Add basic validation when binding textures.
- Make sure transitions end cleanly with proper resource handling (important for rapid switching and Emscripten).

**Goal:** The dual texture system should feel intentional and well-engineered, not like a hack.

### Task 1.2: Milkdrop-Style Variable Naming (Optional but nice)
Where it makes sense, align variable names with Milkdrop conventions (e.g. `m_framebuffer[0]`, `m_framebuffer[1]`, `progress`, `oldPreset`, `newPreset`).

---

## Phase 2: Add Milkdrop-Style Timing + Quality

### Task 2.1: Improve Transition Timing & Easing
- Add support for different easing curves (linear, smoothstep, ease-in/out) controlled by `durationParams`.
- Expose a way to choose easing type per transition (or globally).
- Make sure `progress` values feel natural (Milkdrop transitions rarely feel linear).

### Task 2.2: Higher Quality Texture Filtering During Transitions
- Ensure both `iChannel0` and `iChannel1` use **linear filtering** + proper mipmaps when possible.
- Add anisotropic filtering if supported (especially important on desktop).
- Document the filtering strategy clearly.

### Task 2.3: Better Aspect Ratio Handling (Already partially done)
- Make sure all transition shaders properly use the `iAspectX` / `iAspectY` uniforms we added.
- Fix any remaining shaders that still look stretched.

---

## Execution Rules for kimi-cli

1. Work in small, verifiable steps.
2. Always apply **exact diffs** when possible.
3. After each major task, briefly describe what was changed and why.
4. Keep the existing Milkdrop-style architecture — do **not** rewrite the dual-FBO system from scratch.
5. Prioritize Emscripten compatibility.

---

**Start with Phase 1, Task 1.1** (Improve Dual Texture Handling).

Report back when Phase 1 is complete, then we’ll move to Phase 2.