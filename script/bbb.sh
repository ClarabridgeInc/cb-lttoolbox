#! /usr/bin/env bash
g++ -I/data/wrk/clb/cb-lttoolbox/ltlib/src/main/cpp -I/home/iharh/.sdkman/candidates/java/8.0.172-zulu/include -I/home/iharh/.sdkman/candidates/java/8.0.172-zulu/include/linux /data/wrk/clb/cb-lttoolbox/ltlib/build/classes/java/main/ltt/jniLttLibrary.cpp /data/wrk/clb/cb-lttoolbox/ltlib/build/classes/java/main/jnijavacpp.cpp -march=x86-64 -m64 -O3 -s -Wl,-rpath,$ORIGIN/ -Wl,-z,noexecstack -Wl,-Bsymbolic -Wall -fPIC -shared -o libjniLttLibrary.so 
