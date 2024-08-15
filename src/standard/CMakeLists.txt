# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_executable(build_table
  build_table.cpp
  common.cpp)
add_custom_command(
  OUTPUT subtable.hpp table.hpp
  COMMAND ASAN_OPTIONS=handle_abort=1 ./build_table subtable.hpp table.hpp
  COMMAND cp subtable.hpp "${CMAKE_SOURCE_DIR}/src/standard/subtable.hpp"
  COMMAND cp table.hpp "${CMAKE_SOURCE_DIR}/tsumonya/standard/table.hpp"
  DEPENDS build_table)

add_library(src_standard
  sample_winning_hand.cpp
  winning_hand_enumeration.cpp
  common.cpp
  subtable.hpp)

add_executable(build_map
  build_map.cpp
  table.hpp)
target_link_libraries(build_map
  PRIVATE src_standard
  PRIVATE Boost::python
  PRIVATE Boost::timer
  PRIVATE Boost::headers
  PRIVATE python"${PYTHON_VERSION}")
#add_custom_target(
#  src_standard_build_map_run ALL
#  ASAN_OPTIONS=handle_abort=1 ./build_map "${CMAKE_SOURCE_DIR}/src/standard/map.bin"
#  DEPENDS build_map)

add_executable(build_duplicate_map
  build_duplicate_map.cpp)
target_link_libraries(build_duplicate_map
  PRIVATE src_standard
  PRIVATE Boost::python
  PRIVATE Boost::timer
  PRIVATE Boost::headers
  PRIVATE python"${PYTHON_VERSION}")
add_custom_command(
  OUTPUT duplicate_map.bin
  COMMAND ASAN_OPTIONS=handle_abort=1 ./build_duplicate_map duplicate_map.bin
  COMMAND cp duplicate_map.bin "${CMAKE_SOURCE_DIR}/src/standard/duplicate_map.bin"
  DEPENDS build_duplicate_map)
