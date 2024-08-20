// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(TSUMONYA_SEVEN_PAIRS_CORE_HPP_INCLUDE_GUARD)
#define TSUMONYA_SEVEN_PAIRS_CORE_HPP_INCLUDE_GUARD

#include <vector>
#include <array>
#include <utility>
#include <cstdint>


namespace Tsumonya::SevenPairs_{

using Table = std::array<std::array<std::uint_fast32_t, 8u>, 34u>;
using Map = std::vector<std::pair<std::uint8_t, std::uint8_t>>;

} // namespace Tsumonya::SevenPairs_

#endif
