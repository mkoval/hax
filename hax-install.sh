#! /bin/bash

DIR_BASE="`pwd`/hax_install"
DIR_DOWNLOAD="$DIR_BASE/download"
DIR_EXTRACT="$DIR_BASE/extract"
DIR_COMPILE="$DIR_BASE/compile"

# Issue a warning if this is not a Bash shell to explain potential errors.
if [ ! `echo "$SHELL" | grep "bash"` ]; then
	war "untested shell detected, use bash for full compatability"
fi

# Prints an error message to stderr and exits the program.
function err {
	echo "error: $1" 1>&2
	exit 1
}

function if_err {
	if [ "$1" -ne 0 ]; then
		err "$2"
	fi
}

# Prints a warning message to stderr and continues execution.
function war {
	echo "warning: $1" 1>&2
}

# Checks if the argument is a valid command.
function has {
	command -v "$1"
}

# Gets a variable added to an associative array with assoc_set().
function assoc_get () {
	VARNAME="__assoc_${1}_${2}"
	printf "%s" "${!VARNAME}"
}

# Adds a value to an associative array, to be read with assoc_get().
function assoc_set {
	eval "__assoc_${1}_${2}='${3}'"
}

# Verify a download using its MD5 checksum.
function verify {
	if [ -e "$DIR_DOWNLOAD/$1" ]; then
		MD5=`assoc_get "md5" "$1"`

		if [ "`has "md5"`" ]; then
			md5 "$DIR_DOWNLOAD/$1" | grep "$MD5" &> "/dev/null"
			if [ $? -ne 0 ]; then
				return 1
			fi
		elif [ "`has "md5sum"`" ]; then
			md5sum "$DIR_DOWNLOAD/$1" | grep "$MD5" &> "/dev/null"
			if [ $? -ne 0 ]; then
				return 1
			fi
		else
			war "missing 'md5' and 'md5sum'; not verifying checksum"
		fi
	fi
	return 0
}

# Downloads a list of URLs to a target directory.
function download {
	for NAME in $@; do
		URL=`assoc_get "url" "$NAME"`

		# Skip dependencies that are already installed.
		if [ "`assoc_get "dep" "$NAME"`" ]; then
			echo "$NAME - Already Installed"
			continue
		fi

		# Verify the checksum of an already-present file.
		if [ -e "$DIR_DOWNLOAD/$NAME" ]; then
			verify "$NAME"

			# Redownload if the old file fails the checksum.
			if [ $? -ne 0 ]; then
				war "redownloading '$NAME' due to failed checksum"
				rm -f "$DIR_DOWNLOAD/$NAME"
			else
				echo "$NAME - Already Downloaded"
				continue
			fi
		fi

		echo "$NAME - Downloading"

		# Use curl or wget to download the URL to the target directory.
		if [ "`has 'curl'`" ]; then
			curl -# -o "$DIR_DOWNLOAD/$NAME" -- "$URL"
			if_err $? "unable to download $NAME"
		elif [ "`has 'wget'`" ]; then
			wget -o "$DIR_DOWNLOAD/$NAME" -- "$URL" &> "/dev/null"
			if_err $? "unable to download $NAME"
		else
			error "missing 'wget' and 'curl'"
		fi

		verify "$NAME"
		if_err $? "'$NAME' failed checksum"
	done
}

# Extract an already-downloaded (potentially compressed) tar archive.
function extract {
	for NAME in $@; do
		echo "$NAME - Extracting"
		tar -C "$DIR_EXTRACT" -xzf "$DIR_DOWNLOAD/$NAME" &> "/dev/null"
		if_err $? "unable to extract $NAME"
	done
}

