#include "rng.h"
#include <cstdlib>

int RNG::random(int rangeSize)
{
    return rand() % rangeSize;
}

