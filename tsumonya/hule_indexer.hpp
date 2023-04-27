#include <tsumonya/table.hpp>
#include <tsumonya/core.hpp>
#include <array>
#include <tuple>
#include <stdexcept>
#include <cstdint>
#include <cassert>


#include <iostream>

namespace Tsumonya{

class HuleIndexer
{
private:
    using Hand_ = std::array<std::uint_fast8_t, 34u>;
    using ChiList_ = std::array<std::uint_fast8_t, 21u>;
    using PengList_ = std::array<std::uint_fast8_t, 34u>;
    using GangList_ = std::array<std::uint_fast8_t, 34u>;

private:
    std::uint_fast8_t getTotal_(
        Hand_ const &hand, ChiList_ const &chi_list, PengList_ const &peng_list,
        GangList_ const &angang_list, GangList_ const &minggang_list,
        std::uint_fast8_t const i) const
    {
        std::uint_fast8_t const color = i / 9u;
        std::uint_fast8_t const number = i % 9u;
        std::uint_fast8_t const cindex = color <= 2u && number <= 6u ? color * 7u + number : UINT_FAST8_MAX;

        if (color <= 2u && number == 0u) {
            return hand[i] + chi_list[cindex] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
        }
        if (color <= 2u && number == 1u) {
            assert((cindex >= 1u));
            return hand[i] + chi_list[cindex - 1u] + chi_list[cindex] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
        }
        if (color <= 2u && number <= 6u) {
            assert((cindex >= 2u));
            return hand[i] + chi_list[cindex - 2u] + chi_list[cindex - 1u] + chi_list[cindex] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
        }
        if (color <= 2u && number == 7u) {
            assert((cindex >= 2u));
            return hand[i] + chi_list[color * 7u + 5u] + chi_list[color * 7u + 6u] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
        }
        if (color <= 2u && number == 8u) {
            assert((cindex >= 2u));
            return hand[i] + chi_list[color * 7u + 6u] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
        }
        assert((color == 3u));
        return hand[i] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
    }

private:
    std::tuple<std::uint_fast8_t, std::uint_fast8_t, std::uint_fast32_t> encodeShupai_(
        std::uint_fast8_t const color, Hand_ const &hand, ChiList_ const &chi_list,
        PengList_ const &peng_list, GangList_ const &angang_list, GangList_ const &minggang_list,
        std::uint_fast8_t const head, std::uint_fast8_t m, std::uint_fast8_t h) const
    {
        std::uint_fast32_t code = 0u;

        std::uint_fast8_t aa = hand[color * 9u + 1u];
        std::uint_fast8_t bb = hand[color * 9u + 2u];

        std::uint_fast8_t a = getTotal_(hand, chi_list, peng_list, angang_list, minggang_list, color * 9u + 0u);
        std::uint_fast8_t b = getTotal_(hand, chi_list, peng_list, angang_list, minggang_list, color * 9u + 1u);

        for (std::uint_fast8_t i = color * 9u; i < (color + 1u) * 9u; ++i) {
            std::uint_fast8_t const number = i - color * 9u;
            std::uint_fast8_t const cindex = number <= 6u ? color * 7u + number : UINT_FAST8_MAX;

            std::uint_fast8_t s = 0u;
            for (; s < stable.size(); ++s) {
                std::uint_fast32_t delta = table[i][5u - m][1u - h][s][a][b];
                //std::cout << "head = " << static_cast<unsigned>(head)
                //          << ", i = " << static_cast<unsigned>(i)
                //          << ", m = " << static_cast<unsigned>(5u - m)
                //          << ", h = " << static_cast<unsigned>(1u - h)
                //          << ", s = " << static_cast<unsigned>(s)
                //          << ", a = " << static_cast<unsigned>(a)
                //          << ", b = " << static_cast<unsigned>(b)
                //          << ", aa = " << static_cast<unsigned>(aa)
                //          << ", bb = " << static_cast<unsigned>(bb)
                //          << ": " << table[i][5u - m][1u - h][s][a][b] << std::endl;
                if (aa < 2u * stable[s][0u] + 3u * stable[s][1u] + stable[s][2u]) {
                    code += delta;
                    continue;
                }
                if (bb < stable[s][2u]) {
                    code += delta;
                    continue;
                }
                if (number <= 6u && hand[i + 2u] < stable[s][2u]) {
                    code += delta;
                    continue;
                }
                if (a < ntable[s]) {
                    code += delta;
                    continue;
                }
                if (b < abtable[s]) {
                    code += delta;
                    continue;
                }
                if (number <= 6u && getTotal_(hand, chi_list, peng_list, angang_list, minggang_list, i + 2u) < abtable[s]) {
                    code += delta;
                    continue;
                }
                if (m + mtable[s] > 5u) {
                    code += delta;
                    continue;
                }
                if ((head == i) != (stable[s][0u] == 1u)) {
                    code += delta;
                    continue;
                }
                if (h + stable[s][0u] > 1u) {
                    code += delta;
                    continue;
                }
                if (cindex != UINT_FAST8_MAX && stable[s][3u] != chi_list[cindex]) {
                    code += delta;
                    continue;
                }
                if (cindex == UINT_FAST8_MAX && abtable[s] > 0u) {
                    code += delta;
                    continue;
                }
                if (stable[s][4u] != peng_list[i]) {
                    code += delta;
                    continue;
                }
                if (stable[s][5u] != angang_list[i]) {
                    code += delta;
                    continue;
                }
                if (stable[s][6u] != minggang_list[i]) {
                    code += delta;
                    continue;
                }

                aa = bb - stable[s][2u];
                bb = number <= 6u ? hand[i + 2u] - stable[s][2u] : 0u;
                a = b - abtable[s];
                if (number <= 6u) {
                    b = getTotal_(hand, chi_list, peng_list, angang_list, minggang_list, i + 2u) - abtable[s];
                }
                else {
                    b = 0u;
                }
                m += mtable[s];
                h += stable[s][0u];
                break;
            }
            if (s == stable.size()) {
                return { UINT_FAST8_MAX, UINT_FAST8_MAX, UINT_FAST32_MAX };
            }
        }

        return { m, h, code };
    }

