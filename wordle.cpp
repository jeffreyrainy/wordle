#include "state.h"

int main(int argc, char* argv[])
{
    long total = 0;
    long count = 0;
    bool verbose = false;
    int size = 5;
    int letters = 26;
    int next = 0;

    std::random_device dev;
    std::mt19937 rng(dev());

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
        int moves = 0;
        State s(size, letters);
        string solution;
        string guess;

        solution.resize(size);
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
            guess = s.getGuess();
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
