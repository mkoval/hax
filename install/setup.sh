#! /bin/bash

DIR_BASE="`pwd`/hax_install"
DIR_LOGS="$DIR_BASE/logs"
DIR_CHECKSUM="$DIR_BASE/checksums"
DIR_DOWNLOAD="$DIR_BASE/downloads"
DIR_BUILDING="$DIR_BASE/build"

NAME_SDCC="sdcc-extra-src-20100516-5824"
NAME_M4="m4-1.4.14"
NAME_MPFR="mpfr-2.4.2"
NAME_GMP="gmp-4.3.2"
NAME_BINUTILS="binutils-2.20.1"
NAME_GCC="gcc-4.4.4"
NAME_NEWLIB="newlib-1.18.0"

URL_SDCC="http://sdcc.sourceforge.net/snapshots/sdcc-extra-src/$NAME_SDCC.tar.bz2"
URL_MPFR="http://www.mpfr.org/mpfr-current/$NAME_MPFR.tar.gz"
URL_M4="http://ftp.gnu.org/gnu/m4/$NAME_M4.tar.gz"
URL_GMP="ftp://ftp.gmplib.org/pub/gmp-4.3.2/$NAME_GMP.tar.bz2"
URL_BINUTILS="ftp://ftp.gnu.org/gnu/binutils/$NAME_BINUTILS.tar.gz"
URL_GCC="ftp://ftp.gnu.org/gnu/gcc/$NAME_GCC/$NAME_GCC.tar.gz"
URL_NEWLIB="ftp://sources.redhat.com/pub/newlib/$NAME_NEWLIB.tar.gz"

MD5_SDCC="8db303a896d6d046fb5cb108fcc025dc  $NAME_SDCC.tar.bz2"
MD5_M4="f0542d58f94c7d0ce0d01224e447be66  $NAME_M4.tar.gz"
MD5_MPFR="0e3dcf9fe2b6656ed417c89aa9159428  $NAME_MPFR.tar.gz"
MD5_GMP="dd60683d7057917e34630b4a787932e8  $NAME_GMP.tar.bz2"
MD5_BINUTILS="eccf0f9bc62864b29329e3302c88a228  $NAME_BINUTILS.tar.gz"
MD5_GCC="04b7b74df06b919bc36b8eb462dfef7a  $NAME_GCC.tar.gz"
MD5_NEWLIB="3dae127d4aa659d72f8ea8c0ff2a7a20  $NAME_NEWLIB.tar.gz"

source "helper.sh"

# Build the necessary directory structure.
mkdir_safe $DIR_BASE
mkdir_safe $DIR_LOGS
mkdir_safe $DIR_CHECKSUM
mkdir_safe $DIR_DOWNLOAD
mkdir_safe $DIR_BUILDING

# Populate the checksums directory.
echo "$MD5_SDCC"     > "$DIR_CHECKSUM/$NAME_SDCC.md5"
echo "$MD5_M4"       > "$DIR_CHECKSUM/$NAME_M4.md5"
echo "$MD5_MPFR"     > "$DIR_CHECKSUM/$NAME_MPFR.md5"
echo "$MD5_GMP"      > "$DIR_CHECKSUM/$NAME_GMP.md5"
echo "$MD5_BINUTILS" > "$DIR_CHECKSUM/$NAME_BINUTILS.md5"
echo "$MD5_GCC"      > "$DIR_CHECKSUM/$NAME_GCC.md5"
echo "$MD5_NEWLIB"   > "$DIR_CHECKSUM/$NAME_NEWLIB.md5"

# Default values for environmental variables.
if [[ ! "$PREFIX" ]]; then
	PREFIX="$HOME/hax"
fi

if [[ ! "$PARALLEL" ]]; then
	PARALLEL=5
fi

# Use the correct permissions on "make install"
mkdir -p "$PREFIX" &> /dev/null
if [[ ! -w "$PREFIX" ]]; then
	error "insufficient permissions to install to '$PREFIX'"
	exit 1
fi

# Set the supported architecture based upon the command-line arguments.
unset EN_PIC
unset EN_ARM
unset EN_HAX

