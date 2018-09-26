#! /usr/bin/env bash
GREP_STR=GetTypeName
# fixed_address_empty_string
# GetTypeName
objdump -TC ltlib/build/libs/jniLttLibrary/shared/libjniLttLibrary.so | grep $GREP_STR
