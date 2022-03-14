#include <iostream>
#include <map>
#include <array>
#include <deque>
#include <vector>
#include <iomanip>
#include <set>
#include <string>
#include <random>

using std::cout, std::map, std::array, std::vector, std::swap, std::endl, std::deque, std::ostream, std::set, std::to_string, std::string;

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

    State(int size, int letters);
    string getGuess();
    void tell(vector<Match> matches);

private:
    void solve(vector<bool> pairings);
    void solve(vector<bool> pairings, int letter);

    int Size;
    int Letters;
    string guess;
    vector<Match> matches;
    set<int> placesForLetter[MaxLetters];
    vector<int> pairing;
    vector<vector<int>> pairingSeen;
    std::uniform_int_distribution<std::mt19937::result_type> letterRng;
};

ostream& operator<<(ostream& out, const State& s);
