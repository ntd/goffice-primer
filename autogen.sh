#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

if test ! -f $srcdir/configure.ac -o ! -f $srcdir/chart.c; then
    echo "**Error**: '$srcdir' does not look like the top-level goffice-primer directory"
    exit 1
fi

cd $srcdir
autoreconf -is -Wall || exit $?
./configure "$@" && echo "Now type 'make' to compile $PROJECT."
