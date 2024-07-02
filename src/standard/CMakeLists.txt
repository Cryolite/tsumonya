# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_executable(build_table
  build_table.cpp)
add_custom_target(
  src_standard_build_table_run ALL
  ASAN_OPTIONS=handle_abort=1 ./build_table "${CMAKE_SOURCE_DIR}/src/standard/subtable.hpp" "${CMAKE_SOURCE_DIR}/tsumonya/standard/table.hpp"
  DEPENDS build_table)

add_library(src_standard_winning_hand_enumeration
  winning_hand_enumeration.cpp)

add_executable(build_map
  build_map.cpp)
add_dependencies(build_map
  src_standard_build_table_run)
target_link_libraries(build_map
  PRIVATE src_standard_winning_hand_enumeration
  PRIVATE Boost::python
  PRIVATE Boost::timer
  PRIVATE Boost::headers
  PRIVATE python"${PYTHON_VERSION}")
add_custom_target(
  src_standard_build_map_run ALL
  ASAN_OPTIONS=handle_abort=1 ./build_map "${CMAKE_SOURCE_DIR}/src/standard/map.bin"
  DEPENDS build_map)
