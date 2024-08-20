// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(TSUMONYA_SEVEN_PAIRS_TABLE_HPP_INCLUDE_GUARD)
#define TSUMONYA_SEVEN_PAIRS_TABLE_HPP_INCLUDE_GUARD

#include <tsumonya/seven_pairs/core.hpp>
#include <cstdint>


namespace Tsumonya::SevenPairs_{

inline constexpr std::uint_fast32_t size = 5379616u;

inline constexpr Table table = {{
  {{4272048u,1107568u,0u,0u,0u,0u,0u,0u,}}, // i = 0
  {{3365856u,906192u,201376u,0u,0u,0u,0u,0u,}}, // i = 1
  {{2629575u,736281u,169911u,31465u,0u,0u,0u,0u,}}, // i = 2
  {{2035800u,593775u,142506u,27405u,4060u,0u,0u,0u,}}, // i = 3
  {{1560780u,475020u,118755u,23751u,3654u,406u,0u,0u,}}, // i = 4
  {{1184040u,376740u,98280u,20475u,3276u,378u,28u,0u,}}, // i = 5
  {{888030u,296010u,80730u,17550u,2925u,351u,27u,1u,}}, // i = 6
  {{657800u,230230u,65780u,14950u,2600u,325u,26u,1u,}}, // i = 7
  {{480700u,177100u,53130u,12650u,2300u,300u,25u,1u,}}, // i = 8
  {{346104u,134596u,42504u,10626u,2024u,276u,24u,1u,}}, // i = 9
  {{245157u,100947u,33649u,8855u,1771u,253u,23u,1u,}}, // i = 10
  {{170544u,74613u,26334u,7315u,1540u,231u,22u,1u,}}, // i = 11
  {{116280u,54264u,20349u,5985u,1330u,210u,21u,1u,}}, // i = 12
  {{77520u,38760u,15504u,4845u,1140u,190u,20u,1u,}}, // i = 13
  {{50388u,27132u,11628u,3876u,969u,171u,19u,1u,}}, // i = 14
  {{31824u,18564u,8568u,3060u,816u,153u,18u,1u,}}, // i = 15
  {{19448u,12376u,6188u,2380u,680u,136u,17u,1u,}}, // i = 16
  {{11440u,8008u,4368u,1820u,560u,120u,16u,1u,}}, // i = 17
  {{6435u,5005u,3003u,1365u,455u,105u,15u,1u,}}, // i = 18
  {{3432u,3003u,2002u,1001u,364u,91u,14u,1u,}}, // i = 19
  {{1716u,1716u,1287u,715u,286u,78u,13u,1u,}}, // i = 20
  {{792u,924u,792u,495u,220u,66u,12u,1u,}}, // i = 21
  {{330u,462u,462u,330u,165u,55u,11u,1u,}}, // i = 22
  {{120u,210u,252u,210u,120u,45u,10u,1u,}}, // i = 23
  {{36u,84u,126u,126u,84u,36u,9u,1u,}}, // i = 24
  {{8u,28u,56u,70u,56u,28u,8u,1u,}}, // i = 25
  {{1u,7u,21u,35u,35u,21u,7u,1u,}}, // i = 26
  {{0u,1u,6u,15u,20u,15u,6u,1u,}}, // i = 27
  {{0u,0u,1u,5u,10u,10u,5u,1u,}}, // i = 28
  {{0u,0u,0u,1u,4u,6u,4u,1u,}}, // i = 29
  {{0u,0u,0u,0u,1u,3u,3u,1u,}}, // i = 30
  {{0u,0u,0u,0u,0u,1u,2u,1u,}}, // i = 31
  {{0u,0u,0u,0u,0u,0u,1u,1u,}}, // i = 32
  {{0u,0u,0u,0u,0u,0u,0u,1u,}}, // i = 33
}};

}

#endif // !defined(TSUMONYA_SEVEN_PAIRS_TABLE_HPP_INCLUDE_GUARD)
