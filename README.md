# Tsumonya - ultra-high throughput mahjong hand calculator

*Tsumonya* is a mahjong-related C++ library that calculates the score for a given winning hand (和了形). The main feature of this library is its ultra-high throughput, i.e., it focuses on dramatically reducing the average computation time for a single score calculation when calling many consecutive score calculation routines. On the other hand, the library sacrifices a significant amount of reaction time, or latency, for a single computation routine call. In addition, this library consumes an ridicuously large amount of storage (~26GB). Therefore, users are requested to carefully determine whether this library is suitable to their individual use cases.

Ultra-high throughput is not likely to be required for ordinary mahjong score calculations. However, ultra-high throughput may be required when running large-scale simulations for the development, training, and evaluation of mahjong AIs. In these cases, the score calculation is called not only once at the end of each round in simulations, but also many times to determine whether a zimo (自摸) or rong (ロン) is possible when one of the players has a ready hand and every time they draws a tile or another player discards a tile. Therefore, an ultra-high throughput implementation in calling a large number of scoring routines would contribute significantly to simulation speed-up.

The implementation is based on an idea of computing mahjong scores only by simple array indexing. In other words, this library pre-computes the fu (符) and fan (飜) for **ALL POSSIBLE MAHJONG WINNING HANDS** and stores them in a huge array. In this way, it completely omits the computationally expensive processes normally required for mahjong score calculation, such as member (面子, menzi) decomposition, calculation of fu, determination of hand types (Yaku, 役), calculation of fan, logic to resolve ambiguities in member decomposition for an winning hand based on the highest point principle (高点法), and others.

# How to use

