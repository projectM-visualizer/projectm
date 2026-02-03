#!/bin/bash
# test-macos-framework.sh
# CI test script for validating macOS framework builds
#
# Usage: ./scripts/test-macos-framework.sh <build-dir>
#
# This script validates:
# 1. Framework directory structure
# 2. Header completeness
# 3. Linkability (compile and link a test program)

set -e

# Cleanup temp files on exit
TEMP_FILES=()
cleanup() {
    for f in "${TEMP_FILES[@]}"; do
        rm -f "$f" 2>/dev/null || true
    done
}
trap cleanup EXIT

BUILD_DIR="${1:-.}"

echo "=== macOS Framework CI Tests ==="
echo "Build directory: $BUILD_DIR"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

pass() {
    echo -e "${GREEN}PASS${NC}: $1"
}

fail() {
    echo -e "${RED}FAIL${NC}: $1"
    exit 1
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

    # Check Version A contents
    [ -f "$framework_path/Versions/A/$framework_name" ] || fail "Missing binary: Versions/A/$framework_name"
    [ -d "$framework_path/Versions/A/Headers" ] || fail "Missing Headers directory"
    [ -d "$framework_path/Versions/A/Resources" ] || fail "Missing Resources directory"
    [ -f "$framework_path/Versions/A/Resources/Info.plist" ] || fail "Missing Info.plist"

    # Check top-level symlinks
    [ -L "$framework_path/$framework_name" ] || fail "Missing top-level binary symlink"
    [ -L "$framework_path/Headers" ] || fail "Missing top-level Headers symlink"
    [ -L "$framework_path/Resources" ] || fail "Missing top-level Resources symlink"

    # Verify symlinks point to correct locations
    local binary_target=$(readlink "$framework_path/$framework_name")
    [ "$binary_target" = "Versions/Current/$framework_name" ] || fail "Binary symlink points to wrong location: $binary_target"

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

    for header in "${expected_headers[@]}"; do
        [ -f "$headers_dir/$header" ] || fail "Missing header: $header"
    done

    # Count total headers found
    local header_count=$(find "$headers_dir" -name "*.h" -o -name "*.hpp" | wc -l | tr -d ' ')
    echo "  Found $header_count header files"

    pass "Header validation for $framework_name.framework"
}

# Test 3: Linkability Test
test_linkability() {
    local framework_path="$1"
    local framework_name="$2"
    local test_code="$3"

    echo "--- Testing linkability: $framework_name.framework ---"

    local framework_parent=$(dirname "$framework_path")
    # Headers are in Headers/<framework-name>/ to support include patterns like
    # #include <projectM-4/projectM.h>, so we add -I Framework.framework/Headers
    local include_path="$framework_path/Headers"
    local test_file=$(mktemp -t framework_test).c
    local test_output=$(mktemp -t framework_test)
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

# Main tests

# projectM-4.framework
PROJECTM_FRAMEWORK="$BUILD_DIR/src/libprojectM/projectM-4.framework"
if [ -d "$PROJECTM_FRAMEWORK" ]; then
    test_structure "$PROJECTM_FRAMEWORK" "projectM-4"

    # Expected C API headers
    PROJECTM_HEADERS=(
        "projectM.h"
        "audio.h"
        "core.h"
        "types.h"
        "parameters.h"
        "projectM_export.h"
        "version.h"
    )
    test_headers "$PROJECTM_FRAMEWORK" "projectM-4" "projectM-4" "${PROJECTM_HEADERS[@]}"

    # Linkability test - use the full include path to match existing header patterns
    PROJECTM_TEST_CODE='
#include <projectM-4/projectM.h>
int main() {
    projectm_handle pm = projectm_create();
    if (pm) projectm_destroy(pm);
    return 0;
}
'
    test_linkability "$PROJECTM_FRAMEWORK" "projectM-4" "$PROJECTM_TEST_CODE"
else
    echo "SKIP: projectM-4.framework not found at $PROJECTM_FRAMEWORK"
fi

echo ""

# projectM-4-playlist.framework
PLAYLIST_FRAMEWORK="$BUILD_DIR/src/playlist/projectM-4-playlist.framework"
if [ -d "$PLAYLIST_FRAMEWORK" ]; then
    test_structure "$PLAYLIST_FRAMEWORK" "projectM-4-playlist"

    # Expected headers (in projectM-4/ subdir to match include pattern)
    PLAYLIST_HEADERS=(
        "playlist.h"
        "playlist_core.h"
        "playlist_types.h"
        "projectM_playlist_export.h"
    )
    test_headers "$PLAYLIST_FRAMEWORK" "projectM-4-playlist" "projectM-4" "${PLAYLIST_HEADERS[@]}"

    # Linkability test (needs projectM framework too)
    # Playlist headers use projectM-4/ prefix to match the main library
    PLAYLIST_TEST_CODE='
#include <projectM-4/playlist.h>
int main() {
    return 0;
}
'
    # For playlist, we need both frameworks
    if [ -d "$PROJECTM_FRAMEWORK" ]; then
        echo "--- Testing linkability: projectM-4-playlist.framework ---"
        framework_parent=$(dirname "$PLAYLIST_FRAMEWORK")
        projectm_parent=$(dirname "$PROJECTM_FRAMEWORK")
        # Include paths for both frameworks
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
    fi
else
    echo "SKIP: projectM-4-playlist.framework not found at $PLAYLIST_FRAMEWORK"
fi

echo ""
echo "=== All framework tests passed ==="
