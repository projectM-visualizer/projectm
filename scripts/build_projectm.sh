#!/bin/bash
# ================================================
# build_projectm.sh
# Standalone build script for ford442/Project-M (Emscripten/WASM version)
# Run outside Colab on a Debian/Ubuntu Linux machine.
#
# PREREQUISITES:
#   1. Install Emscripten SDK somewhere (recommended: ~/build_space/emsdk)
#      https://emscripten.org/docs/getting_started/downloads.html
#   2. sudo apt update && sudo apt install -y aptitude (if you want to keep aptitude)
#   3. Run this script with: bash build_projectm.sh
# ================================================

set -e  # Exit on any error

# ========================= CONFIG =========================
BASE_DIR="${HOME}/*/"                  # Change if you want another location
PROJECT_DIR="${BASE_DIR}/projectm"
EMSDK_ENV="${BASE_DIR}/emsdk/emsdk_env.sh"
JVM_HEAP_SIZE=8g
BUILD_JOBS=55
# =========================================================

echo "=== Setting up build directories ==="
mkdir -p "${BASE_DIR}"
cd "${BASE_DIR}"

# 1. Install system dependencies
echo "=== Installing system dependencies ==="
sudo apt update -y
sudo apt install aptitude zip -y
sudo aptitude update -y || sudo apt update -y
sudo aptitude install -y cmake-curses-gui qtbase5-dev llvm-dev libvisual-0.4-dev ninja-build \
    || sudo apt install -y cmake-curses-gui qtbase5-dev llvm-dev libvisual-0.4-dev ninja-build

# 2. Clone / update the repository
echo "=== Cloning / updating Project-M repository ==="
if [ ! -d "${PROJECT_DIR}" ]; then
    git clone https://github.com/ford442/Project-M.git --recursive "${PROJECT_DIR}"
else
    cd "${PROJECT_DIR}"
    git pull --recurse-submodules
fi
cd "${PROJECT_DIR}"

# Copy omp headers (repo contains /omp folder and omp.zip)
cp -r "/root/Project-M/omp/"* "/root/Project-M/"
unzip -o "/root/Project-M/omp.zip" -d "/root/Project-M/"

# 3. Build vendor/projectm-eval
echo "=== Building projectm-eval ==="
cd "${PROJECT_DIR}/vendor/projectm-eval"
rm -rf build 2>/dev/null || true
mkdir -p build
cd build

export JVM_HEAP_SIZE=${JVM_HEAP_SIZE}

source "/root/emsdk/emsdk_env.sh"

cmake ..
make install -j${BUILD_JOBS}

# 4. Prepare main build directory + OpenMP headers hack
echo "=== Preparing main build and OpenMP headers ==="
cd "${PROJECT_DIR}"
rm -rf build 2>/dev/null || true
mkdir -p build
cd build


# 5. CMake + Emscripten build of libprojectM
echo "=== Running emcmake + emmake build ==="
source "/root/emsdk/emsdk_env.sh"

# The original Colab line was incomplete; we set CMAKE_MODULE_PATH properly
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

emmake cmake --build . --target install --config Release -j${BUILD_JOBS}

# 6. Final emcc wrapper (projectM_emscripten.cpp)
echo "=== Building final WASM/JS wrapper with emcc ==="
cd "${PROJECT_DIR}"

export JVM_HEAP_SIZE=${JVM_HEAP_SIZE}
source "/root/emsdk/emsdk_env.sh"

emcc projectM_emscripten.cpp \
    -I /usr/local/include \
    -O3 \
    -l embind -pthread -fopenmp /root/Project-M/libomp.a \
    -o projectm-v.024-thread.js \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s NO_DISABLE_EXCEPTION_CATCHING=1 \
    -s ENVIRONMENT=web,worker \
    -s SHARED_MEMORY=1 \
    -s EXPORTED_FUNCTIONS=_add_audio_data,_main,_pl \
    -s EXPORTED_RUNTIME_METHODS=ccall \
    -s EXPORT_NAME=createModule \
    -s PTHREAD_POOL_SIZE=4 \
    -s FULL_ES2=0 \
    -s FULL_ES3=1 \
    -s MIN_WEBGL_VERSION=2 \
    -s MAX_WEBGL_VERSION=2 \
    -s MODULARIZE=1 \
    -s ASYNCIFY=1 \
    -s FORCE_FILESYSTEM=1 \
    -s WASMFS=1 \
    "${PROJECT_DIR}/build/src/libprojectM/libprojectM-4.a" \
    "${PROJECT_DIR}/build/src/playlist/libprojectM-4-playlist.a"

# 7. Run optimize.sh + iconv conversions (as in original Colab)
echo "=== Running optimize.sh and iconv conversions ==="
bash "${PROJECT_DIR}/optimize.sh" || echo "Warning: optimize.sh returned non-zero (may be expected)"

iconv -f UTF-8 -t UTF-16 "/root/Project-M/projectm-v.024-thread.js" -o "/root/Project-M/projectm-v.024-thread.1ijs"
iconv -f UTF-8 -t UTF-32 "/root/Project-M/projectm-v.024-thread.js" -o "/root/Project-M/projectm-v.024-thread.3ijs"

echo "=== Build completed successfully! ==="
echo "Output files are in: /root/Project-M/"
