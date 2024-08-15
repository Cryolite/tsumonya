// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "sample_winning_hand.hpp"
#include "subtable.hpp"
#include <tsumonya/standard/hash.hpp>
#include <tsumonya/standard/core.hpp>
#include <random>
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
using Tsumonya::Standard_::subtable;
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

void sampleWinningHand_(
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
  std::mt19937 &random_number_engine,
  WinningHandCallback callback)
{
  assert((i <= 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  assert((x <= 4u));
  assert((y <= x));
  for (std::uint_fast8_t const t : pure_hand) {
    assert((t <= 4u));
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
  assert((!!callback));

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

  std::array<std::uint_fast64_t, stable.size()> probability_masses;
  probability_masses.fill(0u);
  for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
    if (!isValidTransition(i, m, h, x, y, a, shunzi_prohibited, has_winning_tile, s)) {
      continue;
    }
    pushHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
    std::uint_fast8_t const ii = i + 1u;
    std::uint_fast8_t const mm = m + mtable[s];
    std::uint_fast8_t const hh = h + stable[s][3u];
    std::uint_fast8_t const ww = has_winning_tile || stable[s][2u] >= 1u ? 1u : 0u;
    std::uint_fast8_t const xx = y + xytable[s];
    std::uint_fast8_t const yy = xytable[s];
    std::uint_fast8_t const aa = number <= 7u && pure_hand[i + 1u] >= 1u ? 1u : 0u;
    std::uint_fast8_t const bb = number <= 6u && pure_hand[i + 2u] >= 1u ? 1u : 0u;
    if (ii <= 33u) {
      probability_masses[s] = subtable[ii][mm][hh][ww][xx][yy][aa][bb];
    }
    else {
      probability_masses[s] = 1u;
    }
    popHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
  }

  std::uint_fast8_t const s = [&]() -> std::uint_fast8_t {
    std::discrete_distribution<std::uint_fast8_t> distribution(
      probability_masses.cbegin(), probability_masses.cend());
    return distribution(random_number_engine);
  }();
  pushHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
  sampleWinningHand_(
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
    random_number_engine,
    callback);
  popHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
}

} // namespace <anonymous>

void sampleWinningHand(WinningHandCallback callback, std::mt19937 &random_number_engine)
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
  sampleWinningHand_(
    0u, // i
    0u, // m
    0u, // h
    0u, // x
    0u, // y
    pure_hand,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    UINT_FAST8_MAX, // winning_tile
    false, // rong
    random_number_engine,
    callback);
}

} // namespace Tsumonya::Standard_
