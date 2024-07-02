// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#include "winning_hand_enumeration.hpp"
#include <tsumonya/standard/hash.hpp>
#include <tsumonya/standard/core.hpp>
#include <boost/python/import.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/long.hpp>
#include <boost/python/object.hpp>
#include <boost/timer/timer.hpp>
#include <boost/io/ios_state.hpp>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <vector>
#include <array>
#include <utility>
#include <stdexcept>
#include <climits>
#include <cstdint>
#include <cassert>


namespace{

using boost::timer::cpu_timer;
namespace python = boost::python;

using Tsumonya::Standard_::stable;
using Tsumonya::Standard_::mtable;
using Tsumonya::Standard_::ntable;
using Tsumonya::Standard_::xytable;
using Tsumonya::Standard_::upper_bound;
using Tsumonya::Standard_::getHash;
using Tsumonya::Standard_::WinningHandCallback;
using Tsumonya::Standard_::enumerateWinningHands;

using PureHand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengGangList = std::array<std::uint_fast8_t, 34u>;
using Map = std::vector<std::pair<std::uint8_t, std::uint8_t>>;

std::mutex mtx;

void dumpEntry(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::ostream &os)
{
  os << "hand = ";
  for (unsigned const t : pure_hand) {
    os << t << ',';
  }
  os << " (" << static_cast<unsigned>(winning_tile) << ", "
     << (!rong ? "zimo)" : "rong)") << '\n';

  os << "chi = ";
  for (unsigned c : chi_list) {
    os << c << ',';
  }
  os << '\n';

  os << "peng = ";
  for (unsigned p : peng_list) {
    os << p << ',';
  }
  os << '\n';

  os << "angang = ";
  for (unsigned ag : angang_list) {
    os << ag << ',';
  }
  os << '\n';

  os << "minggang = ";
  for (unsigned mg : minggang_list) {
    os << mg << ',';
  }
  os << std::endl;
}

void createEntry(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  Map &map,
  std::uint_fast64_t &count,
  cpu_timer const &timer)
{
  constexpr bool debugging = false;

  std::lock_guard<std::mutex> lock(mtx);

  std::uint_fast64_t const hash = [&]() -> std::uint_fast64_t {
    try {
      PureHand pure_hand_copy(pure_hand);
      assert((pure_hand_copy[winning_tile] >= 1u));
      --pure_hand_copy[winning_tile];
      std::uint_fast64_t const hash = getHash(
        pure_hand_copy, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
      if (hash == UINT_FAST64_MAX) {
        throw std::logic_error("Failed to get a hule.");
      }
      if (hash >= upper_bound) {
        std::ostringstream oss;
        oss << hash << " (>= " << upper_bound << ")" << ": An out-of-bound hash.";
        throw std::logic_error(oss.str());
      }
      return hash;
    }
    catch (...) {
      dumpEntry(
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        rong,
        std::cerr);
      throw;
    }
  }();

  if (debugging) {
    dumpEntry(
      pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong, std::cout);
    std::cout << "hash = " << hash << std::endl;
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
    tiles34.append(pure_hand[i]);
  }

  python::list melds;
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    if (peng_list[i] == 1u) {
      // `tiles34` must contain fulus' tiles.
      tiles34[i] += 3u;

      python::list mtiles;
      for (std::uint_fast8_t j = 0u; j < 3u; ++j) {
        mtiles.append(i * 4u + j);
      }

      args = python::tuple();
      kwargs = python::dict();
      kwargs["meld_type"] = m_meld.attr("Meld").attr("PON");
      kwargs["tiles"] = mtiles;
      python::object meld = m_meld.attr("Meld")(*args, **kwargs);
      melds.append(meld);
    }
  }
  for (std::uint_fast8_t i = 0u; i < 21u; ++i) {
    for (std::uint_fast8_t j = 0u; j < chi_list[i]; ++j) {
      std::uint_fast8_t const color = i / 7u;
      std::uint_fast8_t const first_number = i % 7u;
      std::uint_fast8_t const first_tile = color * 9u + first_number;

      // `tiles34` must contain fulus' tiles.
      for (std::uint_fast8_t k = 0u; k < 3u; ++k) {
        tiles34[first_tile + k] += 1u;
      }

      python::list mtiles;
      for (std::uint_fast8_t k = 0u; k < 3u; ++k) {
        mtiles.append((first_tile + k) * 4u);
      }

      args = python::tuple();
      kwargs = python::dict();
      kwargs["meld_type"] = m_meld.attr("Meld").attr("CHI");
      kwargs["tiles"] = mtiles;
      python::object meld = m_meld.attr("Meld")(*args, **kwargs);
      melds.append(meld);
    }
  }
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    if (angang_list[i] == 1u) {
      // `tiles34` must contain fulus' tiles.
      tiles34[i] += 3u;

      python::list mtiles;
      for (std::uint_fast8_t j = 0u; j < 4u; ++j) {
        mtiles.append(i * 4u + j);
      }

      args = python::tuple();
      kwargs = python::dict();
      kwargs["meld_type"] = m_meld.attr("Meld").attr("KAN");
      kwargs["tiles"] = mtiles;
      kwargs["opened"] = false;
      python::object meld = m_meld.attr("Meld")(*args, **kwargs);
      melds.append(meld);
    }
  }
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    if (minggang_list[i] == 1u) {
      // `tiles34` must contain fulus' tiles.
      tiles34[i] += 3u;

      python::list mtiles;
      for (std::uint_fast8_t j = 0u; j < 4u; ++j) {
        mtiles.append(i * 4u + j);
      }

      args = python::tuple();
      kwargs = python::dict();
      kwargs["meld_type"] = m_meld.attr("Meld").attr("KAN");
      kwargs["tiles"] = mtiles;
      kwargs["opened"] = true;
      python::object meld = m_meld.attr("Meld")(*args, **kwargs);
      melds.append(meld);
    }
  }

  python::list tiles134;
  for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    for (std::uint_fast8_t j = 1u; j <= 4u; ++j) {
      if (tiles34[i] >= j) {
        tiles134.append(i * 4u + j - 1u);
      }
    }
  }

  args = python::tuple();
  kwargs = python::dict();
  kwargs["has_open_tanyao"] = true;
  python::object optional_rules = m_hand_config.attr("OptionalRules")(*args, **kwargs);

  args = python::tuple();
  kwargs = python::dict();
  kwargs["is_tsumo"] = !rong;
  kwargs["options"] = optional_rules;
  python::object hand_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

  python::object hand_calculator = m_hand.attr("HandCalculator")();
  args = python::make_tuple(tiles134, winning_tile * 4u);
  kwargs = python::dict();
  kwargs["melds"] = melds;
  kwargs["config"] = hand_config;
  python::object hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);

  std::uint_fast8_t fu = 0u;
  {
    python::object fu_details = hand_response.attr("fu_details");
    if (fu_details.is_none()) {
      dumpEntry(
        pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong, std::cerr);
      throw std::logic_error("A logic error.");
    }
    for (long i = 0; i < python::len(fu_details); ++i) {
      python::object fu_detail = fu_details[i];
      python::object fu_ = fu_detail["fu"];
      fu += python::extract<long>(fu_);
    }
  }

  std::uint_fast8_t const fan = [&]() -> std::uint_fast8_t {
    python::object fan_ = hand_response.attr("han");
    if (fan_.is_none()) {
      dumpEntry(
        pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong, std::cerr);
      python::object error_ = hand_response.attr("error");
      std::string error = python::extract<std::string>(error_);
      throw std::logic_error(error);
    }

    std::uint_fast8_t const fan = python::extract<long>(fan_);
    if (fan <= 12u) {
      return fan;
    }

    bool kazoe_flag = true;
    python::object yaku_config = m_yaku_config.attr("YakuConfig")();
    python::object yaku_list = hand_response.attr("yaku");
    for (long i = 0; i < python::len(yaku_list); ++i) {
      python::object yaku = yaku_list[i];
      if (yaku == yaku_config.attr("daisangen")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("suuankou")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("tsuisou")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("ryuisou")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("chinroto")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("shosuushi")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("suukantsu")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("chuuren_poutou")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("daburu_chuuren_poutou")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("suuankou_tanki")) {
        kazoe_flag = false;
        break;
      }
      if (yaku == yaku_config.attr("daisuushi")) {
        kazoe_flag = false;
        break;
      }
    }
    if (kazoe_flag) {
      return 13u;
    }

    if (fan % 13u != 0) {
      dumpEntry(
        pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong, std::cerr);
      throw std::logic_error("A logic error.");
    }
    return 13u + fan / 13u;
  }();

  if (debugging) {
    std::cout << "fu = " << static_cast<unsigned>(fu)
              << ", fan = " << static_cast<unsigned>(fan) << ")\n";
  }

  if (map[hash].first == UINT_FAST8_MAX) {
    assert((map[hash].second == UINT_FAST8_MAX));
    map[hash].first = fu;
    map[hash].second = fan;
  }
  else {
    assert((map[hash].second != UINT_FAST8_MAX));
    if (fu != map[hash].first || fan != map[hash].second) {
      dumpEntry(
        pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong, std::cerr);
      std::ostringstream oss;
      oss << hash << ": Indices are in conflict.";
      throw std::logic_error(oss.str());
    }
  }
  if (debugging) {
    std::cout << "========================================" << std::endl;
  }

  ++count;
  if (count % 10000u == 0u) {
    boost::io::ios_all_saver ias(std::cout);
    std::cout << count << " (";
    boost::timer::nanosecond_type wall_clock_ = timer.elapsed().wall;
    double wall_clock = static_cast<double>(wall_clock_) / 1000000000;
    double eta = (wall_clock / count) * upper_bound;
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

} // namespace <anonymous>

int main(int const argc, char const * const * const argv)
{
  if (argc < 2) {
    throw std::runtime_error("Too few arguments.");
  }
  if (argc > 2)
  {
    throw std::runtime_error("Too many arguments.");
  }

  std::filesystem::path const path(argv[1u]);
  
  Py_InitializeEx(0);

  Map map(upper_bound, Map::value_type(UINT8_MAX, UINT8_MAX));
  std::uint_fast64_t count = 0u;
  cpu_timer timer;

  WinningHandCallback callback(
    std::bind_back(&createEntry, std::ref(map), std::ref(count), std::ref(timer)));
  enumerateWinningHands(callback, 8u);

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
