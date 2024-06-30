// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(TSUMONYA_STANDARD_CORE_HPP_INCLUDE_GUARD)
#define TSUMONYA_STANDARD_CORE_HPP_INCLUDE_GUARD

#include <array>
#include <cstdint>


namespace Tsumonya::Standard_{

inline constexpr std::array<std::array<std::uint_fast8_t, 8u>, 70u> stable = {{
  // 0th element: The number of open quadraples (明槓子) composed of tile `i`.
  // 1st element: The number of concealed quadraples (暗槓子) composed of tile `i`.
  // 2nd element:
  //   0: Tile `i` is not the winning one.
  //   1: Tile `i` is the winning one by self-draw.
  //   2: Tile `i` is the winning one by deal-in.
  // 3rd element: The number of heads composed of tile `i`.
  // 4th element: The number of open triplets (明刻子) composed of tile `i`.
  // 5th element: The number of open three-in-a-row (明順子) starting with tile `i`.
  // 6th element: The number of non-open triplets (either concealed ones or one including the
  //              winning tile) composed of tile `i`.
  // 7th element: The number of non-open three-in-a-row (either concealed ones or one including the
  //              winning tile) starting with tile `i`.
  {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s =  0
  {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u }}, // s =  1
  {{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 2u }}, // s =  2
  {{ 0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u }}, // s =  3
  {{ 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u }}, // s =  4
  {{ 0u, 0u, 0u, 0u, 0u, 1u, 0u, 0u }}, // s =  5
  {{ 0u, 0u, 0u, 0u, 0u, 1u, 0u, 1u }}, // s =  6
  {{ 0u, 0u, 0u, 0u, 0u, 1u, 0u, 2u }}, // s =  7
  {{ 0u, 0u, 0u, 0u, 0u, 1u, 1u, 0u }}, // s =  8
  {{ 0u, 0u, 0u, 0u, 0u, 2u, 0u, 0u }}, // s =  9
  {{ 0u, 0u, 0u, 0u, 0u, 2u, 0u, 1u }}, // s = 10
  {{ 0u, 0u, 0u, 0u, 0u, 2u, 0u, 2u }}, // s = 11
  {{ 0u, 0u, 0u, 0u, 0u, 3u, 0u, 0u }}, // s = 12
  {{ 0u, 0u, 0u, 0u, 0u, 3u, 0u, 1u }}, // s = 13
  {{ 0u, 0u, 0u, 0u, 0u, 4u, 0u, 0u }}, // s = 14
  {{ 0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u }}, // s = 15
  {{ 0u, 0u, 0u, 0u, 1u, 0u, 0u, 1u }}, // s = 16
  {{ 0u, 0u, 0u, 0u, 1u, 1u, 0u, 0u }}, // s = 17
  {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u, 0u }}, // s = 18
  {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u, 1u }}, // s = 19
  {{ 0u, 0u, 0u, 1u, 0u, 0u, 0u, 2u }}, // s = 20
  {{ 0u, 0u, 0u, 1u, 0u, 1u, 0u, 0u }}, // s = 21
  {{ 0u, 0u, 0u, 1u, 0u, 1u, 0u, 1u }}, // s = 22
  {{ 0u, 0u, 0u, 1u, 0u, 2u, 0u, 0u }}, // s = 23
  {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u }}, // s = 24
  {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u, 1u }}, // s = 25
  {{ 0u, 0u, 1u, 0u, 0u, 0u, 0u, 2u }}, // s = 26
  {{ 0u, 0u, 1u, 0u, 0u, 0u, 1u, 0u }}, // s = 27
  {{ 0u, 0u, 1u, 0u, 0u, 0u, 1u, 1u }}, // s = 28
  {{ 0u, 0u, 1u, 0u, 0u, 1u, 0u, 0u }}, // s = 29
  {{ 0u, 0u, 1u, 0u, 0u, 1u, 0u, 1u }}, // s = 30
  {{ 0u, 0u, 1u, 0u, 0u, 1u, 0u, 2u }}, // s = 31
  {{ 0u, 0u, 1u, 0u, 0u, 1u, 1u, 0u }}, // s = 32
  {{ 0u, 0u, 1u, 0u, 0u, 2u, 0u, 0u }}, // s = 33
  {{ 0u, 0u, 1u, 0u, 0u, 2u, 0u, 1u }}, // s = 34
  {{ 0u, 0u, 1u, 0u, 0u, 2u, 0u, 2u }}, // s = 35
  {{ 0u, 0u, 1u, 0u, 0u, 3u, 0u, 0u }}, // s = 36
  {{ 0u, 0u, 1u, 0u, 0u, 3u, 0u, 1u }}, // s = 37
  {{ 0u, 0u, 1u, 0u, 1u, 0u, 0u, 0u }}, // s = 38
  {{ 0u, 0u, 1u, 0u, 1u, 0u, 0u, 1u }}, // s = 39
  {{ 0u, 0u, 1u, 1u, 0u, 0u, 0u, 0u }}, // s = 40
  {{ 0u, 0u, 1u, 1u, 0u, 0u, 0u, 1u }}, // s = 41
  {{ 0u, 0u, 1u, 1u, 0u, 0u, 0u, 2u }}, // s = 42
  {{ 0u, 0u, 1u, 1u, 0u, 1u, 0u, 0u }}, // s = 43
  {{ 0u, 0u, 1u, 1u, 0u, 1u, 0u, 1u }}, // s = 44
  {{ 0u, 0u, 1u, 1u, 0u, 2u, 0u, 0u }}, // s = 45
  {{ 0u, 0u, 2u, 0u, 0u, 0u, 0u, 0u }}, // s = 46
  {{ 0u, 0u, 2u, 0u, 0u, 0u, 0u, 1u }}, // s = 47
  {{ 0u, 0u, 2u, 0u, 0u, 0u, 0u, 2u }}, // s = 48
  {{ 0u, 0u, 2u, 0u, 0u, 0u, 1u, 0u }}, // s = 49
  {{ 0u, 0u, 2u, 0u, 0u, 0u, 1u, 1u }}, // s = 50
  {{ 0u, 0u, 2u, 0u, 0u, 1u, 0u, 0u }}, // s = 51
  {{ 0u, 0u, 2u, 0u, 0u, 1u, 0u, 1u }}, // s = 52
  {{ 0u, 0u, 2u, 0u, 0u, 1u, 0u, 2u }}, // s = 53
  {{ 0u, 0u, 2u, 0u, 0u, 1u, 1u, 0u }}, // s = 54
  {{ 0u, 0u, 2u, 0u, 0u, 2u, 0u, 0u }}, // s = 55
  {{ 0u, 0u, 2u, 0u, 0u, 2u, 0u, 1u }}, // s = 56
  {{ 0u, 0u, 2u, 0u, 0u, 2u, 0u, 2u }}, // s = 57
  {{ 0u, 0u, 2u, 0u, 0u, 3u, 0u, 0u }}, // s = 58
  {{ 0u, 0u, 2u, 0u, 0u, 3u, 0u, 1u }}, // s = 59
  {{ 0u, 0u, 2u, 0u, 1u, 0u, 0u, 0u }}, // s = 60
  {{ 0u, 0u, 2u, 0u, 1u, 0u, 0u, 1u }}, // s = 61
  {{ 0u, 0u, 2u, 1u, 0u, 0u, 0u, 0u }}, // s = 62
  {{ 0u, 0u, 2u, 1u, 0u, 0u, 0u, 1u }}, // s = 63
  {{ 0u, 0u, 2u, 1u, 0u, 0u, 0u, 2u }}, // s = 64
  {{ 0u, 0u, 2u, 1u, 0u, 1u, 0u, 0u }}, // s = 65
  {{ 0u, 0u, 2u, 1u, 0u, 1u, 0u, 1u }}, // s = 66
  {{ 0u, 0u, 2u, 1u, 0u, 2u, 0u, 0u }}, // s = 67
  {{ 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s = 68
  {{ 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }}, // s = 69
}};

