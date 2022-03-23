#include "state.h"

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

/*    for(int l=0; l < s.Letters; l++)
    {
        out << (char)('A' + l) << ": ";
        for(auto p: s.placesForLetter[l])
        {
            out << p << " ";
        }
        out << " ";
    }
    out << endl;
*/

    return out;
}

State::State(int size, int letters, RNG& rng)
:rng(rng)
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

    matches.resize(Size, grey);
}

void State::tell(vector<Match> matchesFound)
{
    matches = matchesFound;
    set<char> yellows;
    for(int i=0; i < Size; i++)
    {
        if (matches[i] == green || matches[i] == yellow)
        {
            placesForLetter[guess[i] - 'A'].erase(i);
            if (matches[i] == yellow)
            {
                yellows.insert(guess[i]);
            }
            if (matches[i] == green)
            {
                for(int j = 0; j < Letters; j++)
                {
                    placesForLetter[j].erase(i);
                }
            }
        }
        else
        {
            if (yellows.find(guess[i]) == yellows.end())
            {
                // there's no yellow of this letter, and it is grey here. So it is not present
                placesForLetter[guess[i] - 'A'].clear();
            }
            else
            {
                // there's a yellow of this letter, but it is grey here. So it might go elsewhere **for placement**
                placesForLetter[guess[i] - 'A'].erase(i);
                nonFreeLetters.insert(guess[i] - 'A');
            }
        }
    }

    for(auto it = nonFreeLetters.begin(); it != nonFreeLetters.end(); )
    {
        if (yellows.find((*it) + 'A') == yellows.end())
        {
            placesForLetter[(*it)].clear();
            it = nonFreeLetters.erase(it);
        }
        else
        {
            it++;
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

void State::filterPairings(vector<vector<char>>& pickLetters)
{
    // for each lette rposition
    for(int i = 0; i < Size; i++)
    {
        // if we have no letters other than placement for this position
        if (pickLetters[i].size() == 0 && matches[i] != green)
        {
            // for each pairing
            for(int j = 0; j < pairingSeen.size(); )
            {
                auto& p = pairingSeen[j];
                bool bad = true;
                // for each target position
                for(auto m: p)
                {
                    // if we're placing a letter there
                    if (m == i)
                    {
                        bad = false;
                        break;
                    }
                }

                if (bad)
                {
                    pairingSeen[j] = pairingSeen[pairingSeen.size() - 1];
                    pairingSeen.resize(pairingSeen.size() - 1);
                }
                else
                {
                    j++;
                }
            }
        }
    }
}

std::string State::getGuess()
{
    string next;

    next.resize(Size, '.');

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

    // the available letters for the positions not used by the placement are looked-up before the placement.
    // that way we can avoid placement that would leave a position with no possibilities
    vector<vector<char>> pickLetters(Size);
    for(int i = 0; i < Size; i++)
    {
        // for unplaced letters
        // pick a letter that could still go here. (not previously yellow in this column nor grey in any columns except if other yellow present)
        for(int j = 0; j < Letters; j++)
        {
            if (placesForLetter[j].find(i) != placesForLetter[j].end() && nonFreeLetters.find(j) == nonFreeLetters.end())
            {
                pickLetters[i].push_back('A' + j);
            }
        }
    }


    // find a way the yellow letters could be arranged
    solve(pairings);

    filterPairings(pickLetters);

    set<string> nexts;

    for(auto pairing:pairingSeen)
    {
        string potentialNext = next;

        int j=0;
        for(int i=0; i < Size; i++)
        {
            if (matches[i] == yellow)
            {
                potentialNext[pairing[j++]] = guess[i];
            }
        }

        nexts.insert(potentialNext);
    }

    //cout << nexts.size() << " " << pairingSeen.size() << endl;
    assert(nexts.size() == pairingSeen.size() || nexts.size() * 2 == pairingSeen.size() || nexts.size() * 4 == pairingSeen.size());

    vector<string> nextsVector;
    for(auto it:nexts)
    {
        nextsVector.push_back(it);
    }

    // pick a way
    int choice = rng.random(nextsVector.size());

    next = nextsVector[choice];

    intermediate = next;

    for(int i = 0; i < Size; i++)
    {
        if (next[i] == '.')
        {
            next[i] = pickLetters[i][rng.random(pickLetters[i].size())];
        }
    }

    guess = next;

    filterGrey();

    return guess;
}
