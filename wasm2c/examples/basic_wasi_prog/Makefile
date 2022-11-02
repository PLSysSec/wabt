# Use implicit rules for compiling C files.
CFLAGS=-I../..

all: basic_wasi_prog
WASI_SDK_ROOT=/opt/bin/wasi-sdk

clean:
	rm -rf basic_wasi_prog basic_wasi_prog.c basic_wasi_prog.h basic_wasi_prog.wasm *.o

basic_wasi_prog: main.c basic_wasi_prog.c ../../wasm-rt-impl.c -lm

basic_wasi_prog.c: basic_wasi_prog.wasm ../../../bin/wasm2c
	../../../bin/wasm2c $< -o $@

basic_wasi_prog.wasm: basic_wasi_prog_original.c
	$(WASI_SDK_ROOT)/bin/clang --sysroot $(WASI_SDK_ROOT)/share/wasi-sysroot $(CFLAGS) $(LDFLAGS) $< -o $@

.PHONY: all clean