// The table of the numbers of members for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> mtable = {
  stable[0u][0u] + stable[0u][1u] + stable[0u][4u] + stable[0u][5u] + stable[0u][6u] + stable[0u][7u],
  stable[1u][0u] + stable[1u][1u] + stable[1u][4u] + stable[1u][5u] + stable[1u][6u] + stable[1u][7u],
  stable[2u][0u] + stable[2u][1u] + stable[2u][4u] + stable[2u][5u] + stable[2u][6u] + stable[2u][7u],
  stable[3u][0u] + stable[3u][1u] + stable[3u][4u] + stable[3u][5u] + stable[3u][6u] + stable[3u][7u],
  stable[4u][0u] + stable[4u][1u] + stable[4u][4u] + stable[4u][5u] + stable[4u][6u] + stable[4u][7u],
  stable[5u][0u] + stable[5u][1u] + stable[5u][4u] + stable[5u][5u] + stable[5u][6u] + stable[5u][7u],
  stable[6u][0u] + stable[6u][1u] + stable[6u][4u] + stable[6u][5u] + stable[6u][6u] + stable[6u][7u],
  stable[7u][0u] + stable[7u][1u] + stable[7u][4u] + stable[7u][5u] + stable[7u][6u] + stable[7u][7u],
  stable[8u][0u] + stable[8u][1u] + stable[8u][4u] + stable[8u][5u] + stable[8u][6u] + stable[8u][7u],
  stable[9u][0u] + stable[9u][1u] + stable[9u][4u] + stable[9u][5u] + stable[9u][6u] + stable[9u][7u],
  stable[10u][0u] + stable[10u][1u] + stable[10u][4u] + stable[10u][5u] + stable[10u][6u] + stable[10u][7u],
  stable[11u][0u] + stable[11u][1u] + stable[11u][4u] + stable[11u][5u] + stable[11u][6u] + stable[11u][7u],
  stable[12u][0u] + stable[12u][1u] + stable[12u][4u] + stable[12u][5u] + stable[12u][6u] + stable[12u][7u],
  stable[13u][0u] + stable[13u][1u] + stable[13u][4u] + stable[13u][5u] + stable[13u][6u] + stable[13u][7u],
  stable[14u][0u] + stable[14u][1u] + stable[14u][4u] + stable[14u][5u] + stable[14u][6u] + stable[14u][7u],
  stable[15u][0u] + stable[15u][1u] + stable[15u][4u] + stable[15u][5u] + stable[15u][6u] + stable[15u][7u],
  stable[16u][0u] + stable[16u][1u] + stable[16u][4u] + stable[16u][5u] + stable[16u][6u] + stable[16u][7u],
  stable[17u][0u] + stable[17u][1u] + stable[17u][4u] + stable[17u][5u] + stable[17u][6u] + stable[17u][7u],
  stable[18u][0u] + stable[18u][1u] + stable[18u][4u] + stable[18u][5u] + stable[18u][6u] + stable[18u][7u],
  stable[19u][0u] + stable[19u][1u] + stable[19u][4u] + stable[19u][5u] + stable[19u][6u] + stable[19u][7u],
  stable[20u][0u] + stable[20u][1u] + stable[20u][4u] + stable[20u][5u] + stable[20u][6u] + stable[20u][7u],
  stable[21u][0u] + stable[21u][1u] + stable[21u][4u] + stable[21u][5u] + stable[21u][6u] + stable[21u][7u],
  stable[22u][0u] + stable[22u][1u] + stable[22u][4u] + stable[22u][5u] + stable[22u][6u] + stable[22u][7u],
  stable[23u][0u] + stable[23u][1u] + stable[23u][4u] + stable[23u][5u] + stable[23u][6u] + stable[23u][7u],
  stable[24u][0u] + stable[24u][1u] + stable[24u][4u] + stable[24u][5u] + stable[24u][6u] + stable[24u][7u],
  stable[25u][0u] + stable[25u][1u] + stable[25u][4u] + stable[25u][5u] + stable[25u][6u] + stable[25u][7u],
  stable[26u][0u] + stable[26u][1u] + stable[26u][4u] + stable[26u][5u] + stable[26u][6u] + stable[26u][7u],
  stable[27u][0u] + stable[27u][1u] + stable[27u][4u] + stable[27u][5u] + stable[27u][6u] + stable[27u][7u],
  stable[28u][0u] + stable[28u][1u] + stable[28u][4u] + stable[28u][5u] + stable[28u][6u] + stable[28u][7u],
  stable[29u][0u] + stable[29u][1u] + stable[29u][4u] + stable[29u][5u] + stable[29u][6u] + stable[29u][7u],
  stable[30u][0u] + stable[30u][1u] + stable[30u][4u] + stable[30u][5u] + stable[30u][6u] + stable[30u][7u],
  stable[31u][0u] + stable[31u][1u] + stable[31u][4u] + stable[31u][5u] + stable[31u][6u] + stable[31u][7u],
  stable[32u][0u] + stable[32u][1u] + stable[32u][4u] + stable[32u][5u] + stable[32u][6u] + stable[32u][7u],
  stable[33u][0u] + stable[33u][1u] + stable[33u][4u] + stable[33u][5u] + stable[33u][6u] + stable[33u][7u],
  stable[34u][0u] + stable[34u][1u] + stable[34u][4u] + stable[34u][5u] + stable[34u][6u] + stable[34u][7u],
  stable[35u][0u] + stable[35u][1u] + stable[35u][4u] + stable[35u][5u] + stable[35u][6u] + stable[35u][7u],
  stable[36u][0u] + stable[36u][1u] + stable[36u][4u] + stable[36u][5u] + stable[36u][6u] + stable[36u][7u],
  stable[37u][0u] + stable[37u][1u] + stable[37u][4u] + stable[37u][5u] + stable[37u][6u] + stable[37u][7u],
  stable[38u][0u] + stable[38u][1u] + stable[38u][4u] + stable[38u][5u] + stable[38u][6u] + stable[38u][7u],
  stable[39u][0u] + stable[39u][1u] + stable[39u][4u] + stable[39u][5u] + stable[39u][6u] + stable[39u][7u],
  stable[40u][0u] + stable[40u][1u] + stable[40u][4u] + stable[40u][5u] + stable[40u][6u] + stable[40u][7u],
  stable[41u][0u] + stable[41u][1u] + stable[41u][4u] + stable[41u][5u] + stable[41u][6u] + stable[41u][7u],
  stable[42u][0u] + stable[42u][1u] + stable[42u][4u] + stable[42u][5u] + stable[42u][6u] + stable[42u][7u],
  stable[43u][0u] + stable[43u][1u] + stable[43u][4u] + stable[43u][5u] + stable[43u][6u] + stable[43u][7u],
  stable[44u][0u] + stable[44u][1u] + stable[44u][4u] + stable[44u][5u] + stable[44u][6u] + stable[44u][7u],
  stable[45u][0u] + stable[45u][1u] + stable[45u][4u] + stable[45u][5u] + stable[45u][6u] + stable[45u][7u],
  stable[46u][0u] + stable[46u][1u] + stable[46u][4u] + stable[46u][5u] + stable[46u][6u] + stable[46u][7u],
  stable[47u][0u] + stable[47u][1u] + stable[47u][4u] + stable[47u][5u] + stable[47u][6u] + stable[47u][7u],
  stable[48u][0u] + stable[48u][1u] + stable[48u][4u] + stable[48u][5u] + stable[48u][6u] + stable[48u][7u],
  stable[49u][0u] + stable[49u][1u] + stable[49u][4u] + stable[49u][5u] + stable[49u][6u] + stable[49u][7u],
  stable[50u][0u] + stable[50u][1u] + stable[50u][4u] + stable[50u][5u] + stable[50u][6u] + stable[50u][7u],
  stable[51u][0u] + stable[51u][1u] + stable[51u][4u] + stable[51u][5u] + stable[51u][6u] + stable[51u][7u],
  stable[52u][0u] + stable[52u][1u] + stable[52u][4u] + stable[52u][5u] + stable[52u][6u] + stable[52u][7u],
  stable[53u][0u] + stable[53u][1u] + stable[53u][4u] + stable[53u][5u] + stable[53u][6u] + stable[53u][7u],
  stable[54u][0u] + stable[54u][1u] + stable[54u][4u] + stable[54u][5u] + stable[54u][6u] + stable[54u][7u],
  stable[55u][0u] + stable[55u][1u] + stable[55u][4u] + stable[55u][5u] + stable[55u][6u] + stable[55u][7u],
  stable[56u][0u] + stable[56u][1u] + stable[56u][4u] + stable[56u][5u] + stable[56u][6u] + stable[56u][7u],
  stable[57u][0u] + stable[57u][1u] + stable[57u][4u] + stable[57u][5u] + stable[57u][6u] + stable[57u][7u],
  stable[58u][0u] + stable[58u][1u] + stable[58u][4u] + stable[58u][5u] + stable[58u][6u] + stable[58u][7u],
  stable[59u][0u] + stable[59u][1u] + stable[59u][4u] + stable[59u][5u] + stable[59u][6u] + stable[59u][7u],
  stable[60u][0u] + stable[60u][1u] + stable[60u][4u] + stable[60u][5u] + stable[60u][6u] + stable[60u][7u],
  stable[61u][0u] + stable[61u][1u] + stable[61u][4u] + stable[61u][5u] + stable[61u][6u] + stable[61u][7u],
  stable[62u][0u] + stable[62u][1u] + stable[62u][4u] + stable[62u][5u] + stable[62u][6u] + stable[62u][7u],
  stable[63u][0u] + stable[63u][1u] + stable[63u][4u] + stable[63u][5u] + stable[63u][6u] + stable[63u][7u],
  stable[64u][0u] + stable[64u][1u] + stable[64u][4u] + stable[64u][5u] + stable[64u][6u] + stable[64u][7u],
  stable[65u][0u] + stable[65u][1u] + stable[65u][4u] + stable[65u][5u] + stable[65u][6u] + stable[65u][7u],
  stable[66u][0u] + stable[66u][1u] + stable[66u][4u] + stable[66u][5u] + stable[66u][6u] + stable[66u][7u],
  stable[67u][0u] + stable[67u][1u] + stable[67u][4u] + stable[67u][5u] + stable[67u][6u] + stable[67u][7u],
  stable[68u][0u] + stable[68u][1u] + stable[68u][4u] + stable[68u][5u] + stable[68u][6u] + stable[68u][7u],
  stable[69u][0u] + stable[69u][1u] + stable[69u][4u] + stable[69u][5u] + stable[69u][6u] + stable[69u][7u],
};

