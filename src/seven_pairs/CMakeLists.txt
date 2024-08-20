# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_executable(src.seven_pairs.build_table
  build_table.cpp)
add_custom_command(
  OUTPUT table.hpp
  COMMAND ASAN_OPTIONS=handle_abort=1 ./src.seven_pairs.build_table table.hpp
  COMMAND cp table.hpp "${CMAKE_SOURCE_DIR}/tsumonya/seven_pairs/table.hpp"
  DEPENDS src.seven_pairs.build_table)
add_custom_target(src.seven_pairs.table DEPENDS table.hpp)

add_executable(src.seven_pairs.build_map
  build_map.cpp)
add_dependencies(src.seven_pairs.build_map src.seven_pairs.table)
target_link_libraries(src.seven_pairs.build_map
  PRIVATE src_standard
  PRIVATE Boost::python
  PRIVATE Boost::timer
  PRIVATE Boost::headers
  PRIVATE python"${PYTHON_VERSION}")
add_custom_command(
  OUTPUT map.bin
  COMMAND ASAN_OPTIONS=handle_abort=1 ./src.seven_pairs.build_map map.bin
  COMMAND cp map.bin "${CMAKE_SOURCE_DIR}/src/seven_pairs/map.bin"
  DEPENDS src.seven_pairs.build_map)
add_custom_target(src.seven_pairs.map DEPENDS map.bin)
