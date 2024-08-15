#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../../src/standard/sample_winning_hand.hpp"
#include "../../src/standard/winning_hand_callback.hpp"
#include "../../tsumonya/calculator.hpp"
#include "../../tsumonya/standard/table.hpp"
#include <boost/python/import.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <functional>
#include <array>
#include <cstdint>
#include <cstdlib>


namespace {

namespace python = boost::python;

using Tsumonya::Standard_::upper_bound;
using Tsumonya::Standard_::getHash;
using Tsumonya::Standard_::sampleWinningHand;

using PureHand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengGangList = std::array<std::uint_fast8_t, 34u>;

std::mt19937 createRandomNumberEngine()
{
  std::random_device rd;
  std::array<std::mt19937::result_type, std::mt19937::state_size> seed_data;
  std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
  std::seed_seq seq(seed_data.cbegin(), seed_data.cend());
  return std::mt19937(seq);
}

void dumpEntry(
  Tsumonya::Wind const round_wind,
  Tsumonya::Wind const player_wind,
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::ostream &os)
{
  os << "round_wind = " << static_cast<unsigned>(round_wind) - 27u << '\n';
  os << "player_wind = " << static_cast<unsigned>(player_wind) - 27u << '\n';
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

void test(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  std::mt19937 &random_number_engine,
  Tsumonya::Calculator const &calculator,
  std::size_t &count)
{
  Tsumonya::Wind const round_wind = [&]() -> Tsumonya::Wind {
    std::uniform_int_distribution<std::uint_fast8_t> wind_distribution(0u, 3u);
    if (wind_distribution(random_number_engine) == 0u) {
      return Tsumonya::east;
    }
    if (wind_distribution(random_number_engine) == 1u) {
      return Tsumonya::south;
    }
    if (wind_distribution(random_number_engine) == 2u) {
      return Tsumonya::west;
    }
    return Tsumonya::north;
  }();
  Tsumonya::Wind const player_wind = [&]() -> Tsumonya::Wind {
    std::uniform_int_distribution<std::uint_fast8_t> wind_distribution(0u, 3u);
    if (wind_distribution(random_number_engine) == 0u) {
      return Tsumonya::east;
    }
    if (wind_distribution(random_number_engine) == 1u) {
      return Tsumonya::south;
    }
    if (wind_distribution(random_number_engine) == 2u) {
      return Tsumonya::west;
    }
    return Tsumonya::north;
  }();
  Tsumonya::Situation const situation = rong ? Tsumonya::rong : Tsumonya::zimo;

  auto const [fu_our, fan_our] = [&]() -> std::pair<std::uint_fast8_t, std::uint_fast8_t> {
    std::array<std::uint_fast8_t, 34u> pure_hand_copy(pure_hand);
    if (winning_tile >= 34u) {
      std::logic_error("A logic error.");
    }
    if (pure_hand_copy[winning_tile] == 0u) {
      std::logic_error("A logic error.");
    }
    --pure_hand_copy[winning_tile];
    return calculator(
      round_wind,
      player_wind,
      pure_hand_copy,
      chi_list,
      peng_list,
      angang_list,
      minggang_list,
      winning_tile,
      situation,
      0u);
  }();

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
  kwargs["player_wind"] = static_cast<std::uint_fast8_t>(player_wind);
  kwargs["round_wind"] = static_cast<std::uint_fast8_t>(round_wind);
  kwargs["options"] = optional_rules;
  python::object hand_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

  python::object hand_calculator = m_hand.attr("HandCalculator")();
  args = python::make_tuple(tiles134, winning_tile * 4u);
  kwargs = python::dict();
  kwargs["melds"] = melds;
  kwargs["config"] = hand_config;
  python::object hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);

  std::uint_fast8_t fu_their = [&]() -> std::uint_fast8_t {
    python::object fu_ = hand_response.attr("fu");
    if (fu_.is_none()) {
      dumpEntry(
        round_wind,
        player_wind,
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        rong,
        std::cerr);
      throw std::logic_error("A logic error.");
    }

    std::uint_fast8_t const fu = python::extract<long>(fu_);
    return fu;
  }();

  std::uint_fast8_t const fan_their = [&]() -> std::uint_fast8_t {
    python::object fan_ = hand_response.attr("han");
    if (fan_.is_none()) {
      dumpEntry(
        round_wind,
        player_wind,
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        rong,
        std::cerr);
      python::object error_ = hand_response.attr("error");
      std::string error = python::extract<std::string>(error_);
      throw std::logic_error(error);
    }

    std::uint_fast8_t const fan = python::extract<long>(fan_);
    return fan;
  }();

  if (((fu_our + 9u) / 10u) * 10u != fu_their || fan_our != fan_their) {
    if (fan_their <= 13u) {
      dumpEntry(
        round_wind,
        player_wind,
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        rong,
        std::cout);
      std::cout << "Our: " << static_cast<unsigned>(fu_our) << " fu, "
        << static_cast<unsigned>(fan_our) << " fan" << std::endl;
      std::cout << "Their: " << static_cast<unsigned>(fu_their) << " fu, "
        << static_cast<unsigned>(fan_their) << " fan" << std::endl;

      python::object fu_details = hand_response.attr("fu_details");
      for (long i = 0; i < python::len(fu_details); ++i) {
        python::object fu_detail = fu_details[i];
        python::object reason_ = fu_detail["reason"];
        std::string reason = python::extract<std::string>(reason_);
        python::object fu_ = fu_detail["fu"];
        std::cout << reason << ": " << python::extract<long>(fu_) << std::endl;
      }
    }
  }

  ++count;
  if (count % 1000u == 0u) {
    std::cout << count << " samples have been tested." << std::endl;
  }
}

} // namespace <anonymous>

int main(int const argc, char const * const * const argv)
{
  Py_InitializeEx(0);

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <PATH TO MAP FILE> <# OF TEST SAMPLES>" << std::endl;
    return EXIT_FAILURE;
  }

  std::filesystem::path const map_file_path(argv[1]);
  if (!std::filesystem::exists(map_file_path)) {
    std::cerr << map_file_path.string() << ": Does not exist." << std::endl;
    return EXIT_FAILURE;
  }

  long long const num_samples = boost::lexical_cast<long long>(argv[2]);
  if (num_samples <= 0) {
    std::cerr << "The number of test samples must be a positive integer." << std::endl;
    return EXIT_FAILURE;
  }

  std::mt19937 random_number_engine = createRandomNumberEngine();

  Tsumonya::Calculator calculator(map_file_path);

  std::size_t count = 0u;

  Tsumonya::Standard_::WinningHandCallback callback = std::bind_back(
    &test, std::ref(random_number_engine), std::cref(calculator), std::ref(count));

  for (std::size_t i = 0u; i < num_samples; ++i) {
    sampleWinningHand(callback, random_number_engine);
  }

  Py_FinalizeEx();
  return EXIT_SUCCESS;
}