// The table of the numbers of tile `i` for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> ntable = {
  stable[0u][0u] * 4u + stable[0u][1u] * 4u + stable[0u][3u] * 2u + stable[0u][4u] * 3u + stable[0u][5u] + stable[0u][6u] * 3u + stable[0u][7u],
  stable[1u][0u] * 4u + stable[1u][1u] * 4u + stable[1u][3u] * 2u + stable[1u][4u] * 3u + stable[1u][5u] + stable[1u][6u] * 3u + stable[1u][7u],
  stable[2u][0u] * 4u + stable[2u][1u] * 4u + stable[2u][3u] * 2u + stable[2u][4u] * 3u + stable[2u][5u] + stable[2u][6u] * 3u + stable[2u][7u],
  stable[3u][0u] * 4u + stable[3u][1u] * 4u + stable[3u][3u] * 2u + stable[3u][4u] * 3u + stable[3u][5u] + stable[3u][6u] * 3u + stable[3u][7u],
  stable[4u][0u] * 4u + stable[4u][1u] * 4u + stable[4u][3u] * 2u + stable[4u][4u] * 3u + stable[4u][5u] + stable[4u][6u] * 3u + stable[4u][7u],
  stable[5u][0u] * 4u + stable[5u][1u] * 4u + stable[5u][3u] * 2u + stable[5u][4u] * 3u + stable[5u][5u] + stable[5u][6u] * 3u + stable[5u][7u],
  stable[6u][0u] * 4u + stable[6u][1u] * 4u + stable[6u][3u] * 2u + stable[6u][4u] * 3u + stable[6u][5u] + stable[6u][6u] * 3u + stable[6u][7u],
  stable[7u][0u] * 4u + stable[7u][1u] * 4u + stable[7u][3u] * 2u + stable[7u][4u] * 3u + stable[7u][5u] + stable[7u][6u] * 3u + stable[7u][7u],
  stable[8u][0u] * 4u + stable[8u][1u] * 4u + stable[8u][3u] * 2u + stable[8u][4u] * 3u + stable[8u][5u] + stable[8u][6u] * 3u + stable[8u][7u],
  stable[9u][0u] * 4u + stable[9u][1u] * 4u + stable[9u][3u] * 2u + stable[9u][4u] * 3u + stable[9u][5u] + stable[9u][6u] * 3u + stable[9u][7u],
  stable[10u][0u] * 4u + stable[10u][1u] * 4u + stable[10u][3u] * 2u + stable[10u][4u] * 3u + stable[10u][5u] + stable[10u][6u] * 3u + stable[10u][7u],
  stable[11u][0u] * 4u + stable[11u][1u] * 4u + stable[11u][3u] * 2u + stable[11u][4u] * 3u + stable[11u][5u] + stable[11u][6u] * 3u + stable[11u][7u],
  stable[12u][0u] * 4u + stable[12u][1u] * 4u + stable[12u][3u] * 2u + stable[12u][4u] * 3u + stable[12u][5u] + stable[12u][6u] * 3u + stable[12u][7u],
  stable[13u][0u] * 4u + stable[13u][1u] * 4u + stable[13u][3u] * 2u + stable[13u][4u] * 3u + stable[13u][5u] + stable[13u][6u] * 3u + stable[13u][7u],
  stable[14u][0u] * 4u + stable[14u][1u] * 4u + stable[14u][3u] * 2u + stable[14u][4u] * 3u + stable[14u][5u] + stable[14u][6u] * 3u + stable[14u][7u],
  stable[15u][0u] * 4u + stable[15u][1u] * 4u + stable[15u][3u] * 2u + stable[15u][4u] * 3u + stable[15u][5u] + stable[15u][6u] * 3u + stable[15u][7u],
  stable[16u][0u] * 4u + stable[16u][1u] * 4u + stable[16u][3u] * 2u + stable[16u][4u] * 3u + stable[16u][5u] + stable[16u][6u] * 3u + stable[16u][7u],
  stable[17u][0u] * 4u + stable[17u][1u] * 4u + stable[17u][3u] * 2u + stable[17u][4u] * 3u + stable[17u][5u] + stable[17u][6u] * 3u + stable[17u][7u],
  stable[18u][0u] * 4u + stable[18u][1u] * 4u + stable[18u][3u] * 2u + stable[18u][4u] * 3u + stable[18u][5u] + stable[18u][6u] * 3u + stable[18u][7u],
  stable[19u][0u] * 4u + stable[19u][1u] * 4u + stable[19u][3u] * 2u + stable[19u][4u] * 3u + stable[19u][5u] + stable[19u][6u] * 3u + stable[19u][7u],
  stable[20u][0u] * 4u + stable[20u][1u] * 4u + stable[20u][3u] * 2u + stable[20u][4u] * 3u + stable[20u][5u] + stable[20u][6u] * 3u + stable[20u][7u],
  stable[21u][0u] * 4u + stable[21u][1u] * 4u + stable[21u][3u] * 2u + stable[21u][4u] * 3u + stable[21u][5u] + stable[21u][6u] * 3u + stable[21u][7u],
  stable[22u][0u] * 4u + stable[22u][1u] * 4u + stable[22u][3u] * 2u + stable[22u][4u] * 3u + stable[22u][5u] + stable[22u][6u] * 3u + stable[22u][7u],
  stable[23u][0u] * 4u + stable[23u][1u] * 4u + stable[23u][3u] * 2u + stable[23u][4u] * 3u + stable[23u][5u] + stable[23u][6u] * 3u + stable[23u][7u],
  stable[24u][0u] * 4u + stable[24u][1u] * 4u + stable[24u][3u] * 2u + stable[24u][4u] * 3u + stable[24u][5u] + stable[24u][6u] * 3u + stable[24u][7u],
  stable[25u][0u] * 4u + stable[25u][1u] * 4u + stable[25u][3u] * 2u + stable[25u][4u] * 3u + stable[25u][5u] + stable[25u][6u] * 3u + stable[25u][7u],
  stable[26u][0u] * 4u + stable[26u][1u] * 4u + stable[26u][3u] * 2u + stable[26u][4u] * 3u + stable[26u][5u] + stable[26u][6u] * 3u + stable[26u][7u],
  stable[27u][0u] * 4u + stable[27u][1u] * 4u + stable[27u][3u] * 2u + stable[27u][4u] * 3u + stable[27u][5u] + stable[27u][6u] * 3u + stable[27u][7u],
  stable[28u][0u] * 4u + stable[28u][1u] * 4u + stable[28u][3u] * 2u + stable[28u][4u] * 3u + stable[28u][5u] + stable[28u][6u] * 3u + stable[28u][7u],
  stable[29u][0u] * 4u + stable[29u][1u] * 4u + stable[29u][3u] * 2u + stable[29u][4u] * 3u + stable[29u][5u] + stable[29u][6u] * 3u + stable[29u][7u],
  stable[30u][0u] * 4u + stable[30u][1u] * 4u + stable[30u][3u] * 2u + stable[30u][4u] * 3u + stable[30u][5u] + stable[30u][6u] * 3u + stable[30u][7u],
  stable[31u][0u] * 4u + stable[31u][1u] * 4u + stable[31u][3u] * 2u + stable[31u][4u] * 3u + stable[31u][5u] + stable[31u][6u] * 3u + stable[31u][7u],
  stable[32u][0u] * 4u + stable[32u][1u] * 4u + stable[32u][3u] * 2u + stable[32u][4u] * 3u + stable[32u][5u] + stable[32u][6u] * 3u + stable[32u][7u],
  stable[33u][0u] * 4u + stable[33u][1u] * 4u + stable[33u][3u] * 2u + stable[33u][4u] * 3u + stable[33u][5u] + stable[33u][6u] * 3u + stable[33u][7u],
  stable[34u][0u] * 4u + stable[34u][1u] * 4u + stable[34u][3u] * 2u + stable[34u][4u] * 3u + stable[34u][5u] + stable[34u][6u] * 3u + stable[34u][7u],
  stable[35u][0u] * 4u + stable[35u][1u] * 4u + stable[35u][3u] * 2u + stable[35u][4u] * 3u + stable[35u][5u] + stable[35u][6u] * 3u + stable[35u][7u],
  stable[36u][0u] * 4u + stable[36u][1u] * 4u + stable[36u][3u] * 2u + stable[36u][4u] * 3u + stable[36u][5u] + stable[36u][6u] * 3u + stable[36u][7u],
  stable[37u][0u] * 4u + stable[37u][1u] * 4u + stable[37u][3u] * 2u + stable[37u][4u] * 3u + stable[37u][5u] + stable[37u][6u] * 3u + stable[37u][7u],
  stable[38u][0u] * 4u + stable[38u][1u] * 4u + stable[38u][3u] * 2u + stable[38u][4u] * 3u + stable[38u][5u] + stable[38u][6u] * 3u + stable[38u][7u],
  stable[39u][0u] * 4u + stable[39u][1u] * 4u + stable[39u][3u] * 2u + stable[39u][4u] * 3u + stable[39u][5u] + stable[39u][6u] * 3u + stable[39u][7u],
  stable[40u][0u] * 4u + stable[40u][1u] * 4u + stable[40u][3u] * 2u + stable[40u][4u] * 3u + stable[40u][5u] + stable[40u][6u] * 3u + stable[40u][7u],
  stable[41u][0u] * 4u + stable[41u][1u] * 4u + stable[41u][3u] * 2u + stable[41u][4u] * 3u + stable[41u][5u] + stable[41u][6u] * 3u + stable[41u][7u],
  stable[42u][0u] * 4u + stable[42u][1u] * 4u + stable[42u][3u] * 2u + stable[42u][4u] * 3u + stable[42u][5u] + stable[42u][6u] * 3u + stable[42u][7u],
  stable[43u][0u] * 4u + stable[43u][1u] * 4u + stable[43u][3u] * 2u + stable[43u][4u] * 3u + stable[43u][5u] + stable[43u][6u] * 3u + stable[43u][7u],
  stable[44u][0u] * 4u + stable[44u][1u] * 4u + stable[44u][3u] * 2u + stable[44u][4u] * 3u + stable[44u][5u] + stable[44u][6u] * 3u + stable[44u][7u],
  stable[45u][0u] * 4u + stable[45u][1u] * 4u + stable[45u][3u] * 2u + stable[45u][4u] * 3u + stable[45u][5u] + stable[45u][6u] * 3u + stable[45u][7u],
  stable[46u][0u] * 4u + stable[46u][1u] * 4u + stable[46u][3u] * 2u + stable[46u][4u] * 3u + stable[46u][5u] + stable[46u][6u] * 3u + stable[46u][7u],
  stable[47u][0u] * 4u + stable[47u][1u] * 4u + stable[47u][3u] * 2u + stable[47u][4u] * 3u + stable[47u][5u] + stable[47u][6u] * 3u + stable[47u][7u],
  stable[48u][0u] * 4u + stable[48u][1u] * 4u + stable[48u][3u] * 2u + stable[48u][4u] * 3u + stable[48u][5u] + stable[48u][6u] * 3u + stable[48u][7u],
  stable[49u][0u] * 4u + stable[49u][1u] * 4u + stable[49u][3u] * 2u + stable[49u][4u] * 3u + stable[49u][5u] + stable[49u][6u] * 3u + stable[49u][7u],
  stable[50u][0u] * 4u + stable[50u][1u] * 4u + stable[50u][3u] * 2u + stable[50u][4u] * 3u + stable[50u][5u] + stable[50u][6u] * 3u + stable[50u][7u],
  stable[51u][0u] * 4u + stable[51u][1u] * 4u + stable[51u][3u] * 2u + stable[51u][4u] * 3u + stable[51u][5u] + stable[51u][6u] * 3u + stable[51u][7u],
  stable[52u][0u] * 4u + stable[52u][1u] * 4u + stable[52u][3u] * 2u + stable[52u][4u] * 3u + stable[52u][5u] + stable[52u][6u] * 3u + stable[52u][7u],
  stable[53u][0u] * 4u + stable[53u][1u] * 4u + stable[53u][3u] * 2u + stable[53u][4u] * 3u + stable[53u][5u] + stable[53u][6u] * 3u + stable[53u][7u],
  stable[54u][0u] * 4u + stable[54u][1u] * 4u + stable[54u][3u] * 2u + stable[54u][4u] * 3u + stable[54u][5u] + stable[54u][6u] * 3u + stable[54u][7u],
  stable[55u][0u] * 4u + stable[55u][1u] * 4u + stable[55u][3u] * 2u + stable[55u][4u] * 3u + stable[55u][5u] + stable[55u][6u] * 3u + stable[55u][7u],
  stable[56u][0u] * 4u + stable[56u][1u] * 4u + stable[56u][3u] * 2u + stable[56u][4u] * 3u + stable[56u][5u] + stable[56u][6u] * 3u + stable[56u][7u],
  stable[57u][0u] * 4u + stable[57u][1u] * 4u + stable[57u][3u] * 2u + stable[57u][4u] * 3u + stable[57u][5u] + stable[57u][6u] * 3u + stable[57u][7u],
  stable[58u][0u] * 4u + stable[58u][1u] * 4u + stable[58u][3u] * 2u + stable[58u][4u] * 3u + stable[58u][5u] + stable[58u][6u] * 3u + stable[58u][7u],
  stable[59u][0u] * 4u + stable[59u][1u] * 4u + stable[59u][3u] * 2u + stable[59u][4u] * 3u + stable[59u][5u] + stable[59u][6u] * 3u + stable[59u][7u],
  stable[60u][0u] * 4u + stable[60u][1u] * 4u + stable[60u][3u] * 2u + stable[60u][4u] * 3u + stable[60u][5u] + stable[60u][6u] * 3u + stable[60u][7u],
  stable[61u][0u] * 4u + stable[61u][1u] * 4u + stable[61u][3u] * 2u + stable[61u][4u] * 3u + stable[61u][5u] + stable[61u][6u] * 3u + stable[61u][7u],
  stable[62u][0u] * 4u + stable[62u][1u] * 4u + stable[62u][3u] * 2u + stable[62u][4u] * 3u + stable[62u][5u] + stable[62u][6u] * 3u + stable[62u][7u],
  stable[63u][0u] * 4u + stable[63u][1u] * 4u + stable[63u][3u] * 2u + stable[63u][4u] * 3u + stable[63u][5u] + stable[63u][6u] * 3u + stable[63u][7u],
  stable[64u][0u] * 4u + stable[64u][1u] * 4u + stable[64u][3u] * 2u + stable[64u][4u] * 3u + stable[64u][5u] + stable[64u][6u] * 3u + stable[64u][7u],
  stable[65u][0u] * 4u + stable[65u][1u] * 4u + stable[65u][3u] * 2u + stable[65u][4u] * 3u + stable[65u][5u] + stable[65u][6u] * 3u + stable[65u][7u],
  stable[66u][0u] * 4u + stable[66u][1u] * 4u + stable[66u][3u] * 2u + stable[66u][4u] * 3u + stable[66u][5u] + stable[66u][6u] * 3u + stable[66u][7u],
  stable[67u][0u] * 4u + stable[67u][1u] * 4u + stable[67u][3u] * 2u + stable[67u][4u] * 3u + stable[67u][5u] + stable[67u][6u] * 3u + stable[67u][7u],
  stable[68u][0u] * 4u + stable[68u][1u] * 4u + stable[68u][3u] * 2u + stable[68u][4u] * 3u + stable[68u][5u] + stable[68u][6u] * 3u + stable[68u][7u],
  stable[69u][0u] * 4u + stable[69u][1u] * 4u + stable[69u][3u] * 2u + stable[69u][4u] * 3u + stable[69u][5u] + stable[69u][6u] * 3u + stable[69u][7u],
};

