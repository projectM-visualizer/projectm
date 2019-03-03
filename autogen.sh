#!/bin/sh -e

echo "Running autoreconf... make sure you have GNU autotools available on your system"

autoreconf --install

echo "You followed directions! Great work!\nNow run ./configure"
