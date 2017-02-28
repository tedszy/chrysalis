/*

Integer sequence A050376
Numbers of the form p^(2^k) where p is prime and k >= 0.  

2, 3, 4, 5, 7, 9, 11, 13, 16, 17, 19, 23, 25, 29, 31, 37, 41, 
43, 47, 49, 53, 59, 61, 67, 71, 73, 79, 81, 83, 89, 97, 101, 
103, 107, 109, 113, 121, 127, 131, 137, 139, 149, 151, 157, 
163, 167, 169, 173, 179, 181, 191, 193, 197, 199, 211, 223, 
227, 229, 233, 239, 241,...

The smallest number having 2^n divisors is the product of the
first n terms of this sequence. We want to study this.

Integer sequence A037992 
Smallest number with 2^n divisors.
1, 2, 6, 24, 120, 840, 7560, 83160, 1081080, 17297280, 
294053760, 5587021440, 128501493120, 3212537328000, 
93163582512000, 2888071057872000, 106858629141264000, 
4381203794791824000,..

We will generate prime powers p^2^k and keep them in order by 
inserting them into a heap. 

It must be noted that this heap is totally ordered by <.
No element can be equal to another.

Max uint64_t is 18446744073709551616. We can handle all
numbers in A037992 presented above.

*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <sstream>
#include <cstdint> // uint64_t

// Reference sequence. Smallest numbers with 2^N divisors.
const std::vector<uint64_t> A037992{1, 2, 6, 24, 120, 840, 7560, 
    83160, 1081080, 17297280, 294053760, 5587021440, 128501493120, 
    3212537328000, 93163582512000, 2888071057872000, 
    106858629141264000, 4381203794791824000};

/*

Prime_power.

Structure that holds p^2^k data. It holds the prime p, the exponent 2^k
and the value p^2^k which is used to compare the size of two of them.

We are never going to have very large p^2^k because we will never
consider ones which are larger than the largest prime that we work
with. And that would fit in a uint64 or even uint32. So actually
we could use the full integer value of p^2^k to compare two Prime_power
objects. 

*/

// Fast n^2^k by successive squaring.
uint64_t succsq(uint64_t n, int k)
{
  while (k>0) {
    n *= n;
    k--;
  }
  return n;
}

struct Prime_power {
  Prime_power() : prime{1}, exponent{0}, value{1} {};
  Prime_power(uint32_t p, int k) 
    : prime(p), exponent(1 << k), value(succsq(p,k)) {};
  std::string to_string() const;
  uint32_t prime;      
  int exponent;        
  uint64_t value;  
};

std::string Prime_power::to_string() const
{
  std::ostringstream out;
  out << "(" 
      << prime << " " << " " 
      << exponent 
      << " " << value
      << ")";
  return out.str();
}

bool operator<(const Prime_power & lhs, const Prime_power & rhs)
{
  return lhs.value < rhs.value;
}

/*

Prime_heap.

Totally ordered. After inserting a Prime_power object into 
the heap, we won't be using it again outside the heap.

There is a dummy element at index 0, so the heap elements
begin at index 1. Heapsize is the number of heap elements,
which does not include the dummy element.

operator<< needs access to heap, so we make it friend.

*/

class Prime_heap 
{
public:
  bool empty() const;
  void insert(const Prime_power &); 
  void pop(Prime_power &);
  friend std::ostream & operator<<(std::ostream&, const Prime_heap&);
private:
  std::vector<Prime_power> heap{Prime_power(1,0)}; 
  int heapsize{0};
};

bool Prime_heap::empty() const
{
  return heapsize == 0;
}

std::ostream & operator<<(std::ostream & out, const Prime_heap & h)
{
  out << "(heap " << h.heapsize << " ";
  for (auto it = h.heap.begin()+1; it !=h.heap.begin()+1+h.heapsize; ++it) 
    out << " " << it->to_string();
  out << ")";
  return out;
}

void Prime_heap::insert(const Prime_power & p2k)
{
  // We want to insert p2k into the proper place.
  //
  // (1) Add a new element to the end of the heap.
  //     This serves as the hole that will bubble up.
  //     Heapsize is increased by 1.
  // 
  // (2) Start with k = heapsize (hole at end).
  // 
  // (3) Find the right place for the hole.
  //     If p2k < heap[k/2] (p2k < parent)
  //     then move hole upward: 
  //     heap[k] <- heap[k/2], k <- k/2. 
  //     Go to (3).
  //
  // (4) We are at the right place now.
  //     heap[k] <- p2k

  heap.push_back(Prime_power(1,0)); 
  int k = ++heapsize;
  while (p2k < heap[k/2]) {
      heap[k] = heap[k/2];
      k /= 2;
    }
  heap[k] = p2k;
}

