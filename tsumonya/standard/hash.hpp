// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include <tsumonya/standard/table.hpp>
#include <tsumonya/standard/core.hpp>
#include <iterator>
#include <array>
#include <tuple>
#include <stdexcept>
#include <cstdint>
#include <cassert>


namespace Tsumonya::Standard_{

using Hand_ = std::array<std::uint_fast8_t, 34u>;
using ChiList_ = std::array<std::uint_fast8_t, 21u>;
using PengGangList_ = std::array<std::uint_fast8_t, 34u>;
using StateSeq_ = std::array<std::array<std::uint_fast8_t, 8u>, 34u>;

inline std::tuple<std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t> encodeShupai_(
  std::uint_fast8_t const color,
  Hand_ const &hand,
  ChiList_ const &chi_list,
  PengGangList_ const &peng_list,
  PengGangList_ const &angang_list,
  PengGangList_ const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::uint_fast8_t const head,
  std::uint_fast8_t m,
  std::uint_fast8_t h,
  std::uint_fast8_t w,
  StateSeq_ &state_seq)
{
  std::uint_fast8_t x = 0u;
  std::uint_fast8_t y = 0u;
  std::uint_fast8_t a = hand[color * 9u + 0u];
  std::uint_fast8_t b = hand[color * 9u + 1u];
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
      if (stable[ss][2u] >= 1u && hand[i] == 0u) {
        // If the winning tile is `i`, then tile `i` must exist in the pure hand.
        continue;
      }
      if (stable[ss][2u] >= 1u && rong != (stable[ss][2u] == 2u)) {
        // Winning by self-draw or deal-in must be consistent.
        continue;
      }
      if (ptable[ss] != a) {
        // The count of tile i must be exact for a winning hand.
        continue;
      }
      if (number < 7u && stable[ss][7u] > hand[i + 2u]) {
        // The count of tile `i + 2` in the pure hand must be greater than or equal to the number of
        // concealed three-in-a-row starting with tile `i`.
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
      return { UINT_FAST8_MAX, UINT_FAST8_MAX, UINT_FAST8_MAX };
    }

    state_seq[i] = {
      m,
      h,
      w,
      x,
      y,
      hand[i] >= a + 1u ? static_cast<std::uint_fast8_t>(1u) : static_cast<std::uint_fast8_t>(0u),
      (i + 1u < (color + 1u) * 9u && hand[i + 1u] >= b + 1u) ? static_cast<std::uint_fast8_t>(1u)
                                                             : static_cast<std::uint_fast8_t>(0u),
      s
    };

    m += mtable[s];
    h += stable[s][3u];
    w += stable[s][2u];
    x = number < 7u ? y + xytable[s] : y;
    y = number < 7u ? xytable[s] : 0u;
    a = b - stable[s][7u];
    b = number < 7u ? hand[i + 2u] - stable[s][7u] : 0u;
  }

  return { m, h, w };
}

