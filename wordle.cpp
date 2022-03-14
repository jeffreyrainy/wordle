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

std::random_device dev;
std::mt19937 rng(dev());

enum Match
{
    grey = 0,
    green,
    yellow,
};

class State
{
public:
    string guess;
    Match matches[MaxSize] = {grey};
    set<int> placesForLetter[MaxLetters];

    State(int size, int letters);

    void initial();
    void match(string solution);
    void improve();
    void solve(vector<bool> pairings);

    int Size;
    int Letters;

private:
    void solve(vector<bool> pairings, int letter);
    vector<int> pairing;
    vector<vector<int>> pairingSeen;
    std::uniform_int_distribution<std::mt19937::result_type> letterRng;
};

ostream& operator<<(ostream& out, const State& s)
{
    for(int i = 0; i < s.Size; i++)
    {
        switch(s.matches[i])
        {
            case grey:
                out << "\x1b[37;40m";
                break;
            case yellow:
                out << "\x1b[30;43m";
                break;
            case green:
                out << "\x1b[30;42m";
                break;
        }

        out << s.guess[i];
    }
    out << "\x1b[33;0m";
    out << endl;

    return out;
}

State::State(int size, int letters)
{
    Size = size;
    Letters = letters;
    for(int i = 0; i < Letters; i++)
    {
        for(int j = 0; j < Size; j++)
        {
            placesForLetter[i].insert(j);
        }
    }

    letterRng = std::uniform_int_distribution<std::mt19937::result_type>{(unsigned int)0, (unsigned int)(Letters - 1)};

}

void State::initial()
{
    guess.resize(Size);

    for(int i = 0; i < guess.size(); i++)
    {
        guess[i] = letterRng(rng) + 'A';
    }
}

void State::match(string solution)
{
    map<char, int> available;

    for(int i = 0; i < Size; i++)
    {
        matches[i] = grey;
    }

    for(int i = 0; i < Size; i++)
    {
        if (solution[i] == guess[i])
        {
            matches[i] = green;

            for(int j = 0; j < Letters; j++)
            {
                placesForLetter[j].erase(i);
            }
        }
        else
        {
            available[solution[i]]++;
        }
    }
    for(int i = 0; i < Size; i++)
    {
        if (matches[i] != green)
        {
            if (available.find(guess[i]) != available.end() && available[guess[i]])
            {
                available[guess[i]]--;
                matches[i] = yellow;
                placesForLetter[guess[i] - 'A'].erase(i);
            }
            else
            {
                matches[i] = grey;

                // if we didn't have yellow of this letter, don't ever reuse it
                if (available.find(guess[i]) == available.end())
                {
                    placesForLetter[guess[i] - 'A'].clear();
                }
                else
                {
                    // otherwise, it just goes elsewhere
                    placesForLetter[guess[i] - 'A'].erase(i);
                }
            }
        }
    }
}

// recursively try the various way the yellow letters can go over available spots
void State::solve(vector<bool> pairings, int letter)
{
    if (letter == Size)
    {
        pairingSeen.push_back(pairing);
        return;
    }

    if (matches[letter] == yellow)
    {
        vector<int> pos;
        for(int j=0; j < Size; j++)
        {
            if (pairings[letter * Size + j])
            {
                pos.push_back(j);
            }
        }

        if (pos.size() == 0)
        {
            return;
        }

        for(auto choice: pos)
        {
            auto nextPairings = pairings;
            for(int j=0; j < Size; j++)
            {
                nextPairings[letter * Size + j] = false;
            }
            for(int j=0; j < Size; j++)
            {
                nextPairings[j * Size + choice] = false;
            }

            pairing.push_back(choice);
            solve(nextPairings, letter + 1);
            pairing.resize(pairing.size() - 1);
        }
    }
    else
    {
        solve(pairings, letter + 1);
    }
}

// find the various way the yellow letters can go over available spots
void State::solve(vector<bool> pairings)
{
    pairingSeen.clear();
    solve(pairings, 0);
}

void State::improve()
{
    string next;

    next.resize(Size, ' ');

    for(int i = 0; i < Size; i++)
    {
        if (matches[i] == green) // keep the green ones
        {
            next[i] = guess[i];
        }
    }

    vector<bool> pairings(Size * Size, false);
    for(int ii=0; ii < Size; ii++)
    {
        if (matches[ii] == yellow)
        {
            for(auto p: placesForLetter[guess[ii] - 'A'])
            {
                pairings[ii * Size + p] = true;
            }
        }
    }

    // find a way the yellow letters could be arranged
    solve(pairings);

    // pick a way
    std::uniform_int_distribution<std::mt19937::result_type> moveRng(0, pairingSeen.size() - 1);
    int choice = moveRng(rng);

    vector<int> moves = pairingSeen[choice];

    int j=0;
    for(int i=0; i < Size; i++)
    {
        if (matches[i] == yellow)
        {
            next[moves[j++]] = guess[i];
        }
    }

    for(int i = 0; i < Size; i++)
    {
        // for unplaced letters
        if (next[i] == ' ')
        {
            // pick a letter that could still go here. (not previously yellow in this column nor grey in any columns except if other yellow present)
            vector<char> pickLetters;
            for(int j = 0; j < Letters; j++)
            {
                if (placesForLetter[j].find(i) != placesForLetter[j].end())
                {
                    pickLetters.push_back('A' + j);
                }
            }

            std::uniform_int_distribution<std::mt19937::result_type> letterDist(0, pickLetters.size() - 1);
            next[i] = pickLetters[letterDist(rng)];
        }
    }

    guess = next;
}

int main(int argc, char* argv[])
{
    long total = 0;
    long count = 0;
    bool verbose = false;
    int size = 5;
    int letters = 26;
    int next = 0;

    for(int i = 1; i < argc; i++)
    {
        if (argv[i] == (string)"verbose")
        {
            verbose = true;
        }
        else if (argv[i] == (string)"-l")
        {
            next = 0;
        }
        else if (argv[i] == (string)"-s")
        {
            next = 1;
        }
        else
        {
            if (next == 0)
            {
                letters = std::stoi(argv[i]);
            }
            else
            {
                size = std::stoi(argv[i]);
            }
            next = !next;
        }
    }

    std::uniform_int_distribution<std::mt19937::result_type> letterRng;
    letterRng = std::uniform_int_distribution<std::mt19937::result_type>{(unsigned int)0, (unsigned int)(letters - 1)};

    while(true)
    {
        int moves = 1;
        State s(size, letters);
        string solution;
        solution.resize(size);

        for(int i = 0; i < size; i++)
        {
            solution[i] = letterRng(rng) + 'A';
        }

        s.initial();
        s.match(solution);

        if (verbose)
        {
            cout << "=====" << endl;
            cout << solution << endl;
            cout << "=====" << endl;
            cout << s;
        }

        while(solution != s.guess)
        {
            s.improve();
            s.match(solution);
            if (verbose)
            {
                cout << s;
            }
            moves++;
        }
        if (verbose)
        {
            cout << moves << " guesses " << endl << endl;
        }

        total += moves;
        count++;

        if (count % 10000 == 0)
        {
            cout << std::setprecision(8) << (double)total / count << endl;
        }
    }

    return 0;
}
