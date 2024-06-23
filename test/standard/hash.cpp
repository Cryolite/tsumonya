#include <tsumonya/standard/hash.hpp>
#include <tsumonya/standard/table.hpp>
#include <tsumonya/standard/core.hpp>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <climits>
#include <cstdint>
#include <cassert>


namespace{

using Tsumonya::Standard_::stable;
using Tsumonya::Standard_::mtable;
using Tsumonya::Standard_::ntable;
using Tsumonya::Standard_::xytable;
using Tsumonya::Standard_::upper_bound;
using PureHand = std::array<std::uint_fast8_t, 34u>;
using ChiList = std::array<std::uint_fast8_t, 21u>;
using PengGangList = std::array<std::uint_fast8_t, 34u>;
using Tsumonya::Standard_::getHash;
using Table = std::vector<std::atomic_uint8_t>;

std::mutex mtx;

void dump(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong)
{
  std::cout << "hand = ";
  for (unsigned h : pure_hand) {
    std::cout << h << ',';
  }
  std::cout << '\n';

  std::cout << "chi = ";
  for (unsigned c : chi_list) {
    std::cout << c << ',';
  }
  std::cout << '\n';

  std::cout << "peng = ";
  for (unsigned p : peng_list) {
    std::cout << p << ',';
  }
  std::cout << '\n';

  std::cout << "angang = ";
  for (unsigned ag : angang_list) {
    std::cout << ag << ',';
  }
  std::cout << '\n';

  std::cout << "minggang = ";
  for (unsigned mg : minggang_list) {
    std::cout << mg << ',';
  }
  std::cout << '\n';
  std::cout << "winning_tile = " << static_cast<unsigned>(winning_tile) << '\n';
  std::cout << "rong = " << rong << '\n';
  std::cout << "========================================" << std::endl;
}

void testHash(
  PureHand const &pure_hand,
  ChiList const &chi_list,
  PengGangList const &peng_list,
  PengGangList const &angang_list,
  PengGangList const &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  Table &table,
  std::atomic_uint_fast64_t &count)
try {
  PureHand pure_hand_copy(pure_hand);
  assert((winning_tile < 34u));
  assert((pure_hand_copy[winning_tile] >= 1u));
  --pure_hand_copy[winning_tile];
  std::uint_fast64_t const hash = getHash(
    pure_hand_copy,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    winning_tile,
    rong);
  if (hash >= upper_bound) {
    std::cout << "hash = " << hash << '\n';
    dump(pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
    throw std::logic_error("A hash is out of bounds.");
  }
  ++table[hash];

  ++count;
  if (count.load() % 1000000u == 0u) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << count.load() << " / " << upper_bound << std::endl;
  }
}
catch (std::exception const &e) {
  std::cerr << e.what() << std::endl;
  dump(pure_hand, chi_list, peng_list, angang_list, minggang_list, winning_tile, rong);
  throw;
}

bool isValidTransition(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  bool const a,
  bool const shunzi_prohibited,
  bool const has_winning_tile,
  std::uint_fast8_t const s)
{
  assert((i < 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  assert((x <= 4u));
  assert((y <= x));

  std::uint_fast8_t const n = stable[s][3u] * 2u + stable[s][6u] * 3u + stable[s][7u];
  bool const has_tile_in_pure_hand = a || n >= 1u;

  if (m + mtable[s] > 4u) {
    return false;
  }
  if (h + stable[s][3u] > 1u) {
    return false;
  }
  if (has_winning_tile && stable[s][2u] >= 1u) {
    return false;
  }
  if (!has_tile_in_pure_hand && stable[s][2u] >= 1u) {
    return false;
  }
  if (shunzi_prohibited && xytable[s] > 0u) {
    return false;
  }
  if (ntable[s] + x > 4u) {
    return false;
  }
  assert((xytable[s] + y <= 4u));

  return true;
}

void pushHand(
  std::uint_fast8_t const i,
  std::uint_fast8_t const s,
  PureHand &pure_hand,
  ChiList &chi_list,
  PengGangList &peng_list,
  PengGangList &angang_list,
  PengGangList &minggang_list)
{
  assert((i < 34u));
  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  std::uint_fast8_t const cindex = color <= 2u ? color * 7u + number : UINT_FAST8_MAX;
  pure_hand[i] += stable[s][3u] * 2u + stable[s][6u] * 3u + stable[s][7u];
  if (stable[s][7u] >= 1u) {
    assert((number < 7u));
    pure_hand[i + 1u] += stable[s][7u];
    pure_hand[i + 2u] += stable[s][7u];
  }
  if (stable[s][5u] >= 1u) {
    assert((cindex < 21u));
    chi_list[cindex] += stable[s][5u];
  }
  peng_list[i] += stable[s][4u];
  angang_list[i] += stable[s][1u];
  minggang_list[i] += stable[s][0u];
}

void popHand(
  std::uint_fast8_t const i,
  std::uint_fast8_t const s,
  PureHand &pure_hand,
  ChiList &chi_list,
  PengGangList &peng_list,
  PengGangList &angang_list,
  PengGangList &minggang_list)
{
  assert((i < 34u));

  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  std::uint_fast8_t const cindex = color <= 2u ? color * 7u + number : UINT_FAST8_MAX;

  minggang_list[i] -= stable[s][0u];
  angang_list[i] -= stable[s][1u];
  peng_list[i] -= stable[s][4u];
  if (stable[s][5u] >= 1u) {
    assert((cindex < 21u));
    chi_list[cindex] -= stable[s][5u];
  }
  if (stable[s][7u] >= 1u) {
    assert((number < 7u));
    pure_hand[i + 2u] -= stable[s][7u];
    pure_hand[i + 1u] -= stable[s][7u];
  }
  pure_hand[i] -= stable[s][3u] * 2u + stable[s][6u] * 3u + stable[s][7u];
}

void enumerateStandardHules(
  std::uint_fast8_t const i,
  std::uint_fast8_t const m,
  std::uint_fast8_t const h,
  std::uint_fast8_t const x,
  std::uint_fast8_t const y,
  PureHand &pure_hand,
  ChiList &chi_list,
  PengGangList &peng_list,
  PengGangList &angang_list,
  PengGangList &minggang_list,
  std::uint_fast8_t const winning_tile,
  bool const rong,
  Table &table,
  std::atomic_uint_fast64_t &count)
{
  assert((i <= 34u));
  assert((m <= 4u));
  assert((h <= 1u));
  for (std::uint_fast8_t const x : pure_hand) {
    assert((x <= 4u));
  }
  for (std::uint_fast8_t const c : chi_list) {
    assert((c <= 4u));
  }
  for (std::uint_fast8_t const p : peng_list) {
    assert((p <= 1u));
  }
  for (std::uint_fast8_t const ag : angang_list) {
    assert((ag <= 1u));
  }
  for (std::uint_fast8_t const mg : minggang_list) {
    assert((mg <= 1u));
  }
  assert((winning_tile == UINT_FAST8_MAX || winning_tile < 34u));

  if (i == 34u) {
    // Termination of recursion
    assert((x == 0u));
    assert((y == 0u));
    if (m == 4u && h == 1u && winning_tile != UINT_FAST8_MAX) {
      testHash(
        pure_hand,
        chi_list,
        peng_list,
        angang_list,
        minggang_list,
        winning_tile,
        rong,
        table,
        count);
    }
    return;
  }

  std::uint_fast8_t const color = i / 9u;
  std::uint_fast8_t const number = color <= 2u ? i % 9u : UINT_FAST8_MAX;
  std::uint_fast8_t const cindex = color <= 2u ? color * 7u + number: UINT_FAST8_MAX;
  bool const shunzi_prohibited = color <= 2u && number >= 7u || color == 3u;

  bool const a = pure_hand[i] >= 1u;
  bool const has_winning_tile = winning_tile != UINT_FAST8_MAX;

  for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
    if (!isValidTransition(i, m, h, x, y, a, shunzi_prohibited, has_winning_tile, s)) {
      continue;
    }
    pushHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
    enumerateStandardHules(
      i + 1u,
      m + mtable[s],
      h + stable[s][3u],
      xytable[s] + y,
      xytable[s],
      pure_hand,
      chi_list,
      peng_list,
      angang_list,
      minggang_list,
      stable[s][2u] >= 1u ? i : winning_tile,
      rong || stable[s][2u] == 2u,
      table,
      count);
    popHand(i, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
  }
}

void threadMain(
  std::uint_fast8_t const s, Table &table, std::atomic_uint_fast64_t &count)
{
  PureHand pure_hand{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  ChiList chi_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  PengGangList peng_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  PengGangList angang_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };
  PengGangList minggang_list{
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u
  };

  if (!isValidTransition(0u, 0u, 0u, 0u, 0u, false, false, false, s)) {
    return;
  }
  pushHand(0u, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
  enumerateStandardHules(
    1u,
    mtable[s],
    stable[s][3u],
    xytable[s],
    xytable[s],
    pure_hand,
    chi_list,
    peng_list,
    angang_list,
    minggang_list,
    stable[s][2u] >= 1u ? 0u : UINT_FAST8_MAX,
    stable[s][2u] == 2u,
    table,
    count);
  popHand(0u, s, pure_hand, chi_list, peng_list, angang_list, minggang_list);
}

} // namespace <anonymous>

int main(int argc, char const * const *)
{
  if (argc > 1) {
    throw std::runtime_error("Too many arguments.");
  }

  Table table(upper_bound);
  std::atomic_uint_fast64_t count = 0u;

  {
    std::vector<std::thread> threads;
    for (std::uint_fast8_t s = 0u; s < stable.size(); ++s) {
      std::thread th(&threadMain, s, std::ref(table), std::ref(count));
      threads.push_back(std::move(th));
    }      
    for (std::thread &th : threads) {
      th.join();
    }
  }

  std::uint_fast64_t const n = std::count_if(
    table.cbegin(), table.cend(), [](std::uint8_t const x){ return x >= 1u; });
  std::cout << n << " / " << upper_bound << std::endl;
  if (n != 6654031062u) {
    throw std::logic_error("A logic error.");
  }
}