First, set up [git large file storage](https://git-lfs.com/).

```sh
$ git lfs install
```

Next, clone this repository.

```sh
$ git clone https://github.com/Cryolite/tsumonya
$ cd tsumonya
```

Then, decompress `map.bin.gz`.

```sh
tsumonya$ gunzip -k map.bin.gz
```

Finally, add the path to the top directory of this repository to the set of the include paths, and include the `<tsumonya/calculator.hpp>` header file.

# Reference

```c++
namespace Tsumonya{

using Hand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengList = std::array<std::uint_fast8_t, 34u>;
using GangList = std::array<std::uint_fast8_t, 34u>;

enum Situation
  : std::uint_fast16_t;

inline constexpr Situation zimo;
inline constexpr Situation rong;
inline constexpr Situation liqi;
inline constexpr Situation qianggang;
inline constexpr Situation lingshang_kaihua;
inline constexpr Situation haidi_moyue;
inline constexpr Situation haidi_laoyue;
inline constexpr Situation double_liqi;
inline constexpr Situation yifa;
inline constexpr Situation tianhu;
inline constexpr Situation dihu;

class Calculator
{
public:
  explicit Calculator(char const *map_path);
  explicit Calculator(std::string const &map_path);
  explicit Calculator(std::filesystem::path const &map_path);
  std::pair<std::uint_fast8_t, std::uint_fast8_t> operator()(
    Hand const &hand, ChiList const &chi_list, PengList const &peng_list,
    GangList const &angang_list, GangList const &minggang_list, std::uint_fast8_t hupai,
    std::uint_fast8_t num_red_tiles, Situation situation) const;
};

}
```

### `Hand`

The `Hand` type represents a set of tiles in a hand. It MUST NOT contain melded tiles (副露牌) nor the winning tile (和牌). This is an array type with 34 elements. The 0-th element MUST indicate the number of the 1 Character tile (一萬, 1m), the 1st element the number of the 2 Character tile (二萬, 2m), and so on.

### `ChiList`

The `ChiList` type represents a set of chews (chi, 吃, チー). This is an array type with 21 elements. The 0-th element MUST indicate the number of the chew (1m, 2m, 3m), the 1st element the number of the chews (2m, 3m, 4m), and so on.

### `PengList`

The `PengList` type represents a set of pengs (pon, 碰, ポン). This is an array type with 34 elements. The 0-th element MUST indicate the number of the peng (1m, 1m, 1m), the 1st element the number of the peng (2m, 2m, 2m), and so on.

### `GangList`

The `GangList` type represents a set of kongs (gang, 槓, カン). This is an array type with 34 elements. The 0-th element MUST indicate the number of the kong (1m, 1m, 1m, 1m), the 1st element the number of the kong (2m, 2m, 2m, 2m), and so on.

### `Situation`

The `Situation` enum type is used to specifiy situation, especially hand types (yaku, 役), that cannot be determined from just the hand, melded tiles, and the winning tile. Multiple objects of the type `Situation` can be combined with `operator|`.

#### `zimo`

`zimo` represents a self-drawn win (自摸和).

#### `rong`

`rong` represents a dealt-in win (栄和).

#### `liqi`

`liqi` represents a riichi (立直) declaration.

#### `qianggang`

`qianggang` represents a robbing of a kong (槍槓).

#### `lingshang_kaihua`

`lingshang_kaihua` represents an after-kong win (嶺上開花).

#### `haidi_moyue`

`haidi_moyue` represents a last-tile-draw win (海底撈月).

#### `haidi_laoyue`

`haidi_moyue` represents a last-tile-discard win (河底撈魚).

#### `double_liqi`

`liqi` represents a double riichi (ダブル立直) declaration.

#### `yifa`

`yifa` represents Ippatsu (一発).

#### `tianhu`

`tianhu` represents a heavenly hand (天和).

#### `dihu`

`dihu` represents an earthly hand (地和).

### `Calculator::Calculator(char const *map_path)`

### `Calculator::Calculator(std::string const &map_path)`

### `Calculator::Calculator(std::filesystem::path const &map_path)`

These are the constructors of `Calculator`. `map_path` MUST specify the path to the `map.bin` file.

### `std::pair<std::uint_fast8_t, std::uint_fast8_t> Calculator::operator()(Hand const &hand, ChiList const &chi_list, PengList const &peng_list, GangList const &angang_list, GangList const &minggang_list, std::uint_fast8_t hupai, std::uint_fast8_t num_red_tiles, Situation situation)`

This function calculates fu (符) and fan (飜) for the winning situation represented by the arguments. `hand` MUST specify the hand, `chi_list` the set of chews, `peng_list` the set of pengs, `angang_list` the set of closed kongs, `minggang_list` the set of open kongs, `hupai` the winning tile, `num_red_tiles` the number of red tiles, and `situation` the situation of the win.

The return value is a pair of fu and fan. If the situation represented by the arguments is not a win, a pair of 0s is returned.

# Performance comparison

It is approximately 100 times faster on average than the [mahjong Python package \(version 1.1.10\)](https://pypi.org/project/mahjong/1.1.10/).

# Background theory

The most important core of the theory behind this library is [the implementation of a near-minimal perfect (collisionless) hash function for the set of all possible mahjong winning hands](https://github.com/Cryolite/tsumonya/blob/master/tsumonya/hule_indexer.hpp). The current implementation maps an winning hand to a natural number in the range `[0, 1062518406)`. Therefore, by preparing an array with `13 * 1062518406` elements, where the number `13` means that the library has additional information to identify which tile of each winning hand is the winning tile, and by calling some score calculation routine for each winning hand and recording the result of the routine in the \[*i* * 13, *i* * 13 + 13\)-th elements of the array (where *i* is the hash of the winning hand), when calculating scores, it is only necessary to calculate the hash of the given winning hand and access the array element.

# Build from scratch (For developers only)

Execute the following commands with the top directory of the working tree of this repository as the current directory. Note that the build process can take a verrrrrrrrrry long time (~20 days).

```
tsumonya$ docker build --pull -t cryolite/tsumonya .
tsumonya$ docker run -v /path/to/working-tree:/workspace/tsumonya -it --rm cryolite/tsumonya
```
