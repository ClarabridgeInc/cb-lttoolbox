#! /usr/bin/env bash
set -e
CTX_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
BUILD_PATH=$CTX_DIR/ltlib/build
JAR_NAME=`find $BUILD_PATH/libs -name "ltlib-*.jar"`
rm -rf ~/.javacpp
java -cp ~/.gradle/caches/modules-2/files-2.1/org.bytedeco/javacpp/1.4.2/cfa6a0259d98bff5aa8d41ba11b4d1dad648fbaa/javacpp-1.4.2.jar:$JAR_NAME ltt.LttLibrary
