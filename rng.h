#pragma once

#include <random>

class RNG
{
public:
    RNG(int a, int c, int modulus, int seed);
    int random(int rangeSize);

private:
    int a;
    int c;
    int modulus;
    int seed;

    std::random_device dev;
    std::mt19937 rng{dev()};
};