// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(TSUMONYA_SEVEN_PAIRS_HASH_HPP_INCLUDE_GUARD)
#define TSUMONYA_SEVEN_PAIRS_HASH_HPP_INCLUDE_GUARD

#include <tsumonya/seven_pairs/table.hpp>
#include <iterator>
#include <climits>
#include <cstdint>
#include <cassert>


namespace Tsumonya::SevenPairs_{

template<typename HandIterator>
inline std::uint_fast32_t getHash(HandIterator hand_first, HandIterator hand_last)
{
  std::uint_fast32_t hash = 0u;
  {
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t m = 0u;
    while (hand_first != hand_last) {
      assert((i < 34u));
      long long const n = *hand_first++;
      assert((n >= 0));
      assert((n <= 4));
      if (n != 0 && n != 2) {
        return UINT_FAST32_MAX;
      }
      if (n == 2) {
        hash += Tsumonya::SevenPairs_::table[i][m];
        ++m;
        assert((m <= 7u));
      }
      ++i;
    }
    assert((i == 34u));
    assert((m == 7u));
  }
  return hash;
}

template<typename Hand>
inline std::uint_fast32_t getHash(Hand const &hand)
{
  return getHash(std::cbegin(hand), std::cend(hand));
}

} // namespace Tsumonya::SevenPairs_

#endif // !defined(TSUMONYA_SEVEN_PAIRS_HASH_HPP_INCLUDE_GUARD)
