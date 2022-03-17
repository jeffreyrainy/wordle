#include "state.h"
#include "scenariostate.h"

vector<Match> match(string guess, string solution)
{
    vector<Match> matches;
    matches.resize(guess.size());

    map<char, int> potentialYellow;

    for(auto& m: matches)
    {
        m = grey;
    }

    for(int i = 0; i < guess.size(); i++)
    {
        if (solution[i] == guess[i])
        {
            matches[i] = green;
        }
        else
        {
            potentialYellow[solution[i]]++; // letter we can mark as yellow elsewhere
        }
    }
    for(int i = 0; i < guess.size(); i++)
    {
        if (matches[i] != green)
        {
            if (potentialYellow[guess[i]])
            {
                potentialYellow[guess[i]]--;
                matches[i] = yellow;
            }
            else
            {
                matches[i] = grey;
            }
        }
    }

    return matches;
}

int main(int argc, char* argv[])
{
    long total = 0;
    long count = 0;
    bool verbose = false;
    int size = 5;
    int letters = 26;
    int next = 0;
    string solution;
    vector<string> guesses;
    map<string,int> firstWords;
    map<int, int> instances;
    bool scenario = false;


    std::random_device dev;
    std::mt19937 rng(dev());

    for(int i = 1; i < argc; i++)
    {
        if (argv[i] == (string)"--help")
        {
            cout << argv[0] << " [verbose] [-l letters] [-s size] [scenario solution guess1 guess2 ...]" << endl;
            return 0;
        }
        else if (argv[i] == (string)"verbose")
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
        else if (argv[i] == (string)"scenario")
        {
            scenario = true;
            i++;
            solution = argv[i++];

            for(auto& c:solution)
            {
                c = toupper(c);
            }

            while(i < argc)
            {
                string guess = argv[i++];
                for(auto& c:guess)
                {
                    c = toupper(c);
                }
                guesses.push_back(guess);
            }
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
        int moves = 0;
        State* s = scenario ? new ScenarioState(size, letters) : new State(size, letters);
        string guess;

        solution.resize(size);

        // if we're not playing a specific scenario, pick a random solution
        if (!scenario)
        {
            for(int i = 0; i < size; i++)
            {
                solution[i] = letterRng(rng) + 'A';
            }
            if (verbose)
            {
                cout << "=====" << endl;
                cout << solution << endl;
                cout << "=====" << endl;
            }

            while(solution != guess)
            {
                guess = s->getGuess();
                s->tell(match(guess, solution));

                if (verbose)
                {
                    cout << *s;
                }
                moves++;
            }
        }
        else
        {
            while(solution != guess)
            {
                if (moves < guesses.size())
                {
                    s->suggest(guesses[moves]);
                }
                else
                {
                    s->suggest("");
                }

                guess = s->getGuess();
                s->tell(match(guess, solution));

                if (moves == guesses.size())
                {
                    firstWords[guess]++;
                }

                if (verbose)
                {
                    cout << *s;
                }
                moves++;
            }
        }

        if (verbose)
        {
            cout << moves << " guesses " << endl << endl;
        }

        total += moves;
        count++;
        instances[moves]++;

        if (count % 10000 == 0)
        {
            cout << std::setprecision(8) << (double)total / count << " (" << count << ")"<< endl;

            for(auto it:instances)
            {
                cout << it.first << " moves: " << std::setprecision(8) << (double)it.second / count << endl;
            }

            if (scenario)
            {
                for(auto it:firstWords)
                {
                    cout << it.first << " " << (double)it.second / count << endl;
                }
            }
        }

        delete s;
    }

    return 0;
}
