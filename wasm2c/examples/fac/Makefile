# Use implicit rules for compiling C files.
CFLAGS=-I../..

all: fac

clean:
	rm -rf fac fac.wasm fac.c *.o

fac: main.c fac.c ../../wasm-rt-impl.c -lm

fac.wasm: fac.wat ../../../bin/wat2wasm
	../../../bin/wat2wasm $< -o $@

fac.c: fac.wasm ../../../bin/wasm2c
	../../../bin/wasm2c $< -o $@

.PHONY: all clean