inline bool encodeZipai_(
  Hand_ const &hand,
  PengGangList_ const &peng_list,
  PengGangList_ const &angang_list,
  PengGangList_ const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::uint_fast8_t m,
  std::uint_fast8_t h,
  std::uint_fast8_t w,
  StateSeq_ &state_seq)
{
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
      if ((hand[i] == 2u) != (stable[ss][3u] == 1u)) {
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
      if (stable[ss][2u] >= 1u && hand[i] == 0u) {
        // If the winning tile is `i`, then tile `i` must exist in the hand.
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
      if (ptable[ss] != hand[i]) {
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

    state_seq[i] = { m, h, w, 0u, 0u, 0u, 0u, s };

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
  Hand_ const &hand,
  ChiList_ const &chi_list,
  PengGangList_ const &peng_list,
  PengGangList_ const &angang_list,
  PengGangList_ const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool rong)
{
  {
    // Check whether `(hand, chi_list, peng_list, angang_list, minggang_list)` represents a valid
    // hand.
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
        = x + hand[i] + peng_list[i] * 3u + cnum + angang_list[i] * 4u + minggang_list[i] * 4u;
      if (n > 4u) {
        throw std::invalid_argument("An invalid hand.");
      }
      total += x + hand[i] + peng_list[i] * 3u + cnum + angang_list[i] * 3u + minggang_list[i] * 3u;
      x = y + cnum;
      y = cnum;
    }
    assert((x == 0u));
    assert((y == 0u));
    if (total != 14u) {
      throw std::invalid_argument("An invalid hand.");
    }
  }

  std::array<std::uint_fast8_t, 3u> t = { 0u, 0u, 0u };
  for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
    for (std::uint_fast8_t number = 0u; number < 9u; ++number) {
      t[color] += number * hand[color * 9u + number];
    }
  }

  std::uint_fast8_t m = 0u;
  std::uint_fast8_t h = 0u;
  std::uint_fast8_t w = 0u;
  StateSeq_ state_seq;

  for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
    auto [m_, h_, w_] = encodeShupai_(
      color,
      hand,
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
        if (hand[color * 9u + j] < 2u) {
          continue;
        }
        std::tie(m_, h_, w_) = encodeShupai_(
          color,
          hand,
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

  if (!encodeZipai_(hand, peng_list, angang_list, minggang_list, winning_tile, rong, m, h, w, state_seq)) {
    return UINT_FAST64_MAX;
  }

  std::uint_fast64_t const code = encodeStateSeq_(state_seq);
  return code;
}

template<
  typename HandIterator,
  typename ChiIterator,
  typename PengIterator,
  typename AngangIterator,
  typename MinggangIterator>
std::uint_fast64_t getHash(
  HandIterator hand_first,
  HandIterator hand_last,
  ChiIterator chi_first,
  ChiIterator chi_last,
  PengIterator peng_first,
  PengIterator peng_last,
  AngangIterator angang_first,
  AngangIterator angang_last,
  MinggangIterator minggang_first,
  MinggangIterator minggang_last,
  std::uint_fast8_t winning_tile,
  bool rong)
{
  Hand_ hand;
  hand.fill(0u);
  {
    // Check whether `[hand_first, hand_last)` represents a valid pure hand.
    std::uint_fast8_t i = 0u;
    std::uint_fast8_t n = 0u;
    for (HandIterator iter = hand_first; iter != hand_last;) {
      if (i >= 34u) {
        throw std::invalid_argument("An invalid hand.");
      }
      if (n > 14u) {
        throw std::invalid_argument("An invalid hand.");
      }
      hand[i] = *iter++;
      if (hand[i] > 4) {
        throw std::invalid_argument("An invalid hand.");
      }
      n += hand[i];
      ++i;
    }
    if (i != 34u) {
      throw std::invalid_argument("An invalid hand.");
    }
    if (n % 3u != 1u) {
      throw std::invalid_argument("An invalid hand.");    
    }
  }

  ChiList_ chi_list;
  chi_list.fill(0u);
  {
    // Check whether `[chi_first, chi_list)` represents a valid chi list.
    std::uint_fast8_t i = 0u;
    for (ChiIterator iter = chi_first; iter != chi_last;) {
      if (i >= 21u) {
        throw std::invalid_argument("An invalid chi list.");
      }
      chi_list[i] = *iter++;
      if (chi_list[i] > 4) {
        throw std::invalid_argument("An invalid chi list.");
      }
      ++i;
    }
    if (i != 21u) {
      throw std::invalid_argument("An invalid chi list.");
    }
  }

  PengGangList_ peng_list;
  peng_list.fill(0u);
  {
    // Check whether `[peng_first, peng_list)` represents a valid peng list.
    std::uint_fast8_t i = 0u;
    for (PengIterator iter = peng_first; iter != peng_last;) {
      if (i >= 34u) {
        throw std::invalid_argument("An invalid peng list.");
      }
      peng_list[i] = *iter++;
      if (peng_list[i] > 1) {
        throw std::invalid_argument("An invalid peng list.");
      }
      ++i;
    }
    if (i != 34u) {
      throw std::invalid_argument("An invalid peng list.");
    }
  }

  PengGangList_ angang_list;
  angang_list.fill(0u);
  {
    // Check whether `[angang_first, angang_list)` represents a valid angang list.
    std::uint_fast8_t i = 0u;
    for (AngangIterator iter = angang_first; iter != angang_last;) {
      if (i >= 34u) {
        throw std::invalid_argument("An invalid angang list.");
      }
      angang_list[i] = *iter++;
      if (angang_list[i] > 1) {
        throw std::invalid_argument("An invalid angang list.");
      }
      ++i;
    }
    if (i != 34u) {
      throw std::invalid_argument("An invalid angang list.");
    }
  }

  PengGangList_ minggang_list;
  minggang_list.fill(0u);
  {
    // Check whether `[minggang_first, minggang_list)` represents a valid minggang list.
    std::uint_fast8_t i = 0u;
    for (MinggangIterator iter = minggang_first; iter != minggang_last;) {
      if (i >= 34u) {
        throw std::invalid_argument("An invalid minggang list.");
      }
      minggang_list[i] = *iter++;
      if (minggang_list[i] > 1) {
        throw std::invalid_argument("An invalid minggang list.");
      }
      ++i;
    }
    if (i != 34u) {
      throw std::invalid_argument("An invalid minggang list.");
    }
  }

  if (winning_tile >= 34u) {
    throw std::invalid_argument("An invalid winning tile.");
  }

  ++hand[winning_tile];
  if (hand[winning_tile] > 4) {
    throw std::invalid_argument("An invalid hand.");
  }

  return getHash_(hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
}

template<
  typename HandRange,
  typename ChiRange,
  typename PengRange,
  typename AngangRange,
  typename MinggangRange>
std::uint_fast64_t getHash(
  HandRange const &hand,
  ChiRange const &chi_list,
  PengRange const &peng_list,
  AngangRange const &angang_list,
  MinggangRange const &minggang_list,
  std::uint_fast8_t winning_tile,
  bool rong)
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
