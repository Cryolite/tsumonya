// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#if !defined(STANDARD_WINNING_HAND_CALLBACK_HPP_INCLUDE_GUARD)
#define STANDARD_WINNING_HAND_CALLBACK_HPP_INCLUDE_GUARD

#include <functional>
#include <array>
#include <cstdint>
#include <cstddef>


namespace Tsumonya::Standard_{

using WinningHandCallback = std::function<
  void(
    std::array<std::uint_fast8_t, 34u> const &,
    std::array<std::uint_fast8_t, 21u> const &,
    std::array<std::uint_fast8_t, 34u> const &,
    std::array<std::uint_fast8_t, 34u> const &,
    std::array<std::uint_fast8_t, 34u> const &,
    std::uint_fast8_t,
    bool)>;

} // namespace Tsumonya::Standard_

#endif // !defined(STANDARD_WINNING_HAND_CALLBACK_HPP_INCLUDE_GUARD)
