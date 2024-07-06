// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "../../src/standard/winning_hand_enumeration.hpp"
#include <tsumonya/standard/hash.hpp>
#include <tsumonya/standard/table.hpp>
#include <tsumonya/standard/core.hpp>
#include <mutex>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <functional>
#include <cstdint>
#include <cassert>


namespace{

using Tsumonya::Standard_::upper_bound;
using PureHand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengGangList = std::array<std::uint_fast8_t, 34u>;
using Tsumonya::Standard_::getHash;
using Table = std::vector<std::atomic_uint8_t>;
using Tsumonya::Standard_::WinningHandCallback;
using Tsumonya::Standard_::enumerateWinningHands;

std::mutex mtx;

void dump(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong)
{
  std::cout << "hand = ";
  for (unsigned h : pure_hand) {
    std::cout << h << ',';
  }
  std::cout << '\n';

  std::cout << "chi = ";
  for (unsigned c : chi_list) {
    std::cout << c << ',';
  }
  std::cout << '\n';

  std::cout << "peng = ";
  for (unsigned p : peng_list) {
    std::cout << p << ',';
  }
  std::cout << '\n';

  std::cout << "angang = ";
  for (unsigned ag : angang_list) {
    std::cout << ag << ',';
  }
  std::cout << '\n';

  std::cout << "minggang = ";
  for (unsigned mg : minggang_list) {
    std::cout << mg << ',';
  }
  std::cout << '\n';
  std::cout << "winning_tile = " << static_cast<unsigned>(winning_tile) << '\n';
  std::cout << "rong = " << rong << '\n';
  std::cout << "========================================" << std::endl;
}

void testHash(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  Table &table,
  std::atomic_uint_fast64_t &count)
try {
  PureHand pure_hand_copy(pure_hand);
  assert((winning_tile < 34u));
  assert((pure_hand_copy[winning_tile] >= 1u));
  --pure_hand_copy[winning_tile];
  std::uint_fast64_t const hash = getHash(
    pure_hand_copy,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    winning_tile,
    rong);
  if (hash >= upper_bound) {
    std::cout << "hash = " << hash << '\n';
    dump(pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
    throw std::logic_error("A hash is out of bounds.");
  }
  ++table[hash];

  ++count;
  std::uint_fast64_t const count_ = count.load();
  if (count_ % 1000000u == 0u) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << count_ << " / " << upper_bound << std::endl;
  }
}
catch (std::exception const &e) {
  std::cerr << e.what() << std::endl;
  dump(pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
  throw;
}

} // namespace <anonymous>

int main(int argc, char const * const *)
{
  if (argc > 1) {
    throw std::runtime_error("Too many arguments.");
  }

  Table table(upper_bound);
  std::atomic_uint_fast64_t count = 0u;

  WinningHandCallback callback(std::bind_back(&testHash, std::ref(table), std::ref(count)));
  enumerateWinningHands(callback, 8u);

  std::uint_fast64_t const n = std::count_if(
    table.cbegin(), table.cend(), [](std::uint8_t const x){ return x >= 1u; });
  std::cout << n << " / " << upper_bound << std::endl;
  if (n != 6654031062u) {
    throw std::logic_error("A logic error.");
  }
}