# Configure (./configure), make, and install an extracted dependency.
function build {
	DIR_RESTORE="`pwd`"
	NAME="`assoc_get "ext" "$1"`"
	FLAGS="`assoc_get "con" "$1"`"

	# Clean any previous build attempts.
	rm -rf   "$DIR_COMPILE/$NAME"
	mkdir -p "$DIR_COMPILE/$NAME"
	cd       "$DIR_COMPILE/$NAME"

	echo "$1 - Configuring"
	"$DIR_EXTRACT/$NAME/configure" "$FLAGS" &> "/dev/null"
	if_err $? "unable to configure '$1'"

	echo "$1 - Compiling"
	make -j$PARALLEL $2 &> "/dev/null"
	if_err $? "unable to compile '$1'"

	echo "$1 - Installing"
	make $3 &> "/dev/null"
	if_err $? "unable to install '$1'"
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

# Verify permissions on the installation directory.
if [ -z "$PREFIX" ]; then
	PREFIX="/usr/local"
elif [ ! -w "$PREFIX" ]; then
	err "insufficient permission to write to '$PREFIX'"
fi

# Verify permissions of the installation directory.
mkdir -p "$DIR_BASE"     &> "/dev/null"
mkdir -p "$DIR_DOWNLOAD" &> "/dev/null"
mkdir -p "$DIR_EXTRACT"  &> "/dev/null"
mkdir -p "$DIR_COMPILE"  &> "/dev/null"
if [ ! -w "$DIR_BASE" ]; then
	err "insufficient permissions to write to '$DIR_BASE'"
fi

# Issue a warning if $PREFIX is not in the user's $PATH to prevent confusion.
# TODO

# Verify the number of threads that should be used by make.
# TODO 

#
# CONFIG VARIABLES
#
# Direct download URLs for all dependencies.
assoc_set "url" "sdcc"     "http://sdcc.sourceforge.net/snapshots/sdcc-extra-src/sdcc-extra-src-20100516-5824.tar.bz2"
assoc_set "url" "m4"       "http://ftp.gnu.org/gnu/m4/m4-1.4.14.tar.bz2"
assoc_set "url" "gmp"      "ftp://ftp.gmplib.org/pub/gmp-4.3.2/gmp-4.3.2.tar.bz2"
assoc_set "url" "mpfr"     "http://www.mpfr.org/mpfr-current/mpfr-2.4.2.tar.gz"
assoc_set "url" "binutils" "ftp://ftp.gnu.org/gnu/binutils/binutils-2.20.1.tar.gz"
assoc_set "url" "gcc"      "ftp://ftp.gnu.org/gnu/gcc/gcc-4.4.4/gcc-4.4.4.tar.gz"
assoc_set "url" "newlib"   "ftp://sources.redhat.com/pub/newlib/newlib-1.18.0.tar.gz"

# MD5 checksums for the above-listed downloads.
assoc_set "md5" "sdcc"     "8db303a896d6d046fb5cb108fcc025dc"
assoc_set "md5" "m4"       "e6fb7d08d50d87e796069cff12a52a93"
assoc_set "md5" "gmp"      "dd60683d7057917e34630b4a787932e8"
assoc_set "md5" "mpfr"     "0e3dcf9fe2b6656ed417c89aa9159428"
assoc_set "md5" "binutils" "eccf0f9bc62864b29329e3302c88a228"
assoc_set "md5" "gcc"      "04b7b74df06b919bc36b8eb462dfef7a"
assoc_set "md5" "newlib"   "3dae127d4aa659d72f8ea8c0ff2a7a20"

# Name of the folder extracted from the downloaded tarball.
# TODO: name for SDCC
assoc_set "ext" "m4"       "m4-1.4.14"
assoc_set "ext" "gmp"      "gmp-4.3.2"
assoc_set "ext" "mpfr"     "mpfr-2.4.2"
assoc_set "ext" "binutils" "binutils-2.20.1"
assoc_set "ext" "gcc"      "gcc-4.4.4"
assoc_set "ext" "newlib"   "newlib-1.18.0"

# Attempt to detect previous installations of each dependency.
assoc_set "dep" "sdcc"     "`has 'sdcc'`"
assoc_set "dep" "m4"       "`has 'm4'`"
assoc_set "dep" "gmp"      ""
assoc_set "dep" "mpfr"     ""
assoc_set "dep" "binutils" "`has 'arm-none-eabi-objcopy'`"
assoc_set "dep" "gcc"      "`has 'arm-none-eabi-gcc'`"
assoc_set "dep" "newlib"   ""

# Configuration (./configure) flags for each dependency.
assoc_set "con" "m4"       "--prefix='$PREFIX'"
assoc_set "con" "m4"       "--prefix='$PREFIX' --disable-werror"
assoc_set "con" "gmp"      "--prefix='$PREFIX' --disable-werror"
assoc_set "con" "mpfr"     "--prefix='$PREFIX' --disable-werror"
assoc_set "con" "binutils" "--prefix='$PREFIX' --target='arm-none-eabi' --disable-werror"
assoc_set "con" "gcc"      "--prefix='$PREFIX' --target='arm-none-eabi' --disable-werror --with-newlib --enable-languages='c'"
assoc_set "con" "newlib"   "--prefix='$PREFIX' --target='arm-none-eabi'"

#
# COMMAND LINE ARGUMENTS
#
if [ "$1" ]; then
	COMMAND="$1"
else
	COMMAND="help"
fi

if [ "$COMMAND" = "install" ]; then
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
		download "sdcc"
		extract "$DIR_DOWNLOAD" "$DIR_BUILD" "sdcc"
	fi

	# Cortex Dependencies
	if [ $ARCH_CORTEX -ne 0 ]; then
		# Skip if the arm-none-eabi toolchain is installed.
		if [ ! "`assoc_get "dep" "gcc"`" ]; then
			download "m4" "gmp" "mpfr" "binutils" "newlib" "gcc"

			# M4
			if [ ! "`assoc_get "dep" "m4"`" ]; then
				extract "m4"
				build "m4" "all" "install"
			fi

			# Binutils
			if [ ! "`assoc_get "dep" "binutils"`" ]; then
				extract "binutils"
				build "binutils" "all" "install"
			fi

			# GCC (Stage 1), Newlib, and GCC (Stage 2)
			if [ ! "`assoc_get "dep" "gcc"`" ]; then
				NAME_GCC="`assoc_get "ext" "gcc"`"
				NAME_GMP="`assoc_get "ext" "gmp"`"
				NAME_MPFR="`assoc_get "ext" "mpfr"`"

				# GCC Bootstrap (Stage 1)
				extract "gcc" "gmp" "mpfr"
				cp -r "$DIR_EXTRACT/$NAME_GMP"  "$DIR_EXTRACT/$NAME_GCC/gmp"
				cp -r "$DIR_EXTRACT/$NAME_MPFR" "$DIR_EXTRACT/$NAME_GCC/mpfr"
				build "gcc" "all-gcc" "install-gcc"

				# Newlib
				extract "newlib"
				build "newlib" "all" "install"

				# GCC with Newlib (Stage 2)
				build "gcc" "all" "install"
			fi
		fi
	fi

	# ARM9 Dependencies
	if [ $ARCH_ARM9 -ne 0 ]; then
		war "skipping ARM9 architecture; currently unsupported"
	fi
elif [ "$COMMAND" = "help" ]; then
	cat << HELP
usage: ./hax-install.sh COMMAND [ARCH1 [ARCH2 [...]]]
COMMANDS
    install  Install dependencies for the listed archs.
    info     Print a list of dependencies required by the listed archs.
    help     Prints this help text.

ARCHS
    pic      Original PIC Microcontroller v0.5 (using SDCC and Rigel).
    cortex   VexNET-enabled Cortex Microcontroller (using GCC and stm32loader).
    arm9     Currently unreleased ARM9-based Microcontroller. *UNSUPPORTED*

EXAMPLES
    sudo ./hax-install.sh install pic cortex
        Install all PIC and Cortex dependencies to '/usr/local'.

    export PARALLEL=9
    sudo ./hax-install.sh install pic cortex
        Speed up build times on quad-core processors by using more threads.

    export PREFIX="\$HOME/vex"
    ./hax-install.sh install cortex
        Install the Cortex dependencies to '\$HOME/vex'.

HELP
else
	error "unsupported command '$COMMAND'"
fi

