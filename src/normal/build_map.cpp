#include <tsumonya/normal/hule_indexer.hpp>
#include <tsumonya/core.hpp>
#include <boost/python/import.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/long.hpp>
#include <boost/python/object.hpp>
#include <boost/timer/timer.hpp>
#include <boost/io/ios_state.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <vector>
#include <array>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include <cassert>


namespace{

using boost::timer::cpu_timer;
namespace python = boost::python;

using Tsumonya::Table;
using Tsumonya::stable;
using Tsumonya::mtable;
using Tsumonya::ntable;
using Tsumonya::xytable;
using Tsumonya::Hand;
using Tsumonya::ChiList;
using Tsumonya::PengList;
using Tsumonya::GangList;
using Tsumonya::encodeFuFan;
using Tsumonya::Normal::HuleIndexer;

using Map = std::vector<std::uint8_t>;

void dumpEntry(
    Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
    GangList const &angang_list, GangList const &minggang_list, std::ostream &os)
{
    os << "hand = ";
    for (unsigned h : hand) {
        os << h << ',';
    }
    os << '\n';

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
    Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
    GangList const &angang_list, GangList const &minggang_list,
    Map &map, std::uint_fast32_t &count, cpu_timer const &timer)
{
    constexpr bool debugging = false;

    Tsumonya::Normal::HuleIndexer indexer;

    std::uint_fast32_t const index = [&]() -> std::uint_fast32_t {
        try {
            std::uint_fast32_t const index = indexer(hand, chi_list, peng_list, angang_list, minggang_list);
            if (index == -1) {
                throw std::logic_error("Failed to index a hule.");
            }
            std::uint_fast32_t const e = Tsumonya::Normal::Base::e + Tsumonya::Normal::WindHead::e;
            if (index >= e) {
                std::ostringstream oss;
                oss << index << " (>= " << e << ")" << ": An out-of-bound index.";
                throw std::logic_error(oss.str());
            }
            return index;
        }
        catch (...) {
            dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
            throw;
        }
    }();

    bool const wind_head = index >= Tsumonya::Normal::Base::e;

    if (debugging) {
        dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cout);
        std::cout << "index = " << index << std::endl;
        std::cout << "wind head = " << wind_head << std::endl;
    }

    python::object m_constants = python::import("mahjong.constants");
    python::object m_hand_config = python::import("mahjong.hand_calculating.hand_config");
    python::object m_meld = python::import("mahjong.meld");
    python::object m_hand = python::import("mahjong.hand_calculating.hand");

    python::tuple args;
    python::dict kwargs;

