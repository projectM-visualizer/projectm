#!/bin/bash
set -e  # Exit on error for critical operations

echo "🚀 Starting Post-Build Optimization..."

# 1. Define Paths
NATIVE_WASM="/content/build_space/projectm/projectm-v.024-thread.wasm"

# 2. Check for Tools (but don't fail if missing)
WASM_OPT_AVAILABLE=false
TERSER_AVAILABLE=false

if command -v wasm-opt &> /dev/null; then
    WASM_OPT_AVAILABLE=true
    echo "✓ wasm-opt found"
else
    echo "⚠️  wasm-opt not found - skipping WASM optimization"
fi

if command -v terser &> /dev/null; then
    TERSER_AVAILABLE=true
    echo "✓ terser found"
else
    echo "⚠️  terser not found - skipping JS minification"
fi

if ! command -v wasmedge &> /dev/null; then
    echo "⚠️  wasmedge not found! Installing via 'curl -sSf https://raw.githubusercontent.com/WasmEdge/WasmEdge/master/utils/install.sh | bash && source $HOME/.wasmedge/env'"
	curl -sSf https://raw.githubusercontent.com/WasmEdge/WasmEdge/master/utils/install.sh | bash && source $HOME/.wasmedge/env
fi

if ! command -v wasmedge &> /dev/null; then
    echo "⚠️  wasmedge still not found!"
else
    WASMEDGE_AVAILABLE=true
    echo "✓ wasmedge found"
fi

# 4. Optimize Emscripten WASM (Native Effects) - commented out as files may not exist
if [ "$WASM_OPT_AVAILABLE" = true ] && [ -f "$NATIVE_WASM" ]; then
    echo "🔧 Optimizing Native WASM..."
    wasm-opt "$NATIVE_WASM" -o "$NATIVE_WASM" \
      -O4 \
      --converge \
      --strip-debug \
      --enable-simd \
      --enable-threads \
      --enable-bulk-memory \
      --enable-nontrapping-float-to-int \
      --enable-exception-handling
fi

# Try wasmedge optimization if available
if [ "$WASMEDGE_AVAILABLE" = true ] && [ -f "$NATIVE_WASM" ]; then
    echo "🔧 Optimizing WASM (wasmedge)..."
    wasmedgec --optimize=3 --enable-all "$NATIVE_WASM" "$NATIVE_WASM" || true
fi

# 5. Minify Emscripten Loaders (Safety First) - commented out as files may not exist
#if [ "$TERSER_AVAILABLE" = true ] && [ -f "$NATIVE_JS" ]; then
#    echo "📦 Minifying JS Loaders..."
#    terser "$NATIVE_JS" -o "$NATIVE_JS" \
#      --compress defaults=false,dead_code=true,unused=true,loops=true,conditionals=true \
#      --mangle reserved=['Module','FS','GL'] \
#      --comments false
#fi

echo "✅ Optimization Complete!"
exit 0
