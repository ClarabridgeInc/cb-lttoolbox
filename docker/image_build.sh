#! /usr/bin/env bash
set -e
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CTX_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

BASE_IMG=centos

docker build -t cb-ltt-$BASE_IMG:latest -f $CUR_DIR/Dockerfile-$BASE_IMG $CTX_DIR
