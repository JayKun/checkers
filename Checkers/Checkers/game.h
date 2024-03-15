#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

using Board = vector<string>;

enum class PlayerSide
{
    OPlayer,
    XPlayer,
};

struct Coordinates
{
    bool IsValid() const
    {
        return row >= 0 && col >= 0;
    }

    int row = -1;
    int col = -1;
};

class Game
{
public:
    Game(int size) :
        m_size(size),
        m_isRunning(true),
        m_curTurn(PlayerSide::OPlayer),
        m_winner(PlayerSide::OPlayer),
        m_board(size, string(size, ' '))
    {}

    void InitializeBoard();
    bool InitializeCustomBoard(Board&& board);
    bool CheckWinCondition();
    bool ProcessInput(const vector<string>& inputs);

    const Board& GetBoard() const;
    bool IsGameRunning() const;
    PlayerSide GetCurrentPlayerTurn() const;
    PlayerSide GetWinner() const;
    Coordinates GetCoordinates(const string& input) const;
    void PrintBoard() const;

private:
    bool IsCapture(const Coordinates& origin, const Coordinates& dest) const;
    bool MovePiece(const Coordinates& origin, const Coordinates& dest);
    void Set(const Coordinates& coord, char c);
    char Get(const Coordinates& coord) const;
    void NextTurn();
    bool HandleMove(const Coordinates& origin, const Coordinates& dest);
    bool HandleCapture(const Coordinates& origin, const Coordinates& dest);

    vector<Coordinates> GetPossibleMoves(const Coordinates& origin) const;

    // Validation helper functions
    bool CanMove(const Coordinates& origin, const Coordinates& dest) const;
    bool ValidateInputs(const vector<string>& inputs) const;
    bool ValidateFormat(const string& input) const;
    bool ValidateValues(const string& input) const;
    bool ValidateCoordinates(const Coordinates& coord) const;
    inline bool IsReachable(const Coordinates& a, const Coordinates& b, int steps) const;

    const int m_size;
    bool m_isRunning;
    PlayerSide m_curTurn;
    PlayerSide m_winner;

    // Internal representation of checkers board
    Board m_board;
};
