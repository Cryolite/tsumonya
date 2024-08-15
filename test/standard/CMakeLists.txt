# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_executable(hash
  hash.cpp)
target_link_libraries(hash
  src_standard)
add_custom_command(
  OUTPUT hash.run
  COMMAND ASAN_OPTIONS=handle_abort=1 ./hash
  COMMAND touch hash.run
  DEPENDS hash)

add_executable(sample
  sample.cpp)
target_link_libraries(sample
  PRIVATE src_standard
  PRIVATE Boost::python
  PRIVATE Boost::headers
  PRIVATE "python${PYTHON_VERSION}")
add_custom_command(
  OUTPUT sample.run
  COMMAND ASAN_OPTIONS=handle_abort=1 ./sample
  COMMAND touch sample.run
  DEPENDS sample)
