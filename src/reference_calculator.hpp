#if !defined(REFERENCE_CALCULATOR_HPP_INCLUDE_GUARD)
#define REFERENCE_CALCULATOR_HPP_INCLUDE_GUARD

#include <array>
#include <utility>
#include <memory>
#include <cstdint>


namespace Tsumonya::Detail_{

class ReferenceCalculator
{
private:
  class Impl_;

public:
  ReferenceCalculator();

  ReferenceCalculator(ReferenceCalculator const &other) noexcept;

  ReferenceCalculator(ReferenceCalculator &&other) noexcept;

  ReferenceCalculator &operator=(ReferenceCalculator const &other) noexcept;

  ReferenceCalculator &operator=(ReferenceCalculator &&other) noexcept;

  std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
    std::uint_fast8_t round_wind,
    std::uint_fast8_t player_wind,
    std::array<std::uint_fast8_t, 34u> const &pure_hand,
    std::array<std::uint_fast8_t, 21u> const &chi_list,
    std::array<std::uint_fast8_t, 34u> const &peng_list,
    std::array<std::uint_fast8_t, 34u> const &angang_list,
    std::array<std::uint_fast8_t, 34u> const &minggang_list,
    std::uint_fast8_t winning_tile,
    bool rong) const;

private:
  std::shared_ptr<Impl_ const> p_impl_;
}; // class ReferenceCalculator

} // namespace Tsumonya::Detail_

#endif // !defined(REFERENCE_CALCULATOR_HPP_INCLUDE_GUARD)
