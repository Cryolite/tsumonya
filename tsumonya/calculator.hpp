#if !defined(TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD)
#define TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD

#include <tsumonya/hule_indexer.hpp>
#include <tsumonya/table.hpp>
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
}; // enum Situation

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
    static constexpr std::uint_fast64_t map_size_ = 28u * Tsumonya::e;

    class Impl_
    {
    public:
        explicit Impl_(std::filesystem::path const &map_path, bool full_fetch)
            : fd_(-1)
            , p_(nullptr)
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

            if (full_fetch) {
                std::ifstream ifs(map_path, std::ios_base::in | std::ios_base::binary);
                std::istreambuf_iterator<char> iter(ifs);
                std::istreambuf_iterator<char> jter;
                array_.reserve(map_size_);
                std::copy(iter, jter, std::back_inserter(array_));
                array_.shrink_to_fit();
                p_ = &array_[0u];
            }
            else {
                fd_ = open(map_path.c_str(), O_RDONLY);
                if (fd_ == -1) {
                    std::ostringstream oss;
                    oss << map_path << ": Failed to open.";
                    throw std::runtime_error(oss.str());
                }

                p_ = mmap(nullptr, map_size_, PROT_READ, MAP_PRIVATE, fd_, 0);
                if (p_ == reinterpret_cast<void *>(-1)) {
                    close(fd_);
                    std::ostringstream oss;
                    oss << map_path << ": Failed to mmap.";
                    throw std::runtime_error(oss.str());
                }
            }
        }

        Impl_(Impl_ const &) = delete;

        ~Impl_()
        {
            if (fd_ != -1) {
                munmap(p_, map_size_);
                close(fd_);
            }
        }

        Impl_ &operator=(Impl_ const &) = delete;

        std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
            Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
            GangList const &angang_list, GangList const &minggang_list,
            std::uint_fast8_t hupai_index, bool rong) const
        {
            std::uint_fast32_t const index = hule_indexer_(
                hand, chi_list, peng_list, angang_list, minggang_list);
            if (index == UINT_FAST32_MAX) {
                return std::pair<std::uint_fast8_t, std::uint_fast8_t>(0u, 0u);
            }
            std::uint8_t const *p = static_cast<std::uint8_t const *>(p_);
            std::uint_fast64_t const map_index
                = (hupai_index * static_cast<std::uint_fast64_t>(Tsumonya::e) + index) * 2u
                + (rong ? 1u : 0u);
            std::uint8_t const encode = p[map_index];

            std::uint_fast8_t const fu = [](std::uint8_t encode) -> std::uint_fast8_t {
                std::uint_fast8_t const fu_encode = encode / 19u;
                if (fu_encode == 0u) {
                    return 0u;
                }
                else if (fu_encode == 1u) {
                    return 20u;
                }
                else if (fu_encode == 2u) {
                    return 25u;
                }

                return fu_encode * 10u;
            }(encode);

            std::uint_fast8_t const fan = [](std::uint8_t encode) -> std::uint_fast8_t {
                std::uint_fast8_t const fan_encode = encode % 19u;
                if (fan_encode <= 12u) {
                    return fan_encode;
                }
                return (fan_encode - 12u) * 13u;
            }(encode);

            return std::make_pair(fu, fan);
        }

    private:
        int fd_;
        void *p_;
        std::vector<std::uint8_t> array_;
        Tsumonya::HuleIndexer hule_indexer_;
    }; // class Impl_

public:
    explicit Calculator(char const *map_path, bool full_fetch = false)
        : Calculator(std::filesystem::path(map_path), full_fetch)
    {}

    explicit Calculator(std::string const &map_path, bool full_fetch = false)
        : Calculator(std::filesystem::path(map_path), full_fetch)
    {}

    explicit Calculator(std::filesystem::path const &map_path, bool full_fetch = false)
        : p_impl_(new Impl_(map_path, full_fetch))
    {}

    std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
        Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
        GangList const &angang_list, GangList const &minggang_list, std::uint_fast8_t hupai,
        std::uint_fast8_t num_red_tiles, Situation situation) const
    {
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
            new_hand, chi_list, peng_list, angang_list, minggang_list, hupai_index, is_rong);

        if (fan >= 13u) {
            if ((situation & tianhu) != 0u) {
                fan += 13u;
            }
            if ((situation & dihu) != 0u) {
                fan += 13u;
            }
            return std::make_pair(hu, fan);
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
}; // class Calculator

} // namespace Tsumonya


#endif // !defined(TSUMONYA_CALCULATOR_HPP_INCLUDE_GUARD)
