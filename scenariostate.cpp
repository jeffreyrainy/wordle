#include "scenariostate.h"

void ScenarioState::suggest(string s)
{
    suggestion = s;
}

void ScenarioState::filterGrey()
{
    if (suggestion == "")
    {
        return;
    }

    for(int i=0; i < Size; i++)
    {
        if (matches[i] == grey)
        {
            guess[i] = suggestion[i];
        }
    }
}

void ScenarioState::filterPairings()
{
    if (suggestion == "")
    {
        return;
    }

    for(int ii = 0; ii < pairingSeen.size(); )
    {
        auto& pairing = pairingSeen[ii];
        bool reject = false;

        int j=0;
        for(int i=0; i < Size; i++)
        {
            if (matches[i] == yellow)
            {
                if (suggestion[pairing[j++]] != guess[i])
                {
                    reject = true;
                }
            }
        }

        if (reject)
        {
            pairingSeen[ii] = pairingSeen[pairingSeen.size() - 1];
            pairingSeen.resize(pairingSeen.size() - 1);
        }
        else
        {
            ii++;
        }
    }

}