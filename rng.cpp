#include "rng.h"
#include <cstdlib>
#include <iostream>

using std::cout, std::endl;

RNG::RNG(int a, int c, int modulus, int seed):a(a), c(c), modulus(modulus), seed(seed)
{

}

int RNG::random(int rangeSize)
{
    seed = (a * seed + c) % modulus;
    return seed % rangeSize;
}

