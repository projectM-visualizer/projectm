#!/bin/sh

set -x
set -e
emcc -I../libprojectm -I../libprojectM/Renderer/  -s USE_SDL=2 ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp  -lRenderer  -lprojectM  -lMilkdropPresetFactory  -lprojectM   -lRenderer -Llib
