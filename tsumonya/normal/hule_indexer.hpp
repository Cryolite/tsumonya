#include <tsumonya/normal/table.hpp>
#include <tsumonya/core.hpp>
#include <sstream>
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
    using StateSeq_ = std::array<
        std::tuple<
            std::uint_fast8_t, // m
            std::uint_fast8_t, // h
            std::uint_fast8_t, // w
            std::uint_fast8_t, // x
            std::uint_fast8_t, // y
            std::uint_fast8_t, // a
            std::uint_fast8_t, // b
            std::uint_fast8_t  // s
        >, 34u
    >;

private:
    std::tuple<std::uint_fast8_t, std::uint_fast8_t, std::uint_fast8_t> encodeShupai_(
        std::uint_fast8_t const color, Tsumonya::Hand const &hand,
        Tsumonya::PengList const &peng_list, Tsumonya::ChiList const &chi_list,
        Tsumonya::GangList const &angang_list, Tsumonya::GangList const &minggang_list,
        std::uint_fast8_t const winning_tile, bool const rong, std::uint_fast8_t const head,
        std::uint_fast8_t m, std::uint_fast8_t h, std::uint_fast8_t w, StateSeq_ &state_seq) const
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
                std::uint_fast8_t const n = 3u * stable[ss][0u] + stable[ss][1u] + 2u * stable[ss][6u];

                if (m + mtable[ss] > 4u) {
                    // The number of members (menzi, 面子) must not exceed 4.
                    continue;
                }
                if ((head == i) != (stable[ss][6u] == 1u)) {
                    // Tile `i` is designated as the head (雀頭).
                    continue;
                }
                if (h + stable[ss][6u] > 1u) {
                    // The number of heads (雀頭) must not exceed 1.
                    continue;
                }
                if ((winning_tile == i) != (stable[ss][7u] >= 1u)) {
                    // Tile `i` is designated as the winning tile.
                    continue;
                }
                if (w >= 1u && stable[ss][7u] >= 1u) {
                    // The number of winning tiles must not exceed 1.
                    continue;
                }
                if (stable[ss][7u] >= 1u && hand[i] == 0u) {
                    // If the winning tile is `i`, then tile `i` must exist in the hand.
                    continue;
                }
                if (stable[ss][7u] >= 1u && rong != (stable[ss][7u] == 2u)) {
                    // Winning by self-draw or deal-in must be consistent.
                    continue;
                }
                if (a != n) {
                    continue;
                }
                if (number < 8u && b < stable[ss][1u]) {
                    continue;
                }
                if (number < 7u && hand[i + 2u] < stable[ss][1u]) {
                    continue;
                }
                if (peng_list[i] != stable[ss][2u]) {
                    continue;
                }
                if (cindex == UINT_FAST8_MAX && xytable[ss] > 0u) {
                    continue;
                }
                if (cindex != UINT_FAST8_MAX && chi_list[cindex] != stable[ss][3u]) {
                    continue;
                }
                if (angang_list[i] != stable[ss][4u]) {
                    continue;
                }
                if (minggang_list[i] != stable[ss][5u]) {
                    continue;
                }

                s = ss;
                break;
            }
            if (s == UINT_FAST8_MAX) {
                return { UINT_FAST8_MAX, UINT_FAST8_MAX, UINT_FAST8_MAX };
            }

            state_seq[i] = { m, h, w, x, y, hand[i] - a >= 1u ? 1u : 0u, hand[i + 1u] - b >= 1u ? 1u : 0u, s };

            m += mtable[s];
            h += stable[s][6u];
            w += stable[s][7u];
            x = number < 7u ? y + xytable[s] : y;
            y = number < 7u ? xytable[s] : 0u;
            a = b - stable[s][1u];
            b = hand[i + 2u] - stable[s][1u];
        }

        return { m, h, w };
    }

    bool encodeZipai_(
        Tsumonya::Hand const &hand, Tsumonya::PengList const &peng_list,
        Tsumonya::GangList const &angang_list, Tsumonya::GangList const &minggang_list,
        std::uint_fast8_t const winning_tile, bool const rong, std::uint_fast8_t m,
        std::uint_fast8_t h, std::uint_fast8_t w, StateSeq_ &state_seq) const
    {
        for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
            std::uint_fast8_t s = UINT_FAST8_MAX;
            for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
                if (m + mtable[ss] > 4u) {
                    // The number of members (menzi, 面子) must not exceed 4.
                    continue;
                }
                if ((hand[i] == 2u) != (stable[ss][6u] == 1u)) {
                    // The number of tile `i` is equal to 2, then it must be the head (雀頭).
                    continue;
                }
                if (h + stable[ss][6u] > 1u) {
                    // The number of heads (雀頭) must not exceed 1.
                    continue;
                }
                if ((winning_tile == i) != (stable[ss][7u] >= 1u)) {
                    // Tile `i` is designated as the winning tile.
                    continue;
                }
                if (w >= 1u && stable[ss][7u] >= 1u) {
                    // The number of winning tiles must not exceed 1.
                    continue;
                }
                if (stable[ss][7u] >= 1u && hand[i] == 0u) {
                    // If the winning tile is `i`, then tile `i` must exist in the hand.
                    continue;
                }
                if (stable[ss][7u] >= 1u && rong != (stable[ss][7] == 2u)) {
                    // Winning by self-draw or deal-in must be consistent.
                    continue;
                }
                if (xytable[ss] > 0u) {
                    // Three-in-a-row (shunzi, 順子) is not allowed for honors (zipai, 字牌).
                    continue;
                }
                if (hand[i] != 3u * stable[ss][0u] + 2u * stable[ss][6u]) {
                    continue;
                }
                if (peng_list[i] != stable[ss][2u]) {
                    continue;
                }
                if (angang_list[i] != stable[ss][4u]) {
                    continue;
                }
                if (minggang_list[i] != stable[ss][5u]) {
                    continue;
                }

                s = ss;
                break;
            }
            if (s == UINT_FAST8_MAX) {
                return false;
            }

            state_seq[i] = { m, h, w, 0u, 0u, 0u, 0u, s };

            m += mtable[s];
            h += stable[s][6u];
            w += stable[s][7u];
        }

        return m == 4u && h == 1u && w >= 1u;
    }

    std::uint_fast64_t encodeStateSeq_(StateSeq_ const &state_seq) const
    {
        std::uint_fast64_t code = 0u;
        for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
            auto const [m, h, w, x, y, a, b, s] = state_seq[i];
            code += Tsumonya::Normal::table[i][m][h][w][x][y][a][b][s];
        }

        return code;
    }

