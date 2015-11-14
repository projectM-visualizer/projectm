#!/bin/sh

set -x
set -e
emcc -I../libprojectm -s USE_SDL=2 projectM_SDL_emscripten.cpp -lMilkdropPresetFactory -lRenderer -lprojectM -lRenderer -lMilkdropPresetFactory -Llib
