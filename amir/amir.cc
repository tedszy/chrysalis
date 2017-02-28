#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <bitset>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/cstdint.hpp>

/*

The Amir problem (combinatorics). Proposed a long
time ago by an engineering student named Amir.

Consider a lottery where you choose 6 out of 49 numbers.
What is the minimum set of choices that guarantees 3 out
of 6 no matter what winning combination is drawn?

I don't know of a way to compute a provably minimal
Amir set. The smallest I previously found contains 360
elements. The methods presented here find Amir sets
with 245 elements and one with 241 elements. I think 241
elements is a surprisingly small number. How much lower
can it go?

Combinations are represented as bit-permutations of
numbers having popcount of 6 in a 49-bit wide field
of an Int64.

This code demostrates cool i/o ability of std::bitset.

*/

const int num_combinations = 13983816;
typedef uint64_t Combination;
typedef std::array<Combination, num_combinations> All_combinations;
typedef std::vector<Combination> Amir_set;

int binomial(int n, int k)
{
  if (n == k) return 1;
  if (k > n - k) k = n - k;
  int result = 1;
  for (int i = 1; i <= k; i++) {
      result *= n;
      result /= i;
      n--;
  }
  return result;
}

All_combinations make_all_combinations()
{
  All_combinations result;
  Combination a = 63;
  Combination c, r;
  for (int i = 0; i < num_combinations; i++) {
    result[i] = a;
    c = a & -a;
    r = a + c;
    a = (((r ^ a) >> 2) / c) | r;
  }
  return result;
}

// A cross between reversing bits and mirror-image of bits.
Combination mirror_combination(Combination x, int width)
{
  Combination b{0};
  int i = 0;
  while (i < width) {
    b <<= 1;
    b |= (x & 1);
    x >>= 1;
    i += 1;
  }
  return b;
}

// Combinations are equivalent if they have more than
// two bits in common. We use an unrolled variation of
// Kernighan's popcount algorithm. We cut it short since
// we don't need the full popcount.
bool equivalent(Combination a, Combination b)
{
  Combination n{a & b};
  n = n & (n - 1);
  n = n & (n - 1);
  return n > 0;
}

void add_to_amir_set(Amir_set& amir, Combination p)
{
  bool not_in_amir = true;
  for (auto a : amir)
    if (equivalent(a, p)) {
      not_in_amir = false;
      break;
    }
  if (not_in_amir)
    amir.push_back(p);
}

int main()
{
  boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();

  // Add combinations and their mirrors to the amir set.
  Combination seed = 0b111111000000000000000000;
  Amir_set amir;
  add_to_amir_set(amir, seed);
  add_to_amir_set(amir, mirror_combination(seed, 49));

  // First combination is 63: 0b111111
  Combination a = 0b111111;
  Combination c, r;
  for (int i = 0; i < num_combinations; i++) {
    add_to_amir_set(amir, a);
    add_to_amir_set(amir, mirror_combination(a, 49));
    c = a & -a;
    r = a + c;
    a = (((r ^ a) >> 2) / c) | r;
  }

  boost::posix_time::ptime finish = boost::posix_time::microsec_clock::local_time();
  boost::posix_time::time_duration time_taken = finish - start;

  for (auto u : amir)
    std::cout << std::setfill('0') << std::bitset<49>(u) << std::endl;

  std::cout << "amir set size: " << amir.size() << std::endl;
  std::cout << "time: " << time_taken << std::endl;

  return 0;
}
