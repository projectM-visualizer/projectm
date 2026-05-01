#!/bin/bash
set -euo pipefail

# Colab-friendly Project-M build script.
# Usage in Colab:
# %%bash
# /content/build_space/projectm/scripts/colab_build.sh

PROJECT_ROOT="${PROJECT_ROOT:-/content/build_space/projectm}"
EMSDK_ROOT="${EMSDK_ROOT:-/content/build_space/emsdk}"
EMSDK_ENV="${EMSDK_ROOT}/emsdk_env.sh"
JVM_HEAP_SIZE="${JVM_HEAP_SIZE:-8g}"
BUILD_JOBS="${BUILD_JOBS:-8}"

PACKAGES=(bison flex cmake-curses-gui qtbase5-dev llvm-dev libvisual-0.4-dev ninja-build)

if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
elif command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    echo "Error: need root or sudo to install packages." >&2
    exit 1
fi

install_if_missing() {
    local pkg="$1"
    if ! dpkg -s "$pkg" >/dev/null 2>&1; then
        echo "Installing missing package: $pkg"
        $SUDO apt-get install -y "$pkg"
    else
        echo "Package already installed: $pkg"
    fi
}

echo "=== Installing required system packages if missing ==="
if ! command -v dpkg-query >/dev/null 2>&1; then
    echo "Error: dpkg-query not available. Cannot detect installed packages." >&2
    exit 1
fi

MISSING=0
for pkg in "${PACKAGES[@]}"; do
    if ! dpkg -s "$pkg" >/dev/null 2>&1; then
        MISSING=1
        break
    fi
done

if [ "$MISSING" -eq 1 ]; then
    $SUDO apt-get update -y
    for pkg in "${PACKAGES[@]}"; do
        install_if_missing "$pkg"
    done
else
    echo "All required packages already installed."
fi

mkdir -p "$PROJECT_ROOT"
cd "$PROJECT_ROOT"

if [ ! -d .git ]; then
    echo "=== Cloning Project-M repository ==="
    git clone --recursive https://github.com/ford442/Project-M.git "$PROJECT_ROOT"
else
    echo "=== Updating Project-M repository ==="
    git pull --recurse-submodules
    git submodule update --init --recursive
fi

if [ ! -f "$EMSDK_ENV" ]; then
    echo "Error: Emscripten env script not found at $EMSDK_ENV" >&2
    exit 1
fi

export JVM_HEAP_SIZE="$JVM_HEAP_SIZE"
source "$EMSDK_ENV"

echo "=== Building projectm-eval ==="
cd "$PROJECT_ROOT/vendor/projectm-eval"
rm -rf build
mkdir -p build
cd build
cmake ..
make install -j"$BUILD_JOBS"

echo "=== Building main Project-M with emcmake ==="
cd "$PROJECT_ROOT"
rm -rf build
mkdir -p build
cd build
source "$EMSDK_ENV"
emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DSDL_PTHREADS=1 \
    -DUSE_PTHREADS=1 \
    -DBOOST_HAS_THREADS=1 \
    -DBOOST_UBLAS_USE_LONG_DOUBLE=1 \
    -DBOOST_UBLAS_NDEBUG=1 \
    -DENABLE_EMSCRIPTEN=1 \
    -DENABLE_GLES=1 \
    -DBUILD_SHARED_LIBS=0 \
    -DENABLE_CXX_INTERFACE=1 \
    -DCMAKE_MODULE_PATH="/usr/local/lib/cmake/projectM-Eval/"

emmake cmake --build . --target install --config Release -j"$BUILD_JOBS"

echo "=== Building final WASM/JS wrapper ==="
cd "$PROJECT_ROOT"
export JVM_HEAP_SIZE="$JVM_HEAP_SIZE"
source "$EMSDK_ENV"
emcc projectM_emscripten.cpp \
    -I /usr/local/include -O3 \
    -l embind -pthread -fopenmp "$PROJECT_ROOT/libomp.a" \
    -o projectm-v.024-thread.js \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s NO_DISABLE_EXCEPTION_CATCHING=1 \
    -s ENVIRONMENT=web,worker \
    -s SHARED_MEMORY=1 \
    -s EXPORTED_FUNCTIONS=_add_audio_data,_main,_pl,_destruct,_get_projectm_handle,_init,_load_preset_file,_switch_preset,_set_aspect_correction,_render_frame,_start_render,_set_window_size,_set_mesh,_add_preset_path,_add_existing_vfs_presets,_add_preset_file,_add_custom_milk_paths,_projectm_pcm_add_float_wrapper,_create_sprite,_stop_worklet_playback,_set_audio_source_to_stream \
    -s EXPORTED_RUNTIME_METHODS=ccall,FS \
    -s EXPORT_NAME=createModule \
    -s PTHREAD_POOL_SIZE=4 \
    -s FULL_ES2=1 \
    -s FULL_ES3=1 \
    -s MIN_WEBGL_VERSION=2 \
    -s MAX_WEBGL_VERSION=2 \
    -s MODULARIZE=1 \
    -s FORCE_FILESYSTEM=1 \
    "$PROJECT_ROOT/build/src/libprojectM/libprojectM-4.a" \
    "$PROJECT_ROOT/build/src/playlist/libprojectM-4-playlist.a"

echo "=== Running optimize.sh and creating UTF-16/UTF-32 versions ==="
bash "$PROJECT_ROOT/optimize.sh" || echo "Warning: optimize.sh returned non-zero"
iconv -f UTF-8 -t UTF-16 "$PROJECT_ROOT/projectm-v.024-thread.js" -o "$PROJECT_ROOT/projectm-v.024-thread.1ijs"
iconv -f UTF-8 -t UTF-32 "$PROJECT_ROOT/projectm-v.024-thread.js" -o "$PROJECT_ROOT/projectm-v.024-thread.3ijs"

echo "=== Colab build complete ==="
echo "Output files: $PROJECT_ROOT/projectm-v.024-thread.js, .1ijs, .3ijs"
