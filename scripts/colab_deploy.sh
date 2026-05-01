#!/bin/bash
set -euo pipefail

# Colab deployment helper for Project-M / similar builds.
# Usage:
#   PROJECT_ROOT=/content/build_space/projectm bash /content/build_space/projectm/scripts/colab_deploy.sh
# Optional env vars:
#   HOST, USERNAME, PASSWORD, PROJECT_ROOT

PROJECT_ROOT="${PROJECT_ROOT:-/content/build_space/projectm}"
BUILD_DIR="${BUILD_DIR:-${PROJECT_ROOT}/build}"
HTML_PATH="${HTML_PATH:-${BUILD_DIR}/index.html}"

HOST="${HOST:-1ink.us}"
USERNAME="${USERNAME:-ford442}"
PASSWORD="${PASSWORD:-${SFTP_PASS:-GoogleBez12!}}"
PORT="${PORT:-22}"

REMOTE_BASES=("wasm.noahcohn.com/pm/" "noahcohn.com/pm/")
UPLOAD_FILES=(
    "projectm-v.024-thread.1ijs"
    "projectm-v.024-thread.wasm"
    "projectm-v.024-thread.3ijs"
)

if [ ! -d "$PROJECT_ROOT" ]; then
    echo "❌ ERROR: PROJECT_ROOT does not exist: $PROJECT_ROOT"
    exit 1
fi

if [ -f "$HTML_PATH" ]; then
    echo "=== Patching $HTML_PATH to use relative assets ==="
    python3 - <<'PY'
from pathlib import Path
path = Path(r"$HTML_PATH")
text = path.read_text(encoding="utf-8")
text = text.replace('src="/', 'src="./')
text = text.replace('href="/', 'href="./')
path.write_text(text, encoding="utf-8")
PY
    echo "=== Generating UTF-16 index file ==="
    iconv -f UTF-8 -t UTF-16 "$HTML_PATH" -o "${BUILD_DIR}/1ink.1ink"
else
    echo "⚠️  Skipping HTML patch: not found at $HTML_PATH"
fi

upload_file() {
    local local_path="$1"
    local remote_path="$2"

    if [ ! -f "$local_path" ]; then
        echo "⚠️  Skipping missing file: $local_path"
        return 1
    fi

    echo "Uploading $local_path -> $remote_path"
    if curl -T "$local_path" --silent --fail "sftp://${USERNAME}:${PASSWORD}@${HOST}/${remote_path}"; then
        echo "   ✅ curl upload succeeded"
        return 0
    fi

    if ! command -v sshpass >/dev/null 2>&1; then
        echo "sshpass not found, installing..."
        if command -v apt-get >/dev/null 2>&1; then
            sudo apt-get update -y
            sudo apt-get install -y sshpass
        else
            echo "Cannot install sshpass automatically. Please install it or use another upload method."
            return 1
        fi
    fi

    sshpass -p "$PASSWORD" scp -o StrictHostKeyChecking=no "$local_path" "${USERNAME}@${HOST}:${remote_path}"
}

echo "=== Deploying artifacts from $PROJECT_ROOT ==="

for file in "${UPLOAD_FILES[@]}"; do
    local_path="${PROJECT_ROOT}/${file}"
    for base in "${REMOTE_BASES[@]}"; do
        remote_path="${base}${file}"
        upload_file "$local_path" "$remote_path"
    done
done

echo "=== Deploy complete ==="
