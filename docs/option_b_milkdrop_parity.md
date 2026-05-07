# Option B: Milkdrop Blending Parity (Long-term Incremental Project)

**Status:** Planning Phase  
**Approach:** Incremental sessions (at least once per week)  
**Goal:** Make Project-M’s preset transitions feel as smooth, organic, and high-quality as classic Milkdrop.

---

## Overall Vision

Milkdrop’s transitions feel special because of:
- Precise render target handling
- Natural timing and easing
- Multi-pass effects on some transitions
- Careful texture sampling and filtering
- Frame-accurate synchronization

We want to close the gap while staying Emscripten/WebGL compatible.

---

## Phased Roadmap

| Phase | Name                              | Focus                                          | Priority | Est. Sessions | Status    |
|-------|-----------------------------------|------------------------------------------------|----------|---------------|-----------|
| **B1**    | Gap Analysis & Prioritization     | Identify biggest differences vs Milkdrop       | High     | 1–2           | **Next**  |
| **B2**    | Multi-pass Transition Support     | Enable 2-pass and simple multi-pass effects    | High     | 3–4           | Planned   |
| **B3**    | Advanced Blending & Compositing   | Add more sophisticated blending modes          | Medium   | 3–4           | Planned   |
| **B4**    | Timing, Synchronization & Polish  | Match Milkdrop’s frame-accurate feel           | High     | 2–3           | Planned   |
| **B5**    | Exotic Effects & Favorites        | Replicate beloved Milkdrop transitions         | Medium   | Ongoing       | Future    |
| **B6**    | Performance & Parallelism         | Add OpenMP pragmas + other optimizations       | Medium   | 2–4           | Future    |

---

## Session Guidelines

- Work in focused 1–3 hour sessions
- Always leave the codebase in a clean, buildable state
- Document progress in this file after each session
- Prioritize **high visual impact** changes early

---

## Next Session (Today)

**Phase B1: Gap Analysis & Prioritization**

**Goals for this session:**
1. List the top 5–7 biggest differences between current Project-M transitions and classic Milkdrop.
2. Prioritize which gaps give the biggest visual improvement per effort.
3. Decide what to tackle in the next 1–2 sessions.

---

**Let’s begin Phase B1 now.** 

Would you like me to start by listing the current known gaps, or do you want to do a quick collaborative review first?

---

## Detailed Gap Breakdown & Sub-Plans

### 1. Multi-pass Transition Support (Highest Visual Impact)
**Goal:** Enable transitions that require 2–3 render passes (like some classic Milkdrop effects).

**Detailed Steps:**
1. Add support for secondary and tertiary framebuffers in `PresetTransition` class
2. Design and implement a clean multi-pass API (`BeginPass(n)`, `EndPass()`, `GetPassTexture(n)`)
3. Create a base class or interface for multi-pass transitions
4. Port/recreate 2–3 example multi-pass transitions (e.g. complex PageCurl with backface + highlight pass, layered effects)
5. Update GLSL header to support multiple render targets when needed
6. Add automatic cleanup of extra framebuffers after transition ends
7. Write unit tests for multi-pass flow

**Key Files:**
- `src/libprojectM/Renderer/PresetTransition.hpp/.cpp`
- `src/libprojectM/Renderer/Framebuffer.hpp`
- New files: `MultiPassTransition.hpp`, example transition shaders

**Challenges:**
- WebGL 2.0 has limited MRT (Multiple Render Targets) support
- Managing multiple FBOs without leaking memory on Emscripten
- Shader complexity increases significantly

**Success Criteria:**
- At least 3 working multi-pass transitions
- No memory leaks after 100 rapid transitions
- Clean API that future transitions can easily use

**First Small Step:** Add secondary framebuffer support + `BeginPass(1)` / `EndPass()` skeleton.

**Priority:** High | **Est. Effort:** 4–5 sessions

---

### 2. Transition Timing & Frame Accuracy
**Goal:** Make transitions feel as locked and precise as Milkdrop.

**Detailed Steps:**
1. Audit current timing system in `PresetTransition` and `Renderer`
2. Implement frame-accurate progress calculation (using actual frame count instead of time delta)
3. Add per-transition timing curves (linear, ease, custom bezier)
4. Add beat-sync options (start transition on beat, sync progress to beat)
5. Expose fine-grained control (`SetTransitionDuration()`, `SetEasingCurve()`, `SyncToBeat()`)
6. Add optional visual debug overlay (progress bar + beat markers)
7. Compare timing feel against real Milkdrop using side-by-side testing

**Key Files:**
- `src/libprojectM/Renderer/PresetTransition.hpp/.cpp`
- `src/libprojectM/Renderer/Renderer.cpp`
- Possibly new `TransitionTiming.hpp
`

**Challenges:**
- Keeping timing consistent across different frame rates
- Emscripten timing can be less precise than desktop
- Beat detection must remain responsive

**Success Criteria:**
- Transitions feel “locked” and musical
- < 1 frame timing variance at 60fps
- Easy to use beat-sync API

**First Small Step:** Replace delta-time progress with frame-based progress calculation.

**Priority:** High | **Est. Effort:** 3 sessions

---

### 3. Advanced Blending & Compositing
**Goal:** Add more sophisticated ways to combine the two presets beyond basic alpha.

**Detailed Steps:**
1. Implement a `BlendMode` enum (Alpha, Additive, Multiplicative, Screen, Masked, etc.)
2. Create a small library of reusable blending functions in GLSL header
3. Allow transitions to dynamically choose blending mode
4. Add support for mask textures (grayscale mask controls blending strength)
5. Update 4–5 existing shaders to demonstrate new blending modes
6. Add documentation and examples for custom blending

