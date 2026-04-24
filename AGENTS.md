# projectM Agent Guide

This document provides essential context for AI coding agents working on the projectM codebase. projectM is an open-source C++20 library that reimplements the Winamp Milkdrop music visualizer as a modern, cross-platform, reusable library.

**License:** GNU Lesser General Public License 2.1 (LGPL-2.1-only)  
**Version:** 4.1.0 (as of `CMakeLists.txt`)  
**Repository:** https://github.com/projectM-visualizer/projectm

---

## Project Overview

projectM reads audio PCM data, analyzes it (FFT, beat detection), and renders mesmerizing visuals using OpenGL. The core deliverable is **libprojectM** — a shared or static library with a stable C API. End-user frontends (SDL2, Steam, Qt, etc.) live in separate repositories; this repo contains only the library, a playlist management library, an SDL2-based developer test UI, and the test suite.

Supported platforms: Windows, Linux, macOS (including iOS/tvOS), BSD, Android, and WebGL/WASM via Emscripten. Supported architectures: i686, x86_64, armv7, aarch64, WASM.

---

## Repository Structure

```
├── cmake/                  # CMake modules and helper scripts
├── docs/                   # Sphinx/Doxygen documentation sources
├── presets/                # CMake target for preset data directories
├── scripts/                # Build and upload helper shell scripts
├── src/
│   ├── api/                # C API headers (projectM-4/*.h) + export header generation
│   ├── libprojectM/        # Core visualization library
│   │   ├── Audio/          # PCM, FFT, loudness, beat detection, waveform alignment
│   │   ├── MilkdropPreset/ # Preset parsing, Milkdrop equation/shader evaluation, waveforms
│   │   ├── Renderer/       # OpenGL rendering, textures, shader cache, transitions
│   │   ├── UserSprites/    # User sprite rendering
│   │   ├── ProjectM.hpp    # Main C++ class
│   │   └── ProjectMCWrapper.cpp  # C API implementation
│   ├── playlist/           # Playlist management library (C API + C++ implementation)
│   └── sdl-test-ui/        # SDL2-based developer test application
├── tests/
│   ├── libprojectM/        # Unit tests for core library (Google Test)
│   ├── playlist/           # Unit tests for playlist library (Google Test)
│   └── cxx-interface/      # Integration test for installed C++ headers
├── vendor/                 # Bundled third-party code
│   ├── glad/               # OpenGL function loader
│   ├── glm/                # OpenGL Mathematics (optional system override)
│   ├── hlslparser/         # HLSL parser for shaders
│   ├── projectm-eval/      # Milkdrop expression evaluator (Git submodule)
│   └── stb_image/          # Image loading
├── CMakeLists.txt          # Root build configuration
├── vcpkg.json              # vcpkg dependency manifest
├── features.cmake          # Compiler flags, filesystem support, config.h generation
└── config.h.cmake.in       # Template for generated build-config header
```

---

## Technology Stack

| Layer | Technology |
|-------|------------|
| Language | C++20 (enforced by CMake) |
| Build System | CMake >= 3.21 |
| Graphics | OpenGL Core 3.3 or OpenGL ES 3.2 |
| Math | GLM (bundled or system) |
| Expression Eval | projectM-eval (bundled submodule or system) |
| Test UI | SDL2 (optional) |
| Testing | Google Test (optional) |
| Documentation | Doxygen + Sphinx + Breathe + Exhale (optional) |
| Dependency Mgr | vcpkg (recommended, especially on Windows) |

---

## Build System & Commands

### Prerequisites

- CMake 3.21 or higher
- A working C++20 toolchain
- OpenGL (desktop) or GLES3 (embedded/Android/Emscripten) development libraries
- Git (to clone and update submodules)

Optional:
- `ninja-build` (recommended over Make)
- `libsdl2-dev` (for the test UI)
- `libgtest-dev`, `libgmock-dev` (for tests)
- `libglm-dev` (if `ENABLE_SYSTEM_GLM=ON`)

### Quick Build (Linux)

```bash
# Initialize the expression-evaluator submodule
git submodule init
git submodule update

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build . --parallel
sudo cmake --build . --target install
```

