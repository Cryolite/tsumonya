// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD)
#define TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD

#include "standard/hash.hpp"
#include "standard/core.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ios>
#include <algorithm>
#include <iterator>
#include <string>
#include <array>
#include <utility>
#include <memory>
#include <stdexcept>
#include <climits>
#include <cstdint>
#include <cstddef>
#include <cassert>


namespace Tsumonya{

enum struct Wind
  : std::uint_fast8_t
{
  east_  = 27u,
  south_ = 28u,
  west_  = 29u,
  north_ = 30u,
}; // enum struct Wind

inline constexpr Wind east(Wind::east_);
inline constexpr Wind south(Wind::south_);
inline constexpr Wind west(Wind::west_);
inline constexpr Wind north(Wind::north_);

enum struct Situation
  : std::uint_fast16_t
{
  zimo_             = 1u <<  1u,
  rong_             = 1u <<  2u,
  liqi_             = 1u <<  3u,
  qianggang_        = 1u <<  4u,
  lingshang_kaihua_ = 1u <<  5u,
  haidi_moyue_      = 1u <<  6u,
  haidi_laoyue_     = 1u <<  7u,
  double_liqi_      = 1u <<  8u,
  yifa_             = 1u <<  9u,
  tianhu_           = 1u << 10u,
  dihu_             = 1u << 11u
}; // enum struct Situation

inline constexpr Situation zimo(Situation::zimo_);
inline constexpr Situation rong(Situation::rong_);
inline constexpr Situation liqi(Situation::liqi_);
inline constexpr Situation qianggang(Situation::qianggang_);
inline constexpr Situation lingshang_kaihua(Situation::lingshang_kaihua_);
inline constexpr Situation haidi_moyue(Situation::haidi_moyue_);
inline constexpr Situation haidi_laoyue(Situation::haidi_laoyue_);
inline constexpr Situation double_liqi(Situation::double_liqi_);
inline constexpr Situation yifa(Situation::yifa_);
inline constexpr Situation tianhu(Situation::tianhu_);
inline constexpr Situation dihu(Situation::dihu_);

constexpr Situation operator&(Situation const lhs, Situation const rhs)
{
  return static_cast<Situation>(
    static_cast<std::uint_fast16_t>(lhs) & static_cast<std::uint_fast16_t>(rhs));
}

constexpr Situation operator|(Situation const lhs, Situation const rhs)
{
  return static_cast<Situation>(
    static_cast<std::uint_fast16_t>(lhs) | static_cast<std::uint_fast16_t>(rhs));
}

constexpr Situation &operator&=(Situation &lhs, Situation const rhs)
{
  lhs = lhs & rhs;
  return lhs;
}

constexpr Situation &operator|=(Situation &lhs, Situation const rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

class Calculator
{
private:
  class Impl_
  {
  public:
    Impl_() = delete;

    Impl_(std::filesystem::path const &map_path, bool const lian_feng_pai_as_2fu)
      : map_()
      , lian_feng_pai_as_2fu_(lian_feng_pai_as_2fu)
    {
      if (!std::filesystem::exists(map_path)) {
        std::ostringstream oss;
        oss << map_path.string() << ": Does not exist.";
        throw std::runtime_error(oss.str());
      }
      if (!std::filesystem::is_regular_file(map_path) && !std::filesystem::is_symlink(map_path)) {
        std::ostringstream oss;
        oss << map_path.string() << ": Not a file.";
        throw std::runtime_error(oss.str());
      }

      using MapValue = Tsumonya::Standard_::Map::value_type;

      {
        std::size_t const map_size = std::filesystem::file_size(map_path);
        map_.resize(map_size / sizeof(MapValue));
      }

      std::ifstream ifs(map_path, std::ios::in | std::ios::binary);
      if (!ifs) {
        std::ostringstream oss;
        oss << map_path.string() << ": Failed to open.";
        throw std::runtime_error(oss.str());
      }
      ifs.read(reinterpret_cast<char *>(map_.data()), map_.size() * sizeof(MapValue));
      if (!ifs) {
        std::ostringstream oss;
        oss << map_path.string() << ": Failed to read.";
        throw std::runtime_error(oss.str());
      }
      if (ifs.gcount() != static_cast<std::streamsize>(map_.size() * sizeof(MapValue))) {
        std::ostringstream oss;
        oss << map_path.string() << ": Failed to read.";
        throw std::runtime_error(oss.str());
      }
      if (ifs.peek() != std::char_traits<char>::eof()) {
        std::ostringstream oss;
        oss << map_path.string() << ": Failed to read.";
        throw std::runtime_error(oss.str());
      }
    }

    Impl_(Impl_ const &) = delete;

    Impl_(Impl_ &&) = delete;

    Impl_ &operator=(Impl_ const &) = delete;

    Impl_ &operator=(Impl_ &&) = delete;

    std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
      Wind const round_wind,
      Wind const player_wind,
      std::array<std::uint_fast8_t, 34u> const &pure_hand,
      std::array<std::uint_fast8_t, 21u> const &chi_list,
      std::array<std::uint_fast8_t, 34u> const &peng_list,
      std::array<std::uint_fast8_t, 34u> const &angang_list,
      std::array<std::uint_fast8_t, 34u> const &minggang_list,
      std::uint_fast8_t const winning_tile,
      Situation const situation,
      std::uint_fast8_t const num_doras) const
    {
      std::uint_fast64_t const hash = Tsumonya::Standard_::getHash(
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        static_cast<std::uint_fast16_t>(situation & rong) != 0u);
      if (hash == UINT_FAST32_MAX) {
        return {0u, 0u};
      }

      auto const [fu_base, fan_encode] = map_[hash];

      bool const tianhu_ = static_cast<std::uint_fast16_t>(situation & tianhu) != 0u;
      bool const dihu_ = static_cast<std::uint_fast16_t>(situation & dihu) != 0u;
      if (fan_encode >= 128u || tianhu_ || dihu_) {
        // 本役満（数え役満を除く役満）の場合．
        if (fan_encode < 128u) {
          // 天和のみ，もしくは地和のみの場合．
          assert((tianhu_ || dihu_));
          return {fu_base, 13u};
        }
        assert((fan_encode <= 133u));
        return {fu_base, 13u + (fan_encode - 128u) * 13u + (tianhu_ || dihu_) ? 13u : 0u};
      }

      auto const [fan_base, pinghu_flag] = [&]() -> std::pair<std::uint_fast8_t, bool> {
        if (fan_encode >= 64u) {
          return {fan_encode - 64u, true};
        }
        return {fan_encode, false};
      }();

      std::array<std::uint_fast8_t, 34u> const pure_hand_ = [&]() {
        std::array<std::uint_fast8_t, 34u> pure_hand_(pure_hand);
        ++pure_hand_[winning_tile];
        return pure_hand_;
      }();

      std::uint_fast8_t const round_wind_tile = static_cast<std::uint_fast8_t>(round_wind);
      std::uint_fast8_t const player_wind_tile = static_cast<std::uint_fast8_t>(player_wind);
      std::uint_fast8_t const feng_pai_fu = [&]() -> std::uint_fast8_t {
        if (!lian_feng_pai_as_2fu_ && round_wind_tile == player_wind_tile && pure_hand_[round_wind_tile] == 2u) {
          return 4u;
        }
        if (pure_hand_[round_wind_tile] == 2u) {
          return 2u;
        }
        if (pure_hand_[player_wind_tile] == 2u) {
          return 2u;
        }
        return 0u;
      }();
      std::uint_fast8_t const feng_pai_fan = [&]() -> std::uint_fast8_t {
        std::uint_fast8_t feng_pai_fan = 0u;
        if (pure_hand_[round_wind_tile] == 3u) {
          ++feng_pai_fan;
        }
        if (pure_hand_[player_wind_tile] == 3u) {
          ++feng_pai_fan;
        }
        if (peng_list[round_wind_tile] == 1u) {
          ++feng_pai_fan;
        }
        if (peng_list[player_wind_tile] == 1u) {
          ++feng_pai_fan;
        }
        if (angang_list[round_wind_tile] == 1u) {
          ++feng_pai_fan;
        }
        if (angang_list[player_wind_tile] == 1u) {
          ++feng_pai_fan;
        }
        if (minggang_list[round_wind_tile] == 1u) {
          ++feng_pai_fan;
        }
        if (minggang_list[player_wind_tile] == 1u) {
          ++feng_pai_fan;
        }
        return feng_pai_fan;
      }();

      std::uint_fast8_t const conditional_fans = [&]() {
        std::uint_fast8_t conditional_fans = 0u;
        if (static_cast<std::uint_fast16_t>(situation & liqi) != 0u) {
          ++conditional_fans;
        }
        if (static_cast<std::uint_fast16_t>(situation & qianggang) != 0u) {
          ++conditional_fans;
        }
        if (static_cast<std::uint_fast16_t>(situation & lingshang_kaihua) != 0u) {
          ++conditional_fans;
        }
        if (static_cast<std::uint_fast16_t>(situation & haidi_moyue) != 0u) {
          ++conditional_fans;
        }
        if (static_cast<std::uint_fast16_t>(situation & haidi_laoyue) != 0u) {
          ++conditional_fans;
        }
        if (static_cast<std::uint_fast16_t>(situation & double_liqi) != 0u) {
          conditional_fans += 2u;
        }
        if (static_cast<std::uint_fast16_t>(situation & yifa) != 0u) {
          ++conditional_fans;
        }
        return conditional_fans;
      }();

      std::uint_fast8_t const fan = fan_base + feng_pai_fan + conditional_fans + num_doras;

      if (!pinghu_flag || feng_pai_fu == 0u) {
        return {fu_base + feng_pai_fu, std::min<std::uint_fast8_t>(fan, 13u)};
      }

      assert((fan >= 1u));
      // 平和に準じる和了形ではあるが，雀頭が風牌であるため役から平和を除外する．
      return {fu_base + feng_pai_fu, std::min<std::uint_fast8_t>(fan - 1u, 13u)};
    }

  private:
    Tsumonya::Standard_::Map map_;
    bool lian_feng_pai_as_2fu_;
  }; // class Impl_

public:
  Calculator() = delete;

  explicit Calculator(char const * const map_path, bool const lian_feng_pai_as_2fu = false)
    : Calculator(std::filesystem::path(map_path), lian_feng_pai_as_2fu)
  {}

  explicit Calculator(std::string const &map_path, bool const lian_feng_pai_as_2fu = false)
    : Calculator(std::filesystem::path(map_path), lian_feng_pai_as_2fu)
  {}

  explicit Calculator(
    std::filesystem::path const &map_path, bool const lian_feng_pai_as_2fu = false)
    : p_impl_(std::make_shared<Impl_>(map_path, lian_feng_pai_as_2fu))
  {}

  Calculator(Calculator const &) = default;

  Calculator(Calculator &&rhs)
    : p_impl_(rhs.p_impl_)
  {}

  Calculator &operator=(Calculator const &) = default;

  Calculator &operator=(Calculator &&rhs)
  {
    p_impl_ = rhs.p_impl_;
    return *this;
  }

  template<
    typename PureHandIterator,
    typename ChiListIterator,
    typename PengListIterator,
    typename AngangListIterator,
    typename MinggangListIterator>
  std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
    Wind const round_wind,
    Wind const player_wind,
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
    std::uint_fast8_t const winning_tile,
    Situation const situation,
    std::uint_fast8_t const num_doras) const
  {
    // Check whether `round_wind` is valid.
    if (round_wind != east && round_wind != south && round_wind != west && round_wind != north) {
      throw std::invalid_argument("An invalid `round_wind`.");
    }

    // Check whether `player_wind` is valid.
    if (player_wind != east && player_wind != south && player_wind != west && player_wind != north) {
      throw std::invalid_argument("An invalid `player_wind`.");
    }

    std::array<std::uint_fast8_t, 34u> pure_hand = [&]() {
      // Check whether `[pure_hand_first, pure_hand_last)` is valid.
      std::array<std::uint_fast8_t, 34u> pure_hand;
      std::uint_fast8_t i = 0u;
      std::uint_fast8_t n = 0u;
      while (pure_hand_first != pure_hand_last) {
        if (i >= 34u) {
          throw std::invalid_argument(
            "An invalid pure hand: `std::distance(pure_hand_first, pure_hand_last)` exceeds 34.");
        }
        long long const num_tiles = *pure_hand_first++;
        if (num_tiles < 0) {
          std::ostringstream oss;
          oss << "An invalid pure hand: A negative number of tiles (`"
            << num_tiles << "`) at `" << static_cast<unsigned>(i) << "`.";
          throw std::invalid_argument(oss.str());
        }
        if (num_tiles > 4) {
          std::ostringstream oss;
          oss << "An invalid pure hand: The number of tiles at `" << static_cast<unsigned>(i)
            << "` exceeds 4 (`" << static_cast<unsigned>(num_tiles) << "`).";
          throw std::invalid_argument(oss.str());
        }
        pure_hand[i] = static_cast<std::uint_fast8_t>(num_tiles);
        n += num_tiles;
        if (n > 14u) {
          throw std::invalid_argument(
            "An invalid pure hand: The total number of tiles exceeds 14.");
        }
        ++i;
      }
      if (i != 34u) {
        std::ostringstream oss;
        oss << "An invalid pure hand: `std::distance(pure_hand_first, pure_hand_last)` is `"
          << static_cast<unsigned>(i) << "`.";
        throw std::invalid_argument(oss.str());
      }
      if (n % 3u != 1u) {
        throw std::invalid_argument(
          "An invalid pure hand: The total number of tiles in the pure hand is not 1 mod 3.");
      }
      return pure_hand;
    }();

    std::array<std::uint_fast8_t, 21u> chi_list = [&]() -> std::array<std::uint_fast8_t, 21u> {
      // Check whether `[chi_list_first, chi_list_last)` is valid.
      std::array<std::uint_fast8_t, 21u> chi_list;
      std::uint_fast8_t i = 0u;
      std::uint_fast8_t n = 0u;
      while (chi_list_first != chi_list_last) {
        if (i >= 21u) {
          throw std::invalid_argument(
            "An invalid chi list: `std::distance(chi_list_first, chi_list_last)` exceeds 21.");
        }
        long long const num_chi = *chi_list_first++;
        if (num_chi < 0) {
          std::ostringstream oss;
          oss << "An invalid chi list: A negative number of chi (`"
            << num_chi << "`) at `" << static_cast<unsigned>(i) << "`.";
          throw std::invalid_argument(oss.str());
        }
        if (num_chi > 4) {
          std::ostringstream oss;
          oss << "An invalid chi list: The number of chi at `" << static_cast<unsigned>(i)
            << "` exceeds 4 (`" << static_cast<unsigned>(num_chi) << "`).";
          throw std::invalid_argument(oss.str());
        }
        chi_list[i] = static_cast<std::uint_fast8_t>(num_chi);
        n += num_chi;
        if (n > 4u) {
          throw std::invalid_argument("An invalid chi list: The total number of chi exceeds 4.");
        }
        ++i;
      }
      if (i != 21u) {
        std::ostringstream oss;
        oss << "An invalid chi list: `std::distance(chi_list_first, chi_list_last)` is `"
          << static_cast<unsigned>(i) << "`.";
        throw std::invalid_argument(oss.str());
      }
      if (n > 4u) {
        std::ostringstream oss;
        oss << "An invalid chi list: The total number of chi exceeds 4 (`"
          << static_cast<unsigned>(n) << "`).";
        throw std::invalid_argument(oss.str());
      }
      return chi_list;
    }();

    std::array<std::uint_fast8_t, 34u> peng_list = [&]() -> std::array<std::uint_fast8_t, 34u> {
      // Check whether `[peng_list_first, peng_list_last)` is valid.
      std::array<std::uint_fast8_t, 34u> peng_list;
      std::uint_fast8_t i = 0u;
      std::uint_fast8_t n = 0u;
      while (peng_list_first != peng_list_last) {
        if (i >= 34u) {
          throw std::invalid_argument(
            "An invalid peng list: `std::distance(peng_list_first, peng_list_last)` exceeds 34.");
        }
        long long const num_peng = *peng_list_first++;
        if (num_peng < 0) {
          std::ostringstream oss;
          oss << "An invalid peng list: A negative number of peng (`"
            << num_peng << "`) at `" << static_cast<unsigned>(i) << "`.";
          throw std::invalid_argument(oss.str());
        }
        if (num_peng > 1) {
          std::ostringstream oss;
          oss << "An invalid peng list: The number of peng at `" << static_cast<unsigned>(i)
            << "` exceeds 1 (`" << static_cast<unsigned>(num_peng) << "`).";
          throw std::invalid_argument(oss.str());
        }
        peng_list[i] = static_cast<std::uint_fast8_t>(num_peng);
        n += num_peng;
        if (n > 4u) {
          throw std::invalid_argument("An invalid peng list: The total number of peng exceeds 4.");
        }
        ++i;
      }
      if (i != 34u) {
        std::ostringstream oss;
        oss << "An invalid peng list: `std::distance(peng_list_first, peng_list_last)` is `"
          << static_cast<unsigned>(i) << "`.";
        throw std::invalid_argument(oss.str());
      }
      if (n > 4u) {
        std::ostringstream oss;
        oss << "An invalid peng list: The total number of peng exceeds 4 (`"
          << static_cast<unsigned>(n) << "`).";
        throw std::invalid_argument(oss.str());
      }
      return peng_list;
    }();

    std::array<std::uint_fast8_t, 34u> angang_list = [&]() -> std::array<std::uint_fast8_t, 34u> {
      // Check whether `[angang_list_first, angang_list_last)` is valid.
      std::array<std::uint_fast8_t, 34u> angang_list;
      std::uint_fast8_t i = 0u;
      std::uint_fast8_t n = 0u;
      while (angang_list_first != angang_list_last) {
        if (i >= 34u) {
          throw std::invalid_argument(
            "An invalid angang list: `std::distance(angang_list_first, angang_list_last)` "
            "exceeds 34.");
        }
        long long const num_angang = *angang_list_first++;
        if (num_angang < 0) {
          std::ostringstream oss;
          oss << "An invalid angang list: A negative number of angang (`"
            << num_angang << "`) at `" << static_cast<unsigned>(i) << "`.";
          throw std::invalid_argument(oss.str());
        }
        if (num_angang > 1) {
          std::ostringstream oss;
          oss << "An invalid angang list: The number of angang at `" << static_cast<unsigned>(i)
            << "` exceeds 1 (`" << static_cast<unsigned>(num_angang) << "`).";
          throw std::invalid_argument(oss.str());
        }
        angang_list[i] = static_cast<std::uint_fast8_t>(num_angang);
        n += num_angang;
        if (n > 4u) {
          throw std::invalid_argument(
            "An invalid angang list: The total number of angang exceeds 4.");
        }
        ++i;
      }
      if (i != 34u) {
        std::ostringstream oss;
        oss << "An invalid angang list: `std::distance(angang_list_first, angang_list_last)` is `"
          << static_cast<unsigned>(i) << "`.";
        throw std::invalid_argument(oss.str());
      }
      if (n > 4u) {
        std::ostringstream oss;
        oss << "An invalid angang list: The total number of angang exceeds 4 (`"
          << static_cast<unsigned>(n) << "`).";
        throw std::invalid_argument(oss.str());
      }
      return angang_list;
    }();

    std::array<std::uint_fast8_t, 34u> minggang_list = [&]() -> std::array<std::uint_fast8_t, 34u> {
      // Check whether `[minggang_list_first, minggang_list_last)` is valid.
      std::array<std::uint_fast8_t, 34u> minggang_list;
      std::uint_fast8_t i = 0u;
      std::uint_fast8_t n = 0u;
      while (minggang_list_first != minggang_list_last) {
        if (i >= 34u) {
          throw std::invalid_argument(
            "An invalid minggang list: `std::distance(minggang_list_first, minggang_list_last)` "
            "exceeds 34.");
        }
        long long const num_minggang = *minggang_list_first++;
        if (num_minggang < 0) {
          std::ostringstream oss;
          oss << "An invalid minggang list: A negative number of minggang (`"
            << num_minggang << "`) at `" << static_cast<unsigned>(i) << "`.";
          throw std::invalid_argument(oss.str());
        }
        if (num_minggang > 1) {
          std::ostringstream oss;
          oss << "An invalid minggang list: The number of minggang at `"
            << static_cast<unsigned>(i) << "` exceeds 1 (`" << static_cast<unsigned>(num_minggang)
            << "`).";
          throw std::invalid_argument(oss.str());
        }
        minggang_list[i] = static_cast<std::uint_fast8_t>(num_minggang);
        n += num_minggang;
        if (n > 4u) {
          throw std::invalid_argument(
            "An invalid minggang list: The total number of minggang exceeds 4.");
        }
        ++i;
      }
      if (i != 34u) {
        std::ostringstream oss;
        oss << "An invalid minggang list: `std::distance(minggang_list_first, minggang_list_last)`"
          " is `" << static_cast<unsigned>(i) << "`.";
        throw std::invalid_argument(oss.str());
      }
      if (n > 4u) {
        std::ostringstream oss;
        oss << "An invalid minggang list: The total number of minggang exceeds 4 (`"
          << static_cast<unsigned>(n) << "`).";
        throw std::invalid_argument(oss.str());
      }
      return minggang_list;
    }();

    {
      // Check whether the hand is valid.
      std::uint_fast8_t a = 0u;
      std::uint_fast8_t b = 0u;
      std::uint_fast8_t n = 0u;
      for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        std::uint_fast8_t const color = i / 9u;
        std::uint_fast8_t const number = color != 3u ? i % 9u : UINT_FAST8_MAX;
        std::uint_fast8_t const chi_index = (color != 3u && number < 7u) ? color * 7u + number : UINT_FAST8_MAX;
        if (number == 0u || color == 3u) {
          a = 0u;
          b = 0u;
        }
        std::uint_fast8_t const num_chi = chi_index != UINT_FAST8_MAX ? chi_list[chi_index] : 0u;
        if (std::uint_fast8_t const num_tiles = a + pure_hand[i] + num_chi + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i]; num_tiles > 4u) {
          std::ostringstream oss;
          oss << "An invalid hand: The number of tile `" << static_cast<unsigned>(i)
            << "` exceeds 4 (" << static_cast<unsigned>(num_tiles) << ").";
          throw std::invalid_argument(oss.str());
        }
        n += a + pure_hand[i] + num_chi + 3u * peng_list[i] + 3u * angang_list[i] + 3u * minggang_list[i];
        a = b + num_chi;
        b = num_chi;
      }
      if (n != 13u) {
        throw std::invalid_argument("An invalid hand.");
      }
    }

    // Check whether `winning_tile` is valid.
    if (winning_tile >= 34u) {
      std::ostringstream oss;
      oss << "An invalid winning tile: `" << static_cast<unsigned>(winning_tile) << "`.";
      throw std::invalid_argument(oss.str());
    }
    if (pure_hand[winning_tile] == 4u) {
      throw std::invalid_argument(
        "An invalid winning tile: `[pure_hand_first, pure_hand_last)` "
        "must not contain the winning tile.");
    }

    {
      bool const zimo_ = static_cast<std::uint_fast16_t>(situation & zimo) != 0u;
      bool const rong_ = static_cast<std::uint_fast16_t>(situation & rong) != 0u;
      bool const liqi_ = static_cast<std::uint_fast16_t>(situation & liqi) != 0u;
      bool const qianggang_ = static_cast<std::uint_fast16_t>(situation & qianggang) != 0u;
      bool const lingshang_kaihua_
        = static_cast<std::uint_fast16_t>(situation & lingshang_kaihua) != 0u;
      bool const haidi_moyue_ = static_cast<std::uint_fast16_t>(situation & haidi_moyue) != 0u;
      bool const haidi_laoyue_ = static_cast<std::uint_fast16_t>(situation & haidi_laoyue) != 0u;
      bool const double_liqi_ = static_cast<std::uint_fast16_t>(situation & double_liqi) != 0u;
      bool const yifa_ = static_cast<std::uint_fast16_t>(situation & yifa) != 0u;
      bool const tianhu_ = static_cast<std::uint_fast16_t>(situation & tianhu) != 0u;
      bool const dihu_ = static_cast<std::uint_fast16_t>(situation & dihu) != 0u;
      if (!zimo_ && !rong_) {
        throw std::invalid_argument(
          "Either `Tsumonya::zimo` or `Tsumonya::rong` must be specified.");
      }
      if (zimo_ && rong_) {
        throw std::invalid_argument("`Tsumonya::zimo` and `Tsumonya::rong` conflict.");
      }
      if (zimo_ && qianggang_) {
        throw std::invalid_argument("`Tsumonya::zimo` and `Tsumonya::qianggang` conflict.");
      }
      if (zimo_ && haidi_laoyue_) {
        throw std::invalid_argument("`Tsumonya::zimo` and `Tsumonya::haidi_laoyue` conflict.");
      }
      if (rong_ && lingshang_kaihua_) {
        throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::lingshang_kaihua` conflict.");
      }
      if (rong_ && haidi_moyue_) {
        throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::haidi_moyue` conflict.");
      }
      if (rong_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::tianhu` conflict.");
      }
      if (rong_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::dihu` conflict.");
      }
      if (liqi_ && double_liqi_) {
        throw std::invalid_argument("`Tsumonya::liqi` and `Tsumonya::double_liqi` conflict.");
      }
      if (liqi_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::liqi` and `Tsumonya::tianhu` conflict.");
      }
      if (liqi_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::liqi` and `Tsumonya::dihu` conflict.");
      }
      if (qianggang_ && lingshang_kaihua_) {
        throw std::invalid_argument(
          "`Tsumonya::qianggang` and `Tsumonya::lingshang_kaihua` conflict.");
      }
      if (qianggang_ && haidi_moyue_) {
        throw std::invalid_argument("`Tsumonya::qianggang` and `Tsumonya::haidi_moyue` conflict.");
      }
      if (qianggang_ && haidi_laoyue_) {
        throw std::invalid_argument(
          "`Tsumonya::qianggang` and `Tsumonya::haidi_laoyue` conflict.");
      }
      if (qianggang_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::qianggang` and `Tsumonya::tianhu` conflict.");
      }
      if (qianggang_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::qianggang` and `Tsumonya::dihu` conflict.");
      }
      if (lingshang_kaihua_ && haidi_moyue_) {
        throw std::invalid_argument(
          "`Tsumonya::lingshang_kaihua` and `Tsumonya::haidi_moyue` conflict.");
      }
      if (lingshang_kaihua_ && haidi_laoyue_) {
        throw std::invalid_argument(
          "`Tsumonya::lingshang_kaihua` and `Tsumonya::haidi_laoyue` conflict.");
      }
      if (lingshang_kaihua_ && tianhu_) {
        throw std::invalid_argument(
          "`Tsumonya::lingshang_kaihua` and `Tsumonya::tianhu` conflict.");
      }
      if (lingshang_kaihua_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::lingshang_kaihua` and `Tsumonya::dihu` conflict.");
      }
      if (haidi_moyue_ && haidi_laoyue_) {
        throw std::invalid_argument(
          "`Tsumonya::haidi_moyue` and `Tsumonya::haidi_laoyue` conflict.");
      }
      if (haidi_moyue_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::haidi_moyue` and `Tsumonya::tianhu` conflict.");
      }
      if (haidi_moyue_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::haidi_moyue` and `Tsumonya::dihu` conflict.");
      }
      if (haidi_laoyue_ && yifa_) {
        throw std::invalid_argument("`Tsumonya::haidi_laoyue` and `Tsumonya::yifa` conflict.");
      }
      if (haidi_laoyue_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::haidi_laoyue` and `Tsumonya::tianhu` conflict.");
      }
      if (haidi_laoyue_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::haidi_laoyue` and `Tsumonya::dihu` conflict.");
      }
      if (double_liqi_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::double_liqi` and `Tsumonya::tianhu` conflict.");
      }
      if (double_liqi_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::double_liqi` and `Tsumonya::dihu` conflict.");
      }
      if (yifa_ && tianhu_) {
        throw std::invalid_argument("`Tsumonya::yifa` and `Tsumonya::tianhu` conflict.");
      }
      if (yifa_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::yifa` and `Tsumonya::dihu` conflict.");
      }
      if (tianhu_ && dihu_) {
        throw std::invalid_argument("`Tsumonya::tianhu` and `Tsumonya::dihu` conflict.");
      }
    }

    return (*p_impl_)(
      round_wind,
      player_wind,
      pure_hand,
      chi_list,
      peng_list,
      angang_list,
      minggang_list,
      winning_tile,
      situation,
      num_doras);
  }

  template<
    typename PureHandRange,
    typename ChiListRange,
    typename PengListRange,
    typename AngangListRange,
    typename MinggangListRange>
  std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
    Wind const round_wind,
    Wind const player_wind,
    PureHandRange const &pure_hand,
    ChiListRange const &chi_list,
    PengListRange const &peng_list,
    AngangListRange const &angang_list,
    MinggangListRange const &minggang_list,
    std::uint_fast8_t const winning_tile,
    Situation const situation,
    std::uint_fast8_t const num_doras) const
  {
    return operator()(
      round_wind,
      player_wind,
      std::cbegin(pure_hand),
      std::cend(pure_hand),
      std::cbegin(chi_list),
      std::cend(chi_list),
      std::cbegin(peng_list),
      std::cend(peng_list),
      std::cbegin(angang_list),
      std::cend(angang_list),
      std::cbegin(minggang_list),
      std::cend(minggang_list),
      winning_tile,
      situation,
      num_doras);
  }

private:
  std::shared_ptr<Impl_ const> p_impl_;
}; // class Calculator

} // namespace Tsumonya

#endif // !defined(TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD)
