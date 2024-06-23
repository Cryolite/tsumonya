#if !defined(STANDARD_HULE_ENUMERATION_HPP_INCLUDE_GUARD)
#define STANDARD_HULE_ENUMERATION_HPP_INCLUDE_GUARD

#include <functional>
#include <array>
#include <cstdint>


namespace Tsumonya::Standard_{

using HuleCallback = std::function<
  void(
    std::array<std::uint_fast8_t, 34u> const &,
    std::array<std::uint_fast8_t, 21u> const &,
    std::array<std::uint_fast8_t, 34u> const &,
    std::array<std::uint_fast8_t, 34u> const &,
    std::array<std::uint_fast8_t, 34u> const &,
    std::uint_fast8_t,
    bool)>;

void enumerateHules(HuleCallback callback, bool const multithreading);

} // namespace Tsumonya::Standard_

#endif // STANDARD_HULE_ENUMERATION_HPP_INCLUDE_GUARD
