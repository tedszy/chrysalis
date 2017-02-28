/*

Consider a sequence a(1), a(2), .... a(n), ...

We have the following theorem:

If a(n+1)/a(n) tends to limit L, then nthroot(a_n) -> L also.

We want to see this in action. A convenient sequence to work
with is Fibonacci numbers f(n) because f(n+1)/f(n) tends to 
the Golden Ratio, phi. So we expect that nthroot(f(n)) approaches 
phi also. I've never actually seen this phenomenon computationally, 
so let's do it.

*/

#include <limits>
#include <iostream>
#include <iomanip>

#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>

typedef boost::multiprecision::cpp_int bigint;
typedef boost::multiprecision::cpp_dec_float_50 bigfloat;

/* 
Want to find n^k by doing only O(log2 k) multiplications.
Say n = 5, k = 100. Change k = 100 to base 2 by 
Euclidean division: 1100100.

result = 1
power_factor = n
For each Euclidean step:
  power_factor = power_factor^2
  when remainder r is 1, multiply result by power_factor.

r   power factor  result  
0   5       
0   5^2 
1   5^4           5^4
0   5^8
0   5^16
1   5^32          5^4 * 5^32
1   5^64          5^4 * 5^32 * 5^64

5^100 = 5^4 * 5^32 * 5^64 
*/
template<typename T>
T fast_expt(const T & n, int k) {
  T result{1};
  T power_factor{n};
  int r = 0;
  while (k > 0) {
    r = k % 2;
    k = k / 2;
    if (r == 1) result *= power_factor;
    power_factor *= power_factor;
  }
  return result;
}

bigfloat nthroot(const bigfloat & A, int n, const bigfloat & epsilon) {
  bigfloat x{1};
  bigfloat D;
  do {
    D = (A/fast_expt(x, n-1) - x)/n;
    x += D;
  } while (abs(D) > epsilon);
  return x;
}

/*
Matrix method for generating Fibonacci numbers.

[ 1 1 ]^n   = [a b] = [f(n+1) f(n)  ]
[ 1 0 ]       [c d]   [f(n)   f(n-1)]

[a b][a b] = [a^2 + bc  ac + dc ]  
[c d][c d]   [ab + bd   bc + d^2]

[1 1] [a b] = [a + c  b + d]
[1 0] [c d]   [a      b    ]

*/

class Fmatrix
{
public:
int n{1}; // keeps track of itself.
  bigint a{1};
  bigint b{1};
  bigint c{1};
  bigint d{0};
  // Since we will use fast exponentiation,
  // all we need is *= operator.
  Fmatrix & operator*=(const Fmatrix& rhs)
  {
    bigint new_a, new_b, new_c, new_d;
    new_a = a*rhs.a + b*rhs.c;
    new_b = a*rhs.b + b*rhs.d;
    new_c = c*rhs.a + d*rhs.c;
    new_d = c*rhs.b + d*rhs.d;
    a = new_a; b = new_b; c = new_c; d = new_d; n += rhs.n;
    return *this;
  }
};

std::ostream& operator<<(std::ostream& out, const Fmatrix& f)
{
  return out << std::endl 
	     << "n = " << f.n << std::endl 
	     << f.a << "  " << f.b << std::endl 
	     << f.c << "  " << f.d << std::endl;
}

/*
Here's how to get maximum bigfloat precision displayed:
std::cout.precision(std::numeric_limits<bigfloat>::digits10);
*/

int main(){
  // Table headers.
  std::cout << std::endl
	    << std::setw(5)
	    << "n"
	    << std::setw(30)
	    << "phi1 = f(n+1)/f(n)"
	    << std::setw(30) 
	    << "phi2 = nthroot(f(n))" << std::endl;
  std::cout << std::setw(5)
	    << std::string(5,'-')
	    << std::setw(30)
	    << std::string(30,'-')
	    << std::setw(30) 
	    << std::string(30,'-') << std::endl;

  // Body of table.
  Fmatrix f;
  Fmatrix power_factor;
  int k = 2047; // limit - 1 because result f starts off at power 1 not 0.
  int r;
  while (k > 0) 
    {
      r = k % 2;
      k = k / 2;
      if (r == 1) {
	f *= power_factor;
	bigfloat fa = bigfloat(f.a);
	bigfloat fb = bigfloat(f.b);
	bigfloat phi1 = fa/fb;
	bigfloat phi2 = nthroot(fb, f.n, 0.00000001);
	std::cout << std::setw(5)
		  << f.n 
		  << std::setw(30)
		  << std::right << std::setprecision(16) << phi1 
		  << std::setw(30)
		  << std::right << std::setprecision(16) << phi2 
		  << std::endl; 	
      }
      power_factor *= power_factor;
    }
}




