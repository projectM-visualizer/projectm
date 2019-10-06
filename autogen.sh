#!/usr/bin/env sh

set -e # Drop-out from execution if error occurs

printf 'Running autoreconf...'

autoreconf --install

printf '\nSuccess!\nNow please run: ./configure\n'
