#!/usr/bin/env bash

set -euxo pipefail

g++ -I/usr/include/python3.10 -I. -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=undefined -o build_table build_table.cpp
ASAN_OPTIONS=handle_abort=1 ./build_table

g++ -I/usr/include/python3.10 -I. -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=undefined -lboost_python310-mt-d-x64 -lboost_timer-mt-d-x64 -lpython3.10 -o build_map build_map.cpp
ASAN_OPTIONS=handle_abort=1 ./build_map
