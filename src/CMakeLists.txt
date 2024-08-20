# Copyright (c) 2023, 2024 Cryolite
# SPDX-License-Identifier: MIT
# This file is part of https://github.com/Cryolite/tsumonya

add_library(src.common
  reference_calculator.cpp)
target_link_libraries(src.common
  PRIVATE Boost::python
  PRIVATE Boost::headers
  PRIVATE "python${PYTHON_VERSION}")

add_subdirectory(standard)
add_subdirectory(seven_pairs)
