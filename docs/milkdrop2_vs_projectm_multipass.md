# Milkdrop2 vs Project-M: Multi-Pass Transition Comparison

**Date:** 2026-05-07  
**Goal:** Understand how Milkdrop2 implements multi-pass transitions and design the best equivalent for Project-M (OpenGL + Emscripten).

---

## 1. How Milkdrop2 Handles Multi-Pass

### Core Concepts in Milkdrop2:
- Uses **DirectX render targets** (textures) as intermediate buffers
- Supports **up to 4 passes** in some transitions
- Each pass can have its own:
  - Vertex shader
  - Pixel (fragment) shader
  - Render target
  - Blend mode
- Special built-in variables:
  - `pass` (current pass number)
  - `pass_time` (time within current pass)
  - `lastpass` / `lastpass_tex` (previous pass result)
- The engine automatically manages render target ping-ponging
- Transitions are defined in `.milk` files with `passX_` prefixed code blocks

### Strengths of Milkdrop2 Approach:
- Extremely flexible
- Artists have fine-grained control per pass
- Good performance on DirectX
- Many famous transitions rely on 2–3 passes (e.g. complex page curls, layered effects, feedback)

### Limitations:
- Tightly coupled to DirectX
- Complex for beginners
- Some passes are only used for intermediate calculations (not shown on screen)

---

## 2. How We Should Do It in Project-M (OpenGL)

### Recommended Design Principles:
- Keep it **simpler and more modern** than Milkdrop2
- Use a clean C++ API (`BeginPass()`, `EndPass()`)
- Support **2–3 passes** initially (enough for most effects)
- Make it easy for shader authors
- Stay compatible with Emscripten/WebGL 2.0

### Proposed Architecture for Project-M:

```cpp
// In PresetTransition
void BeginPass(int passNumber);
void EndPass();
GLuint GetPassTexture(int passNumber) const;
```

**Key Differences from Milkdrop2:**

| Aspect                    | Milkdrop2 (DirectX)              | Project-M (OpenGL) Recommendation                  |
|---------------------------|----------------------------------|----------------------------------------------------|
| Render Target Management  | Automatic ping-pong per pass     | Explicit `BeginPass(n)` / `EndPass()`              |
| Max Passes                | Up to 4                          | Start with 2–3, make extensible                    |
| Shader per Pass           | Separate vertex + pixel per pass | Single fragment shader + optional pass number      |
| Variables                 | `pass`, `pass_time`, etc.        | `iPass`, `iPassTime`, `iLastPassTex`               |
| WebGL Compatibility       | N/A                              | Careful with MRT (Multiple Render Targets)         |
| Artist Control            | Very high (per-pass code blocks) | High but simpler API                               |

---

## 3. Proposed First Implementation (Minimal Viable)

### Phase 1 (MVP):
- Support **2 passes** maximum
- Add `BeginPass(0)` and `BeginPass(1)`
- Automatically manage 2 framebuffers
- Expose `iPass` and `iLastPassTex` in GLSL
- Update 1–2 existing transitions to use 2 passes as proof of concept

### Phase 2 (Later):
- Support 3 passes
- Add `iPassTime`
- Allow custom blend modes between passes
- Add better debugging tools

---

## 4. Key Challenges & Solutions

| Challenge                        | Solution for Project-M                                      |
|----------------------------------|-------------------------------------------------------------|
| WebGL 2.0 MRT limitations        | Use sequential passes instead of true MRT                   |
| Managing multiple FBOs           | Simple array of 3 framebuffers + automatic cleanup          |
| Shader complexity                | Keep it simple — one shader with `if (iPass == 0)` branches |
| Performance                      | Reuse framebuffers, avoid unnecessary copies                |
| Artist workflow                  | Provide good examples + clear documentation                 |

---

## 5. Next Steps (Recommended)

1. **Today / Next Session**: Design and implement basic 2-pass support in `PresetTransition`
2. Create helper macros in GLSL header (`iPass`, `iLastPassTex`)
3. Port one existing transition to use 2 passes as a test case
4. Document the new API clearly

---

**Would you like me to start preparing the actual code changes for 2-pass support now?** (API sketch + first diffs)