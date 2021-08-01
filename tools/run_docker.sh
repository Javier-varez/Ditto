#!/bin/bash -xe

SCRIPT_DIR=`dirname "$0"`

pushd ${SCRIPT_DIR}/..

docker run -it --rm -v $PWD:$PWD --workdir $PWD  javiervarez/ate_builder:main $@

popd
