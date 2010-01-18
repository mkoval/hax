#!/bin/sh

error() {
	printf "%s: %s\n" "$0" "$1" > /dev/stderr
}

if [ $# != 1 ]; then
	error "bad number of args"
	exit 2
fi

if   which cygpath 2>/dev/null; then 
	cygpath -w "$1"
elif which winepath 2>/dev/null; then
	winepath -w "$1"
else 
	error "path converter not found"
	printf "%s\n" "$1"
fi 
