# projectM for emscripten

This application is a sample entry point for the emscripten-built version of projectM.

It's a very basic application, only displaying the "idle" preset. No presets are currently packaged into the virtual file system.

## Limitations

emscripten builds of projectM only support building as a static library. Shared libraries are only simulated and
basically identical to static libraries.

Multithreading and OpenMP support are disabled due to the lack of real multithreading capabilities in
JavaScript/WebAssembly.

The build will use OpenGL ES 3.0, using `FULL_ES2` compatibility only.

## Prepare for building

Download and build the emscripten SDK: https://emscripten.org/docs/getting_started/downloads.html

No other libraries are required as emscripten has everything built-in that is needed by projectM.

## Configure and compile projectM

CMake will auto-detect that emscripten is used for building as the toolchain sets `CMAKE_SYSTEM_NAME`
accordingly. No other options need to be set except `CMAKE_INSTALL_PREFIX` if you plan to install the build results for
further use.

Running these commands in projectM's source dir will configure and build projectM for Emscripten:

```sh
mkdir cmake-build-emscripten
cd cmake-build-emscripten
emcmake ..
emmake
```

If the build was successful, you will find the projectM_SDL_emscripten.html, .js and .wasm files in
the `cmake-build-emscripten/src/projectM-emscripten`directory.
