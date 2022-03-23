#include "simplifystate.h"
#include <sstream>

using std::ostringstream;

string SimplifyState::description(string solution)
{
    vector<int> cardinality(Size);
    map<int, int> cardinalityMap;
    set<char> seen;
    for(int i = 0; i < Size; i++)
    for(int j = 0; j < Size; j++)
    {
        if (solution[i] == solution[j])
        {
            cardinality[i]++;
        }
    }

    for(int i = 0; i < Size; i++)
    {
        if (cardinality[i] && seen.find(solution[i]) == seen.end())
        {
            cardinalityMap[cardinality[i]]++;
            seen.insert(solution[i]);
        }

    }

    int count = 0;
    int yellowCount = 0;
    ostringstream oss;

    for(auto it:cardinalityMap)
    {
        if (it.first > 1)
        {
            oss << it.second << " solution letters are there " << it.first << " times. ";
        }
    }

    for(int i = 0; i < Size; i++)
    {
        if (matches[i] == green)
        {
            count++;
        }
        if (matches[i] == yellow)
        {
            yellowCount++;
        }
    }
    oss << count << " target letters known, " << yellowCount << " letters to move. ";

    if (nonFreeLetters.size())
    {
        oss << " Non-free letters are ";
        for(auto l:nonFreeLetters)
        {
            oss << (char)(l + 'A') << " ";
        }
        oss << ". ";
    }


    return oss.str();
}