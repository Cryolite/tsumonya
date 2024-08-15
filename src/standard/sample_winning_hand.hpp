// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(STANDARD_SAMPLE_WINNING_HAND_HPP_INCLUDE_GUARD)
#define STANDARD_SAMPLE_WINNING_HAND_HPP_INCLUDE_GUARD

#include "winning_hand_callback.hpp"
#include <random>


namespace Tsumonya::Standard_{

void sampleWinningHand(
  Tsumonya::Standard_::WinningHandCallback callback, std::mt19937 &random_number_engine);

} // namespace Tsumonya::Standard_

#endif // !defined(STANDARD_SAMPLE_WINNING_HAND_HPP_INCLUDE_GUARD)
