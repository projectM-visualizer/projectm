# Using libprojectM in Emscripten

projectM supports OpenGL ES rendering, and can be compiled into WebAssembly for use in browsers. WebGL is similar to
OpenGL ES, but not identical, so a few additional considerations apply to get projectM running with Emscripten.

## Additional Build Settings

A few additional build settings will be required when building an Emscripten wrapper. Pass these flags/parameterrs to
the Emscripten linker:

- `-sUSE_SDL=2`: It is highly recommended to use Emscripten's built-in SDL2 port to set up the rendering context. This
  flag will link the appropriate library.
- `-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2`: Forces the use of WebGL 2, which is required for OpenGL ES 3 emulation.
- `-sFULL_ES2=1 -sFULL_ES3=1`: Enables full emulation support for both OpenGL ES 2.0 and 3.0 variants.
- `-sALLOW_MEMORY_GROWTH=1`: Allows allocating additional memory if necessary. This may be required to load additional
  textures etc. in projectM.

## Dual-Pipeline Preset Transitions (ENABLE_WASM_TRANSITIONS)

Phase 1 of the dual-pipeline preset transition feature introduces dedicated CMake and linker flags for the WASM build
that are prerequisites for smooth preset cross-fading in the browser.

### Required flags (all set automatically under `if(EMSCRIPTEN)` in CMakeLists.txt)

| Flag | Purpose |
|------|---------|
| `-s USE_WEBGL2=1` | Target WebGL 2.0 — required for MRTs and float texture support |
| `-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2` | Strictly target WebGL 2.0 and avoid fallback to WebGL 1.0 |
| `-s FULL_ES3=1` | Enable full OpenGL ES 3.0 emulation for advanced shader features |
| `-O3` | Maximum optimization — needed to handle dual-preset CPU load |
| `-s ALLOW_MEMORY_GROWTH=1` | Allow WASM heap to grow dynamically — prevents OOM crash when loading a second preset |
| `-s ASYNCIFY=1` | Allow synchronous C++ functions to yield to the JS event loop — prevents browser freeze during shader compilation |

### CMake option: `ENABLE_WASM_TRANSITIONS`

An explicit opt-in CMake option is provided to gate additional transition-tuning flags:

```shell
emcmake cmake -B build-wasm -DENABLE_WASM_TRANSITIONS=ON
```

When `ENABLE_WASM_TRANSITIONS=ON`, the following extra flag is applied:

- `-s ASYNCIFY_STACK_SIZE=65536`: Tunes the ASYNCIFY stack size to reduce binary bloat while preserving enough stack
  space for concurrent preset loading and shader compilation.

This option defaults to `OFF` until all 5 phases of the dual-pipeline transition implementation are complete. Enable it
when actively developing or testing preset transition functionality.

### Future phases

The full dual-pipeline transition feature is being implemented in 5 phases:

1. **Phase 1** (this change) — Emscripten build flags (`USE_WEBGL2`, `FULL_ES3`, `ASYNCIFY`, `ALLOW_MEMORY_GROWTH`, `-O3`)
2. **Phase 2** — Dual ping-pong FBO architecture with floating-point texture support
3. **Phase 3** — WebGL state isolation & playlist transition routing
4. **Phase 4** — Async shader transpilation pipeline
5. **Phase 5** — Final compositing pass: dual-texture blend shader

## Initializing Emscripten's OpenGL Context

In addition to the above linker flags, some additional initialization steps must be performed to set up the OpenGL
rendering context for projectM. Specifically, the `OES_texture_float` WenGL extension must be loaded explicitly to
support the required texture format for the motion vector grid. The following code template can be used to set up a
proper SDL2/WebGL context for projectM:

```c

#include <emscripten.h>
#include <emscripten/html5_webgl.h>

#include <GL/gl.h>

#include <SDL.h>

int main(void)
{
    // Init SDL's video and audio subsystems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Create the SDL window (will be tied to the Emscripten HTML5 canvas)
    SDL_window* window = NULL;
    SDL_renderer* renderer = NULL;
    SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_OPENGL, &window, &renderer);
    if (window == NULL || renderer == NULL)
    {
        fprintf(stderr, "Failed to create SDL renderer: %s\n", SDL_GetError());
        return 1;
    }

    // Enable floating-point texture support for motion vector grid.
    auto webGlContext = emscripten_webgl_get_current_context();
    emscripten_webgl_enable_extension(webGlContext, "OES_texture_float");

    // Initialize projectM and put all other stuff below.
    
    return 0;
}

```