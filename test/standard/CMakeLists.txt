# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_executable(hash
  hash.cpp)
add_dependencies(hash
  src_standard_build_table_run)
target_link_libraries(hash
  src_standard_winning_hand_enumeration)

add_custom_target(
  test_standard_hash_run ALL ASAN_OPTIONS=handle_abort=1 ./hash
  DEPENDS hash)
