#!/bin/sh
aclocal -I m4
libtoolize --force
autoconf
automake -a
