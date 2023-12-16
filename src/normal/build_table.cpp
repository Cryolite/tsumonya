#include <tsumonya/core.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <array>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <cassert>


namespace{

using Tsumonya::stable;
using Tsumonya::mtable;
using Tsumonya::ntable;
using Tsumonya::xytable;

std::uint_fast64_t packState(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const w, std::uint_fast8_t const x, std::uint_fast8_t const y,
    std::uint_fast8_t const a, std::uint_fast8_t const b)
{
    assert((i <= 34u));
    assert((m <= 4u));
    assert((h <= 1u));
    assert((w <= 2u));
    assert((x <= 4u));
    assert((y <= 4u));
    assert((a <= 1u));
    assert((b <= 1u));

    std::uint_fast64_t state = 0u;
    state |= (static_cast<std::uint_fast64_t>(i) << (8u * 7u));
    state |= (static_cast<std::uint_fast64_t>(m) << (8u * 6u));
    state |= (static_cast<std::uint_fast64_t>(h) << (8u * 5u));
    state |= (static_cast<std::uint_fast64_t>(w) << (8u * 4u));
    state |= (static_cast<std::uint_fast64_t>(x) << (8u * 3u));
    state |= (static_cast<std::uint_fast64_t>(y) << (8u * 2u));
    state |= (static_cast<std::uint_fast64_t>(a) << (8u * 1u));
    state |= (static_cast<std::uint_fast64_t>(b) << (8u * 0u));
    return state;
}

bool isValidTransition(
    bool const shunzi_prohibited, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const w, std::uint_fast8_t const x, std::uint_fast8_t const y,
    std::uint_fast8_t const a, std::uint_fast8_t const s)
{
    std::uint_fast8_t const n = 3u * stable[s][0u] + stable[s][1u] + 2u * stable[s][6u];

    if (m + mtable[s] > 4u) {
        // The number of members (menzi, 面子) must not exceed 4.
        return false;
    }
    if (h + stable[s][6u] > 1u) {
        // The number of heads (雀頭) must not exceed 1.
        return false;
    }
    if (w >= 1u && stable[s][7u] >= 1u) {
        // The number of winning tiles must not exceed 1.
        return false;
    }
    if (stable[s][7u] >= 1u && a + n == 0u) {
        // If the winning tile is `i`, then tile `i` must exist in the hand.
        return false;
    }
    if (shunzi_prohibited && xytable[s] > 0u) {
        // Three-in-a-row (shunzi, 順子) starting with 8 or 9 must not exist. The same applies to one
        // of honors (zipai, 字牌).
        return false;
    }
    if (x + ntable[s] > 4u) {
        // The number of tile `i` must not exceed 4.
        return false;
    }
    if (y + xytable[s] > 4u) {
        // The number of tile `i + 1` must not exceed 4.
        return false;
    }

    return true;
}

using Memo = std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>;

// Returns `T'(i, m, h, w, x, y, a, b)`.
std::uint_fast64_t countNumTails(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const w, std::uint_fast8_t const x, std::uint_fast8_t const y,
    std::uint_fast8_t const a, std::uint_fast8_t const b, Memo &memo)
{
    assert((i <= 34u));
    assert((m <= 4u));
    assert((h <= 1u));
    assert((w <= 2u));
    assert((x <= 4u));
    assert((y <= 4u));
    assert((a <= 1u));
    assert((b <= 1u));

    std::uint_fast64_t const state = packState(i, m, h, w, x, y, a, b);

    {
        Memo::const_iterator const iter = memo.find(state);
        if (iter != memo.cend()) {
            return iter->second;
        }
    }

    if (i == 34u) {
        std::uint_fast64_t const total = m == 4u && h == 1u && w >= 1u && x == 0u && y == 0u && a == 0u && b == 0u ? 1u : 0u;
        memo.emplace(state, total);
        return total;
    }

    std::uint_fast8_t const color = i / 9u;
    std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
    bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

    std::uint_fast64_t total = 0u;
    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
        if (!isValidTransition(shunzi_prohibited, m, h, w, x, y, a, s)) {
            continue;
        }

        total += countNumTails(
            i + 1u, m + mtable[s], h + stable[s][6u], w + stable[s][7u], y + xytable[s],
            xytable[s], b + stable[s][1u] >= 1u ? 1u : 0u, stable[s][1u] >= 1u ? 1u : 0u, memo);
    }

