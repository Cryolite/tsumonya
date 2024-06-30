// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "winning_hand_enumeration.hpp"
#include <tsumonya/standard/hash.hpp>
#include <tsumonya/standard/core.hpp>
#include <thread>
#include <vector>
#include <array>
#include <functional>
#include <climits>
#include <cstdint>
#include <cassert>


namespace Tsumonya::Standard_{

namespace{

using Tsumonya::Standard_::stable;
using Tsumonya::Standard_::mtable;
using Tsumonya::Standard_::ntable;
using Tsumonya::Standard_::xytable;
using Tsumonya::Standard_::ptable;
using Tsumonya::Standard_::upper_bound;
using PureHand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengGangList = std::array<std::uint_fast8_t, 34u>;
using Tsumonya::Standard_::getHash;

bool isValidTransition(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  bool const a,
  bool const shunzi_prohibited,
  bool const has_winning_tile,
  std::uint_fast8_t const s)
{
  assert((i < 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  assert((x <= 4u));
  assert((y <= x));

  bool const has_tile_in_pure_hand = a || ptable[s] >= 1u;

  if (m + mtable[s] > 4u) {
    return false;
  }
  if (h + stable[s][3u] > 1u) {
    return false;
  }
  if (has_winning_tile && stable[s][2u] >= 1u) {
    return false;
  }
  if (!has_tile_in_pure_hand && stable[s][2u] >= 1u) {
    return false;
  }
  if (shunzi_prohibited && xytable[s] > 0u) {
    return false;
  }
  if (ntable[s] + x > 4u) {
    return false;
  }
  assert((xytable[s] + y <= 4u));

  return true;
}

void pushHand(
  std::uint_fast8_t const i,
  std::uint_fast8_t const s,
  PureHand &pure_hand,
  ChiList &chi_list,
  PengGangList &peng_list,
  PengGangList &angang_list,
  PengGangList &minggang_list)
{
  assert((i < 34u));
  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  std::uint_fast8_t const cindex = color <= 2u ? color * 7u + number : UINT_FAST8_MAX;
  pure_hand[i] += ptable[s];
  if (stable[s][7u] >= 1u) {
    assert((number < 7u));
    pure_hand[i + 1u] += stable[s][7u];
    pure_hand[i + 2u] += stable[s][7u];
  }
  if (stable[s][5u] >= 1u) {
    assert((cindex < 21u));
    chi_list[cindex] += stable[s][5u];
  }
  peng_list[i] += stable[s][4u];
  angang_list[i] += stable[s][1u];
  minggang_list[i] += stable[s][0u];
}

void popHand(
  std::uint_fast8_t const i,
  std::uint_fast8_t const s,
  PureHand &pure_hand,
  ChiList &chi_list,
  PengGangList &peng_list,
  PengGangList &angang_list,
  PengGangList &minggang_list)
{
  assert((i < 34u));

  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  std::uint_fast8_t const cindex = color <= 2u ? color * 7u + number : UINT_FAST8_MAX;

  minggang_list[i] -= stable[s][0u];
  angang_list[i] -= stable[s][1u];
  peng_list[i] -= stable[s][4u];
  if (stable[s][5u] >= 1u) {
    assert((cindex < 21u));
    chi_list[cindex] -= stable[s][5u];
  }
  if (stable[s][7u] >= 1u) {
    assert((number < 7u));
    pure_hand[i + 2u] -= stable[s][7u];
    pure_hand[i + 1u] -= stable[s][7u];
  }
  pure_hand[i] -= ptable[s];
}

void enumerateWinningHands(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  PureHand &pure_hand,
  ChiList &chi_list,
  PengGangList &peng_list,
  PengGangList &angang_list,
  PengGangList &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  WinningHandCallback callback)
{
  assert((i <= 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  assert((x <= 4u));
  assert((y <= x));
  for (std::uint_fast8_t const x : pure_hand) {
    assert((x <= 4u));
  }
  for (std::uint_fast8_t const c : chi_list) {
    assert((c <= 4u));
  }
  for (std::uint_fast8_t const p : peng_list) {
    assert((p <= 1u));
  }
  for (std::uint_fast8_t const ag : angang_list) {
    assert((ag <= 1u));
  }
  for (std::uint_fast8_t const mg : minggang_list) {
    assert((mg <= 1u));
  }
  assert((winning_tile == UINT_FAST8_MAX || winning_tile < 34u));

  if (i == 34u) {
    // Termination of recursion
    assert((x == 0u));
    assert((y == 0u));
    if (m == 4u && h == 1u && winning_tile != UINT_FAST8_MAX) {
      callback(
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        rong);
    }
    return;
  }

  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  std::uint_fast8_t const cindex = color <= 2u ? color * 7u + number: UINT_FAST8_MAX;
  bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

  bool const a = pure_hand[i] >= 1u;
  bool const has_winning_tile = winning_tile != UINT_FAST8_MAX;

  for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
    if (!isValidTransition(i, m, h, x, y, a, shunzi_prohibited, has_winning_tile, s)) {
      continue;
    }
    pushHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
    enumerateWinningHands(
      i + 1u,
      m + mtable[s],
      h + stable[s][3u],
      xytable[s] + y,
      xytable[s],
      pure_hand,
      chi_list,
      peng_list,
      angang_list,
      minggang_list,
      stable[s][2u] >= 1u ? i : winning_tile,
      rong || stable[s][2u] == 2u,
      callback);
    popHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
  }
}

void threadMain(std::uint_fast8_t const s, WinningHandCallback callback)
{
  PureHand pure_hand{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  ChiList chi_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  PengGangList peng_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  PengGangList angang_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  PengGangList minggang_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };

  if (!isValidTransition(0u, 0u, 0u, 0u, 0u, false, false, false, s)) {
    return;
  }
  pushHand(0u, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
  enumerateWinningHands(
    1u,
    mtable[s],
    stable[s][3u],
    xytable[s],
    xytable[s],
    pure_hand,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    stable[s][2u] >= 1u ? 0u : UINT_FAST8_MAX,
    stable[s][2u] == 2u,
    callback);
  popHand(0u, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
}

} // namespace <anonymous>

void enumerateWinningHands(WinningHandCallback callback, std::uint_fast8_t const s)
{
  threadMain(s, callback);
}

void enumerateWinningHands(WinningHandCallback callback, bool const multithreading)
{
  if (multithreading) {
    std::vector<std::thread> threads;
    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
      std::thread th(&threadMain, s, callback);
      threads.push_back(std::move(th));
    }      
    for (std::thread &th : threads) {
      th.join();
    }
  }
  else {
    PureHand pure_hand{
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    ChiList chi_list{
      0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    PengGangList peng_list{
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    PengGangList angang_list{
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    PengGangList minggang_list{
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u
    };

    enumerateWinningHands(
      0u,
      0u,
      0u,
      0u,
      0u,
      pure_hand,
      chi_list,
      peng_list,
      angang_list,
      minggang_list,
      UINT_FAST8_MAX,
      false,
      callback);
  }
}

} // namespace Tsumonya::Standard_