    python::list tiles34;
    for (std::uint_fast8_t i = 0u; i < 34; ++i) {
        tiles34.append(hand[i]);
    }

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
            kwargs["meld_type"] = "chi";
            kwargs["tiles"] = mtiles;
            python::object meld = m_meld.attr("Meld")(*args, **kwargs);
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
            kwargs["meld_type"] = "pon";
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
            kwargs["meld_type"] = "kan";
            kwargs["tiles"] = mtiles;
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
            kwargs["meld_type"] = "kan";
            kwargs["tiles"] = mtiles;
            kwargs["opened"] = true;
            python::object meld = m_meld.attr("Meld")(*args, **kwargs);
            melds.append(meld);
        }
    }

    //if (debugging) {
    //    std::cout << "tiles34 = ";
    //    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
    //        std::cout << python::extract<long>(tiles34[i]) << ',';
    //    }
    //    std::cout << '\n';
    //}

    python::list tiles134;
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        for (std::uint_fast8_t j = 1u; j <= 4u; ++j) {
            if (tiles34[i] >= j) {
                tiles134.append(i * 4u + j - 1u);
            }
        }
    }
    //if (debugging) {
    //    std::cout << "tiles134 = ";
    //    for (std::uint_fast8_t i = 0u; i < python::len(tiles134); ++i) {
    //        std::cout << python::extract<long>(tiles134[i]) << ',';
    //    }
    //    std::cout << '\n';
    //}

    args = python::tuple();
    kwargs = python::dict();
    kwargs["has_open_tanyao"] = true;
    python::object optional_rules = m_hand_config.attr("OptionalRules")(*args, **kwargs);

    python::object hand_calculator = m_hand.attr("HandCalculator")();

    if (wind_head) {
        python::object east = m_constants.attr("EAST");
        python::object south = m_constants.attr("SOUTH");
        python::object west = m_constants.attr("WEST");
        python::object north = m_constants.attr("NORTH");

        std::array<python::object, 3u> round_winds = { east, south, west };
        std::array<python::object, 4u> player_winds = { east, south, west, north };

        for (std::uint_fast8_t round_wind_ = 0u; round_wind_ < round_winds.size(); ++round_wind_) {
            python::object round_wind = round_winds[round_wind_];

            for (std::uint_fast8_t player_wind_ = 0u; player_wind_ < player_winds.size(); ++player_wind_) {
                python::object player_wind = player_winds[player_wind_];

                std::uint_fast8_t wind_head_type = 0u;
                for (std::uint_fast8_t w = 0u; w < 4u; ++w) {
                    if (hand[27u + w] == 2u) {
                        if (round_wind_ == w && player_wind_ != w || round_wind_ != w && player_wind_ == w) {
                            wind_head_type = 1u;
                        }
                        if (round_wind_ == w && player_wind_ == w) {
                            wind_head_type = 2u;
                        }
                    }
                }

                for (std::uint_fast8_t i = 0u, hupai_index = 0u;; ++i, ++hupai_index) {
                    while (hupai_index < 34u && hand[hupai_index] == 0u) {
                        ++hupai_index;
                    }
                    if (hupai_index == 34u) {
                        break;
                    }

                    if (debugging) {
                        std::cout << "hupai index = " << static_cast<unsigned>(i) << std::endl;
                    }

                    python::long_ hupai(hupai_index * 4u);
                    if (debugging) {
                        //std::cout << "hupai = " << python::extract<long>(hupai) << '\n';
                    }

                    args = python::tuple();
                    kwargs = python::dict();
                    kwargs["round_wind"] = round_wind;
                    kwargs["player_wind"] = player_wind;
                    kwargs["is_tsumo"] = true;
                    kwargs["options"] = optional_rules;
                    python::object tsumo_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

                    args = python::make_tuple(tiles134, hupai);
                    kwargs = python::dict();
                    kwargs["melds"] = melds;
                    kwargs["config"] = tsumo_config;
                    python::object hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);
                    python::object zimo_fu_ = hand_response.attr("fu");
                    python::object zimo_fan_ = hand_response.attr("han");
                    if (zimo_fu_.is_none() || zimo_fan_.is_none()) {
                        dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                        python::object error_ = hand_response.attr("error");
                        std::string error = python::extract<std::string>(error_);
                        throw std::logic_error(error);
                    }
                    std::uint_fast8_t const zimo_fu = python::extract<long>(zimo_fu_);
                    std::uint_fast8_t const zimo_fan = python::extract<long>(zimo_fan_);
                    if (debugging) {
                        std::cout << "zimo: (" << static_cast<unsigned>(zimo_fu) << ", " << static_cast<unsigned>(zimo_fan) << ")\n";
                    }

                    args = python::tuple();
                    kwargs = python::dict();
                    kwargs["round_wind"] = round_wind;
                    kwargs["player_wind"] = player_wind;
                    kwargs["options"] = optional_rules;
                    python::object rong_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

                    args = python::make_tuple(tiles134, hupai);
                    kwargs = python::dict();
                    kwargs["melds"] = melds;
                    kwargs["config"] = rong_config;
                    hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);
                    python::object rong_fu_ = hand_response.attr("fu");
                    python::object rong_fan_ = hand_response.attr("han");
                    if (rong_fu_.is_none() || rong_fan_.is_none()) {
                        dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                        python::object error_ = hand_response.attr("error");
                        std::string error = python::extract<std::string>(error_);
                        throw std::logic_error(error);
                    }
                    std::uint_fast8_t const rong_fu = python::extract<long>(rong_fu_);
                    std::uint_fast8_t const rong_fan = python::extract<long>(rong_fan_);
                    if (debugging) {
                        std::cout << "rong: (" << static_cast<unsigned>(rong_fu) << ", "
                                  << static_cast<unsigned>(rong_fan) << ")\n";
                    }

                    std::uint_fast64_t const map_index = [&]() -> std::uint_fast64_t{
                        std::uint_fast64_t map_index = index;
                        map_index = map_index * 3u + wind_head_type;
                        map_index = map_index * 13u + i;
                        map_index *= 2u;
                        return Tsumonya::Normal::Base::e * 13u * 2u + map_index;
                    }();
                    if (map_index >= map.size()) {
                        dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                        std::ostringstream oss;
                        oss << map_index << ": An out-of-bound index.";
                        throw std::logic_error(oss.str());
                    }
                    if (map[map_index] == 0u && map[map_index + 1u] == 0u) {
                        map[map_index] = encodeFuFan(zimo_fu, zimo_fan);
                        map[map_index + 1u] = encodeFuFan(rong_fu, rong_fan);
                    }
                    else {
                        if (encodeFuFan(zimo_fu, zimo_fan) != map[map_index]) {
                            dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                            throw std::logic_error("Indices are in conflict.");
                        }
                        if (encodeFuFan(rong_fu, rong_fan) != map[map_index + 1u]) {
                            dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                            throw std::logic_error("Indices are in conflict.");
                        }
                    }
                }
            }
        }
    }
    else {
        for (std::uint_fast8_t i = 0u, hupai_index = 0u;; ++i, ++hupai_index) {
            while (hupai_index < 34u && hand[hupai_index] == 0u) {
                ++hupai_index;
            }
            if (hupai_index == 34u) {
                break;
            }

            if (debugging) {
                std::cout << "hupai index = " << static_cast<unsigned>(i) << std::endl;
            }

            python::long_ hupai(hupai_index * 4u);
            if (debugging) {
                //std::cout << "hupai = " << python::extract<long>(hupai) << '\n';
            }

            args = python::tuple();
            kwargs = python::dict();
            kwargs["is_tsumo"] = true;
            kwargs["options"] = optional_rules;
            python::object tsumo_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

            args = python::make_tuple(tiles134, hupai);
            kwargs = python::dict();
            kwargs["melds"] = melds;
            kwargs["config"] = tsumo_config;
            python::object hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);
            python::object zimo_fu_ = hand_response.attr("fu");
            python::object zimo_fan_ = hand_response.attr("han");
            if (zimo_fu_.is_none() || zimo_fan_.is_none()) {
                dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                python::object error_ = hand_response.attr("error");
                std::string error = python::extract<std::string>(error_);
                throw std::logic_error(error);
            }
            std::uint_fast8_t const zimo_fu = python::extract<long>(zimo_fu_);
            std::uint_fast8_t const zimo_fan = python::extract<long>(zimo_fan_);
            if (debugging) {
                std::cout << "zimo: (" << static_cast<unsigned>(zimo_fu) << ", " << static_cast<unsigned>(zimo_fan) << ")\n";
            }

            args = python::tuple();
            kwargs = python::dict();
            kwargs["options"] = optional_rules;
            python::object rong_config = m_hand_config.attr("HandConfig")(*args, **kwargs);

            args = python::make_tuple(tiles134, hupai);
            kwargs = python::dict();
            kwargs["melds"] = melds;
            kwargs["config"] = rong_config;
            hand_response = hand_calculator.attr("estimate_hand_value")(*args, **kwargs);
            python::object rong_fu_ = hand_response.attr("fu");
            python::object rong_fan_ = hand_response.attr("han");
            if (rong_fu_.is_none() || rong_fan_.is_none()) {
                dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                python::object error_ = hand_response.attr("error");
                std::string error = python::extract<std::string>(error_);
                throw std::logic_error(error);
            }
            std::uint_fast8_t const rong_fu = python::extract<long>(rong_fu_);
            std::uint_fast8_t const rong_fan = python::extract<long>(rong_fan_);
            if (debugging) {
                std::cout << "rong: (" << static_cast<unsigned>(rong_fu) << ", "
                          << static_cast<unsigned>(rong_fan) << ")\n";
            }

            std::uint_fast64_t const map_index = [&]() -> std::uint_fast64_t{
                std::uint_fast64_t map_index = index;
                map_index = map_index * 13u + i;
                map_index *= 2u;
                return map_index;
            }();
            if (map_index >= Tsumonya::Normal::Base::e * 13u * 2u) {
                dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                std::ostringstream oss;
                oss << index << " * " << 13u << " + " << static_cast<unsigned>(i)
                    << " = " << map_index << ": An out-of-bound index.";
                throw std::logic_error(oss.str());
            }
            if (map[map_index] == 0u && map[map_index + 1u] == 0u) {
                map[map_index] = encodeFuFan(zimo_fu, zimo_fan);
                map[map_index + 1u] = encodeFuFan(rong_fu, rong_fan);
            }
            else {
                if (encodeFuFan(zimo_fu, zimo_fan) != map[map_index]) {
                    dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                    throw std::logic_error("Indices are in conflict.");
                }
                if (encodeFuFan(rong_fu, rong_fan) != map[map_index + 1u]) {
                    dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                    throw std::logic_error("Indices are in conflict.");
                }
            }
        }
    }
    if (debugging) {
        std::cout << "========================================" << std::endl;
    }

    ++count;
    if (count % 1000u == 0u) {
        boost::io::ios_all_saver ias(std::cout);
        std::cout << count << " (";
        boost::timer::nanosecond_type wall_clock_ = timer.elapsed().wall;
        double wall_clock = static_cast<double>(wall_clock_) / 1000000000;
        double eta = (wall_clock / count) * (Tsumonya::Normal::Base::e + Tsumonya::Normal::WindHead::e);
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

void enumerateNormalHule(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const x, std::uint_fast8_t const y, Hand &hand, ChiList &chi_list,
    PengList &peng_list, GangList &angang_list, GangList &minggang_list,
    Map &map, std::uint_fast32_t &count, cpu_timer const &timer)
{
    assert((i <= 34u));
    assert((m <= 4u));
    assert((h <= 1u));
    for (std::uint_fast8_t const x : hand) {
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

    if (i == 34u) {
        assert((x == 0u));
        assert((y == 0u));
        if (m == 4u && h == 1u) {
            createEntry(
                hand, chi_list, peng_list, angang_list, minggang_list, map, count, timer);
        }
        return;
    }

    std::uint_fast8_t const color = i / 9u;
    std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
    std::uint_fast8_t const cindex = color * 7u + number;
    bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
        if (m + mtable[s] > 4u) {
            continue;
        }
        if (h + stable[s][0u] > 1u) {
            continue;
        }
        if (shunzi_prohibited && xytable[s] > 0u) {
            continue;
        }
        if (ntable[s] + x > 4u) {
            continue;
        }
        if (xytable[s] + y > 4u) {
            continue;
        }

        hand[i] += 2u * stable[s][0u] + 3u * stable[s][1u] + stable[s][2u];
        if (!shunzi_prohibited) {
            hand[i + 1u] += stable[s][2u];
            hand[i + 2u] += stable[s][2u];
            assert((cindex < 21u));
            chi_list[cindex] += stable[s][3u];
        }
        peng_list[i] += stable[s][4u];
        angang_list[i] += stable[s][5u];
        minggang_list[i] += stable[s][6u];

        enumerateNormalHule(
            i + 1u, m + mtable[s], h + stable[s][0u], xytable[s] + y, xytable[s],
            hand, chi_list, peng_list, angang_list, minggang_list, map, count, timer);

        angang_list[i] -= stable[s][5u];
        minggang_list[i] -= stable[s][6u];
        peng_list[i] -= stable[s][4u];
        if (!shunzi_prohibited) {
            assert((cindex < 21u));
            chi_list[cindex] -= stable[s][3u];
            hand[i + 2u] -= stable[s][2u];
            hand[i + 1u] -= stable[s][2u];
        }
        hand[i] -= 2u * stable[s][0u] + 3u * stable[s][1u] + stable[s][2u];
    }
}

} // namespace `anonymous`

int main()
{
    Py_InitializeEx(0);

    Hand hand = {
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    ChiList chi_list = {
        0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    PengList peng_list = {
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    GangList angang_list = {
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    GangList minggang_list = {
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };

    std::uint_fast64_t const map_size
        = static_cast<std::uint_fast64_t>(Tsumonya::Normal::Base::e) * 13u * 2u
        + static_cast<std::uint_fast64_t>(Tsumonya::Normal::WindHead::e) * 3u * 13u * 2u;
    std::cout << map_size << std::endl;
    Map map(map_size, 0u);
    std::uint_fast32_t count = 0u;
    cpu_timer timer;

    enumerateNormalHule(
        0u, 0u, 0u, 0u, 0u, hand, chi_list, peng_list, angang_list, minggang_list,
        map, count, timer);

    {
        std::ofstream ofs("src/normal/map.bin", std::ios_base::out | std::ios_base::binary);
        for (std::uint_fast8_t const encode : map) {
            ofs << encode;
        }
    }

    Py_FinalizeEx();
}
