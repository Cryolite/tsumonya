// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "../../tsumonya/seven_pairs/hash.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <cstdlib>


namespace{

using Tsumonya::SevenPairs_::size;
using Tsumonya::SevenPairs_::getHash;

void test(std::array<std::uint_fast8_t, 34u> const &hand, std::vector<std::uint_fast8_t> &check)
{
  std::uint_fast32_t const hash = getHash(hand);
  if (hash == UINT_FAST32_MAX) {
    throw std::logic_error("A logic error.");
  }
  if (hash >= size) {
    throw std::logic_error("A logic error.");
  }

  if (check[hash] != 0u) {
    throw std::logic_error("A logic error.");
  }
  check[hash] = 1u;
}

void enumerateSevenPairs(
  std::uint_fast8_t i,
  std::uint_fast8_t m,
  std::array<std::uint_fast8_t, 34u> &hand,
  std::vector<std::uint_fast8_t> &check)
{
  if (i == 34u) {
    if (m == 7u) {
      test(hand, check);
    }
    return;
  }

  enumerateSevenPairs(i + 1u, m, hand, check);

  hand[i] = 2u;
  enumerateSevenPairs(i + 1u, m + 1u, hand, check);
  hand[i] = 0u;
}

} // namespace <anonymous>

int main()
{
  std::array<std::uint_fast8_t, 34u> hand{};
  std::vector<std::uint_fast8_t> check(size, 0u);
  enumerateSevenPairs(0u, 0u, hand, check);

  for (std::uint_fast8_t const n : check) {
    if (n != 1u) {
      throw std::logic_error("A logic error.");
    }
  }
  return EXIT_SUCCESS;
}
