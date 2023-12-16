#if !defined(TSUMONYA_CORE_HPP_INCLUDE_GUARD)
#define TSUMONYA_CORE_HPP_INCLUDE_GUARD

#include <array>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <cstdint>


namespace Tsumonya{

// T(i, m, h, w, x, y, a, b, s)
using Table = std::array<
    std::array<
        std::array<
            std::array<
                std::array<
                    std::array<
                        std::array<
                            std::array<
                                std::array<
                                    std::uint_fast64_t, 70u
                                >, 2u
                            >, 2u
                        >, 5u
                    >, 5u
                >, 3u
            >, 2u
        >, 5u
    >, 34u
>;

inline constexpr std::array<std::array<std::uint_fast8_t, 8u>, 70u> stable = {{
    // 0th element: The number of non-open triplets (either concealed ones or one including the winning tile) composed of tile `i`.
    // 1st element: The number of non-open three-in-a-row (either concealed ones or one including the winning tile) starting with tile `i`.
    // 2nd element: The number of open triplets composed of a kind of tiles.
    // 3rd element: The number of open three-in-a-row starting with tile `i`.
    // 4th element: The number of concealed quadraples composed of tile `i`.
    // 5th element: The number of open quadraples composed of tile `i`.
    // 6th element: The number of heads composed of tile `i`.
    // 7th element:
    //   0: Tile `i` is not the winning one.
    //   1: Tile `i` is the winning one by self-draw.
    //   2: Tile `i` is the winning one by deal-in.

    {{ 1u, 1u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s =  0
    {{ 1u, 1u, 0u, 0u, 0u, 0u, 0u, 1u }}, // s =  1
    {{ 1u, 1u, 0u, 0u, 0u, 0u, 0u, 2u }}, // s =  2

    {{ 1u, 0u, 0u, 1u, 0u, 0u, 0u, 0u }}, // s =  3
    {{ 1u, 0u, 0u, 1u, 0u, 0u, 0u, 1u }}, // s =  4
    {{ 1u, 0u, 0u, 1u, 0u, 0u, 0u, 2u }}, // s =  5

    {{ 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s =  6
    {{ 1u, 0u, 0u, 0u, 0u, 0u, 0u, 1u }}, // s =  7
    {{ 1u, 0u, 0u, 0u, 0u, 0u, 0u, 2u }}, // s =  8

    {{ 0u, 2u, 0u, 2u, 0u, 0u, 0u, 0u }}, // s =  9
    {{ 0u, 2u, 0u, 2u, 0u, 0u, 0u, 1u }}, // s = 10
    {{ 0u, 2u, 0u, 2u, 0u, 0u, 0u, 2u }}, // s = 11

    {{ 0u, 2u, 0u, 1u, 0u, 0u, 0u, 0u }}, // s = 12
    {{ 0u, 2u, 0u, 1u, 0u, 0u, 0u, 1u }}, // s = 13
    {{ 0u, 2u, 0u, 1u, 0u, 0u, 0u, 2u }}, // s = 14

    {{ 0u, 2u, 0u, 0u, 0u, 0u, 1u, 0u }}, // s = 15
    {{ 0u, 2u, 0u, 0u, 0u, 0u, 1u, 1u }}, // s = 16
    {{ 0u, 2u, 0u, 0u, 0u, 0u, 1u, 2u }}, // s = 17

    {{ 0u, 2u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s = 18
    {{ 0u, 2u, 0u, 0u, 0u, 0u, 0u, 1u }}, // s = 19
    {{ 0u, 2u, 0u, 0u, 0u, 0u, 0u, 2u }}, // s = 20

    {{ 0u, 1u, 1u, 0u, 0u, 0u, 0u, 0u }}, // s = 21
    {{ 0u, 1u, 1u, 0u, 0u, 0u, 0u, 1u }}, // s = 22
    {{ 0u, 1u, 1u, 0u, 0u, 0u, 0u, 2u }}, // s = 23

    {{ 0u, 1u, 0u, 3u, 0u, 0u, 0u, 0u }}, // s = 24
    {{ 0u, 1u, 0u, 3u, 0u, 0u, 0u, 1u }}, // s = 25
    {{ 0u, 1u, 0u, 3u, 0u, 0u, 0u, 2u }}, // s = 26

    {{ 0u, 1u, 0u, 2u, 0u, 0u, 0u, 0u }}, // s = 27
    {{ 0u, 1u, 0u, 2u, 0u, 0u, 0u, 1u }}, // s = 28
    {{ 0u, 1u, 0u, 2u, 0u, 0u, 0u, 2u }}, // s = 29

    {{ 0u, 1u, 0u, 1u, 0u, 0u, 1u, 0u }}, // s = 30
    {{ 0u, 1u, 0u, 1u, 0u, 0u, 1u, 1u }}, // s = 31
    {{ 0u, 1u, 0u, 1u, 0u, 0u, 1u, 2u }}, // s = 32

    {{ 0u, 1u, 0u, 1u, 0u, 0u, 0u, 0u }}, // s = 33
    {{ 0u, 1u, 0u, 1u, 0u, 0u, 0u, 1u }}, // s = 34
    {{ 0u, 1u, 0u, 1u, 0u, 0u, 0u, 2u }}, // s = 35

    {{ 0u, 1u, 0u, 0u, 0u, 0u, 1u, 0u }}, // s = 36
    {{ 0u, 1u, 0u, 0u, 0u, 0u, 1u, 1u }}, // s = 37
    {{ 0u, 1u, 0u, 0u, 0u, 0u, 1u, 2u }}, // s = 38

    {{ 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s = 39
    {{ 0u, 1u, 0u, 0u, 0u, 0u, 0u, 1u }}, // s = 40
    {{ 0u, 1u, 0u, 0u, 0u, 0u, 0u, 2u }}, // s = 41

    {{ 0u, 0u, 1u, 1u, 0u, 0u, 0u, 0u }}, // s = 42

    {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u }}, // s = 43
    {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u, 1u }}, // s = 44
    {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u, 2u }}, // s = 45

    {{ 0u, 0u, 0u, 4u, 0u, 0u, 0u, 0u }}, // s = 46

    {{ 0u, 0u, 0u, 3u, 0u, 0u, 0u, 0u }}, // s = 47
    {{ 0u, 0u, 0u, 3u, 0u, 0u, 0u, 1u }}, // s = 48
    {{ 0u, 0u, 0u, 3u, 0u, 0u, 0u, 2u }}, // s = 49

    {{ 0u, 0u, 0u, 2u, 0u, 0u, 1u, 0u }}, // s = 50
    {{ 0u, 0u, 0u, 2u, 0u, 0u, 1u, 1u }}, // s = 51
    {{ 0u, 0u, 0u, 2u, 0u, 0u, 1u, 2u }}, // s = 52

    {{ 0u, 0u, 0u, 2u, 0u, 0u, 0u, 0u }}, // s = 53
    {{ 0u, 0u, 0u, 2u, 0u, 0u, 0u, 1u }}, // s = 54
    {{ 0u, 0u, 0u, 2u, 0u, 0u, 0u, 2u }}, // s = 55

    {{ 0u, 0u, 0u, 1u, 0u, 0u, 1u, 0u }}, // s = 56
    {{ 0u, 0u, 0u, 1u, 0u, 0u, 1u, 1u }}, // s = 57
    {{ 0u, 0u, 0u, 1u, 0u, 0u, 1u, 2u }}, // s = 58

    {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u }}, // s = 59
    {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u, 1u }}, // s = 60
    {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u, 2u }}, // s = 61

    {{ 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u }}, // s = 62

    {{ 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u }}, // s = 63

    {{ 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u }}, // s = 64
    {{ 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u }}, // s = 65
    {{ 0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u }}, // s = 66

    {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s = 67
    {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u }}, // s = 68
    {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 2u }}, // s = 69
}};

// The table of the numbers of members for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> mtable = {
    2u, 2u, 2u, // s =  0,  1,  2
    2u, 2u, 2u, // s =  3,  4,  5
    1u, 1u, 1u, // s =  6,  7,  8
    4u, 4u, 4u, // s =  9, 10, 11
    3u, 3u, 3u, // s = 12, 13, 14
    2u, 2u, 2u, // s = 15, 16, 17
    2u, 2u, 2u, // s = 18, 19, 20
    2u, 2u, 2u, // s = 21, 22, 23
    4u, 4u, 4u, // s = 24, 25, 26
    3u, 3u, 3u, // s = 27, 28, 29
    2u, 2u, 2u, // s = 30, 31, 32
    2u, 2u, 2u, // s = 33, 34, 35
    1u, 1u, 1u, // s = 36, 37, 38
    1u, 1u, 1u, // s = 39, 40, 41
    2u,         // s = 42
    1u, 1u, 1u, // s = 43, 44, 45
    4u,         // s = 46
    3u, 3u, 3u, // s = 47, 48, 49
    2u, 2u, 2u, // s = 50, 51, 52
    2u, 2u, 2u, // s = 53, 54, 55
    1u, 1u, 1u, // s = 56, 57, 58
    1u, 1u, 1u, // s = 59, 60, 61
    1u,         // s = 62
    1u,         // s = 63
    0u, 0u, 0u, // s = 64, 65, 66
    0u, 0u, 0u, // s = 67, 68, 69
};

// The table of the numbers of tile `i` for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> ntable = {
    4u, 4u, 4u, // s =  0,  1,  2
    4u, 4u, 4u, // s =  3,  4,  5
    3u, 3u, 3u, // s =  6,  7,  8
    4u, 4u, 4u, // s =  9, 10, 11
    3u, 3u, 3u, // s = 12, 13, 14
    4u, 4u, 4u, // s = 15, 16, 17
    2u, 2u, 2u, // s = 18, 19, 20
    4u, 4u, 4u, // s = 21, 22, 23
    4u, 4u, 4u, // s = 24, 25, 26
    3u, 3u, 3u, // s = 27, 28, 29
    4u, 4u, 4u, // s = 30, 31, 32
    2u, 2u, 2u, // s = 33, 34, 35
    3u, 3u, 3u, // s = 36, 37, 38
    1u, 1u, 1u, // s = 39, 40, 41
    4u,         // s = 42
    3u, 3u, 3u, // s = 43, 44, 45
    4u,         // s = 46
    3u, 3u, 3u, // s = 47, 48, 49
    4u, 4u, 4u, // s = 50, 51, 52
    2u, 2u, 2u, // s = 53, 54, 55
    3u, 3u, 3u, // s = 56, 57, 58
    1u, 1u, 1u, // s = 59, 60, 61
    4u,         // s = 62
    4u,         // s = 63
    2u, 2u, 2u, // s = 64, 65, 66
    0u, 0u, 0u, // s = 67, 68, 69
};

