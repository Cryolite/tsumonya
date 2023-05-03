#include <tsumonya/table.hpp>
#include <tsumonya/core.hpp>
#include <array>
#include <tuple>
#include <stdexcept>
#include <cstdint>
#include <cassert>
#include <cstddef>


namespace Tsumonya{

class HuleIndexer
{
private:
    static constexpr bool debugging_ = false;

    using StateSeq_ = std::array<std::tuple<std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t>, 34u>;

public:
    using Hand = std::array<std::uint_fast8_t, 34u>;
    using ChiList = std::array<std::uint_fast8_t, 21u>;
    using PengList = std::array<std::uint_fast8_t, 34u>;
    using GangList = std::array<std::uint_fast8_t, 34u>;

private:
    std::tuple<std::uint_fast8_t, std::uint_fast8_t> encodeShupai_(
        std::uint_fast8_t const color, Hand const &hand, ChiList const &chi_list,
        PengList const &peng_list, GangList const &angang_list, GangList const &minggang_list,
        std::uint_fast8_t const head, std::uint_fast8_t m, std::uint_fast8_t h,
        StateSeq_ &state_seq) const
    {
        std::uint_fast8_t x = 0u;
        std::uint_fast8_t y = 0u;

        std::uint_fast8_t a = hand[color * 9u + 0u];
        std::uint_fast8_t b = hand[color * 9u + 1u];

        for (std::uint_fast8_t i = color * 9u; i < (color + 1u) * 9u; ++i) {
            std::uint_fast8_t const number = i - color * 9u;
            std::uint_fast8_t const cindex = number <= 6u ? color * 7u + number : UINT_FAST8_MAX;

            std::uint_fast8_t s = UINT_FAST8_MAX;
            for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
                if (m + mtable[ss] > 5u) {
                    continue;
                }
                if ((head == i) != (stable[ss][0u] == 1u)) {
                    continue;
                }
                if (h + stable[ss][0u] > 1u) {
                    continue;
                }
                if (ntable[ss] + x > 4u) {
                    continue;
                }
                if (xytable[ss] + y > 4u) {
                    continue;
                }
                if (cindex == UINT_FAST8_MAX && xytable[ss] > 0u) {
                    continue;
                }
                if (a < 2u * stable[ss][0u] + 3u * stable[ss][1u] + stable[ss][2u]) {
                    continue;
                }
                if (b < stable[ss][2u]) {
                    continue;
                }
                if (number <= 6u && hand[i + 2u] < stable[ss][2u]) {
                    continue;
                }
                if (cindex != UINT_FAST8_MAX && stable[ss][3u] != chi_list[cindex]) {
                    continue;
                }
                if (stable[ss][4u] != peng_list[i]) {
                    continue;
                }
                if (stable[ss][5u] != angang_list[i]) {
                    continue;
                }
                if (stable[ss][6u] != minggang_list[i]) {
                    continue;
                }

                if (a > 2u * stable[ss][0u] + 3u * stable[ss][1u] + stable[ss][2u]) {
                    return { UINT_FAST8_MAX, UINT_FAST8_MAX };
                }
                s = ss;
                break;
            }
            if (s == UINT_FAST8_MAX) {
                return { UINT_FAST8_MAX, UINT_FAST8_MAX };
            }

            state_seq[i] = { m, h, s, x, y };

            x = number <= 6u ? xytable[s] + y : y;
            y = number <= 6u ? xytable[s] : 0u;
            a = number <= 6u ? b - stable[s][2u] : b;
            b = number <= 6u ? hand[i + 2u] - stable[s][2u] : 0u;
            m += mtable[s];
            h += stable[s][0u];
        }

        return { m, h };
    }

    bool encodeZipai_(
        Hand const &hand, PengList const &peng_list, GangList const &angang_list,
        GangList const &minggang_list, std::uint_fast8_t m, std::uint_fast8_t h,
        StateSeq_ &state_seq) const
    {
        for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
            std::uint_fast8_t s = UINT_FAST8_MAX;
            for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
                if (m + mtable[ss] > 5u) {
                    continue;
                }
                if ((hand[i] == 2u) != (stable[ss][0u] == 1u)) {
                    continue;
                }
                if (h + stable[ss][0u] > 1u) {
                    continue;
                }
                if (xytable[ss] > 0u) {
                    continue;
                }
                if (hand[i] < 2u * stable[ss][0u] + 3u * stable[ss][1u]) {
                    continue;
                }
                if (stable[ss][4u] != peng_list[i]) {
                    continue;
                }
                if (stable[ss][5u] != angang_list[i]) {
                    continue;
                }
                if (stable[ss][6u] != minggang_list[i]) {
                    continue;
                }

                if (hand[i] > 2u * stable[ss][0u] + 3u * stable[ss][1u]) {
                    return false;
                }
                s = ss;
                break;
            }
            if (s == UINT_FAST8_MAX) {
                return false;
            }

            state_seq[i] = { m, h, s, 0u, 0u };

