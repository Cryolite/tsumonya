#if !defined(STANDARD_COMMON_HPP_INCLUDE_GUARD)
#define STANDARD_COMMON_HPP_INCLUDE_GUARD

#include <cstdint>


namespace Tsumonya::Standard_{

std::uint_fast64_t packState(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const w,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  std::uint_fast8_t const a,
  std::uint_fast8_t const b);

bool isValidTransition(
  bool const shunzi_prohibited,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const w,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  std::uint_fast8_t const a,
  std::uint_fast8_t const s);

} // namespace Tsumonya::Standard_

#endif // STANDARD_COMMON_HPP_INCLUDE_GUARD
