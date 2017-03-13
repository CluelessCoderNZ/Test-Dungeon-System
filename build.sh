#!/bin/bash


COMPILER="g++"
PROGRAM_FILENAME="program.out"
ENTRY_POINT="main.cpp"
COMMON_INCLUDE_DIR=""
COMMON_LINKER_DIR="-L/usr/local/lib"
COMMON_LINKER_FILES="-lsfml-graphics -lsfml-window -lsfml-system -lsfml-network"
COMMON_OPTIONS="-std=c++11 -DDEBUG_TOGGLE -Wreturn-type"

BUILD_AND_RUN=0

while getopts "Rpfdo:" opt; do
    case "$opt" in
    R)
	BUILD_AND_RUN=1
	;;
    p)
	COMMON_OPTIONS="$COMMON_OPTIONS -O3"
	;;
    f)
	COMPILER="clang"
	;;
    o)
	PROGRAM_NAME=$OPTARG
	;;
    d)
	COMMON_OPTIONS="$COMMON_OPTIONS -ggdb"
	;;
    esac
done


$COMPILER $ENTRY_POINT $COMMON_INCLUDE_DIR $COMMON_LINKER_DIR $COMMON_LINKER_FILES -o $PROGRAM_FILENAME $COMMON_OPTIONS

if [ $? = 0 ]; then
	if [ $BUILD_AND_RUN = 1 ]; then
		./$PROGRAM_FILENAME
	fi
fi
