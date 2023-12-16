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
using Tsumonya::PengList;
using Tsumonya::ChiList;
using Tsumonya::GangList;
using Tsumonya::Normal::upper_bound;
using Tsumonya::Normal::HuleIndexer;

using Map = std::vector<std::pair<std::uint8_t, std::uint8_t>>;

void dumpEntry(
    Hand const &hand, PengList const &peng_list, ChiList const &chi_list,
    GangList const &angang_list, GangList const &minggang_list,
    std::uint_fast8_t const winning_tile, bool const rong, std::ostream &os)
{
    os << "hand = ";
    for (unsigned h : hand) {
        os << h << ',';
    }
    os << " (" << static_cast<unsigned>(winning_tile) << ", ";
    os << (!rong ? "zimo)" : "rong)") << '\n';

    os << "peng = ";
    for (unsigned p : peng_list) {
        os << p << ',';
    }
    os << '\n';

    os << "chi = ";
    for (unsigned c : chi_list) {
        os << c << ',';
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
    Hand const &hand, PengList const &peng_list, ChiList const &chi_list,
    GangList const &angang_list, GangList const &minggang_list,
    std::uint_fast8_t const winning_tile, bool rong, Map &map,
    std::uint_fast32_t &count, cpu_timer const &timer)
{
    constexpr bool debugging = false;

    Tsumonya::Normal::HuleIndexer indexer;

    std::uint_fast64_t const index = [&]() -> std::uint_fast64_t {
        try {
            std::uint_fast64_t const index = indexer(
                hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong);
            if (index == UINT_FAST64_MAX) {
                throw std::logic_error("Failed to index a hule.");
            }
            if (index >= upper_bound) {
                std::ostringstream oss;
                oss << index << " (>= " << upper_bound << ")" << ": An out-of-bound index.";
                throw std::logic_error(oss.str());
            }
            return index;
        }
        catch (...) {
            dumpEntry(
                hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong,
                std::cerr);
            throw;
        }
    }();

    if (debugging) {
        dumpEntry(
            hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong, std::cout);
        std::cout << "index = " << index << std::endl;
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
                hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong,
                std::cerr);
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
                hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong,
                std::cerr);
            python::object error_ = hand_response.attr("error");
            std::string error = python::extract<std::string>(error_);
            throw std::logic_error(error);
        }
        return python::extract<long>(fan_);
    }();

    if (debugging) {
        std::cout << "fu = " << static_cast<unsigned>(fu) << ", fan = " << static_cast<unsigned>(fan) << ")\n";
    }

    if (map[index].first == UINT_FAST8_MAX) {
        assert((map[index].second == UINT_FAST8_MAX));
        map[index].first = fu;
        map[index].second = fan;
    }
    else {
        assert((map[index].second != UINT_FAST8_MAX));
        if (fu != map[index].first || fan != map[index].second) {
            dumpEntry(
                hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong,
                std::cerr);
            std::ostringstream oss;
            oss << index << ": Indices are in conflict.";
            throw std::logic_error(oss.str());
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

void enumerateNormalHule(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const w, std::uint_fast8_t const x, std::uint_fast8_t const y, Hand &hand,
    PengList &peng_list, ChiList &chi_list, GangList &angang_list, GangList &minggang_list,
    std::uint_fast8_t winning_tile, bool rong, Map &map, std::uint_fast32_t &count,
    cpu_timer const &timer)
{
    assert((i <= 34u));
    assert((m <= 4u));
    assert((h <= 1u));
    assert((w <= 2u));
    assert((x <= 4u));
    assert((y <= 4u));
    for (std::uint_fast8_t const x : hand) {
        assert((x <= 4u));
    }
    for (std::uint_fast8_t const p : peng_list) {
        assert((p <= 1u));
    }
    for (std::uint_fast8_t const c : chi_list) {
        assert((c <= 4u));
    }
    for (std::uint_fast8_t const ag : angang_list) {
        assert((ag <= 1u));
    }
    for (std::uint_fast8_t const mg : minggang_list) {
        assert((mg <= 1u));
    }

    if (i == 34u) {
        if (m == 4u && h == 1u && w >= 1u && x == 0u && y == 0u) {
            assert((winning_tile < 34u));
            createEntry(
                hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong, map,
                count, timer);
        }
        return;
    }

    std::uint_fast8_t const color = i / 9u;
    std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
    std::uint_fast8_t const cindex = color * 7u + number;
    bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
        std::uint_fast8_t const n = 3u * stable[s][0u] + stable[s][1u] + 2u * stable[s][6u];

        if (m + mtable[s] > 4u) {
            // The number of members (menzi, 面子) must not exceed 4.
            continue;
        }
        if (h + stable[s][6u] > 1u) {
            // The number of heads (雀頭) must not exceed 1.
            continue;
        }
        if (w >= 1u && stable[s][7u] >= 1u) {
            // The number of winning tiles must not exceed 1.
            continue;
        }
        if (stable[s][7u] >= 1u && hand[i] + n == 0u) {
            // If the winning tile is `i`, then tile `i` must exist in the hand.
            continue;
        }
        if (shunzi_prohibited && xytable[s] > 0u) {
            // Three-in-a-row (shunzi, 順子) starting with 8 or 9 must not exist. The same applies to
            // one of honors (zipai, 字牌).
            continue;
        }
        if (hand[i] + n > 4u) {
            // The number of tile `i` must not exceed 4.
            continue;
        }
        if (!shunzi_prohibited && hand[i + 1u] + stable[s][1u] > 4u) {
            // The number of tile `i + 1` in the hand must not exceed 4.
            continue;
        }
        if (!shunzi_prohibited && hand[i + 2u] + stable[s][1u] > 4u) {
            // The number of tile `i + 2` in the hand must not exceed 4.
            continue;
        }
        if (x + ntable[s] > 4u) {
            // The number of tile `i + 1` must not exceed 4.
            continue;
        }
        if (y + xytable[s] > 4u) {
            // The number of tile `i + 2` must not exceed 4.
            continue;
        }

        hand[i] += n;
        if (!shunzi_prohibited) {
            hand[i + 1u] += stable[s][1u];
            hand[i + 2u] += stable[s][1u];
        }
        peng_list[i] += stable[s][2u];
        if (!shunzi_prohibited) {
            assert((cindex < 21u));
            chi_list[cindex] += stable[s][3u];
        }
        angang_list[i] += stable[s][4u];
        minggang_list[i] += stable[s][5u];
        if (stable[s][7u] == 1u) {
            assert((winning_tile == UINT_FAST8_MAX));
            assert((!rong));
            winning_tile = i;
        }
        else if (stable[s][7u] == 2u) {
            assert((winning_tile == UINT_FAST8_MAX));
            assert((!rong));
            winning_tile = i;
            rong = true;
        }

        enumerateNormalHule(
            i + 1u, m + mtable[s], h + stable[s][6u], w + stable[s][7u], y + xytable[s],
            xytable[s], hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong,
            map, count, timer);

        if (stable[s][7u] >= 1u) {
            rong = false;
            winning_tile = UINT_FAST8_MAX;
        }
        minggang_list[i] -= stable[s][5u];
        angang_list[i] -= stable[s][4u];
        if (!shunzi_prohibited) {
            assert((cindex < 21u));
            chi_list[cindex] -= stable[s][3u];
        }
        peng_list[i] -= stable[s][2u];
        if (!shunzi_prohibited) {
            hand[i + 2u] -= stable[s][1u];
            hand[i + 1u] -= stable[s][1u];
        }
        hand[i] -= n;
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
    PengList peng_list = {
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

    Map map(upper_bound, Map::value_type(UINT_FAST8_MAX, UINT_FAST8_MAX));
    std::uint_fast64_t count = 0u;
    cpu_timer timer;

    enumerateNormalHule(
        0u, 0u, 0u, 0u, 0u, 0u, hand, peng_list, chi_list, angang_list, minggang_list,
        UINT_FAST8_MAX, false, map, count, timer);

    {
        std::ofstream ofs("src/normal/map.bin", std::ios_base::out | std::ios_base::binary);
        for (auto const [fu, fan] : map) {
            ofs << fu << fan;
        }
    }

    Py_FinalizeEx();
}
