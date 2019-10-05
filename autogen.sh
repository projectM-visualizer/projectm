#!/usr/bin/env sh

echo "Running autoreconf... make sure you have GNU autotools available on your system"
set -e # Drop-out from execution if error occurs

autoreconf --install

echo "You followed directions! Great work!\nNow run ./configure"
