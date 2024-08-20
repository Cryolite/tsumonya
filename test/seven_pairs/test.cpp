// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../../src/reference_calculator.hpp"
#include "../../tsumonya/calculator.hpp"
#include "../../tsumonya/seven_pairs/table.hpp"
#include <filesystem>
#include <iostream>
#include <random>
#include <array>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>


namespace{

using Tsumonya::SevenPairs_::size;
using Tsumonya::Calculator;
using Tsumonya::Detail_::ReferenceCalculator;

void dump(
  std::array<std::uint_fast8_t, 34u> const &hand,
  std::uint_fast8_t const winning_tile,
  bool const rong)
{
  for (unsigned const t : hand) {
    std::cerr << t << ',';
  }
  std::cerr << " (" << static_cast<unsigned>(winning_tile)
    << ", " << (!rong ? "zimo" : "rong") << ")\n";
}

void testImpl(
  Tsumonya::Wind const round_wind,
  Tsumonya::Wind const player_wind,
  std::array<std::uint_fast8_t, 34u> const &hand,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  Calculator const &calculator,
  ReferenceCalculator const &reference_calculator)
{
  std::array<std::uint_fast8_t, 21u> const chi_list{};
  std::array<std::uint_fast8_t, 34u> const peng_list{};
  std::array<std::uint_fast8_t, 34u> const angang_list{};
  std::array<std::uint_fast8_t, 34u> const minggang_list{};
  auto const [fu_ours, fan_ours] = calculator(
    round_wind,
    player_wind,
    hand,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    winning_tile,
    rong ? Tsumonya::rong : Tsumonya::zimo,
    0u
  );

  auto const [fu_their, fan_encode_their] = reference_calculator(
    static_cast<std::uint_fast8_t>(round_wind),
    static_cast<std::uint_fast8_t>(player_wind),
    hand,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    winning_tile,
    rong
  );
  if (fu_their == 0u) {
    dump(hand, winning_tile, rong);
    throw std::logic_error("A logic error.");
  }
  if (fu_their != 25u) {
    return;
  }
  std::uint_fast8_t fan_their = [&]() -> std::uint_fast8_t {
    if (fan_encode_their >= 128u) {
      return 13u + (fan_encode_their - 128u) * 13u;
    }
    if (fan_encode_their >= 64u) {
      throw std::logic_error("A logic error.");
    }
    return fan_encode_their;
  }();

  if (fu_ours != fu_their || fan_ours != fan_their) {
    dump(hand, winning_tile, rong);
    std::cerr << "Ours: " << static_cast<unsigned>(fu_ours) << " fu, " << static_cast<unsigned>(fan_ours) << " fan\n";
    std::cerr << "Theirs: " << static_cast<unsigned>(fu_their) << " fu, " << static_cast<unsigned>(fan_their) << " fan\n";
    throw std::logic_error("A logic error.");
  }
}

void test(
  std::array<std::uint_fast8_t, 34u> const &hand,
  Calculator const &calculator,
  ReferenceCalculator const &reference_calculator)
{
  std::uint_fast8_t const winning_tile = [&]() -> std::uint_fast8_t {
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
      if (hand[i] == 2u) {
        return i;
      }
    }
    throw std::logic_error("A logic error.");
  }();
  std::array<std::uint_fast8_t, 34u> const hand_ = [&]() {
    std::array<std::uint_fast8_t, 34u> hand_(hand);
    --hand_[winning_tile];
    return hand_;
  }();
  testImpl(
    Tsumonya::east, Tsumonya::east, hand_, winning_tile, false, calculator, reference_calculator);
  testImpl(
    Tsumonya::east, Tsumonya::east, hand_, winning_tile, false, calculator, reference_calculator);
}

void enumerateSevenPairs(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::array<std::uint_fast8_t, 34u> &hand,
  Calculator const &calculator,
  ReferenceCalculator const &reference_calculator,
  std::uint_fast32_t &count)
{
  if (i > 34u) {
    throw std::logic_error("A logic error.");
  }
  if (m > 7u) {
    throw std::logic_error("A logic error.");
  }

  if (i == 34u) {
    if (m == 7u) {
      test(hand, calculator, reference_calculator);
      ++count;
      if (count % 1000u == 0u || count == size) {
        std::cerr << count << '/' << size << '\n';
      }
    }
    return;
  }

  enumerateSevenPairs(i + 1u, m, hand, calculator, reference_calculator, count);

  if (m < 7u) {
    hand[i] = 2u;
    enumerateSevenPairs(i + 1u, m + 1u, hand, calculator, reference_calculator, count);
    hand[i] = 0u;
  }
}

} // namespace <unnamed>

int main(int const argc, char const * const * const argv)
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <PATH TO MAP FILE>" << std::endl;
    return EXIT_FAILURE;
  }

  std::filesystem::path const map_path(argv[1]);
  if (!std::filesystem::exists(map_path)) {
    std::cerr << map_path.string() << " does not exist." << std::endl;
    return EXIT_FAILURE;
  }

  Py_InitializeEx(0);
  {
    std::array<std::uint_fast8_t, 34u> hand{};
    Calculator calculator(map_path);
    ReferenceCalculator reference_calculator;
    std::uint_fast32_t count = 0u;
    enumerateSevenPairs(0u, 0u, hand, calculator, reference_calculator, count);
  }
  Py_FinalizeEx();

  return EXIT_SUCCESS;
}
