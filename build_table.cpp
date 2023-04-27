#include "tsumonya/core.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <array>
#include <stdexcept>
#include <cstdint>

namespace{

using Tsumonya::Table;
using Tsumonya::stable;
using Tsumonya::mtable;
using Tsumonya::ntable;
using Tsumonya::abtable;

void initTable(Table &table)
{
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                    for (std::uint_fast8_t a = 0u; a <= 4u; ++a) {
                        for (std::uint_fast8_t b = 0u; b <= 4u; ++b) {
                            table[i][m][h][s][a][b] = 0u;
                        }
                    }
                }
            }
        }
    }
}

void prepareForPrevColor(Table &table, std::uint_fast8_t const i)
{
    for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
        for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
            for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
                    for (std::uint_fast8_t a = 0u; a <= 4u; ++a) {
                        for (std::uint_fast8_t b = 0u; b <= 4u; ++b) {
                            if (m + mtable[ss] <= 5u && h + stable[ss][0u] <= 1u && abtable[ss] == 0u) {
                                table[i - 1u][m + mtable[ss]][h + stable[ss][0u]][ss][0u][0u] += table[i][m][h][s][a][b];
                            }
                        }
                    }
                }
            }
        }
    }
}

void scan(std::uint_fast8_t const last, std::uint_fast8_t const first, Table &table)
{
    std::uint_fast8_t const n = last - first;

    for (std::uint_fast8_t i = last; i - 1u > first;) {
        --i;
        std::uint_fast8_t const number = i % 9u;
        for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                    for (std::uint_fast8_t a = 0u; a <= 4u; ++a) {
                        for (std::uint_fast8_t b = 0u; b <= 4u; ++b) {
                            for (std::uint_fast8_t ss = 0u; ss < stable.size(); ++ss) {
                                if (n == 9u && number <= 6u) {
                                    if (m + mtable[ss] <= 5u && h + stable[ss][0u] <= 1u && a + abtable[ss] <= 4u && b + abtable[ss] <= 4u) {
                                        table[i - 1u][m + mtable[ss]][h + stable[ss][0u]][ss][ntable[ss]][a + abtable[ss]] += table[i][m][h][s][a][b];
                                    }
                                }
                                else {
                                    if (m + mtable[ss] <= 5u && h + stable[ss][0u] <= 1u && abtable[ss] == 0u) {
                                        table[i - 1u][m + mtable[ss]][h + stable[ss][0u]][ss][ntable[ss]][a] += table[i][m][h][s][a][b];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void dumpTable(Table const &table)
{
    std::uint_fast64_t total = 0u;
    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
        total += table[0u][5u][1u][s][0u][0u];
    }

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
    ofs << "inline constexpr std::uint_fast32_t e = " << total << ";\n";
    ofs << '\n';
    ofs << "inline constexpr Table table = {{\n";
    for (std::uint_fast8_t i = 0u; i < 34u; ++i) {
        ofs << "    {{\n";
        for (std::uint_fast8_t m = 0u; m <= 5u; ++m) {
            ofs << "        {{\n";
            for (std::uint_fast8_t h = 0u; h <= 1u; ++h) {
                ofs << "            {{\n";
                for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
                    ofs << "                {{\n";
                    for (std::uint_fast8_t a = 0u; a <= 4u; ++a) {
                        ofs << "                    {{";
                        for (std::uint_fast8_t b = 0u; b <= 4u; ++b) {
                            ofs << table[i][m][h][s][a][b] << ",";
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
    ofs << "} // namespace Tsumonya\n";
    ofs << '\n';
    ofs << "#endif // !defined(TSUMONYA_TABLE_HPP_INCLUDE_GUARD)" << std::endl;
}

} // namespace `anonymous`

int main()
{
    Table table;
    initTable(table);

    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
        if (abtable[s] == 0u) {
            // Zipai does not allow shunzi nor chi.
            table[34u - 1u][mtable[s]][stable[s][0u]][s][0u][0u] = 1u;
        }
    }
    scan(34u, 27u, table);

    prepareForPrevColor(table, 27u);
    scan(27u, 18u, table);

    prepareForPrevColor(table, 18u);
    scan(18u, 9u, table);

    prepareForPrevColor(table, 9u);
    scan(9u, 0u, table);

    dumpTable(table);
}
