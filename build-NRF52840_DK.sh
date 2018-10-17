#!/bin/sh

# toolchain specific settings
PWD="`pwd`"
EXE="`basename ${PWD}`"

#
# Machine dependent
MACHINE=NRF52840_DK

set -x

# Build for mbedOS on the MACHINE using the GCC toolchain
mbed compile -m ${MACHINE} -t GCC_ARM -c -j0 $*
