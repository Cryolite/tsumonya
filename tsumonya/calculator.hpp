#if !defined(TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD)
#define TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD

#include <tsumonya/normal/hule_indexer.hpp>
#include <tsumonya/normal/wind_head/table.hpp>
#include <tsumonya/normal/base/table.hpp>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ios>
#include <iterator>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include <cstddef>


namespace Tsumonya{

enum Wind
    : std::uint_fast8_t
{
    east_  = 0u,
    south_ = 1u,
    west_  = 2u,
    north_ = 3u,
};

inline constexpr Wind east(east_);
inline constexpr Wind south(south_);
inline constexpr Wind west(west_);
inline constexpr Wind north(north_);

enum Situation
    : std::uint_fast16_t
{
    zimo_             = 1u <<  0u,
    rong_             = 1u <<  1u,
    liqi_             = 1u <<  2u,
    qianggang_        = 1u <<  3u,
    lingshang_kaihua_ = 1u <<  4u,
    haidi_moyue_      = 1u <<  5u,
    haidi_laoyue_     = 1u <<  6u,
    double_liqi_      = 1u <<  7u,
    yifa_             = 1u <<  8u,
    tianhu_           = 1u <<  9u,
    dihu_             = 1u << 10u
};

inline constexpr Situation zimo(zimo_);
inline constexpr Situation rong(rong_);
inline constexpr Situation liqi(liqi_);
inline constexpr Situation qianggang(qianggang_);
inline constexpr Situation lingshang_kaihua(lingshang_kaihua_);
inline constexpr Situation haidi_moyue(haidi_moyue_);
inline constexpr Situation haidi_laoyue(haidi_laoyue_);
inline constexpr Situation double_liqi(double_liqi_);
inline constexpr Situation yifa(yifa_);
inline constexpr Situation tianhu(tianhu_);
inline constexpr Situation dihu(dihu_);

constexpr Situation operator&(Situation lhs, Situation rhs)
{
    return static_cast<Situation>(
        static_cast<std::uint_fast16_t>(lhs) & static_cast<std::uint_fast16_t>(rhs));
}

constexpr Situation operator|(Situation lhs, Situation rhs)
{
    return static_cast<Situation>(
        static_cast<std::uint_fast16_t>(lhs) | static_cast<std::uint_fast16_t>(rhs));
}

constexpr Situation &operator&=(Situation &lhs, Situation rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

constexpr Situation &operator|=(Situation &lhs, Situation rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

class Calculator
{
private:
    static constexpr std::uint_fast64_t map_size_
        = static_cast<std::uint_fast64_t>(Tsumonya::Normal::Base::e) * 13u * 2u
        + static_cast<std::uint_fast64_t>(Tsumonya::Normal::WindHead::e) * 3u * 13u * 2u;

    class Impl_
    {
    public:
        explicit Impl_(std::filesystem::path const &map_path)
            : p_(nullptr)
            , array_()
            , hule_indexer_()
        {
            if (!std::filesystem::exists(map_path)) {
                std::ostringstream oss;
                oss << map_path << ": Does not exist.";
                throw std::runtime_error(oss.str());
            }
            if (!std::filesystem::is_regular_file(map_path) && !std::filesystem::is_symlink(map_path)) {
                std::ostringstream oss;
                oss << map_path << ": Not a file.";
                throw std::runtime_error(oss.str());
            }
            if (std::filesystem::file_size(map_path) != map_size_) {
                std::ostringstream oss;
                oss << map_path << ": Not a map file.";
                throw std::runtime_error(oss.str());
            }

            std::ifstream ifs(map_path, std::ios_base::in | std::ios_base::binary);
            std::istreambuf_iterator<char> iter(ifs);
            std::istreambuf_iterator<char> jter;
            array_.reserve(map_size_);
            std::copy(iter, jter, std::back_inserter(array_));
            array_.shrink_to_fit();
            p_ = &array_[0u];
        }

        Impl_(Impl_ const &) = delete;

        Impl_ &operator=(Impl_ const &) = delete;

        std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
            Wind const round_wind, Wind const player_wind,
            Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
            GangList const &angang_list, GangList const &minggang_list,
            std::uint_fast8_t hupai_index, bool rong) const
        {
            std::uint_fast32_t const index = hule_indexer_(
                hand, chi_list, peng_list, angang_list, minggang_list);
            if (index == UINT_FAST32_MAX) {
                return std::pair<std::uint_fast8_t, std::uint_fast8_t>(0u, 0u);
            }

            std::uint_fast8_t const wind_head_type = [&]() -> std::uint_fast8_t{
                std::uint_fast8_t const wind_head = [&]() -> std::uint_fast8_t{
                    for (std::uint_fast8_t w = 0u; w < 4u; ++w) {
                        if (hand[27u + w] == 2u) {
                            return w;
                        }
                    }
                    return UINT_FAST8_MAX;
                }();
                if (wind_head == UINT_FAST8_MAX) {
                    return UINT_FAST8_MAX;
                }
                if (wind_head != round_wind && wind_head != player_wind) {
                    return 0u;
                }
                if (wind_head == round_wind && wind_head != player_wind || wind_head != round_wind && wind_head == player_wind) {
                    return 1u;
                }
                assert((wind_head == round_wind));
                assert((wind_head == player_wind));
                return 2u;
            }();

            std::uint_fast64_t const map_index = [&]() -> std::uint_fast64_t{
                if (wind_head_type == UINT_FAST8_MAX) {
                    return (index * 13u + hupai_index) * 2u + (rong ? 1u : 0u);
                }
                assert((index >= Tsumonya::Normal::Base::e));
                assert((index < Tsumonya::Normal::Base::e + Tsumonya::Normal::WindHead::e));
                return Tsumonya::Normal::Base::e * 13u * 2u
                    + (((index - Tsumonya::Normal::Base::e) * 3u + wind_head_type) * 13u + hupai_index) * 2u + (rong ? 1u : 0u);
            }();
            std::uint8_t const encode = p_[map_index];

            return fu_fan_table[encode];
        }

    private:
        std::uint_fast8_t const *p_;
        std::vector<std::uint8_t> array_;
        Tsumonya::Normal::HuleIndexer hule_indexer_;
    };

public:
    explicit Calculator(char const *map_path)
        : Calculator(std::filesystem::path(map_path))
    {}

    explicit Calculator(std::string const &map_path)
        : Calculator(std::filesystem::path(map_path))
    {}

    explicit Calculator(std::filesystem::path const &map_path)
        : p_impl_(new Impl_(map_path))
    {}

    std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
        Wind const round_wind, Wind const player_wind,
        Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
        GangList const &angang_list, GangList const &minggang_list, std::uint_fast8_t const hupai,
        std::uint_fast8_t const num_red_tiles, Situation const situation) const
    {
        {
            std::uint_fast16_t total = 0u;

            for (std::uint_fast8_t i = 0u; i < 27u; ++i) {
                std::uint_fast8_t const color = i / 9u;
                std::uint_fast8_t const number = i % 9u;
                std::uint_fast8_t const cindex = color * 7u + number;
                std::uint_fast8_t const c = [&]() -> std::uint_fast8_t {
                    if (number == 0u) {
                        return chi_list[cindex];
                    }
                    assert((cindex >= 1u));
                    if (number == 1u) {
                        return chi_list[cindex - 1u] + chi_list[cindex];
                    }
                    assert((cindex >= 2u));
                    if (2u <= number && number <= 6u) {
                        return chi_list[cindex - 2u] + chi_list[cindex - 1u] + chi_list[cindex];
                    }
                    if (number == 7u) {
                        return chi_list[cindex - 2u] + chi_list[cindex - 1u];
                    }
                    return chi_list[cindex - 2u];
                }();

                if (hand[i] + c + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i] > 4u) {
                    throw std::invalid_argument("Invalid arguments.");
                }

                total += hand[i] + c + 3u * (peng_list[i] + angang_list[i] + minggang_list[i]);
            }
            for (std::uint_fast8_t i = 27u; i < 34u; ++i) {
                if (hand[i] + 3u * peng_list[i] + 4u * angang_list[i] + 4u * minggang_list[i] > 4u) {
                    throw std::invalid_argument("Invalid arguments.");
                }

                total += hand[i] + 3u * (peng_list[i] + angang_list[i] + minggang_list[i]);
            }

            if (total != 13u) {
                throw std::invalid_argument("Invalid arguments.");
            }
        }

        if (hupai >= hand.size()) {
            std::ostringstream oss;
            oss << static_cast<unsigned>(hupai) << ": An invalid hupai.";
            throw std::invalid_argument(oss.str());
        }
        if ((situation & zimo) == 0u && (situation & rong) == 0u) {
            throw std::invalid_argument("`Tsumonya::zimo` or `Tsumonya::rong` MUST be specified.");
        }
        if (((situation & zimo) != 0u) && ((situation & rong) != 0u)) {
            throw std::invalid_argument("`Tsumonya::zimo` and `Tsumonya::rong` conflict.");
        }
        if (((situation & zimo) != 0u) && ((situation & qianggang) != 0u)) {
            throw std::invalid_argument("`Tsumonya::zimo` and `Tsumonya::qianggang` conflict.");
        }
        if (((situation & rong) != 0u) && ((situation & lingshang_kaihua) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::rong` and `Tsumonya::lingshang_kaihua` conflict.");
        }
        if (((situation & qianggang) != 0u) && ((situation & lingshang_kaihua) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::qianggang` and `Tsumonya::lingshang_kaihua` conflict.");
        }
        if (((situation & rong) != 0u) && ((situation & haidi_moyue) != 0u)) {
            throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::haidi_moyue` conflict.");
        }
        if (((situation & qianggang) != 0u) && ((situation & haidi_moyue) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::qianggang` and `Tsumonya::haidi_moyue` conflict.");
        }
        if (((situation & lingshang_kaihua) != 0u) && ((situation & haidi_moyue) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::lingshang_kaihua` and `Tsumonya::haidi_moyue` conflict.");
        }
        if (((situation & zimo) != 0u) && ((situation & haidi_laoyue) != 0u)) {
            throw std::invalid_argument("`Tsumonya::zimo` and `Tsumonya::haidi_laoyue` conflict.");
        }
        if (((situation & qianggang) != 0u) && ((situation & haidi_laoyue) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::qianggang` and `Tsumonya::haidi_laoyue` conflict.");
        }
        if (((situation & lingshang_kaihua) != 0u) && ((situation & haidi_laoyue) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::lingshang_kaihua` and `Tsumonya::haidi_laoyue` conflict.");
        }
        if (((situation & liqi) != 0u) && ((situation & double_liqi) != 0u)) {
            throw std::invalid_argument("`Tsumonya::liqi` and `Tsumonya::double_liqi` conflict.");
        }
        if (((situation & qianggang) != 0u) && ((situation & yifa) != 0u)) {
            throw std::invalid_argument("`Tsumonya::qianggang` and `Tsumonya::yifa` conflict.");
        }
        if (((situation & lingshang_kaihua) != 0u) && ((situation & yifa) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::lingshang_kaihua` and `Tsumonya::yifa` conflict.");
        }
        if (((situation & rong) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & liqi) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::liqi` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & qianggang) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::qianggang` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & lingshang_kaihua) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::lingshang_kaihua` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & haidi_moyue) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::haidi_moyue` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & haidi_laoyue) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::haidi_laoyue` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & double_liqi) != 0u) && ((situation & tianhu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::double_liqi` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & yifa) != 0u) && ((situation & tianhu) != 0)) {
            throw std::invalid_argument("`Tsumonya::yifa` and `Tsumonya::tianhu` conflict.");
        }
        if (((situation & rong) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::rong` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & liqi) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::liqi` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & qianggang) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::qianggang` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & lingshang_kaihua) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument(
                "`Tsumonya::lingshang_kaihua` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & haidi_moyue) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::haidi_moyue` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & haidi_laoyue) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::haidi_laoyue` and `Tsumonya::dihu` conflict");
        }
        if (((situation & double_liqi) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::double_liqi` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & yifa) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::yifa` and `Tsumonya::dihu` conflict.");
        }
        if (((situation & tianhu) != 0u) && ((situation & dihu) != 0u)) {
            throw std::invalid_argument("`Tsumonya::tianhu` and `Tsumonya::dihu` conflict.");
        }

        Hand new_hand(hand);
        ++new_hand[hupai];

        std::uint_fast8_t hupai_index = 0u;
        for (std::uint_fast8_t i = 0u;; ++i, ++hupai_index) {
            while (i < new_hand.size() && new_hand[i] == 0u) {
                ++i;
            }
            if (i == new_hand.size()) {
                throw std::logic_error("A logic error.");
            }

            if (hupai == i) {
                break;
            }
        }
        if (hupai_index >= 13u) {
            throw std::logic_error("A logic error.");
        }

        bool const is_rong = ((situation & rong) != 0u);
        auto [hu, fan] = (*p_impl_)(
            round_wind, player_wind, hand, chi_list, peng_list, angang_list, minggang_list,
            hupai_index, is_rong);

        if (fan >= 13u) {
            if ((situation & tianhu) != 0u) {
                fan += 13u;
            }
            if ((situation & dihu) != 0u) {
                fan += 13u;
            }
            return std::make_pair(hu, fan);
        }

        for (std::uint_fast8_t w = 0u; w < 4u; ++w) {
            if (hand[27u + w] != 3u && minggang_list[27u + w] != 1u && angang_list[27u + w] != 1u && minggang_list[27u + w] != 1u) {
                continue;
            }
            if (round_wind == w) {
                ++fan;
            }
            if (player_wind == w) {
                ++fan;
            }
        }
        fan += num_red_tiles;
        if ((situation & liqi) != 0u) {
            fan += 1u;
        }
        if ((situation & qianggang) != 0u) {
            fan += 1u;
        }
        if ((situation & lingshang_kaihua) != 0u) {
            fan += 1u;
        }
        if ((situation & haidi_moyue) != 0u) {
            fan += 1u;
        }
        if ((situation & haidi_laoyue) != 0u) {
            fan += 1u;
        }
        if ((situation & double_liqi) != 0u) {
            fan += 2u;
        }

        if (fan > 13u) {
            fan = 13u;
        }

        return std::make_pair(hu, fan);
    }

private:
    std::shared_ptr<Impl_ const> p_impl_;
};

}


#endif
