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

### 1. Multi-pass Transition Support
**Goal:** Enable transitions that require 2–3 render passes (like some classic Milkdrop effects).

**Sub-plan:**
- Add support for secondary/tertiary framebuffers in `PresetTransition`
- Create a simple multi-pass API (e.g. `BeginPass()`, `EndPass()`)
- Port or recreate 2–3 example multi-pass transitions (e.g. advanced PageCurl or layered effects)
- Update shader system to support multiple render targets when needed
- Ensure Emscripten compatibility (WebGL supports limited multi-pass)

**Priority:** High | **Est. Effort:** 3–4 sessions

---

### 2. Transition Timing & Frame Accuracy
**Goal:** Make transitions feel as locked and precise as Milkdrop.

**Sub-plan:**
- Improve timing system to be more frame-accurate
- Add per-transition timing curves and beat-sync options
- Expose fine-grained control over transition start/end
- Add visual debugging tools (optional overlay showing progress)
- Test against real Milkdrop presets for feel-matching

**Priority:** High | **Est. Effort:** 2–3 sessions

---

### 3. Advanced Blending & Compositing
**Goal:** Add more sophisticated ways to combine the two presets.

**Sub-plan:**
- Implement additional blending modes (additive, multiplicative, masked, etc.)
- Create a small library of reusable blending functions
- Allow transitions to choose blending mode dynamically
- Add support for mask textures during transitions
- Update a few existing shaders to demonstrate new modes

**Priority:** Medium | **Est. Effort:** 3–4 sessions

---

### 4. Render Target Quality & Filtering
**Goal:** Reach or exceed Milkdrop’s texture/render target quality during transitions.

**Sub-plan:**
- Improve framebuffer creation (formats, mipmaps, multisampling where possible)
- Add automatic mipmap generation for transition textures
- Implement better downsampling/upscaling during transitions
- Add configurable filtering quality levels (performance vs quality)
- Benchmark and document current vs target quality

**Priority:** Medium | **Est. Effort:** 2–3 sessions

---

### 5. Preset State Isolation During Transition
**Goal:** Keep the two presets more cleanly isolated (like Milkdrop).

**Sub-plan:**
- Review current state sharing between old and new presets
- Improve isolation of random seeds, beat reactivity, and per-preset variables
- Add optional “strict isolation” mode for transitions
- Document current behavior and any trade-offs
- Test with complex presets that heavily use per-preset state

**Priority:** Medium | **Est. Effort:** 2 sessions

---

### 6. Visual “Feel” of Specific Effects
**Goal:** Make key transitions (Glitch, HeatWave, WaterDrop, PageCurl, etc.) feel more “alive”.

**Sub-plan:**
- Analyze top 5–6 most-used Milkdrop transitions
- Identify subtle timing, noise, and feedback tricks used in Milkdrop
- Enhance our versions with similar techniques
- Create side-by-side comparison videos (optional)
- Iterate based on visual feedback

**Priority:** Medium-High | **Est. Effort:** 4–6 sessions (ongoing)

---

### 7. Performance on Complex Transitions + OpenMP
**Goal:** Make heavier transitions run smoothly (ties into new B6 phase).

**Sub-plan:**
- Profile current transition performance (especially on desktop)
- Identify hot spots (shader compilation, texture uploads, blending)
- Add OpenMP pragmas in key areas (preset loading, parallel shader work, etc.)
- Optimize framebuffer operations
- Add performance warnings/logging for very heavy transitions

**Priority:** Medium | **Est. Effort:** 3–4 sessions (part of B6)