#! /bin/bash

# Prints an error message to stderr and exits the program.
function err {
	echo "error: $1" 1>&2
	exit 1
}

# Prints a warning message to stderr and continues execution.
function war {
	echo "warning: $1" 1>&2
}

# Checks if the argument is a valid command.
function has {
	command -v "$1"
}

#
# SHELL CONFIG AND ENV-VARS
#
# Verify the presence of non-architecture-specific dependencies.
if [ ! "`has 'make'`" ]; then
	err "missing dependency 'make'"
elif [ ! "`has 'tar'`" ]; then
	err "missing dependency 'tar'"
elif [ ! "`has 'wget'`" -a ! "`has 'curl'`" ]; then
	err "missing dependency 'wget' or 'curl'"
elif [ ! "`has 'gcc'`" ]; then
	err "missing dependency 'gcc'"
fi

# Issue a warning if this is not a Bash shell to explain potential errors.
if [ ! `echo "$SHELL" | grep "bash"` ]; then
	war "untested shell detected, use bash for full compatability"
fi
# Verify permissions on the installation directory.
if [ -z "$PREFIX" ]; then
	PREFIX="/usr/local"
elif [ ! -w "$PREFIX" ]; then
	err "insufficient permission to write to '$PREFIX'"
fi

# Issue a warning if $PREFIX is not in the user's $PATH to prevent confusion.
# TODO

# Verify the number of threads that should be used by make.
# TODO 

#
# COMMAND LINE ARGUMENTS
#
if [ "$1" ]; then
	COMMAND="$1"
else
	COMMAND="help"
fi

if [ "$COMMAND" = "install" ]; then
	INSTALL=""

	# Parse the architecture flags.
	ARCH_PIC=0
	ARCH_CORTEX=0
	ARCH_ARM9=0

	for ARCH in "${@:2}"; do
		if [ "$ARCH" = "pic" ]; then
			ARCH_PIC=1
		elif [ "$ARCH" = "cortex" ]; then
			ARCH_CORTEX=1
		elif [ "$ARCH" = "arm9" ]; then
			ARCH_ARM9=1
		else
			err "unsupported architecture '$ARCH'"
		fi
	done

	# PIC Dependencies
	if [ $ARCH_PIC -ne 0 ]; then
		download "$DIR_DOWNLOAD" "sdcc"
	fi

	# Cortex Dependnecies
	if [ $ARCH_CORTEX -ne 0 ]; then
		download "$DIR_DOWNLOAD" "m4" "gmp" "mpfr" "binutils" "gcc"
	fi

	# ARM9 Dependencies
	if [ $ARCH_ARM9 -ne 0 ]; then
		war "skipping ARM9 architecture; currently unsupported"
	fi
else
	error "unsupported command '$COMMAND'"
fi

