#!/bin/bash -xe

SCRIPT_DIR=`dirname "$0"`

pushd ${SCRIPT_DIR}/../build

iwyu_tool.py -p . -- -Xiwyu --mapping_file=../tools/iwyu.imp | tee iwyu_report.txt

popd
