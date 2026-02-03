#!/bin/bash
# test-macos-framework.sh
# CI test script for validating macOS framework builds
#
# Usage: ./scripts/test-macos-framework.sh <dir> [<dir>...]
#
# Searches the given directories for projectM-4.framework and
# projectM-4-playlist.framework. Works with both build trees
# (where frameworks are in src/libprojectM/ and src/playlist/)
# and install prefixes (where frameworks are in lib/).
#
# Environment:
#   STRICT=1  (default) SKIPs become FAILs - use for CI
#   STRICT=0  SKIPs are warnings - use for local testing
#
# This script validates:
# 1. Framework directory structure (symlinks, directories, Info.plist)
# 2. Header completeness (every expected header must be present)
# 3. No unexpected headers (catch stale or misplaced files)
# 4. Linkability (compile and link a test program against framework)
# 5. dylib identity (install name matches framework convention)

set -euo pipefail

# Cleanup temp files on exit
TEMP_FILES=()
cleanup() {
    for f in "${TEMP_FILES[@]}"; do
        rm -f "$f" 2>/dev/null || true
    done
}
trap cleanup EXIT

SEARCH_DIRS=("${@:-.}")
STRICT="${STRICT:-1}"  # In strict mode, SKIPs become FAILs

# Find a framework by name in the search directories
find_framework() {
    local name="$1"
    for dir in "${SEARCH_DIRS[@]}"; do
        # Check common locations
        for candidate in \
            "$dir/$name.framework" \
            "$dir/src/libprojectM/$name.framework" \
            "$dir/src/playlist/$name.framework" \
            "$dir/lib/$name.framework"; do
            if [ -d "$candidate" ]; then
                echo "$candidate"
                return 0
            fi
        done
    done
    return 1
}

echo "=== macOS Framework CI Tests ==="
echo "Search directories: ${SEARCH_DIRS[*]}"
echo "Strict mode: $STRICT"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

TESTS_RUN=0
TESTS_PASSED=0

pass() {
    echo -e "${GREEN}PASS${NC}: $1"
    TESTS_RUN=$((TESTS_RUN + 1))
    TESTS_PASSED=$((TESTS_PASSED + 1))
}

fail() {
    echo -e "${RED}FAIL${NC}: $1"
    exit 1
}

skip() {
    if [ "$STRICT" = "1" ]; then
        fail "SKIP not allowed in strict mode: $1"
    else
        echo -e "${YELLOW}SKIP${NC}: $1"
    fi
}

# Test 1: Framework Structure Validation
test_structure() {
    local framework_path="$1"
    local framework_name="$2"

    echo "--- Testing structure: $framework_name.framework ---"

    # Check framework directory exists
    [ -d "$framework_path" ] || fail "Framework does not exist: $framework_path"

    # Check Versions directory
    [ -d "$framework_path/Versions" ] || fail "Missing Versions directory"
    [ -d "$framework_path/Versions/A" ] || fail "Missing Versions/A directory"
    [ -L "$framework_path/Versions/Current" ] || fail "Missing Versions/Current symlink"

    # Verify Current symlink target
    local current_target
    current_target=$(readlink "$framework_path/Versions/Current")
    [ "$current_target" = "A" ] || fail "Versions/Current symlink points to '$current_target', expected 'A'"

    # Check Version A contents
    [ -f "$framework_path/Versions/A/$framework_name" ] || fail "Missing binary: Versions/A/$framework_name"
    [ -d "$framework_path/Versions/A/Headers" ] || fail "Missing Headers directory"
    [ -d "$framework_path/Versions/A/Resources" ] || fail "Missing Resources directory"
    [ -f "$framework_path/Versions/A/Resources/Info.plist" ] || fail "Missing Info.plist"

    # Validate Info.plist has correct bundle executable name
    local bundle_exec
    bundle_exec=$(/usr/libexec/PlistBuddy -c "Print :CFBundleExecutable" "$framework_path/Versions/A/Resources/Info.plist" 2>/dev/null || true)
    [ "$bundle_exec" = "$framework_name" ] || fail "Info.plist CFBundleExecutable is '$bundle_exec', expected '$framework_name'"

    # Check top-level symlinks exist and are symlinks (not copies)
    [ -L "$framework_path/$framework_name" ] || fail "Missing top-level binary symlink"
    [ -L "$framework_path/Headers" ] || fail "Missing top-level Headers symlink"
    [ -L "$framework_path/Resources" ] || fail "Missing top-level Resources symlink"

    # Verify symlinks point to correct locations
    local binary_target headers_target resources_target
    binary_target=$(readlink "$framework_path/$framework_name")
    headers_target=$(readlink "$framework_path/Headers")
    resources_target=$(readlink "$framework_path/Resources")
    [ "$binary_target" = "Versions/Current/$framework_name" ] || fail "Binary symlink points to '$binary_target', expected 'Versions/Current/$framework_name'"
    [ "$headers_target" = "Versions/Current/Headers" ] || fail "Headers symlink points to '$headers_target', expected 'Versions/Current/Headers'"
    [ "$resources_target" = "Versions/Current/Resources" ] || fail "Resources symlink points to '$resources_target', expected 'Versions/Current/Resources'"

    # Verify the binary is a Mach-O dylib
    local file_type
    file_type=$(file -b "$framework_path/Versions/A/$framework_name")
    echo "$file_type" | grep -q "Mach-O" || fail "Binary is not Mach-O: $file_type"
    echo "$file_type" | grep -qi "dynamically linked\|dynamic library" || fail "Binary is not a dynamic library: $file_type"

    pass "Structure validation for $framework_name.framework"
}