    std::uint_fast32_t encodeZipai_(
        std::array<std::uint_fast8_t, 34u> const &hand,
        std::array<std::uint_fast8_t, 34u> const &peng_list,
        std::array<std::uint_fast8_t, 34u> const &angang_list,
        std::array<std::uint_fast8_t, 34u> const &minggang_list,
        std::uint_fast8_t m,
        std::uint_fast8_t h) const
    {
        std::uint_fast32_t code = 0u;

        for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
            std::uint_fast8_t const a = hand[i] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);
            std::uint_fast8_t const b = hand[i] + 3u * peng_list[i] + 4u * (angang_list[i] + minggang_list[i]);

            for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                std::uint_fast32_t const delta = table[i][5u - m][1u - h][s][a][b];
                if (hand[i] < 2u * stable[s][0u] + 3u * stable[s][1u]) {
                    code += delta;
                    continue;
                }
                if (m + mtable[s] > 5u) {
                    code += delta;
                    continue;
                }
                if ((hand[i] == 2u) != (stable[s][0u] == 1u)) {
                    code += delta;
                    continue;
                }
                if (h + stable[s][0u] > 1u) {
                    code += delta;
                    continue;
                }
                if (abtable[s] > 0u) {
                    code += delta;
                    continue;
                }
                if (stable[s][4u] != peng_list[i]) {
                    code += delta;
                    continue;
                }
                if (stable[s][5u] != angang_list[i]) {
                    code += delta;
                    continue;
                }
                if (stable[s][6u] != minggang_list[i]) {
                    code += delta;
                    continue;
                }

                if (hand[i] > 2u * stable[s][0u] + 3u * stable[s][1u]) {
                    return UINT_FAST32_MAX;
                }
                m += mtable[s];
                h += stable[s][0u];
                break;
            }
        }

        return code;
    }

public:
    std::uint_fast32_t operator()(
        std::array<std::uint_fast8_t, 34u> const &hand,
        std::array<std::uint_fast8_t, 21u> const &chi_list,
        std::array<std::uint_fast8_t, 34u> const &peng_list,
        std::array<std::uint_fast8_t, 34u> const &angang_list,
        std::array<std::uint_fast8_t, 34u> const &minggang_list) const
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
        std::uint_fast32_t code = 0u;

        for (std::uint_fast8_t i = 0u; i < 3u; ++i) {
            auto [m_, h_, headless_code] = encodeShupai_(
                i, hand, chi_list, peng_list, angang_list, minggang_list, -1, m, h);
            if (headless_code != UINT_FAST32_MAX) {
                m = m_;
                h = h_;
                code += headless_code;
            }
            else {
                std::uint_fast32_t headful_code = 0u;
                for (std::uint_fast8_t j = (s[i] * 2u) % 3u; j < 9u; j += 3u) {
                    if (hand[i * 9u + j] < 2u) {
                        continue;
                    }
                    std::tie(m_, h_, headful_code) = encodeShupai_(
                        i, hand, chi_list, peng_list, angang_list, minggang_list, i * 9u + j, m, h);
                    if (headful_code != UINT_FAST32_MAX) {
                        m = m_;
                        h = h_;
                        assert((h == 1u));
                        code += headful_code;
                        break;
                    }
                }
                if (headful_code == UINT_FAST32_MAX) {
                    return UINT_FAST32_MAX;
                }
            }
        }

        std::uint_fast32_t const zipai_code = encodeZipai_(
            hand, peng_list, angang_list, minggang_list, m, h);
        if (zipai_code == UINT_FAST32_MAX) {
            return UINT_FAST32_MAX;
        }

        return code + zipai_code;
    }
}; // class HuleIndexer

} // namespace Tsumonya
