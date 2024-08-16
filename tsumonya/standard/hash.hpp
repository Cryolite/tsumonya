// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(TSUMONYA_STANDARD_HASH_HPP_INCLUDE_GUARD)
#define TSUMONYA_STANDARD_HASH_HPP_INCLUDE_GUARD

#include <tsumonya/standard/table.hpp>
#include <tsumonya/standard/core.hpp>
#include <iterator>
#include <array>
#include <tuple>
#include <climits>
#include <cstdint>
#include <cassert>


namespace Tsumonya::Standard_{

using StateSeq_ = std::array<std::array<std::uint_fast8_t, 8u>, 34u>;

inline std::tuple<std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t> encodeShupai_(
  std::uint_fast8_t const color,
  std::array<std::uint_fast8_t, 34u> const &pure_hand,
  std::array<std::uint_fast8_t, 21u> const &chi_list,
  std::array<std::uint_fast8_t, 34u> const &peng_list,
  std::array<std::uint_fast8_t, 34u> const &angang_list,
  std::array<std::uint_fast8_t, 34u> const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::uint_fast8_t const head,
  std::uint_fast8_t m,
  std::uint_fast8_t h,
  std::uint_fast8_t w,
  StateSeq_ &state_seq)
{
  assert((color <= 2u));
  assert((winning_tile < 34u));
  assert((head < 34u || head == UINT_FAST8_MAX));

  std::uint_fast8_t x = 0u;
  std::uint_fast8_t y = 0u;
  std::uint_fast8_t a = pure_hand[color * 9u + 0u];
  std::uint_fast8_t b = pure_hand[color * 9u + 1u];
  for (std::uint_fast8_t i = color * 9u; i < (color + 1u) * 9u; ++i) {
    assert((m <= 4u));
    assert((h <= 1u));
    assert((w <= 2u));
    assert((x <= 4u));
    assert((y <= x));
    assert((a <= 4u));
    assert((b <= 4u));

    std::uint_fast8_t const number = i - color * 9u;
    std::uint_fast8_t const cindex = number < 7u ? color * 7u + number : UINT_FAST8_MAX;
    std::uint_fast8_t const cnum = cindex != UINT_FAST8_MAX ? chi_list[cindex] : 0u;

    std::uint_fast8_t s = UINT_FAST8_MAX;
    for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
      if (m + mtable[ss] > 4u) {
        // The number of members (menzi, 面子) must not exceed 4.
        continue;
      }
      if ((head == i) != (stable[ss][3u] == 1u)) {
        // Tile `i` is designated as the head (雀頭).
        continue;
      }
      if (h + stable[ss][3u] > 1u) {
        // The number of heads (雀頭) must not exceed 1.
        continue;
      }
      if ((winning_tile == i) != (stable[ss][2u] >= 1u)) {
        // Tile `i` is designated as the winning tile.
        continue;
      }
      if (w >= 1u && stable[ss][2u] >= 1u) {
        // The number of winning tiles must not exceed 1.
        continue;
      }
      if (stable[ss][2u] >= 1u && pure_hand[i] == 0u) {
        // If the winning tile is `i`, then tile `i` must exist in the pure hand.
        continue;
      }
      if (stable[ss][2u] >= 1u && rong != (stable[ss][2u] == 2u)) {
        // Winning by self-draw or deal-in must be consistent.
        continue;
      }
      if (ptable[ss] != a) {
        // The count of tile `i` must be exact for a winning hand.
        continue;
      }
      if (number < 7u && stable[ss][7u] > pure_hand[i + 2u]) {
        // The count of tile `i + 2` in the pure hand must be greater than or equal to the number
        // of concealed three-in-a-row starting with tile `i`.
        continue;
      }
      if (stable[ss][7u] > b) {
        continue;
      }
      if (cindex == UINT_FAST8_MAX && xytable[ss] > 0u) {
        // Three-in-a-row starting with 8 or 9 must not exist.
        continue;
      }
      if (stable[ss][5u] != cnum) {
        // The number of open three-in-a-row starting with tile `i` must be exact.
        continue;
      }
      if (peng_list[i] != stable[ss][4u]) {
        // The number of peng consisting of tile `i` must be exact.
        continue;
      }
      if (angang_list[i] != stable[ss][1u]) {
        // The number of angang consisting of tile `i` must be exact.
        continue;
      }
      if (minggang_list[i] != stable[ss][0u]) {
        // The number of minggang consisting of tile `i` must be exact.
        continue;
      }

      s = ss;
      break;
    }
    if (s == UINT_FAST8_MAX) {
      return {UINT_FAST8_MAX, UINT_FAST8_MAX, UINT_FAST8_MAX};
    }

    std::uint_fast8_t const aa = pure_hand[i] > a ? 1u : 0u;
    std::uint_fast8_t const bb = (i + 1u < (color + 1u) * 9u && pure_hand[i + 1u] > b) ? 1u : 0u;
    state_seq[i] = {
      m,
      h,
      w,
      x,
      y,
      aa,
      bb,
      s
    };

    m += mtable[s];
    h += stable[s][3u];
    w += stable[s][2u];
    x = number < 7u ? y + xytable[s] : y;
    y = number < 7u ? xytable[s] : 0u;
    a = b - stable[s][7u];
    b = number < 7u ? pure_hand[i + 2u] - stable[s][7u] : 0u;
  }

