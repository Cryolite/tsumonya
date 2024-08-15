// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "common.hpp"
#include <tsumonya/standard/core.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <array>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <cassert>


namespace{

using Tsumonya::Standard_::stable;
using Tsumonya::Standard_::mtable;
using Tsumonya::Standard_::ntable;
using Tsumonya::Standard_::xytable;
using Tsumonya::Standard_::ptable;
using Tsumonya::Standard_::packState;
using Tsumonya::Standard_::isValidTransition;

using Memo = std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>;

// Returns `T'(i, m, h, w, x, y, a, b)`.
std::uint_fast64_t countNumTails(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const w,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  std::uint_fast8_t const a,
  std::uint_fast8_t const b,
  Memo &memo)
{
  assert((i <= 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  assert((w <= 2u));
  assert((x <= 4u));
  assert((y <= x));
  assert((a <= 1u));
  assert((b <= a));

  std::uint_fast64_t const state = packState(i, m, h, w, x, y, a, b);

  {
    Memo::const_iterator const iter = memo.find(state);
    if (iter != memo.cend()) {
      return iter->second;
    }
  }

  if (i == 34u) {
    std::uint_fast64_t const total = m == 4u && h == 1u && w >= 1u && x == 0u && y == 0u && a == 0u && b == 0u ? 1u : 0u;
    memo.emplace(state, total);
    return total;
  }

  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

  std::uint_fast64_t total = 0u;
  for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
    if (!isValidTransition(shunzi_prohibited, m, h, w, x, y, a, s)) {
      continue;
    }

    total += countNumTails(
      i + 1u,
      m + mtable[s],
      h + stable[s][3u],
      w + stable[s][2u],
      y + xytable[s],
      xytable[s],
      b + stable[s][7u] >= 1u ? 1u : 0u,
      stable[s][7u] >= 1u ? 1u : 0u,
      memo);
  }

  memo.emplace(state, total);
  return total;
}

void dumpTables(
  Memo const &memo,
  std::filesystem::path const &subtable_path,
  std::filesystem::path const &table_path)
{
  {
    std::ofstream ofs(subtable_path);
    if (!ofs) {
      throw std::runtime_error("Failed to open the subtable file.");
    }
    ofs << "// Copyright (c) 2023, 2024 Cryolite\n";
    ofs << "// SPDX-License-Identifier: MIT\n";
    ofs << "// This file is part of https://github.com/Cryolite/tsumonya\n";
    ofs << '\n';
    ofs << "#if !defined(STANDARD_SUBTABLE_HPP_INCLUDE_GUARD)\n";
    ofs << "#define STANDARD_SUBTABLE_HPP_INCLUDE_GUARD\n";
    ofs << '\n';
    ofs << "#include <tsumonya/standard/core.hpp>\n";
    ofs << '\n';
    ofs << '\n';
    ofs << "namespace Tsumonya::Standard_{\n";
    ofs << '\n';
    ofs << "inline constexpr Subtable subtable = {{\n";
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
      ofs << "  {{ // i = " << static_cast<unsigned>(i) << '\n';
      for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
        ofs << "    {{ // m = " << static_cast<unsigned>(m) << '\n';
        for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
          ofs << "      {{ // h = " << static_cast<unsigned>(h) << '\n';
          for (std::uint_fast8_t w = 0u; w <= 2u; ++w) {
            ofs << "        {{ // w = " << static_cast<unsigned>(w) << '\n';
            for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
              ofs << "          {{ // x = " << static_cast<unsigned>(x) << '\n';
              for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                ofs << "            {{ // y = " << static_cast<unsigned>(y) << '\n';
                for (std::uint_fast8_t a = 0u; a <= 1u; ++a) {
                  ofs << "              {{";
                  for (std::uint_fast8_t b = 0u; b <= 1u; ++b) {
                    std::uint_fast64_t const state = packState(i, m, h, w, x, y, a, b);
                    Memo::const_iterator const iter = memo.find(state);
                    if (iter != memo.cend()) {
                      ofs << iter->second << "u,";
                    }
                    else {
                      ofs << "0u,";
                    }
                  }
                  ofs << "}}, // a = " << static_cast<unsigned>(a) << '\n';
                }
                ofs << "            }},\n";
              }
              ofs << "          }},\n";
            }
            ofs << "        }},\n";
          }
          ofs << "      }},\n";
        }
        ofs << "    }},\n";
      }
      ofs << "  }},\n";
    }
    ofs << "}};\n";
    ofs << '\n';
    ofs << "}\n";
    ofs << '\n';
    ofs << "#endif // STANDARD_SUBTABLE_HPP_INCLUDE_GUARD" << std::endl;
  }

  {
    std::uint_fast64_t const upper_bound = [&]() -> std::uint_fast64_t {
      std::uint_fast64_t const state = packState(0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
      Memo::const_iterator const iter = memo.find(state);
      if (iter == memo.cend()) {
        throw std::logic_error("A logic error.");
      }
      return iter->second;
    }();

    std::ofstream ofs(table_path);
    if (!ofs) {
      throw std::runtime_error("Failed to open the table file.");
    }
    ofs << "// Copyright (c) 2023, 2024 Cryolite\n";
    ofs << "// SPDX-License-Identifier: MIT\n";
    ofs << "// This file is part of https://github.com/Cryolite/tsumonya\n";
    ofs << '\n';
    ofs << "#if !defined(TSUMONYA_STANDARD_TABLE_HPP_INCLUDE_GUARD)\n";
    ofs << "#define TSUMONYA_STANDARD_TABLE_HPP_INCLUDE_GUARD\n";
    ofs << '\n';
    ofs << "#include <tsumonya/standard/core.hpp>\n";
    ofs << "#include <cstdint>\n";
    ofs << '\n';
    ofs << '\n';
    ofs << "namespace Tsumonya::Standard_{\n";
    ofs << '\n';
    ofs << "inline constexpr std::uint_fast64_t upper_bound = " << upper_bound << ";\n";
    ofs << '\n';
    ofs << "inline constexpr Table table = {{\n";
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
      std::uint_fast8_t const color = i / 9u;
      std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
      bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

      ofs << "  {{ // i = " << static_cast<unsigned>(i) << '\n';
      for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
        ofs << "    {{ // m = " << static_cast<unsigned>(m) << '\n';
        for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
          ofs << "      {{ // h = " << static_cast<unsigned>(h) << '\n';
          for (std::uint_fast8_t w = 0u; w <= 2u; ++w) {
            ofs << "        {{ // w = " << static_cast<unsigned>(w) << '\n';
            for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
              ofs << "          {{ // x = " << static_cast<unsigned>(x) << '\n';
              for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                ofs << "            {{ // y = " << static_cast<unsigned>(y) << '\n';
                for (std::uint_fast8_t a = 0u; a <= 1u; ++a) {
                  ofs << "              {{ // a = " << static_cast<unsigned>(a) << '\n';
                  for (std::uint_fast8_t b = 0u; b <= 1u; ++b) {
                    ofs << "                {{";
                    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                      std::uint_fast64_t sum = 0u;
                      for (std::uint_fast8_t ss = 0u; ss < s; ++ss) {
                        if (!isValidTransition(shunzi_prohibited, m, h, w, x, y, a, ss)) {
                          continue;
                        }

                        sum += [&]() -> std::uint_fast64_t {
                          std::uint_fast8_t const mm = m + mtable[ss];
                          std::uint_fast8_t const hh = h + stable[ss][3u];
                          std::uint_fast8_t const ww = w + stable[ss][2u];
                          std::uint_fast8_t const xx = y + xytable[ss];
                          std::uint_fast8_t const yy = xytable[ss];
                          std::uint_fast8_t const aa = b + stable[ss][7u] >= 1u ? 1u : 0u;
                          std::uint_fast8_t const bb = stable[ss][7u] >= 1u ? 1u : 0u;
                          std::uint_fast64_t const state
                            = packState(i + 1, mm, hh, ww, xx, yy, aa, bb);
                          Memo::const_iterator const iter = memo.find(state);
                          return iter != memo.cend() ? iter->second : 0u;
                        }();
                      }
                      ofs << sum << "u,";
                    }
                    ofs << "}}, // b = " << static_cast<unsigned>(b) << '\n';
                  }
                  ofs << "              }},\n";
                }
                ofs << "            }},\n";
              }
              ofs << "          }},\n";
            }
            ofs << "        }},\n";
          }
          ofs << "      }},\n";
        }
        ofs << "    }},\n";
      }
      ofs << "  }},\n";
    }
    ofs << "}};\n";
    ofs << '\n';
    ofs << "}\n";
    ofs << '\n';
    ofs << "#endif // TSUMONYA_STANDARD_TABLE_HPP_INCLUDE_GUARD" << std::endl;
  }
}

} // namespace <anonymous>

int main(int const argc, char const * const * const argv)
{
    if (argc < 3) {
        throw std::runtime_error("Too few arguments.");
    }
    if (argc > 3) {
        throw std::runtime_error("Too many arguments.");
    }

    std::filesystem::path const subtable_path(argv[1]);
    std::filesystem::path const table_path(argv[2]);

    Memo memo;
    countNumTails(0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, memo);

    dumpTables(memo, subtable_path, table_path);
}
