#include "state.h"
#include "rng.h"

class ScenarioState: public State
{
public:
    ScenarioState(int size, int letters, RNG& rng):State(size, letters, rng){};
    virtual void suggest(string);
    virtual void filterPairings();
    virtual void filterGrey();

private:
    string suggestion;

};