  return {m, h, w};
}

inline bool encodeZipai_(
  std::array<std::uint_fast8_t, 34u> const &pure_hand,
  std::array<std::uint_fast8_t, 34u> const &peng_list,
  std::array<std::uint_fast8_t, 34u> const &angang_list,
  std::array<std::uint_fast8_t, 34u> const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::uint_fast8_t m,
  std::uint_fast8_t h,
  std::uint_fast8_t w,
  StateSeq_ &state_seq)
{
  assert((winning_tile < 34u));

  for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
    assert((m <= 4u));
    assert((h <= 1u));
    assert((w <= 2u));

    std::uint_fast8_t s = UINT_FAST8_MAX;
    for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
      if (m + mtable[ss] > 4u) {
        // The number of members (menzi, 面子) must not exceed 4.
        continue;
      }
      if ((pure_hand[i] == 2u) != (stable[ss][3u] == 1u)) {
        // The number of tile `i` is equal to 2, then it must be the head (雀頭).
        continue;
      }
      if (h + stable[ss][3u] > 1u) {
        // The number of heads (雀頭) must not exceed 1.
        continue;
      }
      if ((winning_tile == i) != (stable[ss][2u] >= 1u)) {
        // Tile `i` is designated as the winning tile.
        continue;
      }
      if (w >= 1u && stable[ss][2u] >= 1u) {
          // The number of winning tiles must not exceed 1.
          continue;
      }
      if (stable[ss][2u] >= 1u && pure_hand[i] == 0u) {
        // If the winning tile is `i`, then tile `i` must exist in the pure hand.
        continue;
      }
      if (stable[ss][2u] >= 1u && rong != (stable[ss][2u] == 2u)) {
        // Winning by self-draw or deal-in must be consistent.
        continue;
      }
      if (xytable[ss] > 0u) {
        // Three-in-a-row (shunzi, 順子) is not allowed for honors (zipai, 字牌).
        continue;
      }
      assert((stable[ss][7u] == 0u));
      if (ptable[ss] != pure_hand[i]) {
        continue;
      }
      if (peng_list[i] != stable[ss][4u]) {
        // The number of peng consisting of tile `i` must be exact.
        continue;
      }
      if (angang_list[i] != stable[ss][1u]) {
        // The number of angang consisting of tile `i` must be exact.
        continue;
      }
      if (minggang_list[i] != stable[ss][0u]) {
        // The number of minggang consisting of tile `i` must be exact.
        continue;
      }

      s = ss;
      break;
    }
    if (s == UINT_FAST8_MAX) {
      return false;
    }

    state_seq[i] = {m, h, w, 0u, 0u, 0u, 0u, s};

    m += mtable[s];
    h += stable[s][3u];
    w += stable[s][2u];
  }

  return m == 4u && h == 1u && w >= 1u;
}

inline std::uint_fast64_t encodeStateSeq_(StateSeq_ const &state_seq)
{
  std::uint_fast64_t code = 0u;
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    auto const [m, h, w, x, y, a, b, s] = state_seq[i];
    code += Tsumonya::Standard_::table[i][m][h][w][x][y][a][b][s];
  }
  return code;
}

