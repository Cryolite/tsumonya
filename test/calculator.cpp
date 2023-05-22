#include <tsumonya/calculator.hpp>
#include <tsumonya/core.hpp>
#include <iostream>
#include <cstdint>


namespace{

using Tsumonya::stable;
using Tsumonya::mtable;
using Tsumonya::ntable;
using Tsumonya::xytable;

using Tsumonya::Hand;
using Tsumonya::ChiList;
using Tsumonya::PengList;
using Tsumonya::GangList;
using Tsumonya::zimo;
using Tsumonya::rong;
using Tsumonya::Calculator;

void dump(
    Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
    GangList const &angang_list, GangList const &minggang_list, std::uint_fast8_t const hupai,
    std::uint_fast8_t const fu, std::uint_fast8_t const fan)
{
    std::cerr << "hand = ";
    for (unsigned h : hand) {
        std::cerr << h << ',';
    }
    std::cerr << '\n';

    std::cerr << "chi = ";
    for (unsigned c : chi_list) {
        std::cerr << c << ',';
    }
    std::cerr << '\n';

    std::cerr << "peng = ";
    for (unsigned p : peng_list) {
        std::cerr << p << ',';
    }
    std::cerr << '\n';

    std::cerr << "angang = ";
    for (unsigned ag : angang_list) {
        std::cerr << ag << ',';
    }
    std::cerr << '\n';

    std::cerr << "minggang = ";
    for (unsigned mg : minggang_list) {
        std::cerr << mg << ',';
    }
    std::cerr << '\n';

    std::cerr << "hupai = " << static_cast<unsigned>(hupai) << '\n';

    std::cerr << "fu = " << static_cast<unsigned>(fu)
              << ", fan = " << static_cast<unsigned>(fan) << '\n';

    std::cerr << "========================================" << std::endl;
}

void calculate(
    Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
    GangList const &angang_list, GangList const &minggang_list, Calculator const &calculator)
{
    constexpr bool debugging = false;

    for (std::uint_fast8_t i = 0u, hupai_index = 0u;; ++i, ++hupai_index) {
        while (i < hand.size() && hand[i] == 0u) {
            ++i;
        }
        if (i >= hand.size()) {
            break;
        }

        if (hupai_index >= 13u) {
            throw std::logic_error("A logic error.");
        }

        Hand new_hand(hand);
        --new_hand[i];

        {
            auto const [fu, fan] = calculator(
                new_hand, chi_list, peng_list, angang_list, minggang_list, i, 0u, zimo);
            if constexpr (debugging) {
                dump(hand, chi_list, peng_list, angang_list, minggang_list, i, fu, fan);
            }
            if (fu == 0u && fan == 0u) {
                if constexpr (!debugging) {
                    dump(hand, chi_list, peng_list, angang_list, minggang_list, i, fu, fan);
                }
                throw std::logic_error("A logic error.");
            }
        }

        {
            auto const [fu, fan] = calculator(
                new_hand, chi_list, peng_list, angang_list, minggang_list, i, 0u, rong);
            if constexpr (debugging) {
                dump(hand, chi_list, peng_list, angang_list, minggang_list, i, fu, fan);
            }
            if (fu == 0u && fan == 0u) {
                if constexpr (!debugging) {
                    dump(hand, chi_list, peng_list, angang_list, minggang_list, i, fu, fan);
                }
                throw std::logic_error("A logic error.");
            }
        }
    }
}

void enumerateNormalHule(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const x, std::uint_fast8_t const y, Hand &hand, ChiList &chi_list,
    PengList &peng_list, GangList &angang_list, GangList &minggang_list,
    Calculator const &calculator)
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
            calculate(hand, chi_list, peng_list, angang_list, minggang_list, calculator);
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
            hand, chi_list, peng_list, angang_list, minggang_list, calculator);

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

}

int main()
{
    Tsumonya::Calculator calculator("map.bin");

    Tsumonya::Hand hand{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::ChiList chi_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::PengList peng_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::GangList angang_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::GangList minggang_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };

    enumerateNormalHule(
        0u, 0u, 0u, 0u, 0u, hand, chi_list, peng_list, angang_list, minggang_list, calculator);
}