// The table of the numbers of three-in-a-row for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> xytable = {
    1u, 1u, 1u, // s =  0,  1,  2
    1u, 1u, 1u, // s =  3,  4,  5
    0u, 0u, 0u, // s =  6,  7,  8
    4u, 4u, 4u, // s =  9, 10, 11
    3u, 3u, 3u, // s = 12, 13, 14
    2u, 2u, 2u, // s = 15, 16, 17
    2u, 2u, 2u, // s = 18, 19, 20
    1u, 1u, 1u, // s = 21, 22, 23
    4u, 4u, 4u, // s = 24, 25, 26
    3u, 3u, 3u, // s = 27, 28, 29
    2u, 2u, 2u, // s = 30, 31, 32
    2u, 2u, 2u, // s = 33, 34, 35
    1u, 1u, 1u, // s = 36, 37, 38
    1u, 1u, 1u, // s = 39, 40, 41
    1u,         // s = 42
    0u, 0u, 0u, // s = 43, 44, 45
    4u,         // s = 46
    3u, 3u, 3u, // s = 47, 48, 49
    2u, 2u, 2u, // s = 50, 51, 52
    2u, 2u, 2u, // s = 53, 54, 55
    1u, 1u, 1u, // s = 56, 57, 58
    1u, 1u, 1u, // s = 59, 60, 61
    0u,         // s = 62
    0u,         // s = 63
    0u, 0u, 0u, // s = 64, 65, 66
    0u, 0u, 0u, // s = 67, 68, 69
};

using Hand = std::array<std::uint_fast8_t, 34u>;
using PengList = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using GangList = std::array<std::uint_fast8_t, 34u>;

} // namespace Tsumonya

#endif