if [[ ! $1 ]]; then
	EN_PIC=1
	EN_ARM=1
	EN_HAX=1
fi

for CMD in $*; do
	if [[ "$CMD" = "pic" ]]; then
		EN_PIC=1
	elif [[ "$CMD" = "arm" ]]; then
		EN_ARM=1
	elif [[ "$CMD" = "hax" ]]; then
		EN_HAX=1
	else
		error "unexpected commandline argument '$CMD'"
		exit 1
	fi
done

#
# Add $PREFIX to the user's $PATH environmental variable.
#
if [[ ! "`echo "$PATH" | grep "$PREFIX/bin"`" ]]; then
	header "Pre-Install" "Updating PATH"
	echo "export PATH=\"\$PATH:$PREFIX/bin\"" >> "$HOME/.bashrc"
fi

#
# Add $PREFIX to the user's $HAX_PATH environment variable.
#
if [[ ! "$HAX_PATH" ]]; then
	echo "export HAX_PATH=\"$PREFIX/hax\""    >> "$HOME/.bashrc"
fi
source "$HOME/.bashrc"

echo "$PATH" | grep "$PREFIX/bin" > /dev/null
if_error $? "manually add \'$PREFIX/bin\' to your PATH to continue"
#
# PIC TOOLCHAIN
#
if [[ $EN_PIC -eq 1 ]]; then
	# SDCC
	cd "$DIR_BASE"

	header "PIC" "SDCC" "Downloading"
	download "$URL_SDCC" "$NAME_SDCC" "tar.bz2"

	header "PIC" "SDCC" "Extracting"
	extract "$DIR_DOWNLOAD/$NAME_SDCC.tar.bz2" "$DIR_BUILDING"

	header "PIC" "SDCC" "Configuring"
	mkdir_safe "$DIR_BUILDING/sdcc-extra/build"
	cd "$DIR_BUILDING/sdcc-extra/build"
	../configure --prefix="$PREFIX" \
	             &> "$DIR_LOGS/${NAME_SDCC}_configure.log"
	if_error $? "unable to configure SDCC"

	header "PIC" "SDCC" "Compiling"
	make -j$PARALLEL &> "$DIR_LOGS/${NAME_SDCC}_compile.log"
	if_error $? "unable to build SDCC"

	header "PIC" "SDCC" "Installing"
	make install  &> "$DIR_LOGS/${NAME_SDCC}_install.log"
	if_error $? "unable to install SDCC"

	header "PIC" "SDCC" "Verifying Installation"
	"$PREFIX/bin/sdcc" -v | grep "SDCC" > /dev/null
	if_error $@ "\"sdcc\" failed verification"

	header "PIC" "SDCC" "Updating PATH"
	echo "$PATH" | grep "$PREFIX/bin" > /dev/null
	if [ $? ]; then
		echo "export PATH=\"\$PATH:$PREFIX/bin\"" >> ~/.bashrc
		source ~/.bashrc
	fi
	echo "$PATH" | grep "$PREFIX/bin" > /dev/null
	if_error $? "manually add \'$PREFIX/bin\' to your PATH to continue"

	header "PIC" "SDCC" "Cleaning Up"
	rm -rf "$DIR_DOWNLOAD/$NAME_SDCC.tar.bz2" \
		   "$DIR_BUILDING/sdcc-extra"
	
	header "PIC" "DONE"
fi

