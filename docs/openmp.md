# OpenMP Plan for projectM ✅

## Summary

This document lists candidate hotspots for OpenMP parallelization, suggested pragma placements, build/CMake changes, tests, and an incremental implementation plan. The goal is to add optional, safe multi-threading (via OpenMP) with runtime/compile guards and tests to ensure correctness and measurable performance gains. 💡

---

## High-potential targets (priority order) 🔧

1) PerPixel mesh calculation (big win on CPU-bound rendering path)
   - File: `src/libprojectM/MilkdropPreset/PerPixelMesh.cpp`
   - Function: `PerPixelMesh::CalculateMesh`
   - Why: This computes per-vertex vertex attributes for a mesh sized (meshX+1) * (meshY+1). When presets do NOT use the dynamic per-pixel scripting (`perPixelContext.perPixelCodeHandle == nullptr`) each vertex is independent.
   - Suggested change: Replace the nested x/y loops with a single index loop and add `#pragma omp parallel for schedule(static)` guarded by a runtime check (only when `perPixelCodeHandle` is null).
   - Safety: **Do NOT** parallelize when the per-pixel script is active (existing comment warns of gmegabuf/regXX shared state). Must ensure each iteration uses only thread-local temporaries and writes to distinct buffer indices.
   - Example (pseudo):
     ```cpp
     if (!perPixelContext.perPixelCodeHandle) {
 #pragma omp parallel for schedule(static)
         for (int v = 0; v < vertexCount; ++v) {
             int y = v / (m_gridSizeX + 1);
             int x = v % (m_gridSizeX + 1);
             // compute and write m_zoomRotWarpBuffer[v], m_centerBuffer[v], etc.
         }
     } else {
         // original serial loops
     }
     ```

2) FFT & spectral processing (moderate)
   - Files: `src/libprojectM/Audio/MilkdropFFT.cpp`, `src/libprojectM/Audio/PCM.cpp`
   - Functions: `MilkdropFFT::TimeToFrequencyDomain`, `InitEnvelopeTable`, `InitEqualizeTable`
   - Why: FFT is compute heavy. Some parts (input setup, final magnitude/equalize loop, table inits) are embarrassingly parallel.
   - Suggested changes:
     - Parallelize the initial copy/mapping and the final loop that computes magnitudes with `#pragma omp parallel for` (or `#pragma omp for` inside a parallel region).
     - Be cautious about the core iterative butterfly loops — they are stage-dependent and require care; consider leaving them serial, or refactor to a thread-parallel FFT library if larger speedups desired.
   - Safety: Ensure thread-local temporaries for complex accumulators or apply parallelization only to independent parts.

3) Waveform vertex generation (low/medium)
   - Files: `src/libprojectM/MilkdropPreset/Waveforms/*` (e.g., `SpectrumLine.cpp`, `Line.cpp`, `DoubleLine.cpp`)
   - Why: Loops over `m_samples` (typically 256 or 512) are independent per sample and safe to parallelize when not doing shared state writes.
   - Suggested change: Add `#pragma omp parallel for schedule(static)` to loops that fill per-sample arrays (e.g., `m_wave1Vertices[i] = ...`).
   - Safety: Ensure no shared temporary variables are updated without atomics or reductions.

4) Initialization and helper tables (low)
   - Files: `MilkdropFFT::InitEnvelopeTable`, `InitEqualizeTable`, `InitBitRevTable`.
   - Why: These simple loops can be parallelized and are good first changes to validate OpenMP build and correctness.

5) Loudness band sums, reductions (low)
   - Files: `src/libprojectM/Audio/Loudness.cpp`
   - Change candidate: Use `#pragma omp parallel for reduction(+:m_current)` for sample sums.

6) PNG Encoding (stb_image_write) (moderate/high)
   - File: `vendor/SOIL2/src/SOIL2/stb_image_write.h`
   - Function: `stbi_write_png_to_mem`
   - Why: PNG encoding involves filtering each row, which is independent and computationally expensive. Parallelizing this can significantly speed up screenshot saving.
   - Suggested change:
     - Parallelize the row loop `for (j=0; j < y; ++j)` using `#pragma omp parallel for`.
     - Move the allocation of `line_buffer` inside the parallel region (or allocate per-thread) to avoid race conditions.
   - Safety: This is a vendor file, so changes should be applied carefully (e.g., via patch). Ensure `line_buffer` is thread-private.

---

## Build / CMake changes 🔧

- Add a CMake option to enable OpenMP (default OFF): `-DENABLE_OPENMP=ON`.
- Use `find_package(OpenMP)` and link via `target_link_libraries(libprojectM PRIVATE OpenMP::OpenMP_CXX)` or use `target_compile_options`/`target_link_options` when OpenMP is found.
- Define a compile-time macro (e.g., `PRJM_ENABLE_OPENMP`) when OpenMP is enabled to guard includes and calls to `omp_get_num_threads()`.
- Make OpenMP optional; keep the legacy behavior by default.

