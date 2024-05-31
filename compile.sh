#!/bin/bash

# This is the main compilation file for Mamba
#
# Here select which version of the assembly backend you want
# to use, old or new version.
#
# To use the old compilation pipeline and all the switches
# use
#   ./compile-mamba.py

shift $[OPTIND-1]

ARGS=("$@")
# Get the last argument
target=${ARGS[-1]}
# Drop it from the array
unset ARGS[${#ARGS[@]}-1]

./compile-new.sh "${ARGS[@]}" $target || exit 1
#./compile-old.sh $target || exit 1

make clean
make progs -j

# Check if -F is present in the arguments
if [[ " ${ARGS[@]} " =~ " -F " ]]; then
    sed -i "s/^#define ignore_memory_sizes .*/#define ignore_memory_sizes 0/g" ./src/config.h
fi

