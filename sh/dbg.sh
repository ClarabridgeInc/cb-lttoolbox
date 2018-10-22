#! /usr/bin/env bash
gdb -ix gdb/stl-views-1.03.txt -x gdb/gdb.br --args ltlib/build/exe/lt/lt ben/ben.automorf.bin data/in2.txt
