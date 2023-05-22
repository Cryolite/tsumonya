#include <tsumonya/hule_indexer.hpp>
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
using Tsumonya::HuleIndexer;

using Hand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengList = std::array<std::uint_fast8_t, 34u>;
using GangList = std::array<std::uint_fast8_t, 34u>;
using Map = std::vector<std::pair<std::uint8_t, std::uint8_t>>;

std::uint_fast8_t encodeFuFan(std::uint_fast8_t const fu, std::uint_fast8_t const fan)
{
    std::uint_fast8_t fu_encode = 0u;
    if (fu == 20u) {
        fu_encode = 1u;
    }
    else if (fu == 25u) {
        fu_encode = 2u;
    }
    else if (fu == 30u) {
        fu_encode = 3u;
    }
    else if (fu == 40u) {
        fu_encode = 4u;
    }
    else if (fu == 50u) {
        fu_encode = 5u;
    }
    else if (fu == 60u) {
        fu_encode = 6u;
    }
    else if (fu == 70u) {
        fu_encode = 7u;
    }
    else if (fu == 80u) {
        fu_encode = 8u;
    }
    else if (fu == 90u) {
        fu_encode = 9u;
    }
    else if (fu == 100u) {
        fu_encode = 10u;
    }
    else if (fu == 110u) {
        fu_encode = 11u;
    }
    else {
        throw std::invalid_argument("An invalid fu.");
    }

    std::uint_fast8_t fan_encode = 0u;
    if (fan <= 12u) {
        fan_encode = fan;
    }
    else if (13u <= fan && fan < 26u) {
        // 役満
        fan_encode = 13u;
    }
    else if (26u <= fan && fan < 39u) {
        // 二倍役満
        fan_encode = 14u;
    }
    else if (39u <= fan && fan < 52u) {
        // 三倍役満
        fan_encode = 15u;
    }
    else if (52u <= fan && fan < 65u) {
        // 四倍役満
        fan_encode = 16u;
    }
    else if (65u <= fan && fan < 78u) {
        // 五倍役満
        fan_encode = 17u;
    }
    else if (fan >= 78u) {
        // 六倍役満
        fan_encode = 18u;
    }

    return fu_encode * 19u + fan_encode;
}

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
    HuleIndexer const &indexer, Map &map, std::uint_fast32_t &count, cpu_timer const &timer)
{
    constexpr bool debugging = false;

    std::uint_fast32_t const index = [&]() -> std::uint_fast32_t {
        try {
            std::uint_fast32_t const index = indexer(hand, chi_list, peng_list, angang_list, minggang_list);
            if (index == -1) {
                throw std::logic_error("Failed to index a hule.");
            }
            if (index >= Tsumonya::e) {
                std::ostringstream oss;
                oss << index << " (>= " << Tsumonya::e << ")" << ": An out-of-bound index.";
                throw std::logic_error(oss.str());
            }
            return index;
        }
        catch (...) {
            dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
            throw;
        }
    }();

    if (debugging) {
        dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cout);
        std::cout << "index = " << index << std::endl;
    }

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

        std::uint_fast64_t const map_index = static_cast<std::uint_fast64_t>(index) * 13u + i;
        if (map_index >= map.size()) {
            dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
            std::ostringstream oss;
            oss << index << " * " << 13u << " + " << static_cast<unsigned>(i)
                << " = " << map_index << ": An out-of-bound index.";
            throw std::logic_error(oss.str());
        }
        if (map[map_index].first == 0u && map[map_index].second == 0u) {
            map[map_index] = std::make_pair(
                encodeFuFan(zimo_fu, zimo_fan), encodeFuFan(rong_fu, rong_fan));
        }
        else {
            if (encodeFuFan(zimo_fu, zimo_fan) != map[map_index].first || encodeFuFan(rong_fu, rong_fan) != map[map_index].second) {
                dumpEntry(hand, chi_list, peng_list, angang_list, minggang_list, std::cerr);
                throw std::logic_error("Indices are in conflict.");
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
        double eta = (wall_clock / count) * Tsumonya::e;
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
    PengList &peng_list, GangList &angang_list, GangList &minggang_list, HuleIndexer const &indexer,
    Map &map, std::uint_fast32_t &count, cpu_timer const &timer)
{
    assert((i <= 34u));
    assert((m <= 5u));
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
        // Termination of recursion
        if (m == 5u && h == 1u) {
            createEntry(
                hand, chi_list, peng_list, angang_list, minggang_list, indexer, map, count, timer);
        }
        return;
    }

    std::uint_fast8_t const color = i / 9u;
    std::uint_fast8_t const number = i % 9u;
    std::uint_fast8_t const cindex = color * 7u + number;
    bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
        if (m + mtable[s] > 5u) {
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
            hand, chi_list, peng_list, angang_list, minggang_list, indexer, map, count, timer);

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

    HuleIndexer indexer;
    Map map(13u * Tsumonya::e, std::pair<std::uint8_t, std::uint8_t>(0u, 0u));
    std::uint_fast32_t count = 0u;
    cpu_timer timer;

    enumerateNormalHule(
        0u, 0u, 0u, 0u, 0u, hand, chi_list, peng_list, angang_list, minggang_list,
        indexer, map, count, timer);

    {
        std::ofstream ofs("map.bin", std::ios_base::out | std::ios_base::binary);
        for (auto const [zimo_encode, rong_encode] : map) {
            ofs << zimo_encode << rong_encode;
        }
    }

    Py_FinalizeEx();
}
