#include <tsumonya/core.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <array>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <cassert>


namespace{

using Tsumonya::Table;
using Tsumonya::stable;
using Tsumonya::mtable;
using Tsumonya::ntable;
using Tsumonya::xytable;

using BaseTable = std::array<std::array<std::array<std::array<std::array<std::uint_fast32_t, 5u>, 5u>, 2u>, 5u>, 34u>;

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

void initBaseTable(BaseTable &base_table)
{
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                        base_table[i][m][h][x][y] = 0u;
                    }
                }
            }
        }
    }
}

std::uint_fast32_t countNumTails(
    bool const wind_head, std::uint_fast8_t const i, std::uint_fast8_t const m,
    std::uint_fast8_t const h, std::uint_fast8_t const x, std::uint_fast8_t const y, Memo &memo)
{
    assert((i <= 34u));
    assert((m <= 4u));
    assert((h <= 1u));
    assert((x <= 4u));
    assert((y <= 4u));

    if (i == 34u) {
        assert((x == 0u));
        assert((y == 0u));
        return m == 4u && h == 1u ? 1u : 0u;
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
    bool const wind_pai = 27u <= i && i < 31u;

    std::uint_fast32_t total = 0u;
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
        if ((color <= 2u && number == 8u || color == 3u) && y > 0u) {
            continue;
        }
        if (ntable[s] + x > 4u) {
            continue;
        }
        if (xytable[s] + y > 4u) {
            continue;
        }

        if (stable[s][0u] == 1u && wind_pai != wind_head) {
            continue;
        }

        total += countNumTails(
            wind_head, i + 1u, m + mtable[s], h + stable[s][0u], xytable[s] + y, xytable[s], memo);
    }

    memo.emplace(state, total);

    return total;
}

void dumpTable(bool const wind_head, BaseTable const &base_table)
{
    Table table;
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        std::uint_fast8_t const color = i / 9u;
        std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
        bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

        for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                        for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                            table[i][m][h][x][y][s] = 0u;
                            for (std::uint_fast8_t ss = 0u; ss < s; ++ss) {
                                if (m + mtable[ss] > 4u) {
                                    continue;
                                }
                                if (h + stable[ss][0u] > 1u) {
                                    continue;
                                }
                                if (shunzi_prohibited && xytable[ss] > 0u) {
                                    continue;
                                }
                                if (ntable[ss] + x > 4u) {
                                    continue;
                                }
                                if (xytable[ss] + y > 4u) {
                                    continue;
                                }

                                if (i + 1u < 34u) {
                                    table[i][m][h][x][y][s] += base_table[i + 1u][m + mtable[ss]][h + stable[ss][0u]][xytable[ss] + y][xytable[ss]];
                                }
                                else {
                                    table[i][m][h][x][y][s] += 1u;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::ofstream ofs = wind_head ? std::ofstream("tsumonya/normal/wind_head/table.hpp")
                                  : std::ofstream("tsumonya/normal/base/table.hpp");
    if (wind_head) {
        ofs << "#if !defined(TSUMONYA_NORMAL_WIND_HEAD_TABLE_HPP_INCLUDE_GUARD)\n";
        ofs << "#define TSUMONYA_NORMAL_WIND_HEAD_TABLE_HPP_INCLUDE_GUARD\n";
    }
    else {
        ofs << "#if !defined(TSUMONYA_NORMAL_BASE_TABLE_HPP_INCLUDE_GUARD)\n";
        ofs << "#define TSUMONYA_NORMAL_BASE_TABLE_HPP_INCLUDE_GUARD\n";
    }
    ofs << '\n';
    ofs << "#include <tsumonya/core.hpp>\n";
    ofs << "#include <cstdint>\n";
    ofs << '\n';
    ofs << '\n';
    if (wind_head) {
        ofs << "namespace Tsumonya::Normal::WindHead{\n";
    }
    else {
        ofs << "namespace Tsumonya::Normal::Base{";
    }
    ofs << '\n';
    ofs << "inline constexpr std::uint_fast32_t e = " << base_table[0u][0u][0u][0u][0u] << ";\n";
    ofs << '\n';
    ofs << "inline constexpr Table table = {{\n";
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        ofs << "    {{\n";
        for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
            ofs << "        {{\n";
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                ofs << "            {{\n";
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    ofs << "                {{\n";
                    for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                        ofs << "                    {{";
                        for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                            ofs << table[i][m][h][x][y][s] << ",";
                        }
                        ofs << "}},\n";
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
    if (argc != 2) {
        throw std::runtime_error("Too few arguments.");
    }

    bool const wind_head = [&]() -> bool{
        int const wind_head = boost::lexical_cast<int>(argv[1u]);
        if (wind_head == 0) {
            return false;
        }
        else if (wind_head == 1) {
            return true;
        }
        throw std::runtime_error("An invalid argument.");
    }();

    BaseTable base_table;
    initBaseTable(base_table);

    Memo memo;
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        for (std::uint_fast8_t m = 0u; m <= 4u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t x = 0u; x <= 4u; ++x) {
                    for (std::uint_fast8_t y = 0u; y <= 4u; ++y) {
                        base_table[i][m][h][x][y] = countNumTails(wind_head, i, m, h, x, y, memo);
                    }
                }
            }
        }
    }

    dumpTable(wind_head, base_table);
}
