// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include <filesystem>
#include <fstream>
#include <iostream>
#include <array>
#include <stdexcept>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cassert>


namespace{

using Subtable = std::array<std::array<std::uint_fast32_t, 8u>, 34u>;

std::uint_fast32_t countNumSuffixes(std::uint_fast8_t i, std::uint_fast8_t m, Subtable &subtable)
{
  assert((i <= 34u));
  assert((m <= 7u));

  if (i == 34u) {
    return m == 7u ? 1u : 0u;
  }

  if (subtable[i][m] != UINT_FAST32_MAX) {
    return subtable[i][m];
  }

  subtable[i][m] = countNumSuffixes(i + 1u, m, subtable);
  if (m < 7u) {
    subtable[i][m] += countNumSuffixes(i + 1u, m + 1u, subtable);
  }

  return subtable[i][m];
}

} // namespace `anonymous`

int main(int const argc, char const * const * const argv)
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <PATH TO TABLE FILE>" << std::endl;
    return EXIT_FAILURE;
  }

  std::filesystem::path const path(argv[1]);

  std::array<std::array<std::uint_fast32_t, 8u>, 34u> subtable;
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    for (std::uint_fast8_t m = 0u; m <= 7u; ++m) {
      subtable[i][m] = UINT_FAST32_MAX;
    }
  }
  std::uint_fast32_t const size = countNumSuffixes(0u, 0u, subtable);
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    for (std::uint_fast8_t m = 0u; m <= 7u; ++m) {
      if (subtable[i][m] == UINT_FAST32_MAX) {
        subtable[i][m] = 0u;
      }
    }
  }

  std::ofstream ofs(path);
  ofs << "// Copyright (c) 2023, 2024 Cryolite\n";
  ofs << "// SPDX-License-Identifier: MIT\n";
  ofs << "// This file is part of https://github.com/Cryolite/tsumonya\n";
  ofs << '\n';
  ofs << "#if !defined(TSUMONYA_SEVEN_PAIRS_TABLE_HPP_INCLUDE_GUARD)\n";
  ofs << "#define TSUMONYA_SEVEN_PAIRS_TABLE_HPP_INCLUDE_GUARD\n";
  ofs << '\n';
  ofs << "#include <tsumonya/seven_pairs/core.hpp>\n";
  ofs << "#include <cstdint>\n";
  ofs << '\n';
  ofs << '\n';
  ofs << "namespace Tsumonya::SevenPairs_{\n";
  ofs << '\n';
  ofs << "inline constexpr std::uint_fast32_t size = " << size << "u;\n";
  ofs << '\n';
  ofs << "inline constexpr Table table = {{\n";
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    ofs << "  {{";
    for (std::uint_fast8_t m = 0u; m <= 7u; ++m) {
      if (i + 1u < 34u) {
        ofs << subtable[i + 1u][m] << "u,";
      }
      else {
        ofs << (m == 7u ? 1u : 0u) << "u,";
      }
    }
    ofs << "}}, // i = " << static_cast<unsigned>(i) << '\n';
  }
  ofs << "}};\n";
  ofs << '\n';
  ofs << "}\n";
  ofs << '\n';
  ofs << "#endif // !defined(TSUMONYA_SEVEN_PAIRS_TABLE_HPP_INCLUDE_GUARD)" << std::endl;
}
