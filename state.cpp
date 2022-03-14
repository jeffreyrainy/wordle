#include "state.h"

std::random_device dev;
std::mt19937 rng(dev());

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

std::string State::getGuess()
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

    return guess;
}
