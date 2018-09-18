#! /usr/bin/env bash
set -e
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CTX_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

BASE_IMG=centos

docker run --rm -ti\
    -v $CTX_DIR:/usr/local/clarabridge/current\
    -u $(id -u):$(id -g)\
    cb-ltt-$BASE_IMG:latest\
    bash
