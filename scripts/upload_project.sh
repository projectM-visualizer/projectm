#!/bin/bash
# ================================================
# upload_projectm.sh
# SFTP upload script (no Python / no paramiko)
# Works great inside containerized code-server
# ================================================

set -euo pipefail

# ============== CONFIG ==============
HOST="1ink.us"
USER="ford442"
# ← BEST PRACTICE: use environment variable instead of hardcoding
PASS="${SFTP_PASS:-GoogleBez12!}"

LOCAL_DIR="/root/Project-M/"     # ← change only if your build folder is different

# Files to upload (exactly the same as your Colab)
FILES=(
    "projectm-v.024-thread.1ijs"
    "projectm-v.024-thread.wasm"
    "projectm-v.024-thread.3ijs"
)

# Both destinations (matches your original script)
REMOTE_BASES=(
    "wasm.noahcohn.com/pm/"
    "noahcohn.com/pm/"
)
# =====================================

if [ -z "$PASS" ] || [ "$PASS" = "GoogleBez12!" ]; then
    echo "⚠️  WARNING: Using default/hardcoded password!"
    echo "   Better: export SFTP_PASS='your-real-password' && ./upload_projectm.sh"
fi

echo "=== Starting upload to $HOST ==="

for file in "${FILES[@]}"; do
    local_path="${LOCAL_DIR}/${file}"

    if [ ! -f "$local_path" ]; then
        echo "⚠️  File not found (skipping): $file"
        continue
    fi

    echo "→ Uploading $file ..."

    for base in "${REMOTE_BASES[@]}"; do
        echo "   to ${base}${file}"

        # Try curl first (usually already installed, zero extra deps)
        if curl -T "$local_path" \
                --fail --silent \
                "sftp://${USER}:${PASS}@${HOST}/${base}${file}"; then
            echo "   ✅ curl OK"
        else
            echo "   curl failed → falling back to sshpass + scp"
            
            # Auto-install sshpass if missing (works on Debian/Ubuntu/Alpine containers)
            if ! command -v sshpass >/dev/null 2>&1; then
                echo "   Installing sshpass..."
                if command -v apt-get >/dev/null 2>&1; then
                    sudo apt-get update -qq && sudo apt-get install -y sshpass
                elif command -v apk >/dev/null 2>&1; then
                    sudo apk add --no-cache sshpass
                else
                    echo "   Could not auto-install sshpass. Install manually and retry."
                    exit 1
                fi
            fi

            sshpass -p "$PASS" scp -o StrictHostKeyChecking=no \
                "$local_path" "${USER}@${HOST}:${base}${file}"
            echo "   ✅ scp OK"
        fi
    done
done

echo "=== All uploads finished! ==="
