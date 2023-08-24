#if !defined(TSUMONYA_CORE_HPP_INCLUDE_GUARD)
#define TSUMONYA_CORE_HPP_INCLUDE_GUARD

#include <array>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <cstdint>


namespace Tsumonya{

using Table = std::array<std::array<std::array<std::array<std::array<std::array<std::uint_fast32_t, 26u>, 5u>, 5u>, 2u>, 5u>, 34u>;

inline constexpr std::array<std::array<std::uint_fast8_t, 7u>, 26u> stable = {{
    {{ 1u, 0u, 2u, 0u, 0u, 0u, 0u }},
    {{ 1u, 0u, 1u, 1u, 0u, 0u, 0u }},
    {{ 1u, 0u, 1u, 0u, 0u, 0u, 0u }},
    {{ 1u, 0u, 0u, 2u, 0u, 0u, 0u }},
    {{ 1u, 0u, 0u, 1u, 0u, 0u, 0u }},
    {{ 1u, 0u, 0u, 0u, 0u, 0u, 0u }},
    {{ 0u, 1u, 1u, 0u, 0u, 0u, 0u }},
    {{ 0u, 1u, 0u, 1u, 0u, 0u, 0u }},
    {{ 0u, 1u, 0u, 0u, 0u, 0u, 0u }},
    {{ 0u, 0u, 2u, 2u, 0u, 0u, 0u }},
    {{ 0u, 0u, 2u, 1u, 0u, 0u, 0u }},
    {{ 0u, 0u, 2u, 0u, 0u, 0u, 0u }},
    {{ 0u, 0u, 1u, 3u, 0u, 0u, 0u }},
    {{ 0u, 0u, 1u, 2u, 0u, 0u, 0u }},
    {{ 0u, 0u, 1u, 1u, 0u, 0u, 0u }},
    {{ 0u, 0u, 1u, 0u, 1u, 0u, 0u }},
    {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u }},
    {{ 0u, 0u, 0u, 4u, 0u, 0u, 0u }},
    {{ 0u, 0u, 0u, 3u, 0u, 0u, 0u }},
    {{ 0u, 0u, 0u, 2u, 0u, 0u, 0u }},
    {{ 0u, 0u, 0u, 1u, 1u, 0u, 0u }},
    {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u }},
    {{ 0u, 0u, 0u, 0u, 1u, 0u, 0u }},
    {{ 0u, 0u, 0u, 0u, 0u, 1u, 0u }},
    {{ 0u, 0u, 0u, 0u, 0u, 0u, 1u }},
    {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u }}
}};
inline constexpr std::array<std::uint_fast8_t, 26u> mtable  = {
    2u, 2u, 1u, 2u, 1u, 0u, 2u, 2u, 1u, 4u, 3u, 2u, 4u, 3u, 2u, 2u, 1u, 4u, 3u, 2u, 2u, 1u, 1u, 1u, 1u, 0u
};
inline constexpr std::array<std::uint_fast8_t, 26u> ntable  = {
    4u, 4u, 3u, 4u, 3u, 2u, 4u, 4u, 3u, 4u, 3u, 2u, 4u, 3u, 2u, 4u, 1u, 4u, 3u, 2u, 4u, 1u, 3u, 4u, 4u, 0u
};
inline constexpr std::array<std::uint_fast8_t, 26u> xytable = {
    2u, 2u, 1u, 2u, 1u, 0u, 1u, 1u, 0u, 4u, 3u, 2u, 4u, 3u, 2u, 1u, 1u, 4u, 3u, 2u, 1u, 1u, 0u, 0u, 0u, 0u
};

using Hand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengList = std::array<std::uint_fast8_t, 34u>;
using GangList = std::array<std::uint_fast8_t, 34u>;

inline constexpr std::array<std::pair<std::uint_fast8_t, std::uint_fast8_t>, 51u> fu_fan_table = {
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(   0u,  0u ), //  0
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  30u,  0u ), //  1
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  40u,  0u ), //  2
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  50u,  0u ), //  3
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  60u,  0u ), //  4
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  70u,  0u ), //  5
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  30u,  1u ), //  6
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  40u,  1u ), //  7
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  50u,  1u ), //  8
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  60u,  1u ), //  9
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  70u,  1u ), // 10
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  80u,  1u ), // 11
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  90u,  1u ), // 12
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( 100u,  1u ), // 13
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( 110u,  1u ), // 14
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  20u,  2u ), // 15
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  25u,  2u ), // 16
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  30u,  2u ), // 17
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  40u,  2u ), // 18
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  50u,  2u ), // 19
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  60u,  2u ), // 20
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  70u,  2u ), // 21
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  80u,  2u ), // 22
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  90u,  2u ), // 23
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( 100u,  2u ), // 24
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( 110u,  2u ), // 25
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  20u,  3u ), // 26
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  25u,  3u ), // 27
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  30u,  3u ), // 28
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  40u,  3u ), // 29
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  50u,  3u ), // 30
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  60u,  3u ), // 31
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  20u,  4u ), // 32
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  25u,  4u ), // 33
    std::pair<std::uint_fast8_t, std::uint_fast8_t>(  30u,  4u ), // 34
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  3u ), // 35
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  4u ), // 36
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  5u ), // 37
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  6u ), // 38
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  7u ), // 39
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  8u ), // 40
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX,  9u ), // 41
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 10u ), // 42
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 11u ), // 43
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 12u ), // 44
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 13u ), // 45
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 26u ), // 46
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 39u ), // 47
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 52u ), // 48
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 65u ), // 49
    std::pair<std::uint_fast8_t, std::uint_fast8_t>( UINT_FAST8_MAX, 78u ), // 50
};

inline std::uint_fast8_t encodeFuFan(std::uint_fast8_t const fu, std::uint_fast8_t const fan)
{
    if (fu >= 70u && fan == 3u) {
        return 35u;
    }
    if (fu >= 40u && fan == 4u) {
        return 36u;
    }
    if (fan <= 4u) {
        for (std::uint_fast8_t i = 0u; i <= 34u; ++i) {
            if (fu == fu_fan_table[i].first && fan == fu_fan_table[i].second) {
                return i;
            }
        }
        throw std::invalid_argument("An invalid argument.");
    }

    std::uint_fast8_t const fan_truncated = fan <= 13u ? fan : (fan / 13u) * 13u;
    for (std::uint_fast8_t i = 30u; i < fu_fan_table.size(); ++i) {
        if (fan_truncated == fu_fan_table[i].second) {
            return i;
        }
    }
    throw std::invalid_argument("An invalid argument.");
}

} // namespace Tsumonya

#endif
