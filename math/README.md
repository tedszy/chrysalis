# Math

You need ```gcc version 5.2``` and ```boost 1.55.``` Then do:

```
$ make all
```

## nthroot

Consider a sequence ```a(1), a(2), ...``` If the ratio ```a(k+1)/a(k)```
tends to ```L,``` then ```nthroot(a(n))``` also does. Run this program
to see this in action with the Fibonacci sequence. Notice
that ```nthroot(f(n))``` converges far slower to the Golden ratio
than ```a(k+1)/a(k)``` does.


## prime heap

If the sequence of prime powers ```p^(k^2), k=0,1,2...``` are put in
order, and if we take the product of the first ```N``` of these, we
get the _smallest number_ having exactly ```2^N``` divisors. Apparently
this is something noticed by Ramanujan.

Verifying this idea gives us a fine excuse to write our own binary heap.
The heap keeps the ```p^(2^k)``` prime powers organized in order
as we add many of them. Popping the top of the heap and accumulating
the products gives us the desired numbers. 






