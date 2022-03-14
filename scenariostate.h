#include "state.h"

class ScenarioState: public State
{
public:
    ScenarioState(int size, int letters):State(size, letters){};
    virtual void suggest(string);
    virtual void filterPairings();
    virtual void filterGrey();

private:
    string suggestion;

};