# Test 2: Header Completeness
# Usage: test_headers <framework_path> <framework_name> <header_subdir> <expected_headers...>
test_headers() {
    local framework_path="$1"
    local framework_name="$2"
    local header_subdir="$3"
    shift 3
    local expected_headers=("$@")

    echo "--- Testing headers: $framework_name.framework ---"

    # Headers are in Headers/<header-subdir>/ to support #include <subdir/header.h>
    local headers_dir="$framework_path/Headers/$header_subdir"
    [ -d "$headers_dir" ] || fail "Headers directory does not exist: $headers_dir"

    # Check each expected header is present
    for header in "${expected_headers[@]}"; do
        [ -f "$headers_dir/$header" ] || fail "Missing expected header: $header_subdir/$header"
    done

    # Check for unexpected headers (catches stale files or wrong headers being copied)
    local actual_count expected_count
    actual_count=$(find "$headers_dir" -name "*.h" -o -name "*.hpp" | wc -l | tr -d ' ')
    expected_count=${#expected_headers[@]}
    echo "  Found $actual_count header files (expected $expected_count)"
    if [ "$actual_count" -ne "$expected_count" ]; then
        echo "  Expected headers:"
        for h in "${expected_headers[@]}"; do echo "    $h"; done
        echo "  Actual headers:"
        find "$headers_dir" \( -name "*.h" -o -name "*.hpp" \) -exec basename {} \; | sort | while read -r h; do echo "    $h"; done
        fail "Header count mismatch: found $actual_count, expected $expected_count"
    fi

    pass "Header validation for $framework_name.framework ($actual_count headers)"
}

# Test 3: Linkability Test
test_linkability() {
    local framework_path="$1"
    local framework_name="$2"
    local test_code="$3"

    echo "--- Testing linkability: $framework_name.framework ---"

    local framework_parent
    framework_parent=$(dirname "$framework_path")
    # Headers are in Headers/<framework-name>/ to support include patterns like
    # #include <projectM-4/projectM.h>, so we add -I Framework.framework/Headers
    local include_path="$framework_path/Headers"
    local test_file test_output
    test_file=$(mktemp -t framework_test).c
    test_output=$(mktemp -t framework_test)
    TEMP_FILES+=("$test_file" "$test_output")

    echo "$test_code" > "$test_file"

    local compile_errors
    if compile_errors=$(clang -F "$framework_parent" -I "$include_path" -framework "$framework_name" "$test_file" -o "$test_output" 2>&1); then
        pass "Linkability test for $framework_name.framework"
    else
        echo "$compile_errors"
        fail "Failed to compile and link against $framework_name.framework"
    fi
}

# Test 4: dylib install name check
test_install_name() {
    local framework_path="$1"
    local framework_name="$2"

    echo "--- Testing install name: $framework_name.framework ---"

    local binary="$framework_path/Versions/A/$framework_name"
    local install_name
    install_name=$(otool -D "$binary" 2>/dev/null | tail -1)

    # For a framework, the install name should contain the framework name
    if echo "$install_name" | grep -q "$framework_name"; then
        echo "  Install name: $install_name"
        pass "Install name check for $framework_name.framework"
    else
        echo "  Install name: $install_name"
        fail "Install name does not reference framework name '$framework_name'"
    fi
}

# ============================================================================
# Main tests
# ============================================================================

# projectM-4.framework
PROJECTM_FRAMEWORK=$(find_framework "projectM-4" || true)
if [ -n "$PROJECTM_FRAMEWORK" ] && [ -d "$PROJECTM_FRAMEWORK" ]; then
    echo "Found: $PROJECTM_FRAMEWORK"
    test_structure "$PROJECTM_FRAMEWORK" "projectM-4"

    # All expected C API headers (must be exhaustive)
    PROJECTM_HEADERS=(
        "projectM.h"
        "audio.h"
        "callbacks.h"
        "core.h"
        "debug.h"
        "logging.h"
        "memory.h"
        "parameters.h"
        "render_opengl.h"
        "touch.h"
        "types.h"
        "user_sprites.h"
        "projectM_export.h"
        "version.h"
    )
    # C++ headers are present when built with ENABLE_CXX_INTERFACE
    # Check if any C++ headers exist to determine if we should validate them
    if [ -d "$PROJECTM_FRAMEWORK/Headers/projectM-4/Audio" ]; then
        PROJECTM_HEADERS+=(
            "projectM_cxx_export.h"
            "Logging.hpp"
            "ProjectM.hpp"
            "Audio/AudioConstants.hpp"
            "Audio/FrameAudioData.hpp"
            "Audio/Loudness.hpp"
            "Audio/MilkdropFFT.hpp"
            "Audio/PCM.hpp"
            "Audio/WaveformAligner.hpp"
            "Renderer/RenderContext.hpp"
            "Renderer/TextureTypes.hpp"
        )
    fi
    test_headers "$PROJECTM_FRAMEWORK" "projectM-4" "projectM-4" "${PROJECTM_HEADERS[@]}"

    # Linkability test with the C API
    PROJECTM_TEST_CODE='
#include <projectM-4/projectM.h>
int main() {
    projectm_handle pm = projectm_create();
    if (pm) projectm_destroy(pm);
    return 0;
}
'
    test_linkability "$PROJECTM_FRAMEWORK" "projectM-4" "$PROJECTM_TEST_CODE"
    test_install_name "$PROJECTM_FRAMEWORK" "projectM-4"
else
    skip "projectM-4.framework not found in search directories"
fi

echo ""

# projectM-4-playlist.framework
PLAYLIST_FRAMEWORK=$(find_framework "projectM-4-playlist" || true)
if [ -n "$PLAYLIST_FRAMEWORK" ] && [ -d "$PLAYLIST_FRAMEWORK" ]; then
    echo "Found: $PLAYLIST_FRAMEWORK"
    test_structure "$PLAYLIST_FRAMEWORK" "projectM-4-playlist"

    # All expected playlist headers (must be exhaustive)
    PLAYLIST_HEADERS=(
        "playlist.h"
        "playlist_callbacks.h"
        "playlist_core.h"
        "playlist_filter.h"
        "playlist_items.h"
        "playlist_memory.h"
        "playlist_playback.h"
        "playlist_types.h"
        "projectM_playlist_export.h"
    )
    test_headers "$PLAYLIST_FRAMEWORK" "projectM-4-playlist" "projectM-4" "${PLAYLIST_HEADERS[@]}"

    # Linkability test (needs projectM framework too)
    PLAYLIST_TEST_CODE='
#include <projectM-4/playlist.h>
int main() {
    return 0;
}
'
    if [ -d "$PROJECTM_FRAMEWORK" ]; then
        echo "--- Testing linkability: projectM-4-playlist.framework ---"
        framework_parent=$(dirname "$PLAYLIST_FRAMEWORK")
        projectm_parent=$(dirname "$PROJECTM_FRAMEWORK")
        include_path="$PLAYLIST_FRAMEWORK/Headers"
        projectm_include_path="$PROJECTM_FRAMEWORK/Headers"
        test_file=$(mktemp -t framework_test).c
        test_output=$(mktemp -t framework_test)
        TEMP_FILES+=("$test_file" "$test_output")
        echo "$PLAYLIST_TEST_CODE" > "$test_file"

        compile_errors=""
        if compile_errors=$(clang -F "$framework_parent" -F "$projectm_parent" \
              -I "$include_path" -I "$projectm_include_path" \
              -framework "projectM-4-playlist" -framework "projectM-4" \
              "$test_file" -o "$test_output" 2>&1); then
            pass "Linkability test for projectM-4-playlist.framework"
        else
            echo "$compile_errors"
            fail "Failed to compile and link against projectM-4-playlist.framework"
        fi
    else
        skip "Cannot test playlist linkability without projectM-4.framework"
    fi

    test_install_name "$PLAYLIST_FRAMEWORK" "projectM-4-playlist"
else
    skip "projectM-4-playlist.framework not found in search directories"
fi

echo ""
echo "=== All framework tests passed ($TESTS_PASSED/$TESTS_RUN) ==="
