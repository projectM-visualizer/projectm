# Agent Swarm Prompt — Option B: Milkdrop Blending Parity

**Project:** Project-M (Milkdrop-compatible music visualizer)  
**Goal:** Incrementally bring Project-M’s preset transition system to Milkdrop-level quality using a long-term, multi-session approach.

**Date:** 2026-05-07

---

## Swarm Objective

Execute **Option B** (Milkdrop Blending Parity) in a coordinated, incremental way across multiple weekly sessions. Work in focused phases, always leaving the codebase clean and buildable.

---

## Available Artifacts (Read These First)

All files are in `docs/` and `grok_agent/`:

- `docs/option_b_milkdrop_parity.md` — Master roadmap with 6 phases (B1–B6) + detailed sub-plans for all 7 gaps
- `grok_agent/multi_pass_starter.md` — Starter code for 2-pass support (already partially implemented)
- `docs/milkdrop2_vs_projectm_multipass.md` — Comparison of Milkdrop2 vs Project-M multi-pass approach
- `grok_agent/openmp_candidates.md` — OpenMP parallelization opportunities (Phase B6)
- `grok_agent/glsl_shader_system_overview.md` — Current state of the GLSL transition shader system

---

## Swarm Roles

| Role | Responsibility |
|------|----------------|
| Architect | Overall design decisions, API design, roadmap updates |
| Implementer | Writes C++ / GLSL code, applies diffs |
| Tester | Builds, tests on desktop + Emscripten, reports issues |
| Documenter | Updates comments, READMEs, and roadmap after each session |
| Reviewer | Code review + suggests improvements |

---

## Execution Rules

1. **Always work in small, verifiable steps**
2. **Never break the build** — every commit must compile cleanly
3. **Document progress** in `option_b_milkdrop_parity.md` after each session
4. **Prioritize high visual impact** changes first
5. **Respect Emscripten/WebGL constraints** — guard platform-specific code
6. **Use prepared artifacts** — refer to the starter files before writing from scratch

---

## Current State (as of 2026-05-07)

**Completed:**
- Phase 1: Aspect uniforms + dual-FBO robustness
- Phase 2: Easing + texture filtering + documentation
- Phase B2 (partial): Basic 2-pass support + test shader

**Next Recommended Focus:**
- Complete **Phase B2** (Multi-pass) — port real transitions (e.g. PageCurl)
- Then move to **Phase B1** (Gap Analysis) or **B6** (OpenMP) depending on priority

---

## Swarm Task Format

When starting a new sub-task, use this format:

```markdown
**Task:** [Short name]
**Phase:** Bx
**Assigned to:** [Role]
**Goal:** [One sentence]
**First Step:** [Concrete first action]
**Success Criteria:** [How we know it’s done]
```

---

## Start Command

**Begin with this task:**

**Task:** Port PageCurl (or another high-impact transition) to use 2 passes  
**Phase:** B2  
**Goal:** Demonstrate real visual benefit from multi-pass system  
**First Step:** Use the skeleton in `grok_agent/multi_pass_starter.md` to split PageCurl into geometry pass + highlight pass

---

**Swarm — begin execution.** Report progress after each major step.