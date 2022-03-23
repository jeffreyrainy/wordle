#pragma once

#include "state.h"

class SimplifyState: public State
{
public:
    using State::State;

    string description(string solution);
};