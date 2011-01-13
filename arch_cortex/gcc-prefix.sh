#!/bin/bash

if type "arm-none-eabi-gcc" 1&>2 > "/dev/null"; then
	echo "arm-none-eabi-"
	exit 0
elif type "arm-elf-gcc" 1&>2 > "dev/null"; then
	echo "arm-elf-"
	exit 0
else
	exit 1
fi
