#!/usr/bin/bash
if [ -z "$1" ]
  then
    echo "Compiles wasm file into standalone binary"
    echo "usage: build.sh foo.wasm"
    echo "outputs: foo.elf"
    exit
fi

ORIGIN_DIR=$(pwd)
SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
output_file=$(basename -- "$1")
output_file="${output_file%%.*}.elf"

set -v

cd $SCRIPT_DIR
make clean

cp ${ORIGIN_DIR}/$1 ./input.wasm
make input.c

make input.elf
cp -f input.elf ${ORIGIN_DIR}/${output_file}