inline std::uint_fast64_t getHash_(
  std::array<std::uint_fast8_t, 34u> const &pure_hand,
  std::array<std::uint_fast8_t, 21u> const &chi_list,
  std::array<std::uint_fast8_t, 34u> const &peng_list,
  std::array<std::uint_fast8_t, 34u> const &angang_list,
  std::array<std::uint_fast8_t, 34u> const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong)
{
  {
    // Check whether `(pure_hand, chi_list, peng_list, angang_list, minggang_list)` represents a
    // valid hand.
    std::uint_fast8_t x = 0u;
    std::uint_fast8_t y = 0u;
    std::uint_fast8_t total = 0u;
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
      std::uint_fast8_t const color = i / 9u;
      std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
      std::uint_fast8_t const cindex
        = color <= 2u && number <= 6u ? color * 7u + number : UINT_FAST8_MAX;
      std::uint_fast8_t const cnum = cindex != UINT_FAST8_MAX ? chi_list[cindex] : 0u;
      std::uint_fast8_t const n
        = x + pure_hand[i] + peng_list[i] * 3u + cnum + angang_list[i] * 4u + minggang_list[i] * 4u;
      assert((n <= 4u));
      total += x + pure_hand[i] + peng_list[i] * 3u + cnum + angang_list[i] * 3u + minggang_list[i] * 3u;
      x = y + cnum;
      y = cnum;
    }
    assert((x == 0u));
    assert((y == 0u));
    assert((total == 14u));
  }

  std::array<std::uint_fast8_t, 3u> t = {0u, 0u, 0u};
  for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
    for (std::uint_fast8_t number = 0u; number < 9u; ++number) {
      t[color] += number * pure_hand[color * 9u + number];
    }
  }

  std::uint_fast8_t m = 0u;
  std::uint_fast8_t h = 0u;
  std::uint_fast8_t w = 0u;
  StateSeq_ state_seq;

  for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
    auto [m_, h_, w_] = encodeShupai_(
      color,
      pure_hand,
      chi_list,
      peng_list,
      angang_list,
      minggang_list,
      winning_tile,
      rong,
      UINT_FAST8_MAX,
      m,
      h,
      w,
      state_seq);
    if (m_ != UINT_FAST8_MAX) {
      assert((h_ != UINT_FAST8_MAX));
      assert((w_ != UINT_FAST8_MAX));
      m = m_;
      h = h_;
      w = w_;
    }
    else {
      assert((h_ == UINT_FAST8_MAX));
      assert((w_ == UINT_FAST8_MAX));
      for (std::uint_fast8_t j = (t[color] * 2u) % 3u; j < 9u; j += 3u) {
        if (pure_hand[color * 9u + j] < 2u) {
          continue;
        }
        std::tie(m_, h_, w_) = encodeShupai_(
          color,
          pure_hand,
          chi_list,
          peng_list,
          angang_list,
          minggang_list,
          winning_tile,
          rong,
          color * 9u + j,
          m,
          h,
          w,
          state_seq);
        if (m_ != UINT_FAST8_MAX) {
          assert((h_ != UINT_FAST8_MAX));
          assert((w_ != UINT_FAST8_MAX));
          m = m_;
          h = h_;
          w = w_;
          assert((h == 1u));
          break;
        }
      }
      if (m_ == UINT_FAST8_MAX) {
        assert((h_ == UINT_FAST8_MAX));
        assert((w_ == UINT_FAST8_MAX));
        return UINT_FAST64_MAX;
      }
    }
  }

  if (!encodeZipai_(pure_hand, peng_list, angang_list, minggang_list, winning_tile, rong, m, h, w, state_seq)) {
    return UINT_FAST64_MAX;
  }

  std::uint_fast64_t const code = encodeStateSeq_(state_seq);
  return code;
}

template<
  typename PureHandIterator,
  typename ChiListIterator,
  typename PengListIterator,
  typename AngangListIterator,
  typename MinggangListIterator>
