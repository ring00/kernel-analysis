#!/usr/bin/env bash

# Prepare [wllvm](https://github.com/travitch/whole-program-llvm)
pip install wllvm
export LLVM_COMPILER=clang

# Build liunx
cd linux
make mrproper
make CC=wllvm HOSTCC=wllvm defconfig O=../build
make CC=wllvm HOSTCC=wllvm menuconfig O=../build
make CC=wllvm HOSTCC=wllvm O=../build -j8
cd ..

# Generate whole linux bitcode
extract-bc build/vmlinux.o -o linux.bc

# Generate callgraph.so
cd callgraph && make all && cd ..

# Generate allfunctions.txt
opt -load=callgraph/callgraph.so -callgraph linux.bc

cd analysis && make all && cd ..

# Analysis with allfunctions.txt
opt -load=analysis/analysis.so -analysis linux.bc
