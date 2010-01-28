#!/bin/sh

which=$(which which)

error() {
	printf "%s: %s\n" "$0" "$1" > /dev/stderr
}

if [ $# != 1 ]; then
	error "bad number of args"
	exit 2
fi

cyg_=$(which cygpath 2>/dev/null)
cyg=$?
wine_=$(which winepath 2>/dev/null)
wine=$?

if [ $cyg -eq 0 ] ; then 
	cygpath -w "$1"
elif [ $wine -eq 0 ] ; then
	winepath -w "$1"
else 
	error "path converter not found"
	printf "%s\n" "$1"
fi 