#
# ARM TOOLCHAIN
#
if [[ $EN_ARM -eq 1 ]]; then
	# M4 (dependency of GMP)
	if [[ ! "`which m4`" ]]; then
		cd "$DIR_BASE"

		header "Dependency" "M4" "Downloading"
		download "$URL_M4" "$NAME_M4" "tar.gz"

		header "Dependency" "M4" "Extracting"
		extract "$DIR_DOWNLOAD/$NAME_M4.tar.gz" "$DIR_BUILDING"

		header "Dependency" "M4" "Configuring"
		mkdir_safe "$DIR_BUILDING/$NAME_M4/build"
		cd "$DIR_BUILDING/$NAME_M4/build"
		../configure --prefix="$PREFIX" --disable-werror \
			     &> "$DIR_LOGS/${NAME_M4}_configure.log"
		if_error $? "unable to configure M4"

		header "Dependency" "M4" "Compiling"
		make -j$PARALLEL &> "$DIR_LOGS/${NAME_M4}_compile.log"
		if_error $? "unable to compile M4"

		header "Dependency" "M4" "Installing"
		make install &> "$DIR_LOGS/${NAME_M4}_install.log"
	fi
	
	# GMP (dependency of Binutils)
	cd "$DIR_BASE"

	header "Dependency" "GMP" "Downloading"
	download "$URL_GMP" "$NAME_GMP" "tar.bz2"

	header "Dependency" "GMP" "Extracting"
	extract "$DIR_DOWNLOAD/$NAME_GMP.tar.bz2" "$DIR_BUILDING"

	# MPFR (dependency of GCC)
	cd "$DIR_BASE"

	header "Dependency" "MPFR" "Downloading"
	download "$URL_MPFR" "$NAME_MPFR" "tar.gz"

	header "Dependency" "MPFR" "Extracting"
	extract "$DIR_DOWNLOAD/$NAME_MPFR.tar.gz" "$DIR_BUILDING"

	# Binutils
	cd "$DIR_BASE"

	header "ARM" "Binutils" "Downloading"
	download "$URL_BINUTILS" "$NAME_BINUTILS" "tar.gz"

	header "ARM" "Binutils" "Extracting"
	extract "$DIR_DOWNLOAD/$NAME_BINUTILS.tar.gz" "$DIR_BUILDING"

	header "ARM" "Binutils" "Configuring"
	mkdir_safe "$DIR_BUILDING/$NAME_BINUTILS/build"
	cd "$DIR_BUILDING/$NAME_BINUTILS/build"
	../configure --prefix="$PREFIX" --target=arm-none-eabi --disable-werror   \
	             &> "$DIR_LOGS/${NAME_BINUTILS}_configure.log"
	if_error $? "unable to configure Binutils"

	header "ARM" "Binutils" "Compiling"
	make -j$PARALLEL &> "$DIR_LOGS/${NAME_BINUTILS}_compile.log"
	if_error $? "unable to build Binutils"

	header "ARM" "Binutils" "Installing"
	make install &> "$DIR_LOGS/${NAME_BINUTILS}_install.log"
	if_error $? "unable to install Binutils"

	# GCC
	cd "$DIR_BASE"

	header "ARM" "GCC" "Downloading"
	download "$URL_GCC" "$NAME_GCC" "tar.gz"

	header "ARM" "GCC" "Extracting"
	extract "$DIR_DOWNLOAD/$NAME_GCC.tar.gz" "$DIR_BUILDING"

	header "ARM" "GCC" "Copying GMP"
	cp -r "$DIR_BUILDING/$NAME_GMP" "$DIR_BUILDING/$NAME_GCC/gmp"

	header "ARM" "GCC" "Copying MPFR"
	cp -r "$DIR_BUILDING/$NAME_MPFR" "$DIR_BUILDING/$NAME_GCC/mpfr"

	header "ARM" "GCC" "Configuring"
	mkdir_safe "$DIR_BUILDING/$NAME_GCC/build"
	cd "$DIR_BUILDING/$NAME_GCC/build"
	../configure --prefix="$PREFIX" --target=arm-none-eabi --without-headers  \
	             --with-newlib --disable-shared --enable-languages="c"        \
	             --disable-werror                                             \
	             &> "$DIR_LOGS/${NAME_GCC}_configure.log"
	if_error $? "unable to configure GCC"

	header "ARM" "GCC" "Compiling"
	make -j$PARALLEL all-gcc &> "$DIR_LOGS/${NAME_GCC}_compile.log"
	if_error $? "unable to build GCC"

	header "ARM" "GCC" "Installing"
	make install-gcc &> "$DIR_LOGS/${NAME_GCC}_install.log"
	if_error $? "unable to install GCC"

	# Newlib
	cd "$DIR_BASE"

	header "ARM" "Newlib" "Downloading"
	download "$URL_NEWLIB" "$NAME_NEWLIB" "tar.gz"

	header "ARM" "Newlib" "Extracting"
	extract "$DIR_DOWNLOAD/$NAME_NEWLIB.tar.gz" "$DIR_BUILDING"

	header "ARM" "Newlib" "Configuring"
	mkdir_safe "$DIR_BUILDING/$NAME_NEWLIB/build"
	cd "$DIR_BUILDING/$NAME_NEWLIB/build"
	../configure --prefix="$PREFIX" --target=arm-none-eabi --disable-werror   \
	             &> "$DIR_LOGS/${NAME_NEWLIB}_configure.log"
	if_error $? "unable to configure Newlib"

	header "ARM" "Newlib" "Compiling"
	make -j$PARALLEL &> "$DIR_LOGS/${NAME_NEWLIB}_compile.log"
	if_error $? "unable to build Newlib"

	header "ARM" "Newlib" "Installing"
	make install &> "$DIR_LOGS/${NAME_NEWLIB}_install.log"
	if_error $? "unable to install Newlib"

	# GCC (Bootstrap)
	cd "$DIR_BASE"

	header "ARM" "GCC (Bootstrap)" "Configuring"
	mkdir_safe "$DIR_BUILDING/$NAME_GCC/bootstrap"
	cd "$DIR_BUILDING/$NAME_GCC/bootstrap"
	../configure --prefix="$PREFIX" --target=arm-none-eabi --with-newlib      \
	             --disable-shared --enable-languages="c" --disable-werror     \
                 --with-headers --with-libs                                   \
	             &> "$DIR_LOGS/${NAME_GCC}_bs_configure.log"
	if_error $? "unable to boostrap GCC"

	header "ARM" "GCC (Bootstrap)" "Compiling"
	make -j$PARALLEL &> "$DIR_LOGS/${NAME_GCC}_bs_compile.log"
	if_error $? "unable to bootstrap GCC"

	header "ARM" "GCC (Bootstrap)" "Installing"
	make install &> "$DIR_LOGS/${NAME_GCC}_bs_install.log"
	if_error $? "unable to install bootstrapped GCC"

	# Test GCC's Installation
	header "ARM" "Verifying Installation"
	"$PREFIX/bin/arm-none-eabi-gcc" -v | grep "^Target: arm-none-eabi$"       \
	                                > /dev/null
	if_error $? "\"arm-none-eabi-gcc\" failed verification"

	# Remove temporary files
	header "ARM" "Cleaning Up"
	rm -rf "$DIR_DOWNLOAD/$NAME_BINUTILS.tar.gz" \
		   "$DIR_DOWNLOAD/$NAME_GCC.tar.gz"      \
		   "$DIR_DOWNLOAD/$NAME_NEWLIB.tar.gz"   \
		   "$DIR_BUILDING/$NAME_BINUTILS"        \
		   "$DIR_BUILDING/$NAME_GCC"             \
		   "$DIR_BUILDING/$NAME_NEWLIB"

	header "ARM" "DONE"
fi

#
# HAX
#
if [[ $EN_HAX -eq 1 ]]; then
	cd "$DIR_BASE"

	# Build the correct directory structure
	header "HAX" "Building Directory Structure"
	mkdir_safe "$PREFIX"
	if_error $? "unable to make directory '$PREFIX'"
	mkdir_safe "$PREFIX/hax"
	if_error $? "unable to make directory '$PREFIX/hax'"

	# Copy the Hax installation into the desired directory.
	cp -r "$DIR_BASE/arch_cortex" \
	      "$DIR_BASE/arch_pic"    \
	      "$DIR_BASE/skel"        \
	      "$DIR_BASE/test"        \
	      "$DIR_BASE/hax.h"       \
	      "$DIR_BASE/compilers.h" \
	      "$DIR_BASE/hax_main.c"  \
          "$PREFIX/hax"
	
	header "HAX" "DONE"
fi