// The table of the numbers of (open and concealed) three-in-a-row for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> xytable = {
  stable[0u][5u] + stable[0u][7u],
  stable[1u][5u] + stable[1u][7u],
  stable[2u][5u] + stable[2u][7u],
  stable[3u][5u] + stable[3u][7u],
  stable[4u][5u] + stable[4u][7u],
  stable[5u][5u] + stable[5u][7u],
  stable[6u][5u] + stable[6u][7u],
  stable[7u][5u] + stable[7u][7u],
  stable[8u][5u] + stable[8u][7u],
  stable[9u][5u] + stable[9u][7u],
  stable[10u][5u] + stable[10u][7u],
  stable[11u][5u] + stable[11u][7u],
  stable[12u][5u] + stable[12u][7u],
  stable[13u][5u] + stable[13u][7u],
  stable[14u][5u] + stable[14u][7u],
  stable[15u][5u] + stable[15u][7u],
  stable[16u][5u] + stable[16u][7u],
  stable[17u][5u] + stable[17u][7u],
  stable[18u][5u] + stable[18u][7u],
  stable[19u][5u] + stable[19u][7u],
  stable[20u][5u] + stable[20u][7u],
  stable[21u][5u] + stable[21u][7u],
  stable[22u][5u] + stable[22u][7u],
  stable[23u][5u] + stable[23u][7u],
  stable[24u][5u] + stable[24u][7u],
  stable[25u][5u] + stable[25u][7u],
  stable[26u][5u] + stable[26u][7u],
  stable[27u][5u] + stable[27u][7u],
  stable[28u][5u] + stable[28u][7u],
  stable[29u][5u] + stable[29u][7u],
  stable[30u][5u] + stable[30u][7u],
  stable[31u][5u] + stable[31u][7u],
  stable[32u][5u] + stable[32u][7u],
  stable[33u][5u] + stable[33u][7u],
  stable[34u][5u] + stable[34u][7u],
  stable[35u][5u] + stable[35u][7u],
  stable[36u][5u] + stable[36u][7u],
  stable[37u][5u] + stable[37u][7u],
  stable[38u][5u] + stable[38u][7u],
  stable[39u][5u] + stable[39u][7u],
  stable[40u][5u] + stable[40u][7u],
  stable[41u][5u] + stable[41u][7u],
  stable[42u][5u] + stable[42u][7u],
  stable[43u][5u] + stable[43u][7u],
  stable[44u][5u] + stable[44u][7u],
  stable[45u][5u] + stable[45u][7u],
  stable[46u][5u] + stable[46u][7u],
  stable[47u][5u] + stable[47u][7u],
  stable[48u][5u] + stable[48u][7u],
  stable[49u][5u] + stable[49u][7u],
  stable[50u][5u] + stable[50u][7u],
  stable[51u][5u] + stable[51u][7u],
  stable[52u][5u] + stable[52u][7u],
  stable[53u][5u] + stable[53u][7u],
  stable[54u][5u] + stable[54u][7u],
  stable[55u][5u] + stable[55u][7u],
  stable[56u][5u] + stable[56u][7u],
  stable[57u][5u] + stable[57u][7u],
  stable[58u][5u] + stable[58u][7u],
  stable[59u][5u] + stable[59u][7u],
  stable[60u][5u] + stable[60u][7u],
  stable[61u][5u] + stable[61u][7u],
  stable[62u][5u] + stable[62u][7u],
  stable[63u][5u] + stable[63u][7u],
  stable[64u][5u] + stable[64u][7u],
  stable[65u][5u] + stable[65u][7u],
  stable[66u][5u] + stable[66u][7u],
  stable[67u][5u] + stable[67u][7u],
  stable[68u][5u] + stable[68u][7u],
  stable[69u][5u] + stable[69u][7u],
};

