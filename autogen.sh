#!/bin/sh
aclocal -I m4 &&
libtoolize -c &&
automake --add-missing --copy &&
autoconf --force &&
./configure $*
