# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_executable(test.seven_pairs.hash
  hash.cpp)
add_dependencies(test.seven_pairs.hash
  src.seven_pairs.table)
add_custom_command(
  OUTPUT hash.run
  COMMAND ASAN_OPTIONS=handle_abort=1 ./hash
  COMMAND touch hash.run
  DEPENDS test.seven_pairs.hash)

add_executable(test.seven_pairs.test
  test.cpp)
target_link_libraries(test.seven_pairs.test
  PRIVATE src.common)
add_dependencies(test.seven_pairs.test
  src.seven_pairs.table)
