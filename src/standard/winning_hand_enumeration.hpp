// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(STANDARD_WINNING_HAND_ENUMERATION_HPP_INCLUDE_GUARD)
#define STANDARD_WINNING_HAND_ENUMERATION_HPP_INCLUDE_GUARD

#include "winning_hand_callback.hpp"
#include <functional>
#include <array>
#include <cstdint>
#include <cstddef>


namespace Tsumonya::Standard_{

void enumerateWinningHands(WinningHandCallback callback, std::size_t concurrency = 1u);

} // namespace Tsumonya::Standard_

#endif // !defined(STANDARD_WINNING_HAND_ENUMERATION_HPP_INCLUDE_GUARD)