### Important CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | `ON` | Build as shared libraries. Set `OFF` for static. |
| `BUILD_TESTING` | `OFF` | Build the unit test suite. |
| `ENABLE_PLAYLIST` | `ON` | Build the playlist management library. |
| `ENABLE_SDL_UI` | `OFF` | Build the SDL2 developer test UI (`projectM-Test-UI`). |
| `ENABLE_GLES` | `OFF` | Use OpenGL ES 3 instead of OpenGL Core. Forced `ON` on Android/Emscripten. |
| `ENABLE_EMSCRIPTEN` | auto | Set automatically when using Emscripten toolchain. |
| `ENABLE_CXX_INTERFACE` | `OFF` | **Experimental/unsupported.** Export C++ symbols for `ProjectM` and `PCM` classes. |
| `ENABLE_SYSTEM_GLM` | `OFF` | Use system-installed GLM instead of bundled. |
| `ENABLE_SYSTEM_PROJECTM_EVAL` | `ON` | Use system-installed projectM-eval instead of submodule. |
| `ENABLE_BOOST_FILESYSTEM` | `OFF` | Force `boost::filesystem` even if `std::filesystem` is available. |
| `ENABLE_OPENMP` | `OFF` | Enable OpenMP parallelization in performance-critical sections. |
| `ENABLE_HDR_RENDERING` | `OFF` | Enable 16-bit float (GL_RGBA16F) framebuffer + Reinhard tone-mapping. |
| `ENABLE_HDR_P3` | `OFF` | Enable Display-P3 wide-gamut output (requires HDR rendering). |
| `ENABLE_SRGB_FRAMEBUFFER` | `OFF` | Enable hardware sRGB encode/decode (SDR only, incompatible with HDR). |
| `ENABLE_VERBOSE_LOGGING` | `OFF` | Compile `TRACE`/`DEBUG` log levels into release builds (hurts performance). |
| `ENABLE_DEBUG_POSTFIX` | `ON` | Append `d` to debug binary names. |
| `ENABLE_MACOS_FRAMEWORK` | `OFF` | Build macOS Framework bundles instead of plain dylibs. |
| `ENABLE_INSTALL` | `OFF` | Enable install targets when built as a CMake sub-project. |

### Using vcpkg (especially on Windows)

The repository includes `vcpkg.json`. Pass the vcpkg toolchain file when configuring:

```bash
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake -S <src> -B <build>
```

---

## Code Organization

### `src/api/`
Defines the stable **C API** used by all language bindings and frontends. Headers are installed under `include/projectM-4/`. Key headers:
- `projectM.h` — Convenience umbrella header
- `core.h`, `audio.h`, `render_opengl.h`, `callbacks.h`, `parameters.h`, `touch.h`, `user_sprites.h`

Export headers (`projectM_export.h`, `projectM_cxx_export.h`) are generated by CMake.