    memo.emplace(state, total);
    return total;
}

void dumpTable(Memo const &memo)
{
    std::uint_fast64_t const upper_bound = [&]() -> std::uint_fast64_t {
        std::uint_fast64_t const state = packState(0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
        Memo::const_iterator const iter = memo.find(state);
        if (iter == memo.cend()) {
            throw std::logic_error("A logic error.");
        }
        return iter->second;
    }();

    std::ofstream ofs("tsumonya/normal/table.hpp");
    ofs << "#if !defined(TSUMONYA_NORMAL_TABLE_HPP_INCLUDE_GUARD)\n";
    ofs << "#define TSUMONYA_NORMAL_TABLE_HPP_INCLUDE_GUARD\n";
    ofs << '\n';
    ofs << "#include <tsumonya/core.hpp>\n";
    ofs << "#include <cstdint>\n";
    ofs << '\n';
    ofs << '\n';
    ofs << "namespace Tsumonya::Normal{\n";
    ofs << '\n';
    ofs << "inline constexpr std::uint_fast64_t upper_bound = " << upper_bound << ";\n";
    ofs << '\n';
    ofs << "inline constexpr Table table = {{\n";
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        std::uint_fast8_t const color = i / 9u;
        std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
        bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

        ofs << "    {{ // i = " << static_cast<unsigned>(i) << '\n';
        for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
            ofs << "        {{ // m = " << static_cast<unsigned>(m) << '\n';
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                ofs << "            {{ // h = " << static_cast<unsigned>(h) << '\n';
                for (std::uint_fast8_t w = 0u; w <= 2u; ++w) {
                    ofs << "                {{ // w = " << static_cast<unsigned>(w) << '\n';
                    for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                        ofs << "                    {{ // x = " << static_cast<unsigned>(x) << '\n';
                        for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                            ofs << "                        {{ // y = " << static_cast<unsigned>(y) << '\n';
                            for (std::uint_fast8_t a = 0u; a <= 1u; ++a) {
                                ofs << "                            {{ // a = " << static_cast<unsigned>(a) << '\n';
                                for (std::uint_fast8_t b = 0u; b <= 1u; ++b) {
                                    ofs << "                                {{";
                                    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                                        std::uint_fast64_t sum = 0u;
                                        for (std::uint_fast8_t ss = 0u; ss < s; ++ss) {
                                            if (!isValidTransition(shunzi_prohibited, m, h, w, x, y, a, ss)) {
                                                continue;
                                            }

                                            sum += [&]() -> std::uint_fast64_t {
                                                std::uint_fast8_t const mm = m + mtable[ss];
                                                std::uint_fast8_t const hh = h + stable[ss][6u];
                                                std::uint_fast8_t const ww = w + stable[ss][7u];
                                                std::uint_fast8_t const xx = y + xytable[ss];
                                                std::uint_fast8_t const yy = xytable[ss];
                                                std::uint_fast8_t const aa = b + stable[ss][1u] >= 1u ? 1u : 0u;
                                                std::uint_fast8_t const bb = stable[ss][1u] >= 1u ? 1u : 0u;
                                                std::uint_fast64_t const state
                                                    = packState(i + 1, mm, hh, ww, xx, yy, aa, bb);
                                                Memo::const_iterator const iter = memo.find(state);
                                                return iter != memo.cend() ? iter->second : 0u;
                                            }();
                                        }
                                        ofs << sum << ",";
                                    }
                                    ofs << "}},\n";
                                }
                                ofs << "                            }},\n";
                            }
                            ofs << "                        }},\n";
                        }
                        ofs << "                    }},\n";
                    }
                    ofs << "                }},\n";
                }
                ofs << "            }},\n";
            }
            ofs << "        }},\n";
        }
        ofs << "    }},\n";
    }
    ofs << "}};\n";
    ofs << '\n';
    ofs << "}\n";
    ofs << '\n';
    ofs << "#endif" << std::endl;
}

} // namespace `anonymous`

int main(int const argc, char const * const * const argv)
{
    if (argc >= 2) {
        throw std::runtime_error("Too many arguments.");
    }

    Memo memo;
    countNumTails(0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, memo);

    dumpTable(memo);
}
