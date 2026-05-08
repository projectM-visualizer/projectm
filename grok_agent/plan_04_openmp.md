# Plan 04: Start Phase B6 — OpenMP Parallelization

**Phase:** B6 (Performance & Parallelism)  
**Priority:** Medium  
**Est. Sessions:** 2–3

## Goal
Add OpenMP pragmas to gain measurable performance improvements, especially on desktop.

## Best Candidates (in order)
1. **Preset loading** (non-GL parts) — highest user impact
2. **Texture / image loading**
3. **Noise / procedural texture generation**

## Tasks
1. Identify safe parallelizable loops (no OpenGL calls)
2. Add `#pragma omp parallel for` with proper scheduling
3. Handle thread safety for any shared data
4. Measure before/after performance
5. Document which areas benefit most

## Success Criteria
- Clear performance improvement on multi-core systems
- No regressions on Emscripten or single-core
- Clean, maintainable use of OpenMP

## First Step
Profile preset loading time and identify the biggest CPU hotspots.