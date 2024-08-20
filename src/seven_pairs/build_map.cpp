// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "../../tsumonya/seven_pairs/hash.hpp"
#include "../../tsumonya/seven_pairs/table.hpp"
#include "../../tsumonya/seven_pairs/core.hpp"
#include <boost/python/import.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/long.hpp>
#include <boost/python/object.hpp>
#include <boost/timer/timer.hpp>
#include <boost/io/ios_state.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <vector>
#include <array>
#include <functional>
#include <utility>
#include <stdexcept>
#include <climits>
#include <cstdint>
#include <cassert>


namespace{

using boost::timer::cpu_timer;
namespace python = boost::python;

using Tsumonya::SevenPairs_::size;
using Tsumonya::SevenPairs_::getHash;
using Tsumonya::SevenPairs_::Map;

void createEntry(
  std::array<std::uint_fast8_t, 34u> const &hand,
  Map &map,
  std::uint_fast32_t &count,
  cpu_timer const &timer)
{
  std::uint_fast8_t const winning_tile = [&]() -> std::uint_fast8_t {
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
      if (hand[i] == 2u) {
        return i;
      }
    }
    throw std::logic_error("A logic error.");
  }();

  std::uint_fast32_t const hash = getHash(hand);
  if (hash == UINT_FAST32_MAX) {
    throw std::logic_error("A logic error.");
  }
  if (hash >= size) {
    throw std::logic_error("A logic error.");
  }

  python::object m_constants = python::import("mahjong.constants");
  python::object m_yaku_config = python::import("mahjong.hand_calculating.yaku_config");
  python::object m_hand_config = python::import("mahjong.hand_calculating.hand_config");
  python::object m_meld = python::import("mahjong.meld");
  python::object m_hand = python::import("mahjong.hand_calculating.hand");

  python::tuple args;
  python::dict kwargs;

  python::list tiles34;
  for (std::uint_fast8_t i = 0u; i < 34; ++i) {
    tiles34.append(hand[i]);
  }

