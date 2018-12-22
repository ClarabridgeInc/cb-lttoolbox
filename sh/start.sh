#! /usr/bin/env bash
set -e
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $CUR_DIR/env-vars.sh
CTX_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

BUILD_PATH=$CTX_DIR/service/build
JAR_NAME=`find $BUILD_PATH/libs -name "service-*.jar"`
JPDA_OPTS="-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=4144"

    # -Dlog4j.debug=true\
    #-Dspring.profiles.active=prod\
    # -Dserver.port=$LTT_SVC_PORT\
    # -Dord.bytedeco.javacpp.maxbytes=1G\

SERVER_PORT=$LTT_SVC_PORT java \
    -Dmanagement.server.port=$LTT_MGMT_PORT\
    -Dserver.max-http-header-size=65536\
    -Dspring.profiles.active=dev\
    $JPDA_OPTS\
    -jar $JAR_NAME
