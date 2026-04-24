# libprojectM – Copilot Instructions

## What this project is

**libprojectM** is the core music-visualization library that reimplements Winamp MilkDrop as a cross-platform, reusable shared/static library (LGPL 2.1). It ingests raw PCM audio, runs beat detection + FFT, evaluates MilkDrop-style preset equations, and renders the result with OpenGL. This repository contains only the library — frontends live in separate repos.

---

## Build

**Requires:** CMake ≥ 3.21, OpenGL (or GLES3 for embedded), a C++20-capable toolchain. SDL2 is optional (for the dev test UI). Submodules must be initialised.

```bash
git submodule update --init --recursive

# Configure (Debug recommended while developing)
cmake -G Ninja -S . -B cmake-build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

# Build
cmake --build cmake-build --parallel

# Run all tests
ctest --test-dir cmake-build --verbose

# Run a single test by name pattern
ctest --test-dir cmake-build -R WaveformAlignerTest --verbose
```

Key CMake options:

| Option | Default | Purpose |
|---|---|---|
| `BUILD_TESTING` | OFF | Build Google Test suite |
| `ENABLE_SDL_UI` | OFF | Build the SDL2 dev test app |
| `ENABLE_GLES` | OFF | OpenGL ES path (RPi, Android) |
| `BUILD_SHARED_LIBS` | ON | Shared vs. static library |
| `ENABLE_OPENMP` | OFF | OpenMP parallelisation |

### Emscripten / WASM build

CI uses emsdk **3.1.53**.

```bash
emcmake cmake -G "Unix Makefiles" -S . -B cmake-build \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
emmake cmake --build cmake-build --parallel
```

Required linker flags (set in CMakeLists): `-sUSE_SDL=2`, `-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2`, `-sFULL_ES2=1 -sFULL_ES3=1`, `-sALLOW_MEMORY_GROWTH=1`. The `OES_texture_float` WebGL extension must be enabled at runtime for the motion-vector grid.

---

## Architecture

```
src/
  api/include/projectM-4/   ← Installed public C headers (the ABI surface)
  libprojectM/
    ProjectM.cpp/.hpp        ← Central C++ class — owns all subsystems
    ProjectMCWrapper.cpp     ← Bridges public C API → internal C++ class
    Audio/                   ← PCM ingestion, FFT (MilkdropFFT), beat/loudness
    MilkdropPreset/          ← .milk file parser + per-frame/per-pixel evaluator
    Renderer/                ← OpenGL: shaders, framebuffers, textures, mesh
    UserSprites/             ← Sprite overlay system
  playlist/                  ← Optional libprojectM-playlist library
  sdl-test-ui/               ← Developer test application (SDL2)
vendor/                      ← Bundled: hlslparser, glm, glad, projectm-eval, stb_image
tests/
  libprojectM/               ← Unit tests for core library (GTest)
  playlist/                  ← Unit tests for playlist library (GTest)
presets/tests/               ← .milk preset files used in render regression tests
projectM_emscripten.cpp      ← WASM-specific EGL/WebGL init + Emscripten bindings
projectm_audio_processor.js  ← Web Audio Worklet (audio pipeline for browser builds)
```

### C API ↔ C++ separation

The **public ABI** is the C API (`src/api/include/projectM-4/*.h`). All symbols use the `projectm_` prefix and snake_case. The C++ class (`libprojectM::ProjectM`) is internal. `ProjectMCWrapper` is the glue layer — add new public functions there, not directly to `ProjectM`.

### Preset pipeline

1. `PresetFactory` / `PresetFactoryManager` resolve and load `.milk` files.
2. `MilkdropPreset` parses them; `projectm-eval` (vendored) compiles the equations.
3. Per-frame expressions run on the CPU; per-pixel mesh deformation and compositing shaders run on the GPU via `MilkdropShader`.
4. `PresetTransition` handles cross-fading between presets using `TransitionShaders/`.

### Audio path

`PCM` → `MilkdropFFT` → `FrameAudioData` (bass/mid/treble + waveform arrays) → fed into preset equation context each frame.

---

## Naming conventions (enforced by `.clang-tidy`)

| Entity | Convention | Example |
|---|---|---|
| Classes / Structs (C++) | `CamelCase` | `TextureManager` |
| Public/private methods | `CamelCase` | `LoadPreset()` |
| Local variables / parameters | `camelBack` | `frameCount` |
| Private & protected members | `m_` + `camelBack` | `m_meshResolution` |
| Public C API functions | `projectm_` + `snake_case` | `projectm_load_preset_file()` |
| Public C API structs | `projectm_` + `snake_case` | `projectm_settings` |
| Macros | `UPPER_CASE` | `PROJECTM_EXPORT` |

---

## Code style

Formatting is LLVM-based (see `.clang-format`). Run before committing:

```bash
clang-format -i <file>
# or for the whole tree:
find src -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
```

Linting:

```bash
clang-tidy src/libprojectM/ProjectM.cpp -- -std=c++20 -Isrc/api/include
```

`config.h` is force-included into every compilation unit by CMake — do not `#include` it manually.

---

## WASM-specific patterns

- C++ → JS calls use `EM_JS` macros (prefer over `EM_ASM` for type safety and optimisation).
- JS → C++ calls go through the `EMSCRIPTEN_BINDINGS` block at the bottom of `projectM_emscripten.cpp`.
- Global EGL/WebGL state lives in file-scope variables (`display`, `surface`, `ctxegl`, `gl_ctx`). `init()` is not safe to call more than once.
- Audio is delivered via `projectm_audio_processor.js` (an `AudioWorkletProcessor`); use `postMessage` for all cross-thread communication.