Example CMake snippet:
```cmake
option(ENABLE_OPENMP "Build with OpenMP parallelization" OFF)
if(ENABLE_OPENMP)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    target_link_libraries(libprojectM PRIVATE OpenMP::OpenMP_CXX)
    target_compile_definitions(libprojectM PRIVATE PRJM_ENABLE_OPENMP)
  endif()
endif()
```

### Emscripten / Web builds (special requirements) 🌐

Emscripten requires an OpenMP runtime at link time and the OpenMP headers at compile time. For our repo this implies:
- Provide `libomp.a` and `omp.h` to Emscripten (you've placed `libomp.a` and `omp.h` in the repository `/omp` directory).
- Compile and link with `-fopenmp`.
- Link using `-lomp` or the full path to `/omp/libomp.a`.
- Ensure the include path (e.g. `-I${PROJECT_SOURCE_DIR}/omp`) is added so `#include <omp.h>` resolves.

CMake notes when targeting Emscripten:
- Detect the emscripten toolchain and add `-fopenmp` to compile flags and `-lomp` (or `${PROJECT_SOURCE_DIR}/omp/libomp.a`) to link flags for the Emscripten target.
- Add `${PROJECT_SOURCE_DIR}/omp` to `target_include_directories` for the Emscripten build.

Important: Emscripten/WebAssembly may also require thread support configuration (pthreads/workers) to get parallel threads working in the browser — plan a separate integration step and CI job to validate the web artifact.

---

## Implementation recommendations (incremental) 🚶

1) Add `docs/openmp.md` (this file) describing targets & approach. ✅
2) Add CMake option and build detection (small, safe change). ✅
3) Add OpenMP *read-only* and small trivial parallelizations first: `InitEnvelopeTable`, `InitEqualizeTable`, `Loudness::SumBand`. Add tests that verify identical results with and without OpenMP.
4) Implement `PerPixelMesh` parallelization under explicit runtime check (`if (!perPixelContext.perPixelCodeHandle)`), with unit tests + frame regression tests.
5) Implement safe parts of FFT parallelization (initial input copy, final magnitude loop). Benchmark vs. baseline; consider switching to a parallel FFT library if necessary.
6) Add CI job for Linux that builds with `-DENABLE_OPENMP=ON` and runs the test suite and a small perf benchmark to detect regressions.
7) Apply parallelization to `stb_image_write.h` for PNG encoding. Since this is a vendor file, ensure the change is maintainable (e.g. via patch file or careful vendor branch management).

---

## Testing & validation ✅

- Create CPU-bound microbenchmarks that render a frame or series of frames at known mesh sizes and check frame-times and CPU usage.
- Add unit/regression tests to confirm numeric equality within tolerance between OpenMP on/off builds.
- Run visual regression (render reference frames) for presets that rely heavily on per-vertex calculations.

---

## Safety & gotchas ⚠️

- The per-pixel scripting engine uses shared memory (`gmegabuf`, `regXX`) and is explicitly not thread-safe; **do not parallelize** the execution path that runs these scripts. Add a runtime guard for this condition.
- Beware of non-thread-safe third-party API calls or global state. Prefer local temporaries in the parallel region.
- Floating-point reductions and parallel scheduling can lead to tiny bitwise differences in results; validate with tolerances.

---

## Example code patterns (recommended)

- Guarding and parallelizing a safe loop:
```cpp
#ifdef PRJM_ENABLE_OPENMP
#include <omp.h>
#endif

if (!perPixelContext.perPixelCodeHandle) {
    const int vertexCount = (m_gridSizeX + 1) * (m_gridSizeY + 1);

#pragma omp parallel for schedule(static)
    for (int v = 0; v < vertexCount; ++v) {
        // compute vertex x,y from v and write into buffers[v]
    }
} else {
    // original serial code (unchanged)
}
```

- Use `reduction(+:sum)` for numeric reductions and `private(...)` for per-thread local variables as required.

---

## Next steps (I can implement these for you) ▶️

1. Add the CMake `ENABLE_OPENMP` option and guard macros.
2. Add small parallel patches to `MilkdropFFT::InitEnvelopeTable` and `InitEqualizeTable` as a proof-of-concept and tests.
3. Implement `PerPixelMesh` guarded parallelization and add render microbenchmarks.
4. Apply OpenMP parallelization to `stb_image_write.h` for faster screenshots.

Would you like me to start by adding the CMake option and a trivial OpenMP-enabled unit test (step 1 and 2)?

---

*Document created: docs/openmp.md* ✨
