#if !defined(TSUMONYA_CORE_HPP_INCLUDE_GUARD)
#define TSUMONYA_CORE_HPP_INCLUDE_GUARD

#include <array>
#include <cstdint>


namespace Tsumonya{

using Table = std::array<std::array<std::array<std::array<std::array<std::uint_fast32_t, 5u>, 5u>, 2u>, 6u>, 34u>;

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
inline constexpr std::array<std::uint_fast8_t, 26u> mtable  = { 3u, 3u, 2u, 3u, 2u, 1u, 2u, 2u, 1u, 4u, 3u, 2u, 4u, 3u, 2u, 2u, 1u, 4u, 3u, 2u, 2u, 1u, 1u, 1u, 1u, 0u };
inline constexpr std::array<std::uint_fast8_t, 26u> ntable  = { 4u, 4u, 3u, 4u, 3u, 2u, 4u, 4u, 3u, 4u, 3u, 2u, 4u, 3u, 2u, 4u, 1u, 4u, 3u, 2u, 4u, 1u, 3u, 4u, 4u, 0u };
inline constexpr std::array<std::uint_fast8_t, 26u> xytable = { 2u, 2u, 1u, 2u, 1u, 0u, 1u, 1u, 0u, 4u, 3u, 2u, 4u, 3u, 2u, 1u, 1u, 4u, 3u, 2u, 1u, 1u, 0u, 0u, 0u, 0u };

} // namespace Tsumonya

#endif // !defined(TSUMONYA_CORE_HPP_INCLUDE_GUARD)
