# ProjectM Build System Enhancement & Audit Task

## Objective
Conduct a thorough audit of the ProjectM Emscripten build pipeline and CMake configuration to identify performance bottlenecks, maintainability issues, and optimization opportunities.

## Areas to Investigate

### 1. **CMake Configuration Analysis**
- [ ] Review all CMakeLists.txt files for:
  - Unused build options (especially OpenMP with Emscripten compatibility)
  - Compiler flag optimizations (currently using -O3)
  - Link-time optimization (LTO) feasibility
  - Dead code elimination flags
- [ ] Analyze dependency management:
  - Are all VCPKG features necessary?
  - Can vendored dependencies be updated?
  - Are there circular dependency issues?
- [ ] Check cross-platform build configurations:
  - Emscripten-specific optimizations
  - Android build constraints
  - Windows/macOS parity issues

### 2. **Emscripten-Specific Optimizations**
- [ ] WASM module size reduction:
  - Current output size of projectm-v.024-thread.js
  - Potential for gzip/brotli compression
  - Unused function elimination
  - Dead code stripping effectiveness
- [ ] Memory optimization:
  - ALLOW_MEMORY_GROWTH=1 impact vs. pre-allocated memory
  - SHARED_MEMORY=1 and PTHREAD_POOL_SIZE=4 tuning
  - Heap size requirements vs. WASM limitations
- [ ] Async/Threading review:
  - ASYNCIFY usage impact on performance
  - PTHREAD_POOL_SIZE=4 optimization (vs. CPU cores)
  - Audio buffer synchronization with Web Workers
- [ ] WebGL configuration:
  - FULL_ES2=0 vs FULL_ES3=1 trade-offs
  - Texture format optimization
  - Fragment shader compilation for WASM

### 3. **Audio & Performance Pipeline**
- [ ] Audio buffer optimization:
  - Current buffer sizes and latency profile
  - Ring buffer vs. circular queue efficiency
  - Sample rate conversion overhead
  - OpenMP utilization in audio processing (if enabled for WASM)
- [ ] CustomWaveform & stbi_write_png OpenMP pragmas:
  - Verify thread overhead doesn't exceed gains (small buffers?)
  - Check for data race conditions
  - Memory alignment for SIMD operations

### 4. **Build Script & Process Improvements**
- [ ] Error handling & robustness:
  - Add `set -e` and `set -o pipefail`
  - Implement pre-flight checks (tool availability, disk space)
  - Better error messages with context
  - Automatic recovery for transient failures
- [ ] Build parallelization:
  - Parallel CMake configuration phase
  - Incremental build support
  - Ccache/sccache integration for faster rebuilds
- [ ] Dependency installation automation:
  - Script to install all apt dependencies
  - Version pinning for reproducible builds
  - Docker/container build image optimization

### 5. **Code Quality & Maintainability**
- [ ] C++ code analysis:
  - Memory leak detection (valgrind/clang-analyzer)
  - Undefined behavior sanitizers (UBSAN)
  - Thread sanitizer (TSAN) for multi-threaded code
  - Profile-guided optimization (PGO) feasibility
- [ ] Preset/Milkdrop compatibility:
  - Are all required presets loading correctly in Emscripten?
  - Performance profiling of preset evaluation
  - GLSL shader compilation issues
- [ ] Plugin system optimization:
  - Any unused plugin APIs?
  - Plugin loading overhead

### 6. **Testing & Validation**
- [ ] Build verification:
  - Currently commented out ctest in GitHub Actions - enable?
  - Create integration tests for web build
  - Regression testing for audio pipeline
  - Visual output verification tests
- [ ] Performance benchmarking:
  - FPS measurement during playback
  - Audio latency measurement
  - WASM memory usage profiling
  - JavaScript bridge overhead quantification

### 7. **Upstream Synchronization**
- [ ] Check original projectM repo for:
  - New build improvements (especially Emscripten support)
  - Performance enhancements in main library
  - Security patches
  - New preset format support
- [ ] Merge strategy:
  - Branch update frequency
  - Conflict resolution for custom patches
  - Backport requirements for upstream

## Deliverables

1. **Build Optimization Report** - Quantified improvements with metrics
2. **CMake Refactoring Plan** - Simplified configuration with better organization
3. **GitHub Actions Workflow Improvements** - Faster, more reliable CI/CD
4. **Performance Baseline** - Established metrics for FPS, memory, load time
5. **Automated Build Script** - Enhanced shell script with error handling
6. **Documentation** - Updated build guide with optimization flags explained

## Success Criteria

- [ ] Build time reduced by >20%
- [ ] WASM module size reduced or justified
- [ ] Zero undefined behavior in sanitizer runs
- [ ] All tests passing consistently
- [ ] Build script handles failures gracefully
- [ ] Clear documentation of all non-standard flags
