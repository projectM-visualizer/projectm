# OpenMP Alternatives for Project-M

**Date:** 2026-05-07  
**Context:** Exploring parallelization options for Phase B6 (Performance & Parallelism)

---

## 1. Quick Comparison Table

| Technology              | Ease of Use | Emscripten Support | Performance | Best For                          | Recommendation |
|-------------------------|-------------|--------------------|-------------|-----------------------------------|----------------|
| **OpenMP**              | High        | Limited (pthreads) | Excellent   | Simple loops, preset loading      | **Strong**     |
| **std::thread / async** | Medium      | Good (pthreads)    | Good        | Fine-grained control              | Good           |
| **TBB (Threading Building Blocks)** | Medium   | Poor               | Excellent   | Complex task graphs               | Not recommended |
| **Taskflow**            | Medium-High | Limited            | Excellent   | Modern task-based parallelism     | Interesting    |
| **Emscripten Workers**  | Low         | Native             | Good        | Web-specific heavy tasks          | Niche          |
| **Compute Shaders**     | Low         | Good (WebGL 2)     | Excellent   | GPU-bound work (noise, filters)   | Strong for some tasks |

---

## 2. Detailed Breakdown

### OpenMP (Current Plan)
**Pros:**
- Very easy to add (`#pragma omp parallel for`)
- Excellent performance on desktop
- Mature and well-supported

**Cons:**
- Emscripten support is limited (requires `-pthread` and has overhead)
- Not ideal for fine-grained or irregular workloads

**Best Use:** Preset loading, texture processing, noise generation

---

### std::thread + std::async (C++11 Standard)
**Pros:**
- No external dependency
- Good Emscripten support via pthreads
- More control than OpenMP

**Cons:**
- More boilerplate code
- Manual thread management

**Best Use:** When you need more control than OpenMP provides

**Example:**
```cpp
std::vector<std::future<void>> futures;
for (auto& item : items) {
    futures.push_back(std::async(std::launch::async, [&]() {
        process(item);
    }));
}
for (auto& f : futures) f.wait();
```

---

### TBB (Threading Building Blocks)
**Pros:**
- Very powerful task scheduler
- Excellent for complex parallel algorithms

**Cons:**
- Heavy dependency
- Poor Emscripten support
- Overkill for most needs in Project-M

**Recommendation:** Skip for now

---

### Taskflow
**Pros:**
- Modern, header-only, easy to use
- Excellent task dependency graph support
- Good performance

**Cons:**
- Still relatively new
- Emscripten support is experimental

**Interesting Option** if we want something more modern than OpenMP.

---

### Emscripten-Specific Options
- **Web Workers** — Good for very heavy background tasks
- **Emscripten pthreads** — Required for OpenMP anyway
- **OffscreenCanvas + WebGL** — For moving rendering off main thread (advanced)

**Best for:** Very specific heavy workloads that don't need OpenGL

---

### Compute Shaders (GPU)
**Pros:**
- Massive parallelism for suitable workloads (noise, image processing, filters)
- Works well on both desktop and WebGL 2.0

**Cons:**
- Requires rewriting logic in GLSL compute shaders
- Not suitable for all CPU-bound work

**Strong Recommendation** for certain tasks (e.g. procedural noise, post-processing effects).

---

## 3. My Recommendation for Project-M

| Workload                    | Recommended Technology     | Reason |
|----------------------------|----------------------------|--------|
| Preset loading             | OpenMP or std::async       | Simple loops, big win |
| Texture loading            | OpenMP                     | Easy and effective |
| Noise / procedural textures| **Compute Shaders**        | GPU is much faster |
| Per-frame calculations     | std::async (if needed)     | Fine control |
| Complex task graphs        | Taskflow (future)          | If we grow more complex |

---

## 4. Suggested First Experiment

**Start with OpenMP** on preset loading (as originally planned), then evaluate:

1. Add `#pragma omp parallel for` to preset loading
2. Measure improvement on desktop
3. Test on Emscripten (with `-pthread`)
4. If Emscripten performance is poor, switch that part to `std::async`

---

**Would you like me to prepare a concrete example** of using `std::async` as an alternative to OpenMP for preset loading? Or focus on compute shaders for noise generation?