// Assumes heap is not empty.
void Prime_heap::pop(Prime_power & top)
{  
  // (1) temp <- heap(heapsize--). Drop the last element from
  //     the heap and store it in temp. We do this to avoid 
  //     all the swapping during minheapify.
  // 
  // (2) k = 1, heap(1) is the hole element. We have moved it out
  //     to the user, so we don't care about what's in it. Effectively
  //     it is empty.
  //
  // (3) if k > heapsize/2, there are no children. 
  //     therefore heap(k) <- temp. Done.
  //
  // (4) if k = heapsize/2 and heapsize is even
  //     then there must be only one child. So...
  //       if temp < heap(2k) then heap(k) <- temp. Done.
  //       else heap(k) <- heap(2k), k <- 2k
  //       we move the hole element down. Done.
  // 
  // (5) It must be that we have 2 children: heap(2k) and heap(2k+1).
  //     If temp is smaller than these children, then we have found
  //     the right place for temp. If not, move the smaller child up
  //     and the hole element down.
  //     (a) if temp < heap(2k) and temp < heap(2k+1)
  //         then heap(k) <- temp. Done.
  //     (b) if heap(2k) < heap(2k+1) 
  //         then heap(k) <- heap(2k), k <- 2k (move down).
  //         else heap(k) <- heap(2k+1), k <- 2k+1 (move down).
  //         Go to (3).

  top = std::move(heap[1]);
  Prime_power temp = heap[heapsize--];
  int k = 1;
  int left, right;
  while (true) {
    left = 2*k;
    right = 2*k+1;
    if (k > heapsize/2) { // No children.
      heap[k] = temp;
      break;
    }
    if (k == heapsize/2 && heapsize % 2 == 0) { // One child.
      if (temp < heap[left]) {
	heap[k] = temp;
	break;
      } else {
	heap[k] = heap[left];
	k = left;
	break;
      }
    }
    if (temp < heap[left] && temp < heap[right]) { // Two children.
      heap[k] = temp;
      break;
    } else {
      if (heap[left] < heap[right]) {
	heap[k] = heap[left];
	k = left;
      } else {
	heap[k] = heap[right];
	k = right;
      }
    }
  }
}

/*

Prime sieve using bit vector of size N.

*/

template<int N> struct Prime_sieve
{
  std::bitset<N> sieve;
  std::vector<int> primes;
  void build(void);
};

template<int N> void Prime_sieve<N>::build(void)
{
  for (int i = 0; i < sieve.size(); ++i)
    sieve.set(i);
  sieve.reset(0);
  sieve.reset(1);
  for (int p = 2; p < sieve.size(); ++p)
    if (sieve[p])
      for (int k = 2; k*p < sieve.size(); ++k)
	sieve.reset(k*p);

  for (int i = 0; i < sieve.size(); ++i)
    if (sieve[i])
      primes.push_back(i);
}

template<int N> std::ostream & operator<<(std::ostream & out, Prime_sieve<N> & p)
{
  for (auto u : p.primes)
    out << u << " ";
  return out;
}

/*

Products of the first N pops of the heap are accumulated here
and printed out nicely.

To do: merge prime powers with the same bases.

*/
struct Minimal_accumulator
{
  std::vector<uint64_t> prime;
  std::vector<uint64_t> exponent;
  uint64_t value{1};
  void insert(const Prime_power & pk);
  std::string to_string() const;
};

void Minimal_accumulator::insert(const Prime_power & pk)
{
  prime.push_back(pk.prime);
  exponent.push_back(pk.exponent);
  value *= pk.value;
}

std::string Minimal_accumulator::to_string() const
{
  std::ostringstream out;
  for (auto a = prime.begin(), b = exponent.begin(); 
       a != prime.end() || b != exponent.end(); 
       ++a, ++b)
    if (*b==1)
      out << "(" << *a << ")";
    else
      out << "(" << *a << "^" << *b << ")";
  return out.str();
}

int main()
{ 
  Prime_sieve<1000> ps;
  ps.build();

  // The limit for p^2^k is the last prime in the prime sieve.
  Prime_power limit(ps.primes.back(),0);

  // Populate the prime heap. k is increased until prime^2^k is 
  // above the limit, then move on to the next prime. 
  Prime_heap h;
  int k;
  for (auto p : ps.primes) {
    k = 0;
    while (true) {
      Prime_power pk(p,k);
      if (!(pk < limit))
	break;
      h.insert(pk);
      ++k;
    }
  }

  // Table of form...
  //
  // N  |  product of first N p^2^k  |  value  |  Sloane A037992
  
  int N_width = 5;
  int product_width = 70;
  int value_width = 25;

  // Title
  std::cout << std::endl 
	    << "Smallest numbers with 2^N divisors is the product of the first" 
    // << std::endl
	    << " N terms in the ordered heap of prime powers p^2^k. (Ramanujan)"
	    << std::endl;

  // Table header.
  std::cout << std::endl;
  std::cout << std::setw(N_width) << "N" 
	    << std::setw(product_width) << "product of first N values of p^2^k from heap" 
	    << std::setw(value_width) << "my value" 
	    << std::setw(value_width) << "Sloan A037992" << std::endl;
  std::cout << std::setw(N_width) << std::string(N_width,'=')
	    << std::setw(product_width) << std::string(product_width,'=') 
	    << std::setw(value_width) << std::string(value_width,'=') 
	    << std::setw(value_width) << std::string(value_width,'=') << std::endl;

  // Table body.
  Minimal_accumulator ma;
  Prime_power temp; 
  for (int N = 1; N < 18; ++N) {
    h.pop(temp);
    ma.insert(temp);
    std::cout << std::setw(N_width) << N
	      << std::setw(product_width) << ma.to_string() 
	      << std::setw(value_width) << ma.value 
	      << std::setw(value_width) << A037992[N] << std::endl;
  }

  return 0;
}
