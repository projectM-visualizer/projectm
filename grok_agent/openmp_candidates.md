# OpenMP Parallelization Candidates in Project-M

**Date:** 2026-05-07  
**Goal:** Identify good places to add `#pragma omp parallel for` for performance gains (especially for Phase B6).

---

## Top Recommended Locations

### 1. Preset Loading / Initialization (Highest Potential)
**File(s):** `MilkdropPreset.cpp`, preset parser

**Why good:**
- Loading a preset involves many independent operations (parsing equations, compiling shaders, loading textures)
- Can be parallelized per-preset when loading multiple presets

**Example:**
```cpp
#pragma omp parallel for
for (int i = 0; i < numPresets; i++) {
    loadPreset(presets[i]);
}
```

**Challenges:**
- Thread safety of OpenGL calls (shader compilation must stay on main thread)
- Shared state during loading

**Recommendation:** Parallelize **non-GL** parts only (parsing, variable setup).

---

### 2. Texture / Image Loading
**File(s):** Texture loading code, stb_image usage

**Why good:**
- Loading multiple textures or large images is CPU-bound
- Independent per texture

**Example:**
```cpp
#pragma omp parallel for
for (int i = 0; i < textureCount; i++) {
    loadTexture(textures[i]);
}
```

**Challenges:**
- stb_image is not always thread-safe by default
- Need to be careful with memory allocation

---

### 3. Shader Compilation Caching
**File(s):** `TransitionShaderManager.cpp`, shader cache

**Why good:**
- Compiling many shaders at startup or during preset changes
- Each shader compilation is independent

**Example:**
```cpp
#pragma omp parallel for
for (int i = 0; i < shaderList.size(); i++) {
    compileShader(shaderList[i]);
}
```

**Challenges:**
- OpenGL context must be current (main thread only)
- Better to pre-compile in background thread if possible

---

### 4. Per-Frame Calculations (if any heavy loops exist)
Look for:
- Heavy per-vertex or per-pixel CPU calculations
- Custom per-frame equation evaluation (if not done on GPU)

**Example:**
```cpp
#pragma omp parallel for
for (int i = 0; i < vertexCount; i++) {
    calculateVertex(vertices[i]);
}
```

---

### 5. Noise / Procedural Texture Generation
**File(s):** `MilkdropNoise.cpp` or similar

**Why good:**
- Generating noise textures (Perlin, etc.) is very parallelizable

**Example:**
```cpp
#pragma omp parallel for collapse(2)
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        noise[y][x] = generateNoise(x, y);
    }
}
```

---

## Recommended First Steps

1. **Start with preset loading** (non-GL parts) — biggest user-facing win
2. Add OpenMP to texture loading
3. Profile before/after to measure gains
4. Be very careful with OpenGL calls (they must stay on main thread)

---

## Safety Rules

- Never call OpenGL functions inside parallel regions
- Use `omp critical` or atomic operations when writing shared data
- Test thoroughly on both desktop and Emscripten (Emscripten has limited OpenMP support)
- Start with `schedule(static)` for predictable performance

---

**Next Action:** Would you like me to prepare a specific example (e.g. preset loading parallelization) with actual code diffs?