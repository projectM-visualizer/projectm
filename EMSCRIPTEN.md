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