// The number of tile `i` in the pure hand for each `s`.
inline constexpr std::array<std::uint_fast8_t, 70u> ptable = {
  stable[0u][3u] * 2u + stable[0u][6u] * 3u + stable[0u][7u],
  stable[1u][3u] * 2u + stable[1u][6u] * 3u + stable[1u][7u],
  stable[2u][3u] * 2u + stable[2u][6u] * 3u + stable[2u][7u],
  stable[3u][3u] * 2u + stable[3u][6u] * 3u + stable[3u][7u],
  stable[4u][3u] * 2u + stable[4u][6u] * 3u + stable[4u][7u],
  stable[5u][3u] * 2u + stable[5u][6u] * 3u + stable[5u][7u],
  stable[6u][3u] * 2u + stable[6u][6u] * 3u + stable[6u][7u],
  stable[7u][3u] * 2u + stable[7u][6u] * 3u + stable[7u][7u],
  stable[8u][3u] * 2u + stable[8u][6u] * 3u + stable[8u][7u],
  stable[9u][3u] * 2u + stable[9u][6u] * 3u + stable[9u][7u],
  stable[10u][3u] * 2u + stable[10u][6u] * 3u + stable[10u][7u],
  stable[11u][3u] * 2u + stable[11u][6u] * 3u + stable[11u][7u],
  stable[12u][3u] * 2u + stable[12u][6u] * 3u + stable[12u][7u],
  stable[13u][3u] * 2u + stable[13u][6u] * 3u + stable[13u][7u],
  stable[14u][3u] * 2u + stable[14u][6u] * 3u + stable[14u][7u],
  stable[15u][3u] * 2u + stable[15u][6u] * 3u + stable[15u][7u],
  stable[16u][3u] * 2u + stable[16u][6u] * 3u + stable[16u][7u],
  stable[17u][3u] * 2u + stable[17u][6u] * 3u + stable[17u][7u],
  stable[18u][3u] * 2u + stable[18u][6u] * 3u + stable[18u][7u],
  stable[19u][3u] * 2u + stable[19u][6u] * 3u + stable[19u][7u],
  stable[20u][3u] * 2u + stable[20u][6u] * 3u + stable[20u][7u],
  stable[21u][3u] * 2u + stable[21u][6u] * 3u + stable[21u][7u],
  stable[22u][3u] * 2u + stable[22u][6u] * 3u + stable[22u][7u],
  stable[23u][3u] * 2u + stable[23u][6u] * 3u + stable[23u][7u],
  stable[24u][3u] * 2u + stable[24u][6u] * 3u + stable[24u][7u],
  stable[25u][3u] * 2u + stable[25u][6u] * 3u + stable[25u][7u],
  stable[26u][3u] * 2u + stable[26u][6u] * 3u + stable[26u][7u],
  stable[27u][3u] * 2u + stable[27u][6u] * 3u + stable[27u][7u],
  stable[28u][3u] * 2u + stable[28u][6u] * 3u + stable[28u][7u],
  stable[29u][3u] * 2u + stable[29u][6u] * 3u + stable[29u][7u],
  stable[30u][3u] * 2u + stable[30u][6u] * 3u + stable[30u][7u],
  stable[31u][3u] * 2u + stable[31u][6u] * 3u + stable[31u][7u],
  stable[32u][3u] * 2u + stable[32u][6u] * 3u + stable[32u][7u],
  stable[33u][3u] * 2u + stable[33u][6u] * 3u + stable[33u][7u],
  stable[34u][3u] * 2u + stable[34u][6u] * 3u + stable[34u][7u],
  stable[35u][3u] * 2u + stable[35u][6u] * 3u + stable[35u][7u],
  stable[36u][3u] * 2u + stable[36u][6u] * 3u + stable[36u][7u],
  stable[37u][3u] * 2u + stable[37u][6u] * 3u + stable[37u][7u],
  stable[38u][3u] * 2u + stable[38u][6u] * 3u + stable[38u][7u],
  stable[39u][3u] * 2u + stable[39u][6u] * 3u + stable[39u][7u],
  stable[40u][3u] * 2u + stable[40u][6u] * 3u + stable[40u][7u],
  stable[41u][3u] * 2u + stable[41u][6u] * 3u + stable[41u][7u],
  stable[42u][3u] * 2u + stable[42u][6u] * 3u + stable[42u][7u],
  stable[43u][3u] * 2u + stable[43u][6u] * 3u + stable[43u][7u],
  stable[44u][3u] * 2u + stable[44u][6u] * 3u + stable[44u][7u],
  stable[45u][3u] * 2u + stable[45u][6u] * 3u + stable[45u][7u],
  stable[46u][3u] * 2u + stable[46u][6u] * 3u + stable[46u][7u],
  stable[47u][3u] * 2u + stable[47u][6u] * 3u + stable[47u][7u],
  stable[48u][3u] * 2u + stable[48u][6u] * 3u + stable[48u][7u],
  stable[49u][3u] * 2u + stable[49u][6u] * 3u + stable[49u][7u],
  stable[50u][3u] * 2u + stable[50u][6u] * 3u + stable[50u][7u],
  stable[51u][3u] * 2u + stable[51u][6u] * 3u + stable[51u][7u],
  stable[52u][3u] * 2u + stable[52u][6u] * 3u + stable[52u][7u],
  stable[53u][3u] * 2u + stable[53u][6u] * 3u + stable[53u][7u],
  stable[54u][3u] * 2u + stable[54u][6u] * 3u + stable[54u][7u],
  stable[55u][3u] * 2u + stable[55u][6u] * 3u + stable[55u][7u],
  stable[56u][3u] * 2u + stable[56u][6u] * 3u + stable[56u][7u],
  stable[57u][3u] * 2u + stable[57u][6u] * 3u + stable[57u][7u],
  stable[58u][3u] * 2u + stable[58u][6u] * 3u + stable[58u][7u],
  stable[59u][3u] * 2u + stable[59u][6u] * 3u + stable[59u][7u],
  stable[60u][3u] * 2u + stable[60u][6u] * 3u + stable[60u][7u],
  stable[61u][3u] * 2u + stable[61u][6u] * 3u + stable[61u][7u],
  stable[62u][3u] * 2u + stable[62u][6u] * 3u + stable[62u][7u],
  stable[63u][3u] * 2u + stable[63u][6u] * 3u + stable[63u][7u],
  stable[64u][3u] * 2u + stable[64u][6u] * 3u + stable[64u][7u],
  stable[65u][3u] * 2u + stable[65u][6u] * 3u + stable[65u][7u],
  stable[66u][3u] * 2u + stable[66u][6u] * 3u + stable[66u][7u],
  stable[67u][3u] * 2u + stable[67u][6u] * 3u + stable[67u][7u],
  stable[68u][3u] * 2u + stable[68u][6u] * 3u + stable[68u][7u],
  stable[69u][3u] * 2u + stable[69u][6u] * 3u + stable[69u][7u],
};

// T'(i, m, h, w, x, y, a, b)
using Subtable = std::array<
  std::array<
    std::array<
      std::array<
        std::array<
          std::array<
            std::array<
              std::array<std::uint_fast64_t, 2u>, // b = 0, 1
              2u // a = 0, 1
            >, 5u // y = 0, 1, 2, 3, 4
          >, 5u // x = 0, 1, 2, 3, 4
        >, 3u // w = 0, 1, 2
      >, 2u // h = 0, 1
    >, 5u // m = 0, 1, 2, 3, 4
  >, 34u // i = 0, 1, 2, ..., 33
>;

// T(i, m, h, w, x, y, a, b, s)
using Table = std::array<
  std::array<
    std::array<
      std::array<
        std::array<
          std::array<
            std::array<
              std::array<
                std::array<std::uint_fast64_t, 70u>,
                2u
              >, 2u
            >, 5u
          >, 5u
        >, 3u
      >, 2u
    >, 5u
  >, 34u
>;

} // namespace Tsumonya::Standard_

#endif // !defined(TSUMONYA_STANDARD_CORE_HPP_INCLUDE_GUARD)
