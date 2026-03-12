# Project-M: Development Guidelines for Claude Code

## Project Overview

**projectM** is an open-source music visualizer that reimplements the legendary Winamp MilkDrop visualizer as a modern, cross-platform reusable library. It reads audio input and produces mesmerizing visuals by detecting tempo and rendering advanced equations.

This repository contains **libprojectM**, the core visualization library that can be compiled to multiple platforms including:
- Windows, macOS, Linux (native)
- WebAssembly via Emscripten
- Android
- iOS (experimental)

## Repository Structure

```
Project-M/
├── src/
│   ├── api/              # Public C API
│   ├── libprojectM/      # Core visualization engine
│   ├── playlist/         # Playlist management
│   └── sdl-test-ui/      # SDL2 test UI
├── presets/              # Visualization presets (.milk files)
├── projectM_emscripten.cpp  # WASM/Emscripten bindings
├── projectm_audio_processor.js  # Web Audio Worklet for audio processing
├── CMakeLists.txt        # Build configuration
├── EMSCRIPTEN.md         # WASM-specific documentation
└── docs/                 # Additional documentation
```

## Key Development Areas

### C++ Code
- **Main Files**: `projectM_emscripten.cpp`, src/libprojectM/
- **Language**: C++11/14
- **Build System**: CMake
- **Code Style**: Follow .clang-format and .clang-tidy configs

### WASM/JavaScript Integration
- **Emscripten Bindings**: Lines 832-849 in projectM_emscripten.cpp
- **Audio Processing**: projectm_audio_processor.js (Web Audio Worklet)
- **Interop**: EM_JS macros for C++/JS communication

## Known Issues in WASM Port (Review Priority)

### Critical Issues
1. **Memory Leak in preset switching** (Line 342-346)
   - String allocated via `stringToNewUTF8()` is never freed
   - Impact: Memory accumulates when presets are switched
   - Fix: Use `free()` after the string is used, or switch to Emscripten string binding

2. **EGL Configuration Bug** (Lines 39, 649-661)
   - `eglconfig` initialized to NULL but used in eglGetConfigAttrib calls
   - eglChooseConfig() result not checked before using eglconfig
   - Impact: May query uninitialized config, leading to undefined behavior
   - Fix: Check eglChooseConfig result; handle NULL config

3. **AudioBuffer Property Access Error** (projectm_audio_processor.js, Line 61)
   - `this.mainAudioBuffer.length` is incorrect
   - AudioBuffer doesn't have a `.length` property
   - Correct access: `this.mainAudioBuffer.duration * this.mainAudioBuffer.sampleRate`
   - Impact: Loop detection breaks, playback might stop prematurely

### High Priority Issues
4. **Global State Not Reinitialized** (Lines 36-64, 757-761)
   - Calling init() multiple times causes undefined behavior
   - Global pointers (pm, playlist, display, surface, ctxegl, gl_ctx) not reset
   - Fix: Add safety checks or redesign to support multiple contexts

5. **Missing Error Handling**
   - EGL initialization (lines 646-729) has no error handling
   - WebGL context creation (lines 723-730) minimal error checking
   - projectm_create() failure leads to NULL pointer (line 757)
   - Fix: Add comprehensive error checks and recovery

6. **Canvas Size Hardcoding** (Lines 296, 408-411)
   - Viewport hardcoded to 8192x8192 then overwritten to window height
   - Not responsive to window resize events
   - Fix: Use dynamic sizing and add resize event listener

### Medium Priority Issues
7. **Type Safety in Emscripten Bindings** (Lines 832-849)
   - std::string passed to C-style functions may have encoding issues
   - No validation of parameters passed from JS
   - Fix: Add input validation, consider using proper Emscripten string types

8. **Deprecated EM_ASM Usage**
   - Many EM_JS functions could be optimized
   - Some EM_ASM blocks could use proper BIND_FUNCTION patterns
   - Fix: Modernize to Emscripten 3.x+ patterns

9. **Race Conditions in Async Operations**
   - js_load_song_into_worklet uses async without proper synchronization
   - May have race conditions if called while previous load is pending
   - Fix: Add state tracking (loading, loaded, error states)

### Low Priority Enhancements
10. **WebGL Extension Handling** (Lines 746-755)
    - Extensions enabled but not checked if supported
    - Missing fallback for unsupported formats
    - Consider: Device capability detection and graceful degradation

11. **Performance Optimization Opportunities**
    - EM_JS inline JS could be cached/memoized
    - Preset switching could batch updates
    - Consider: Implement double-buffering for smooth transitions

## Building the Project

### Native Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### WASM Build with Emscripten
Requires Emscripten SDK installed:
```bash
emconfigure cmake -B build-wasm
emmake make -C build-wasm -j$(nproc)
```

Key Emscripten flags used:
- `-sUSE_SDL=2`: Emscripten SDL2 port
- `-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2`: WebGL 2.0
- `-sFULL_ES2=1 -sFULL_ES3=1`: OpenGL ES 2.0/3.0 emulation
- `-sALLOW_MEMORY_GROWTH=1`: Dynamic memory allocation

## Code Standards

- **C++ Standard**: C++11 minimum (C++14 preferred)
- **Formatting**: Run `clang-format` before commit
- **Linting**: Use `clang-tidy` with `.clang-tidy` config
- **Comments**: Document complex logic, especially in WASM bindings

## Testing

Run tests after modifications:
```bash
ctest -j$(nproc) --output-on-failure
```

WASM-specific testing requires serving files over HTTP due to browser security policies.

## Common Tasks

### Adding a New Emscripten Binding
1. Define function in C++
2. Add EMSCRIPTEN_KEEPALIVE if needed
3. Add to EMSCRIPTEN_BINDINGS block
4. Test from JavaScript

### Debugging WASM Build Issues
- Check Emscripten version compatibility
- Review browser console for WebGL errors
- Use `-g4` for debug symbols in WASM
- Test with `emrun` for better error messages

### Adding Web Audio Features
1. Extend projectm_audio_processor.js
2. Add message handler in constructor
3. Update C++ js_* functions to trigger new JS code
4. Add bindings in EMSCRIPTEN_BINDINGS

## Important Notes

- **WASM Memory**: Emscripten uses a linear memory model. Monitor memory growth for long-running sessions.
- **Audio Context**: WebAudio API requires user interaction to start (autoplay policy).
- **WebGL Compatibility**: Not all features work in all browsers/devices; test on target platforms.
- **Performance**: Profile in browser DevTools; WASM overhead is significant for real-time graphics.

## Useful References

- [Emscripten Documentation](https://emscripten.org/docs/)
- [WebGL 2.0 Spec](https://www.khronos.org/webgl/wiki/Getting_Started_with_WebGL)
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)
- [projectM Documentation](./EMSCRIPTEN.md)
- [GLSL MilkDrop Preset Format](https://github.com/projectM-visualizer/projectm/wiki)

## Session Notes

This development branch (`claude/review-wasm-port-bfKhi`) is focused on reviewing and improving the WASM port. See the "Known Issues" section above for detailed defects found during this review.
