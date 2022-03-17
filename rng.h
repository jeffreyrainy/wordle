#pragma once

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
};