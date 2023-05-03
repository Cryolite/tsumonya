#include <tsumonya/core.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <array>
#include <functional>
#include <cstdint>
#include <cassert>


namespace{

using Tsumonya::Table;
using Tsumonya::stable;
using Tsumonya::mtable;
using Tsumonya::ntable;
using Tsumonya::xytable;

std::uint_fast64_t packState(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const x, std::uint_fast8_t const y)
{
    std::uint_fast64_t state = 0u;
    state |= (static_cast<std::uint_fast64_t>(i) << (8u * 4u));
    state |= (static_cast<std::uint_fast64_t>(m) << (8u * 3u));
    state |= (static_cast<std::uint_fast64_t>(h) << (8u * 2u));
    state |= (static_cast<std::uint_fast64_t>(x) << (8u * 1u));
    state |= (static_cast<std::uint_fast64_t>(y) << (8u * 0u));
    return std::hash<std::uint_fast64_t>()(state);
}

using Memo = std::unordered_map<std::uint_fast64_t, std::uint_fast32_t>;

void initTable(Table &table)
{
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    for (std::uint_fast32_t y = 0u; y <= 4u; ++y) {
                        table[i][m][h][x][y] = 0u;
                    }
                }
            }
        }
    }
}

std::uint_fast32_t countNumTails(
    std::uint_fast8_t const i, std::uint_fast8_t const m, std::uint_fast8_t const h,
    std::uint_fast8_t const x, std::uint_fast8_t const y, Memo &memo)
{
    assert((i <= 34u));
    assert((m <= 5u));
    assert((h <= 1u));
    assert((x <= 4u));
    assert((y <= 4u));

    if (i == 34u) {
        assert((x == 0u));
        assert((y == 0u));
        return m == 5u && h == 1u ? 1u : 0u;
    }

    std::uint_fast64_t const state = packState(i, m, h, x, y);
    {
        Memo::iterator iter = memo.find(state);
        if (iter != memo.cend()) {
            return iter->second;
        }
    }

    std::uint_fast8_t const color = i / 9u;
    std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
    bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

    std::uint_fast32_t total = 0u;
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
        if ((color <= 2u && number == 8u || color == 3u) && y > 0u) {
            continue;
        }
        if (ntable[s] + x > 4u) {
            continue;
        }
        if (xytable[s] + y > 4u) {
            continue;
        }

        total += countNumTails(
            i + 1u, m + mtable[s], h + stable[s][0u], xytable[s] + y, xytable[s], memo);
    }

    memo.emplace(state, total);

    return total;
}

void dumpTable(Table const &table)
{
    std::ofstream ofs("tsumonya/table.hpp");
    ofs << "#if !defined(TSUMONYA_TABLE_HPP_INCLUDE_GUARD)\n";
    ofs << "#define TSUMONYA_TABLE_HPP_INCLUDE_GUARD\n";
    ofs << '\n';
    ofs << "#include <tsumonya/core.hpp>\n";
    ofs << "#include <cstdint>\n";
    ofs << '\n';
    ofs << '\n';
    ofs << "namespace Tsumonya{\n";
    ofs << '\n';
    ofs << "inline constexpr std::uint_fast32_t e = " << table[0u][0u][0u][0u][0u] << ";\n";
    ofs << '\n';
    ofs << "inline constexpr Table table = {{\n";
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        ofs << "    {{\n";
        for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
            ofs << "        {{\n";
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                ofs << "            {{\n";
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    ofs << "                {{";
                    for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                        ofs << table[i][m][h][x][y] << ",";
                    }
                    ofs << "}},\n";
                }
                ofs << "            }},\n";
            }
            ofs << "        }},\n";
        }
        ofs << "    }},\n";
    }
    ofs << "}};\n";
    ofs << '\n';
    ofs << "} // namespace Tsumonya\n";
    ofs << '\n';
    ofs << "#endif // !defined(TSUMONYA_TABLE_HPP_INCLUDE_GUARD)" << std::endl;
}

} // namespace `anonymous`

int main()
{
    Table table;
    initTable(table);

    Memo memo;
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                        std::cout << '(' << static_cast<unsigned>(i)
                                  << ", " << static_cast<unsigned>(m)
                                  << ", " << static_cast<unsigned>(h)
                                  << ", " << static_cast<unsigned>(x)
                                  << ", " << static_cast<unsigned>(y)
                                  << ')' << std::endl;
                        table[i][m][h][x][y] = countNumTails(i, m, h, x, y, memo);
                    }
                }
            }
        }
    }

    dumpTable(table);
}
