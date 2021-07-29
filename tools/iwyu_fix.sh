#!/bin/bash -xe

iwyu_tool.py -p . -- -Xiwyu --mapping_file=$PWD/tools/iwyu.imp | fix_includes.py
