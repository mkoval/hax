# Prints a decorative header with the specified title and subtitle.
header () {
	if [ "$1" ]; then
		echo -n "$1"
		
		if [ "$2" ]; then
			echo -n " :: $2"
			
			if [ "$3" ]; then
				echo -n " - $3"
			fi
		fi
	fi
	echo ""
}

error () {
	echo "err: $1" >&2
}

if_error () {
	if [ $1 -ne 0 ]; then
		error "$2"
		exit $1
	fi
}

# Wrapper for mkdir that does not print an error message if a direcotry already
# exists.
mkdir_safe () {
	if [ ! -d $* ]; then
		mkdir $*

		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi
}

download () {
	# Only download the file if it does not already exist.
	if [ ! -e "$DIR_DOWNLOAD/$2.$3" ]; then
		curl -# -o "$DIR_DOWNLOAD/$2.$3" "$1"
	fi

	# Check the downloaded file's MD5 checksum.
	DIR_RESTORE=`pwd`
	cd "$DIR_DOWNLOAD"

	md5sum -c "$DIR_CHECKSUM/$2.md5" > /dev/null
	if_error $? "download of \"$2.$3\" failed checksum"

	cd "$DIR_RESTORE"
}

extract () {
	if [ ! -f $1 ]; then
		error "unable to extract tarball"
		exit 1
	fi

	DIR_RESTORE=`pwd`
	cd "$2"
	tar xzpf $1
	cd "$DIR_RESTORE"
}
