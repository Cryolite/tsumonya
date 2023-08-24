#include <tsumonya/normal/wind_head/table.hpp>
#include <tsumonya/normal/base/table.hpp>
#include <tsumonya/core.hpp>
#include <array>
#include <tuple>
#include <stdexcept>
#include <cstdint>
#include <cassert>
#include <cstddef>


namespace Tsumonya::Normal{

class HuleIndexer
{
private:
    using StateSeq_ = std::array<std::tuple<std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t>, 34u>;

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
                if (m + mtable[ss] > 4u) {
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

    std::uint_fast8_t encodeZipai_(
        Hand const &hand, PengList const &peng_list, GangList const &angang_list,
        GangList const &minggang_list, std::uint_fast8_t m, std::uint_fast8_t h,
        StateSeq_ &state_seq) const
    {
        bool wind_head = false;

        for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
            bool const wind_pai = 27u <= i && i < 31u;

            std::uint_fast8_t s = UINT_FAST8_MAX;
            for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
                if (m + mtable[ss] > 4u) {
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
                    return 0u;
                }

                if (wind_pai && stable[ss][0u] == 1u) {
                    wind_head = true;
                }

                s = ss;
                break;
            }
            if (s == UINT_FAST8_MAX) {
                return 0u;
            }

            state_seq[i] = { m, h, s, 0u, 0u };

            m += mtable[s];
            h += stable[s][0u];
        }

        return wind_head ? 2u : 1u;
    }

    std::uint_fast32_t encodeStateSeq_(StateSeq_ const &state_seq, bool const wind_head) const
    {
        Table const &table = wind_head ? Tsumonya::Normal::WindHead::table
                                       : Tsumonya::Normal::Base::table;
        std::uint_fast32_t code = 0u;
        for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
            auto const [m, h, s, x, y] = state_seq[i];
            code += table[i][m][h][x][y][s];
        }

        return code + (wind_head ? Tsumonya::Normal::Base::e : 0u);
    }

public:
    std::uint_fast32_t operator()(
        Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
        GangList const &angang_list, GangList const &minggang_list) const
    {
        {
            std::uint_fast16_t total = 0u;

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

                total += hand[i] + c + 3u * (peng_list[i] + angang_list[i] + minggang_list[i]);
            }
            for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
                if (hand[i] + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i] > 4u) {
                    throw std::invalid_argument("Invalid arguments.");
                }

                total += hand[i] + 3u * (peng_list[i] + angang_list[i] + minggang_list[i]);
            }

            if (total != 14u) {
                throw std::invalid_argument("Invalid arguments.");
            }
        }

        std::array<std::uint_fast8_t, 3u> t = { 0u, 0u, 0u };
        for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
            for (std::uint_fast8_t number = 0u; number < 9u; ++number) {
                t[color] += number * hand[color * 9u + number];
            }
        }

        std::uint_fast8_t m = 0u;
        std::uint_fast8_t h = 0u;
        StateSeq_ state_seq;

        for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
            auto [m_, h_] = encodeShupai_(
                color, hand, chi_list, peng_list, angang_list, minggang_list,
                UINT_FAST8_MAX, m, h, state_seq);
            if (m_ != UINT_FAST8_MAX) {
                assert((h_ != UINT_FAST8_MAX));
                m = m_;
                h = h_;
            }
            else {
                assert((h_ == UINT_FAST8_MAX));
                for (std::uint_fast8_t j = (t[color] * 2u) % 3u; j < 9u; j += 3u) {
                    if (hand[color * 9u + j] < 2u) {
                        continue;
                    }
                    std::tie(m_, h_) = encodeShupai_(
                        color, hand, chi_list, peng_list, angang_list, minggang_list,
                        color * 9u + j, m, h, state_seq);
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

        bool wind_head = false;
        switch (encodeZipai_(hand, peng_list, angang_list, minggang_list, m, h, state_seq)) {
            case 0u:
                return UINT_FAST32_MAX;
            case 1u:
                break;
            case 2u:
                wind_head = true;
                break;
            default:
                throw std::logic_error("A logic error.");
        }

        return encodeStateSeq_(state_seq, wind_head);
    }
};

}
