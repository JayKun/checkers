// Checkers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "game.h"

#include <algorithm>

constexpr auto s_promptPrefix = "player ";
constexpr auto s_promptSuffix = "> ";

namespace
{
    vector<string> SplitString(string str, string delimiter = " ")
    {
        // Trim leading whitespace
        str.erase(0, str.find_first_not_of(' '));

        // Trim ending whitespace
        str.erase(str.find_last_not_of(' ') + 1);

        // Trim consecutive whitespaces
        str.erase(unique(str.begin(), str.end(),
            [](char a, char b)
            { return isspace(a) && isspace(b); }),
            str.end());

        vector<string> result;
        size_t start = 0, end = 0;
        string token;
        while ((end = str.find(delimiter, start)) != string::npos)
        {
            token = str.substr(start, end - start);
            start = end + delimiter.size();
            result.push_back(token);
        }

        result.push_back(str.substr(start));
        return result;
    }
}

int main()
{
    // Initialize 8 x 8 board
    Game game(8);
    game.InitializeBoard();

    // Fetch input from player
    while (game.IsGameRunning())
    {
        game.PrintBoard();

        // Generate input prompt
        string prompt = s_promptPrefix;
        switch (game.GetCurrentPlayerTurn())
        {
            case PlayerSide::OPlayer:
            {
                prompt += "\'o\'";
                break;
            }
            case PlayerSide::XPlayer:
            {
                prompt += "\'x\'";
                break;
            }
        }
        prompt += s_promptSuffix;

        cout << prompt;

        // Get input from player
        string input;
        getline(cin, input);

        // Parse input with the format
        auto parsedInput = SplitString(input);
        if (parsedInput.size() < 2)
        {
            cout << "Input length must be at least 2" << endl;
        }

        if (!game.ProcessInput(parsedInput))
        {
            cout << "Invalid input" << endl;
        }
    }

    // Current Player Turn is the winner
    switch (game.GetCurrentPlayerTurn())
    {
        case PlayerSide::OPlayer:
        {
            return 1;
        }
        case PlayerSide::XPlayer:
        {
            return 2;
        }
    }
}