public:
    std::uint_fast64_t operator()(
        Tsumonya::Hand const &hand, Tsumonya::PengList const &peng_list,
        Tsumonya::ChiList const &chi_list, Tsumonya::GangList const &angang_list,
        Tsumonya::GangList const &minggang_list, std::uint_fast8_t const winning_tile,
        bool rong) const
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
                    std::ostringstream oss;
                    oss << "Too many of tile `" << static_cast<unsigned>(i) << "`.";
                    throw std::invalid_argument(oss.str());
                }

                total += hand[i] + c + 3u * (peng_list[i] + angang_list[i] + minggang_list[i]);
            }
            for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
                if (hand[i] + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i] > 4u) {
                    std::ostringstream oss;
                    oss << "Too many of tile `" << static_cast<unsigned>(i) << "`.";
                    throw std::invalid_argument(oss.str());
                }

                total += hand[i] + 3u * (peng_list[i] + angang_list[i] + minggang_list[i]);
            }

            if (total < 14u) {
                throw std::invalid_argument("Too few of tiles.");
            }
            if (total > 14u) {
                throw std::invalid_argument("Too many of tiles.");
            }

            if (winning_tile >= 34u) {
                throw std::invalid_argument("An invalid winning tile.");
            }
            if (hand[winning_tile] == 0u) {
                throw std::invalid_argument("The winning tile is not in the hand.");
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
        std::uint_fast8_t w = 0u;
        StateSeq_ state_seq;

        for (std::uint_fast8_t color = 0u; color < 3u; ++color) {
            auto [m_, h_, w_] = encodeShupai_(
                color, hand, peng_list, chi_list, angang_list, minggang_list, winning_tile, rong,
                UINT_FAST8_MAX, m, h, w, state_seq);
            if (m_ != UINT_FAST8_MAX) {
                assert((h_ != UINT_FAST8_MAX));
                assert((w_ != UINT_FAST8_MAX));
                m = m_;
                h = h_;
                w = w_;
            }
            else {
                assert((h_ == UINT_FAST8_MAX));
                assert((w_ == UINT_FAST8_MAX));
                for (std::uint_fast8_t j = (t[color] * 2u) % 3u; j < 9u; j += 3u) {
                    if (hand[color * 9u + j] < 2u) {
                        continue;
                    }
                    std::tie(m_, h_, w_) = encodeShupai_(
                        color, hand, peng_list, chi_list, angang_list, minggang_list, winning_tile,
                        rong, color * 9u + j, m, h, w, state_seq);
                    if (m_ != UINT_FAST8_MAX) {
                        assert((h_ != UINT_FAST8_MAX));
                        assert((w_ != UINT_FAST8_MAX));
                        m = m_;
                        h = h_;
                        w = w_;
                        assert((h == 1u));
                        break;
                    }
                }
                if (m_ == UINT_FAST8_MAX) {
                    assert((h_ == UINT_FAST8_MAX));
                    assert((w_ == UINT_FAST8_MAX));
                    return UINT_FAST64_MAX;
                }
            }
        }

        if (!encodeZipai_(hand, peng_list, angang_list, minggang_list, winning_tile, rong, m, h, w, state_seq)) {
            return UINT_FAST64_MAX;
        }

        std::uint_fast64_t const code = encodeStateSeq_(state_seq);
        return code;
    }
};

}