std::uint_fast64_t getHash(
  PureHandIterator pure_hand_first,
  PureHandIterator pure_hand_last,
  ChiListIterator chi_list_first,
  ChiListIterator chi_list_last,
  PengListIterator peng_list_first,
  PengListIterator peng_list_last,
  AngangListIterator angang_list_first,
  AngangListIterator angang_list_last,
  MinggangListIterator minggang_list_first,
  MinggangListIterator minggang_list_last,
  std::uint_fast8_t winning_tile,
  bool rong)
{
  std::array<std::uint_fast8_t, 34u> pure_hand = [&]() {
    // Check whether `[pure_hand_first, pure_hand_last)` represents a valid pure hand.
    std::array<std::uint_fast8_t, 34u> pure_hand;
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t n = 0u;
    for (PureHandIterator iter = pure_hand_first; iter != pure_hand_last;) {
      assert((i < 34u));
      pure_hand[i] = *iter++;
      assert((pure_hand[i] <= 4));
      n += pure_hand[i];
      assert((n <= 13u));
      ++i;
    }
    assert((i == 34u));
    assert((n % 3u == 1u));
    return pure_hand;
  }();

  std::array<std::uint_fast8_t, 21u> chi_list = [&]() {
    // Check whether `[chi_list_first, chi_list_last)` represents a valid chi list.
    std::array<std::uint_fast8_t, 21u> chi_list;
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t m = 0u;
    for (ChiListIterator iter = chi_list_first; iter != chi_list_last;) {
      assert((i < 21u));
      chi_list[i] = *iter++;
      assert((chi_list[i] <= 4));
      m += chi_list[i];
      assert((m <= 4u));
      ++i;
    }
    assert((i == 21u));
    return chi_list;
  }();

  std::array<std::uint_fast8_t, 34u> peng_list = [&]() {
    // Check whether `[peng_list_first, peng_list_last)` represents a valid peng list.
    std::array<std::uint_fast8_t, 34u> peng_list;
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t m = 0u;
    for (PengListIterator iter = peng_list_first; iter != peng_list_last;) {
      assert((i < 34u));
      peng_list[i] = *iter++;
      assert((peng_list[i] <= 1));
      m += peng_list[i];
      assert((m <= 4u));
      ++i;
    }
    assert((i == 34u));
    return peng_list;
  }();

  std::array<std::uint_fast8_t, 34u> angang_list = [&]() {
    // Check whether `[angang_list_first, angang_list_last)` represents a valid angang list.
    std::array<std::uint_fast8_t, 34u> angang_list;
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t m = 0u;
    for (AngangListIterator iter = angang_list_first; iter != angang_list_last;) {
      assert((i < 34u));
      angang_list[i] = *iter++;
      assert((angang_list[i] <= 1));
      m += angang_list[i];
      assert((m <= 4u));
      ++i;
    }
    assert((i == 34u));
    return angang_list;
  }();

  std::array<std::uint_fast8_t, 34u> minggang_list = [&]() {
    // Check whether `[minggang_list_first, minggang_list)` represents a valid minggang list.
    std::array<std::uint_fast8_t, 34u> minggang_list;
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t m = 0u;
    for (MinggangListIterator iter = minggang_list_first; iter != minggang_list_last;) {
      assert((i < 34u));
      minggang_list[i] = *iter++;
      assert((minggang_list[i] <= 1));
      m += minggang_list[i];
      assert((m <= 4u));
      ++i;
    }
    assert((i == 34u));
    return minggang_list;
  }();

  assert((winning_tile < 34u));
  ++pure_hand[winning_tile];
  assert((pure_hand[winning_tile] <= 4));

  return getHash_(pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
}

template<
  typename PureHand,
  typename ChiList,
  typename PengList,
  typename AngangList,
  typename MinggangList>
std::uint_fast64_t getHash(
  PureHand const &hand,
  ChiList const &chi_list,
  PengList const &peng_list,
  AngangList const &angang_list,
  MinggangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong)
{
  return getHash(
    std::cbegin(hand),
    std::cend(hand),
    std::cbegin(chi_list),
    std::cend(chi_list),
    std::cbegin(peng_list),
    std::cend(peng_list),
    std::cbegin(angang_list),
    std::cend(angang_list),
    std::cbegin(minggang_list),
    std::cend(minggang_list),
    winning_tile,
    rong);
}

} // namespace Tsumonya::Standard_

#endif // !defined(TSUMONYA_STANDARD_HASH_HPP_INCLUDE_GUARD)
