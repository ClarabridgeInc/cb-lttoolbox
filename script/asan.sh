#! /usr/bin/env bash
#ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer
ASAN_OPTIONS=symbolize=1 build/exe/lt/lt ben/ben.automorf.bin data/in.txt "$@"
