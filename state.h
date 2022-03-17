#pragma once

#include <iostream>
#include <map>
#include <array>
#include <deque>
#include <vector>
#include <iomanip>
#include <set>
#include <string>
#include <random>
#include "rng.h"

using std::cout, std::map, std::array, std::vector, std::swap, std::endl, std::deque, std::ostream, std::set, std::to_string, std::string, std::toupper;

constexpr const int MaxSize = 15;
constexpr const int MaxLetters = 26;

enum Match
{
    grey = 0,
    green,
    yellow,
};

class State
{
public:
    friend ostream& operator<<(ostream& out, const State& s);

    State(int size, int letters, RNG& rng);
    virtual ~State() {};

    string getGuess();
    string getIntermediate() {return intermediate;}
    void tell(vector<Match> matches);

    virtual void suggest(string) {};
    virtual void filterPairings() {};
    virtual void filterGrey() {};

protected:
    void solve(vector<bool> pairings);
    void solve(vector<bool> pairings, int letter);

    int Size;
    int Letters;
    string guess;
    string intermediate;
    vector<Match> matches;
    set<int> placesForLetter[MaxLetters];
    vector<int> pairing;
    vector<vector<int>> pairingSeen;
    RNG& rng;
};

ostream& operator<<(ostream& out, const State& s);
