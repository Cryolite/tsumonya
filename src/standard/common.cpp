#include "common.hpp"

#include <tsumonya/standard/core.hpp>
#include <cassert>


namespace Tsumonya::Standard_{

using Tsumonya::Standard_::stable;
using Tsumonya::Standard_::mtable;
using Tsumonya::Standard_::ntable;
using Tsumonya::Standard_::xytable;
using Tsumonya::Standard_::ptable;

std::uint_fast64_t packState(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const w,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  std::uint_fast8_t const a,
  std::uint_fast8_t const b)
{
  assert((i <= 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  assert((w <= 2u));
  assert((x <= 4u));
  assert((y <= 4u));
  assert((a <= 1u));
  assert((b <= 1u));

  std::uint_fast64_t state = 0u;
  state |= (static_cast<std::uint_fast64_t>(i) << (8u * 7u));
  state |= (static_cast<std::uint_fast64_t>(m) << (8u * 6u));
  state |= (static_cast<std::uint_fast64_t>(h) << (8u * 5u));
  state |= (static_cast<std::uint_fast64_t>(w) << (8u * 4u));
  state |= (static_cast<std::uint_fast64_t>(x) << (8u * 3u));
  state |= (static_cast<std::uint_fast64_t>(y) << (8u * 2u));
  state |= (static_cast<std::uint_fast64_t>(a) << (8u * 1u));
  state |= (static_cast<std::uint_fast64_t>(b) << (8u * 0u));
  return state;
}

bool isValidTransition(
  bool const shunzi_prohibited,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const w,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  std::uint_fast8_t const a,
  std::uint_fast8_t const s)
{
  if (m + mtable[s] > 4u) {
    // The number of members (menzi, 面子) must not exceed 4.
    return false;
  }
  if (h + stable[s][3u] > 1u) {
    // The number of heads (雀頭) must not exceed 1.
    return false;
  }
  if (w >= 1u && stable[s][2u] >= 1u) {
    // The number of winning tiles must not exceed 1.
    return false;
  }
  if (stable[s][2u] >= 1u && a == 0u && ptable[s] == 0u) {
    // If the winning tile is `i`, then tile `i` must exist in the hand.
    return false;
  }
  if (shunzi_prohibited && xytable[s] > 0u) {
    // Three-in-a-row (shunzi, 順子) starting with 8 or 9 must not exist. The same applies to one
    // of honors (zipai, 字牌).
    return false;
  }
  if (x + ntable[s] > 4u) {
    // The number of tile `i` must not exceed 4.
    return false;
  }
  if (y > x) {
    return false;
  }
  assert((y + xytable[s] <= 4u));

  return true;
}

} // namespace Tsumonya::Standard_