            m += mtable[s];
            h += stable[s][0u];
        }

        return true;
    }

    std::uint_fast32_t encodeStateSeq_(StateSeq_ const &state_seq) const
    {
        std::uint_fast32_t code = 0u;

        for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
            std::uint_fast8_t const color = i / 9u;
            std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
            bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;
            auto const [m, h, s, x, y] = state_seq[i];

            for (std::uint_fast8_t ss = 0u; ss < s; ++ss) {
                if constexpr (debugging_) {
                    std::cout << "i = " << static_cast<unsigned>(i)
                              << ", m = " << static_cast<unsigned>(m + mtable[ss])
                              << ", h = " << static_cast<unsigned>(h + stable[ss][0u])
                              << ", s = " << static_cast<unsigned>(ss)
                              << ", s* = " << static_cast<unsigned>(s)
                              << ", x = " << static_cast<unsigned>(xytable[ss] + y)
                              << ", y = " << static_cast<unsigned>(xytable[ss]);
                }
                if (m + mtable[ss] > 5u) {
                    if constexpr (debugging_) {
                        std::cout << ", delta = (skip)" << std::endl;
                    }
                    continue;
                }
                if (h + stable[ss][0u] > 1u) {
                    if constexpr (debugging_) {
                        std::cout << ", delta = (skip)" << std::endl;
                    }
                    continue;
                }
                if (shunzi_prohibited && xytable[ss] > 0u) {
                    if constexpr (debugging_) {
                        std::cout << ", delta = (skip)" << std::endl;
                    }
                    continue;
                }
                if (ntable[ss] + x > 4u) {
                    if constexpr (debugging_) {
                        std::cout << ", delta = (skip)" << std::endl;
                    }
                    continue;
                }
                if (xytable[ss] + y > 4u) {
                    if constexpr (debugging_) {
                        std::cout << ", delta = (skip)" << std::endl;
                    }
                    continue;
                }

                assert((i != 0u || y == 0u));
                assert((!(color <= 2u && number == 8u || color == 3u) || y == 0u));
                std::uint_fast32_t const delta = [&]() -> std::uint_fast32_t {
                    if (i + 1u < 34u) {
                        return table[i + 1u][m + mtable[ss]][h + stable[ss][0u]][xytable[ss] + y][xytable[ss]];
                    }
                    return 1u;
                }();
                code += delta;
                if constexpr (debugging_) {
                    std::cout << ", delta = " << delta << std::endl;
                }
            }
        }

        return code;
    }

public:
    std::uint_fast32_t operator()(
        Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
        GangList const &angang_list, GangList const &minggang_list) const
    {
        for (std::uint_fast8_t i = 0u; i < 27u; ++i) {
            std::uint_fast8_t const color = i / 9u;
            std::uint_fast8_t const number = i % 9u;
            std::uint_fast8_t const cindex = color * 7u + number;
            std::uint_fast8_t const c = [&]() -> std::uint_fast8_t {
                if (number == 0u) {
                    return chi_list[cindex];
                }
                assert((cindex >= 1u));
                if (number == 1u) {
                    return chi_list[cindex - 1u] + chi_list[cindex];
                }
                assert((cindex >= 2u));
                if (2u <= number && number <= 6u) {
                    return chi_list[cindex - 2u] + chi_list[cindex - 1u] + chi_list[cindex];
                }
                if (number == 7u) {
                    return chi_list[cindex - 2u] + chi_list[cindex - 1u];
                }
                return chi_list[cindex - 2u];
            }();

            if (hand[i] + c + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i] > 4u) {
                throw std::invalid_argument("Invalid arguments.");
            }
        }
        for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
            if (hand[i] + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i] > 4u) {
                throw std::invalid_argument("Invalid arguments.");
            }
        }

        std::array<std::uint_fast8_t, 3u> s = { 0u, 0u, 0u };
        for (std::uint_fast8_t i = 0u; i < 3u; ++i) {
            for (std::uint_fast8_t j = 0u; j < 9u; ++j) {
                s[i] += j * hand[i * 9u + j];
            }
        }

        std::uint_fast8_t m = 0u;
        std::uint_fast8_t h = 0u;
        StateSeq_ state_seq;

        for (std::uint_fast8_t i = 0u; i < 3u; ++i) {
            auto [m_, h_] = encodeShupai_(
                i, hand, chi_list, peng_list, angang_list, minggang_list,
                UINT_FAST8_MAX, m, h, state_seq);
            if (m_ != UINT_FAST8_MAX) {
                assert((h_ != UINT_FAST8_MAX));
                m = m_;
                h = h_;
            }
            else {
                assert((h_ == UINT_FAST8_MAX));
                for (std::uint_fast8_t j = (s[i] * 2u) % 3u; j < 9u; j += 3u) {
                    if (hand[i * 9u + j] < 2u) {
                        continue;
                    }
                    std::tie(m_, h_) = encodeShupai_(
                        i, hand, chi_list, peng_list, angang_list, minggang_list,
                        i * 9u + j, m, h, state_seq);
                    if (m_ != UINT_FAST8_MAX) {
                        assert((h_ != UINT_FAST8_MAX));
                        m = m_;
                        h = h_;
                        assert((h == 1u));
                        break;
                    }
                }
                if (m_ == UINT_FAST8_MAX) {
                    assert((h_ == UINT_FAST8_MAX));
                    return UINT_FAST32_MAX;
                }
            }
        }

        if (!encodeZipai_(hand, peng_list, angang_list, minggang_list, m, h, state_seq)) {
            return UINT_FAST32_MAX;
        }

        return encodeStateSeq_(state_seq);
    }
}; // class HuleIndexer

} // namespace Tsumonya