  python::list tiles136;
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    for (std::uint_fast8_t j = 1u; j <= 4u; ++j) {
      if (tiles34[i] >= j) {
        tiles136.append(i * 4u + j - 1u);
      }
    }
  }

  args = python::tuple();
  kwargs = python::dict();
  kwargs["has_open_tanyao"] = true;
  python::object optional_rules = m_hand_config.attr("OptionalRules")(*args, **kwargs);

  args = python::tuple();
  kwargs = python::dict();
  kwargs["is_tsumo"] = false;
  kwargs["options"] = optional_rules;
  python::object hand_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

  python::object hand_calculator = m_hand.attr("HandCalculator")();
  args = python::make_tuple(tiles136, winning_tile * 4u);
  kwargs = python::dict();
  kwargs["config"] = hand_config;
  python::object hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);

  //   0 <= fan < 128
  // 128 <= fan < 256
  std::uint_fast8_t const fan = [&]() -> std::uint_fast8_t {
    python::object fan_ = hand_response.attr("han");
    if (fan_.is_none()) {
      python::object error_ = hand_response.attr("error");
      std::string error = python::extract<std::string>(error_);
      throw std::logic_error(error);
    }

    std::uint_fast8_t const fan = python::extract<long>(fan_);

    // 本役満（数え役満を除く役満）かどうかのフラグ．
    bool const true_yiman_flag = [&]() -> bool {
      python::object yaku_list = hand_response.attr("yaku");
      if (yaku_list.is_none()) {
        return false;
      }
      for (long i = 0; i < python::len(yaku_list); ++i) {
        python::object yaku = yaku_list[i];
        if (yaku.attr("name") == "Daisangen") {
          return true;
        }
        if (yaku.attr("name") == "Suu Ankou") {
          return true;
        }
        if (yaku.attr("name") == "Tsuu Iisou") {
          return true;
        }
        if (yaku.attr("name") == "Ryuuiisou") {
          return true;
        }
        if (yaku.attr("name") == "Chinroutou") {
          return true;
        }
        if (yaku.attr("name") == "Shousuushii") {
          return true;
        }
        if (yaku.attr("name") == "Suu Kantsu") {
          return true;
        }
        if (yaku.attr("name") == "Chuuren Poutou") {
          return true;
        }
        if (yaku.attr("name") == "Daburu Chuuren Poutou") {
          return true;
        }
        if (yaku.attr("name") == "Suu Ankou Tanki") {
          return true;
        }
        if (yaku.attr("name") == "Dai Suushii") {
          return true;
        }
      }
      return false;
    }();
    if (true_yiman_flag) {
      // 本役満（数え役満を除く役満）の場合．
      //   - fan = 128: 役満
      //   - fan = 129: 二倍役満
      //   - fan = 130: 三倍役満
      //   - fan = 131: 四倍役満
      //   - fan = 132: 五倍役満
      //   - fan = 133: 六倍役満
      if (fan < 13u) {
        throw std::logic_error("A logic error.");
      }
      if (fan % 13u != 0u) {
        throw std::logic_error("A logic error.");
      }
      return 128u + (fan - 13u) / 13u;
    }

    return fan;
  }();

  std::uint_fast8_t fu = [&]() -> std::uint_fast8_t {
    python::object fu_details = hand_response.attr("fu_details");
    if (fu_details.is_none()) {
      throw std::logic_error("A logic error.");
    }
    std::uint_fast8_t fu = 0u;
    for (long i = 0; i < python::len(fu_details); ++i) {
      python::object fu_detail = fu_details[i];
      python::object fu_ = fu_detail["fu"];
      fu += python::extract<long>(fu_);
    }
    return fu;
  }();

  bool const flag = [&]() -> bool {
    python::object yaku_list = hand_response.attr("yaku");
    if (yaku_list.is_none()) {
      throw std::logic_error("A logic error.");
    }
    for (long i = 0; i < python::len(yaku_list); ++i) {
      python::object yaku = yaku_list[i];
      if (yaku.attr("name") == "Ryanpeikou") {
        return true;
      }
    }
    return false;
  }();

  if (fan < 128u && !flag && fu != 25u) {
    throw std::logic_error("A logic error.");
  }

  if (map[hash].first == UINT_FAST8_MAX) {
    assert((map[hash].second == UINT_FAST8_MAX));
    if (fu == 25u) {
      map[hash].first = fu;
      map[hash].second = fan;
    }
  }
  else {
    throw std::logic_error("A logic error.");
  }

  ++count;
  if (count % 10000u == 0u) {
    boost::io::ios_all_saver ias(std::cout);
    std::cout << count << " (";
    boost::timer::nanosecond_type wall_clock_ = timer.elapsed().wall;
    double wall_clock = static_cast<double>(wall_clock_) / 1000000000;
    double eta = (wall_clock / count) * size;
    std::int_least64_t const wall_clock_in_days = wall_clock / (60.0 * 60.0 * 24.0);
    wall_clock -= wall_clock_in_days * 60 * 60 * 24;
    std::int_least64_t const wall_clock_in_hours = wall_clock / (60.0 * 60.0);
    wall_clock -= wall_clock_in_hours * 60 * 60;
    std::int_least64_t const wall_clock_in_minutes = wall_clock / 60.0;
    wall_clock -= wall_clock_in_minutes * 60;
    if (wall_clock_in_days > 0) {
      std::cout << wall_clock_in_days << " days ";
    }
    std::cout << std::setfill('0') << std::setw(2) << wall_clock_in_hours << ':'
              << std::setfill('0') << std::setw(2) << wall_clock_in_minutes << ':'
              << std::setfill('0') << std::setw(2) << static_cast<int>(wall_clock) << " / ";
    std::int_least64_t const eta_in_days = eta / (60.0 * 60.0 * 24.0);
    eta -= eta_in_days * 60 * 60 * 24;
    std::int_least64_t const eta_in_hours = eta / (60.0 * 60.0);
    eta -= eta_in_hours * 60 * 60;
    std::int_least64_t const eta_in_minutes = eta / 60.0;
    eta -= eta_in_minutes * 60;
    if (eta_in_days > 0) {
      std::cout << eta_in_days << " days ";
    }
    std::cout << std::setfill('0') << std::setw(2) << eta_in_hours << ':'
              << std::setfill('0') << std::setw(2) << eta_in_minutes << ':'
              << std::setfill('0') << std::setw(2) << static_cast<int>(eta) << ')' << std::endl;
  }
}

void enumerateSevenPairs(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::array<std::uint_fast8_t, 34u> &hand,
  Map &map,
  std::uint_fast32_t &count,
  cpu_timer const &timer)
{
  if (i == 34u) {
    if (m == 7u) {
      createEntry(hand, map, count, timer);
    }
    return;
  }

  enumerateSevenPairs(i + 1u, m, hand, map, count, timer);

  if (m < 7u) {
    hand[i] = 2u;
    enumerateSevenPairs(i + 1u, m + 1u, hand, map, count, timer);
    hand[i] = 0u;
  }
}

} // namespace <anonymous>

int main(int const argc, char const * const * const argv)
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <PATH TO MAP FILE>" << std::endl;
    return EXIT_FAILURE;
  }

  std::filesystem::path const path(argv[1u]);

  Py_InitializeEx(0);

  std::array<std::uint_fast8_t, 34u> hand{};
  Map map(size, Map::value_type(UINT8_MAX, UINT8_MAX));
  std::uint_fast64_t count = 0u;
  cpu_timer timer;

  enumerateSevenPairs(0u, 0u, hand, map, count, timer);

  {
    std::ofstream ofs(path, std::ios_base::out | std::ios_base::binary);
    if (!ofs) {
      throw std::runtime_error("Failed to open the map file.");
    }
    for (auto const [fu, fan] : map) {
      ofs << fu << fan;
    }
  }

  Py_FinalizeEx();
}