### `src/libprojectM/`
The core visualization engine. It is subdivided into:
- **Audio/** — `PCM`, `MilkdropFFT`, `Loudness`, `WaveformAligner`, `FrameAudioData`. Handles all audio ingestion and feature extraction.
- **MilkdropPreset/** — Preset factory, file parser, HLSL/GLSL shader generation, equation evaluation, custom waveforms, and the Milkdrop preset format implementation.
- **Renderer/** — `Renderer`, `RenderContext`, texture managers, shader cache, transition shaders, and platform abstractions.
- **UserSprites/** — `SpriteManager` and shaders for user-provided sprite overlays.
- **ProjectM.hpp/cpp** — The main orchestrator class. Owns the audio pipeline, preset manager, renderer, and time keeper.
- **ProjectMCWrapper.cpp** — Thin C wrappers around the C++ `ProjectM` and `PCM` classes.

### `src/playlist/`
A separate library (`libprojectM-playlist`) for managing preset playlists: filtering, item metadata, playback order, and callbacks. It has its own C API in `src/playlist/api/projectM-4/`.

### `src/sdl-test-ui/`
A minimal SDL2 application (`projectM-Test-UI`) for manual developer testing. It is **not installed** and is not an end-user frontend. Requires `ENABLE_SDL_UI=ON`.

### `vendor/`
Third-party code that is compiled as part of the project:
- `glad` — OpenGL loader
- `glm` — Header-only math library (skipped if `ENABLE_SYSTEM_GLM=ON`)
- `hlslparser` — HLSL-to-GLSL transpiler helpers
- `projectm-eval` — Expression evaluator for Milkdrop equations (skipped if system package found and `ENABLE_SYSTEM_PROJECTM_EVAL=ON`)
- `stb_image` — Image decoding

---

## Development Conventions

### Build Discipline
- **Always build out-of-tree.** Never run CMake directly in the source root. Use a subdirectory like `build/` or `cmake-build/`.
- The source tree is treated as read-only; all generated files belong in `CMAKE_BINARY_DIR`.

### Code Formatting
- A `.clang-format` file is provided at the repository root. **Run `clang-format` before committing.**
- Based on `LLVM` style with customizations:
  - Indent: 4 spaces (no tabs)
  - No column limit (`ColumnLimit: 0`)
  - Braces on new lines after classes, functions, enums, and control statements (Allman-style)
  - Pointer alignment: left (`int* ptr`)
  - Short functions/lambdas: allowed on single line; other blocks: never

### Static Analysis
- A `.clang-tidy` file is provided. It enables checks from:
  - `readability-*`
  - `cppcoreguidelines-*`
  - `bugprone-*`
  - `modernize-*`
  - `performance-*`
  - `misc-*`
- Disabled checks include `magic-numbers`, `owning-memory`, `pro-bounds-pointer-arithmetic`, and `easily-swappable-parameters`.

### Naming Conventions (enforced by `.clang-tidy`)
| Entity | Style | Example |
|--------|-------|---------|
| Classes / Structs | `CamelCase` | `class PresetFactory` |
| Methods / Functions | `CamelCase` | `void LoadPreset()` |
| Global functions with C API | `projectm_` prefix | `projectm_load_preset()` |
| Variables / Parameters | `camelBack` | `int sampleCount` |
| Private / Protected members | `m_` + `camelBack` | `int m_sampleCount` |
| Macros | `UPPER_CASE` | `#define ENABLE_FOO` |

### Symbol Visibility
- Default visibility is **hidden** for both C and C++ (`CMAKE_C_VISIBILITY_PRESET hidden`, `CMAKE_CXX_VISIBILITY_PRESET hidden`).
- Only symbols explicitly annotated with `PROJECTM_API` / `PROJECTM_CXX_EXPORT` are exported from shared libraries.
- If `ENABLE_CXX_INTERFACE=OFF` (the default), C++ symbols are **not** exported and `ProjectMCWrapper.cpp` is the only exported compilation unit.

### Logging
- The project uses an internal logging framework (see `Logging.hpp`).
- `TRACE` and `DEBUG` levels are stripped in release builds unless `ENABLE_VERBOSE_LOGGING=ON`.
- Prefer the project's logging macros over `printf` or `std::cout`.

---

## Testing Instructions

### Enabling Tests

```bash
cmake -DBUILD_TESTING=ON ...
cmake --build . --parallel
```

### Running Tests

```bash
# Run all tests via CTest
ctest --test-dir <build-dir> --verbose --build-config <Debug|Release>
```

### Test Suites

| Directory | Framework | Coverage |
|-----------|-----------|----------|
| `tests/libprojectM/` | Google Test | HLSL parser, logging, preset file parser, waveform aligner |
| `tests/playlist/` | Google Test | Playlist API, filter logic, item handling |
| `tests/cxx-interface/` | CMake compile test | Verifies installed C++ headers can be consumed by an external project |

The CI builds both `Debug` and `Release` configurations and runs `ctest` for each.

---

## CI & Deployment

GitHub Actions workflows are in `.github/workflows/`:
- `build_linux.yml` — Ubuntu 22.04/24.04 matrix (shared/static × stl/boost filesystem), runs unit tests and C++ interface test
- `build_windows.yml` — Visual Studio / Ninja builds with vcpkg
- `build_osx.yml` — macOS shared/static builds, with/without framework
- `build_emscripten.yml` — Emscripten WASM build
- `build_android.yml` — Android NDK build
- `push_release.yml` — Release artifact publishing

### Packaging Outputs
- CMake config files: `projectM4Config.cmake`, `projectM4Targets.cmake`
- pkg-config `.pc` files on UNIX (not for macOS frameworks)
- macOS Framework bundles when `ENABLE_MACOS_FRAMEWORK=ON`
- CPack configuration is included in the root `CMakeLists.txt`

---

## Security Considerations

- **C API is the supported integration surface.** The C++ interface (`ENABLE_CXX_INTERFACE`) is explicitly discouraged because using C++ STL types across shared-library boundaries is unsafe unless every component is built with the exact same toolchain and C++ standard library. If you must expose C++ symbols, understand the ABI risks.
- Preset files are parsed from user-provided content. The parser should be resilient to malformed input, but any changes to preset parsing or expression evaluation should be reviewed for potential memory-safety issues (buffer overruns, use-after-free) because preset data is externally supplied.
- The SDL2 test UI is for development only; do not ship it as a production frontend.
