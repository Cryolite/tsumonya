#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "reference_calculator.hpp"

#include <boost/python/import.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>


namespace Tsumonya::Detail_{

namespace{

namespace python = boost::python;

} // namespace <anonymous>

class ReferenceCalculator::Impl_
{
public:
  Impl_()
    : m_constants_(python::import("mahjong.constants"))
    , m_yaku_config_(python::import("mahjong.hand_calculating.yaku_config"))
    , m_hand_config_(python::import("mahjong.hand_calculating.hand_config"))
    , m_meld_(python::import("mahjong.meld"))
    , m_hand_(python::import("mahjong.hand_calculating.hand"))
  {}

  Impl_(Impl_ const &) = delete;

  Impl_(Impl_ &&) = delete;

  Impl_ &operator=(Impl_ const &) = delete;

  Impl_ &operator=(Impl_ &&) = delete;

  std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
    std::uint_fast8_t round_wind,
    std::uint_fast8_t player_wind,
    std::array<std::uint_fast8_t, 34u> const &pure_hand,
    std::array<std::uint_fast8_t, 21u> const &chi_list,
    std::array<std::uint_fast8_t, 34u> const &peng_list,
    std::array<std::uint_fast8_t, 34u> const &angang_list,
    std::array<std::uint_fast8_t, 34u> const &minggang_list,
    std::uint_fast8_t winning_tile,
    bool rong) const
  {
    if (round_wind != UINT_FAST8_MAX && (round_wind < 27u || round_wind >= 31u)) {
      throw std::invalid_argument("An invalid `round_wind`.");
    }
    if (player_wind != UINT_FAST8_MAX && (player_wind < 27u || player_wind >= 31u)) {
      throw std::invalid_argument("An invalid `player_wind`.");
    }
    if ((round_wind == UINT_FAST8_MAX) != (player_wind == UINT_FAST8_MAX)) {
      throw std::invalid_argument("An invalid combination of `round_wind` and `player_wind`.");
    }
    if (winning_tile >= 34u) {
      throw std::invalid_argument("An invalid `winning_tile`.");
    }

    python::tuple args;
    python::dict kwargs;

    python::list tiles34;
    for (std::uint_fast8_t i = 0u; i < 34; ++i) {
      tiles34.append(pure_hand[i]);
    }
    tiles34[winning_tile] += 1u;

    python::list melds;
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
        kwargs["meld_type"] = m_meld_.attr("Meld").attr("CHI");
        kwargs["tiles"] = mtiles;
        python::object meld = m_meld_.attr("Meld")(*args, **kwargs);
        melds.append(meld);
      }
    }
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
        kwargs["meld_type"] = m_meld_.attr("Meld").attr("PON");
        kwargs["tiles"] = mtiles;
        python::object meld = m_meld_.attr("Meld")(*args, **kwargs);
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
        kwargs["meld_type"] = m_meld_.attr("Meld").attr("KAN");
        kwargs["tiles"] = mtiles;
        kwargs["opened"] = false;
        python::object meld = m_meld_.attr("Meld")(*args, **kwargs);
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
        kwargs["meld_type"] = m_meld_.attr("Meld").attr("KAN");
        kwargs["tiles"] = mtiles;
        kwargs["opened"] = true;
        python::object meld = m_meld_.attr("Meld")(*args, **kwargs);
        melds.append(meld);
      }
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
    python::object optional_rules = m_hand_config_.attr("OptionalRules")(*args, **kwargs);

    args = python::tuple();
    kwargs = python::dict();
    kwargs["is_tsumo"] = !rong;
    if (player_wind != UINT_FAST8_MAX) {
      kwargs["player_wind"] = player_wind;
    }
    if (round_wind != UINT_FAST8_MAX) {
      kwargs["round_wind"] = round_wind;
    }
    kwargs["options"] = optional_rules;
    python::object hand_config = m_hand_config_.attr("HandConfig")(*args, **kwargs);

    python::object hand_calculator = m_hand_.attr("HandCalculator")();
    args = python::make_tuple(tiles136, winning_tile * 4u);
    kwargs = python::dict();
    kwargs["melds"] = melds;
    kwargs["config"] = hand_config;
    python::object hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);

    std::uint_fast8_t const fu = [&]() -> std::uint_fast8_t {
      python::object fu_details = hand_response.attr("fu_details");
      if (fu_details.is_none()) {
        return 0u;
      }
      std::uint_fast8_t fu = 0u;
      for (long i = 0; i < python::len(fu_details); ++i) {
        python::object fu_detail = fu_details[i];
        python::object fu_ = fu_detail["fu"];
        fu += python::extract<long>(fu_);
      }
      return fu;
    }();

    std::uint_fast8_t const fan_encode = [&]() -> std::uint_fast8_t {
      python::object fan_ = hand_response.attr("han");
      if (fan_.is_none()) {
        if (fu != 0u) {
          throw std::logic_error("A logic error.");
        }
        return 0u;
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

      // 平和形か平和に準じる形（場風・自風を指定していない場合で雀頭が東・南・西・北のいずれかの場合，
      // 常に平和が役として含まれる）である場合を示すフラグ．
      bool const pinghu = [&]() {
        python::object yaku_list = hand_response.attr("yaku");
        if (yaku_list.is_none()) {
          return false;
        }
        for (long i = 0; i < python::len(yaku_list); ++i) {
          python::object yaku = yaku_list[i];
          if (yaku.attr("name") == "Pinfu") {
            return true;
          }
        }
        return false;
      }();
      return pinghu ? (fan + 64u) : fan;
    }();

    return {fu, fan_encode};
  }

private:
  python::object m_constants_;
  python::object m_yaku_config_;
  python::object m_hand_config_;
  python::object m_meld_;
  python::object m_hand_;
}; // class ReferenceCalculator::Impl_

ReferenceCalculator::ReferenceCalculator()
  : p_impl_()
{
  if (Py_IsInitialized() == 0) {
    throw std::runtime_error("Python is not initialized.");
  }
  p_impl_ = std::make_shared<Impl_>();
}

ReferenceCalculator::ReferenceCalculator(ReferenceCalculator const &) noexcept = default;

ReferenceCalculator::ReferenceCalculator(ReferenceCalculator &&other) noexcept
  : ReferenceCalculator(other)
{}

ReferenceCalculator &ReferenceCalculator::operator=(ReferenceCalculator const &) noexcept = default;

ReferenceCalculator &ReferenceCalculator::operator=(ReferenceCalculator &&other) noexcept
{
  return *this = other;
}

std::pair<std::uint_fast8_t, std::uint_fast8_t> ReferenceCalculator::operator()(
  std::uint_fast8_t round_wind,
  std::uint_fast8_t player_wind,
  std::array<std::uint_fast8_t, 34u> const &pure_hand,
  std::array<std::uint_fast8_t, 21u> const &chi_list,
  std::array<std::uint_fast8_t, 34u> const &peng_list,
  std::array<std::uint_fast8_t, 34u> const &angang_list,
  std::array<std::uint_fast8_t, 34u> const &minggang_list,
  std::uint_fast8_t winning_tile,
  bool rong) const
{
  return (*p_impl_)(
    round_wind,
    player_wind,
    pure_hand,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    winning_tile,
    rong);
}

} // namespace Tsumonya::Detail_
