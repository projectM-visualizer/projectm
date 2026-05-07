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

## Next Session (Today

**Phase B1: Gap Analysis & Prioritization**

**Goals for this session:**
1. List the top 5–7 biggest differences between current Project-M transitions and classic Milkdrop.
2. Prioritize which gaps give the biggest visual improvement per effort.
3. Decide what to tackle in the next 1–2 sessions.

---

**Let’s begin Phase B1 now.** 

Would you like me to start by listing the current known gaps, or do you want to do a quick collaborative review first?