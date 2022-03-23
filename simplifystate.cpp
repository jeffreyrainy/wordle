#include "simplifystate.h"
#include <sstream>

using std::ostringstream;

string SimplifyState::description(string solution)
{
    map<char, int> letterCardinality;
    map<int, int> cardinalityCount;

    for(int i = 0; i < Size; i++)
    {
        letterCardinality[solution[i]]++;
    }

    for(auto it:letterCardinality)
    {
        if (it.second > 1)
        {
            cardinalityCount[it.second]++;
        }
    }

    int count = 0;
    int yellowCount = 0;
    ostringstream oss;

    for(auto it:cardinalityCount)
    {
        oss << it.second << " solution letters present " << it.first << " times. ";
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