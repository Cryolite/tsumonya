**THIS LIBRARY IS UNDER ACTIVE DEVELOPMENT.**

# Tsumonya - ultra-high throughput mahjong hand calculator

*Tsumonya* is a mahjong-related C++ library that calculates the score for a given winning hand (和了形). The main feature of this library is its ultra-high throughput, i.e., it focuses on dramatically reducing the average computation time for a single score calculation when calling many consecutive score calculation routines. On the other hand, the library sacrifices a significant amount of reaction time, or latency, for a single computation routine call.

Ultra-high throughput is not likely to be required for ordinary mahjong score calculations. However, ultra-high throughput may be required when running large-scale simulations for the development, training, and evaluation of mahjong AIs. In these cases, the score calculation is called not only once at the end of each round in simulations, but also many times to determine whether a zimo (自摸) or rong (ロン) is possible when one of the players has a ready hand, and every time they draws a tile or another player discards a tile. Therefore, an ultra-high throughput implementation in calling a large number of scoring routines would contribute significantly to simulation speed-up.

This library pre-computes the fu (符) and fan (飜) for **ALL POSSIBLE MAHJONG WINNING HANDS** and stores them in a huge array. In this way, it completely omits the computationally expensive processes normally required for mahjong score calculation, such as menzi (面子) decomposition, calculation of fu, determination of roles (役, yaku), calculation of fan, and logic to resolve ambiguities in menzi decomposition for an winning hand based on the highest point principle, and others. The implementation is based on such an idea of computing mahjong scores only by simple array indexing.

# Background theory

The most important core of the theory behind this library is [the implementation of a near-minimal perfect (collisionless) hash function](https://github.com/Cryolite/tsumonya/blob/master/tsumonya/hule_indexer.hpp) for the set of all possible mahjong winning hands. The current implementation maps an winning hand to a natural number in the range `[0, 1062518406)`. Therefore, by preparing an array with 1062518406 elements, and by calling some score calculation routine for each winning hand and recording the result of the routine in the *i*-th element of the array (where *i* is the hash of the winning hand), when calculating scores, it is only necessary to calculate the hash of the given winning hand and access the array element.

# Build from scratch (For developers only)

(This process can take a verrrrrrrrrry long time. ~100 days)

```
tsumonya$ docker build --pull -t cryolite/tsumonya .
tsumonya$ docker run -v /path/to/working-tree:/workspace/tsumonya -it --rm cryolite/tsumonya
```
