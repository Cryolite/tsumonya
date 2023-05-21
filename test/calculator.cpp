#include <tsumonya/calculator.hpp>
#include <cstdint>


#include <iostream>

int main()
{
    Tsumonya::Calculator calculator("map.bin");
    Tsumonya::Hand hand{
        3u, 4u, 4u, 2u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::ChiList chi_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::PengList peng_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::GangList angang_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    Tsumonya::GangList minggang_list{
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    std::uint8_t const hupai = 0u;
    auto const [hu, fan] = calculator(
        hand, chi_list, peng_list, angang_list, minggang_list, hupai, 0u, Tsumonya::rong);
    std::cout << static_cast<unsigned>(hu) << ", " << static_cast<unsigned>(fan) << std::endl;
}
