#!/bin/bash -xe

docker run -it --rm -v $PWD:$PWD --workdir $PWD  javiervarez/ate_builder:main $@