**Key Files:**
- `TransitionShaderHeaderGlsl330.frag`
- `PresetTransition.cpp`
- Several transition shader `.frag` files

**Challenges:**
- Some blending modes can look bad at low opacity
- Mask texture management adds complexity
- Performance cost of more complex blending

**Success Criteria:**
- At least 5 distinct blending modes working
- Clean API for choosing mode per transition
- Good visual results across different presets

**First Small Step:** Add `BlendMode` enum + basic Additive and Multiplicative blending.

**Priority:** Medium | **Est. Effort:** 3–4 sessions

---

### 4. Render Target Quality & Filtering
**Goal:** Match or exceed Milkdrop’s render target quality during transitions.

**Detailed Steps:**
1. Review current framebuffer creation in `MilkdropPreset` and `PresetTransition`
2. Add automatic mipmap generation for transition textures
3. Implement better downsampling (use proper box filter or Lanczos where possible)
4. Add configurable quality levels (Performance / Balanced / Quality)
5. Add anisotropic filtering controls (desktop only)
6. Benchmark current vs target quality using test patterns
7. Document recommended settings per platform

**Key Files:**
- `src/libprojectM/Renderer/MilkdropPreset.cpp`
- `src/libprojectM/Renderer/PresetTransition.cpp`
- `src/libprojectM/Renderer/Framebuffer.cpp`

**Challenges:**
- Mipmap generation cost on Emscripten
- WebGL has limited filtering options compared to desktop OpenGL
- Memory usage increases with higher quality settings

**Success Criteria:**
- Visible improvement in smooth gradients and fine details during transitions
- No significant performance regression on target hardware

**First Small Step:** Add automatic mipmap generation for transition textures.

**Priority:** Medium | **Est. Effort:** 2–3 sessions

---

### 5. Preset State Isolation During Transition
**Goal:** Keep the two presets more cleanly isolated (like Milkdrop).

**Detailed Steps:**
1. Audit all shared state between old and new presets
2. Improve isolation of:
   - Random number generators / seeds
   - Beat reactivity variables
   - Per-preset `q` variables and custom variables
3. Add optional “strict isolation” mode
4. Document current behavior and trade-offs
5. Test with complex presets that heavily modify state
6. Add unit tests for state isolation

**Key Files:**
- `src/libprojectM/MilkdropPreset/MilkdropPreset.cpp`
- `src/libprojectM/Renderer/PresetTransition.cpp`
- Possibly `PerFrameContext` or similar

**Challenges:**
- Some presets intentionally share state for artistic effect
- Strict isolation may break some existing presets
- Performance cost of full isolation

**Success Criteria:**
- Old and new presets no longer interfere unexpectedly
- Easy to enable strict isolation when desired
- Backward compatibility maintained

**First Small Step:** Document current state sharing behavior + add isolation toggle.

**Priority:** Medium | **Est. Effort:** 2 sessions

---

### 6. Visual “Feel” of Specific Effects
**Goal:** Make key transitions feel more alive and Milkdrop-like.

**Detailed Steps:**
1. Select top 6–8 most visually important transitions (Glitch, HeatWave, WaterDrop, PageCurl, Kaleidoscope, etc.)
2. Analyze Milkdrop versions for subtle techniques (timing offsets, noise modulation, feedback, color cycling)
3. Enhance our versions with similar micro-details
4. Create side-by-side comparison (optional but very useful)
5. Iterate based on visual feedback from multiple people
6. Add parameters to let users tweak the “Milkdrop feel” intensity

**Key Files:**
- Individual transition shader files
- Possibly new helper functions in GLSL header

**Challenges:**
- “Feel” is subjective
- Some Milkdrop tricks rely on DirectX-specific behavior
- Risk of overcomplicating simple effects

**Success Criteria:**
- Key transitions feel noticeably more dynamic and “alive”
- Users familiar with Milkdrop recognize the improvement

**First Small Step:** Pick 2 transitions (e.g. Glitch + HeatWave) and add 1–2 subtle Milkdrop-style enhancements.

**Priority:** Medium-High | **Est. Effort:** 5–7 sessions (ongoing)

---

### 7. Performance on Complex Transitions + OpenMP (B6)
**Goal:** Make heavier transitions run smoothly and take advantage of multi-core CPUs.

**Detailed Steps:**
1. Profile current transition performance (especially on desktop)
2. Identify hot spots (shader compilation, texture uploads, blending passes)
3. Add OpenMP pragmas in key areas:
   - Preset loading / initialization
   - Parallel shader variable evaluation (if applicable)
   - Texture processing
4. Optimize framebuffer operations (avoid unnecessary copies)
5. Add performance warnings for very heavy transitions
6. Create performance regression tests
7. Document recommended settings per platform

**Key Files:**
- `src/libprojectM/Renderer/PresetTransition.cpp`
- `src/libprojectM/MilkdropPreset/MilkdropPreset.cpp`
- Various preset loading code

**Challenges:**
- OpenMP support varies across compilers and Emscripten
- Thread safety of OpenGL calls
- Debugging parallel code is harder

**Success Criteria:**
- Measurable performance improvement on multi-core desktop systems
- No regressions on Emscripten
- Clean, maintainable use of OpenMP

**First Small Step:** Add OpenMP to preset loading/initialization and measure improvement.

**Priority:** Medium | **Est. Effort:** 4–5 sessions (part of B6)