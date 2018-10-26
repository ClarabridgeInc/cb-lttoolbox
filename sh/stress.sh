#! /usr/bin/env bash
set -e
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $CUR_DIR/env-vars.sh

#while true; do
    # ps -o pid,pmem,vsz 5559;
#done

curl -G "http://localhost:$LTT_SVC_PORT/analyze"\
    --data-urlencode "text=লিনি"
    # --data-urlencode "text=খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’..!"
