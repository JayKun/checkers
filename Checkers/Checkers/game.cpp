#include "game.h"

#include <string>
#include <vector>

using namespace std;

constexpr auto s_oPiece = 'o';
constexpr auto s_oKingPiece = 'O';
constexpr auto s_xPiece = 'x';
constexpr auto s_xKingPiece = 'X';

constexpr auto s_emptyPiece = '.';

//------------------------------------------------------------------------
// Game Implementation - Public API
//------------------------------------------------------------------------
bool Game::InitializeBoard()
{
    // Initialize x pieces
    for (int row = 0; row < 1; ++row)
    {
        const int startPoint = row % 2 == 0 ? 1 : 0;
        for (int col = startPoint; col < m_size; col += 2)
        {
            m_board[row][col] = s_xPiece;
        }
    }

    // Initialize empty rows
    for (int row = 1; row < m_size - 3; ++row)
    {
        const int startPoint = row % 2 == 0 ? 1 : 0;
        for (int col = startPoint; col < m_size; col += 2)
        {
            m_board[row][col] = s_emptyPiece;
        }
    }

    // Initialize o pieces
    for (int row = m_size - 3; row < m_size; ++row)
    {
        const int startPoint = row % 2 == 0 ? 1 : 0;
        for (int col = startPoint; col < m_size; col += 2)
        {
            m_board[row][col] = s_oKingPiece;
        }
    }

    return true;
}

bool Game::CheckWinCondition()
{
    // Case 1: One side has no more pieces remaining
    int nOPieces = 0;
    int nXPieces = 0;
    for (const auto& row : m_board)
    {
        for (auto c : row)
        {
            if (c == s_oPiece || c == s_oKingPiece)
                nOPieces++;
            else if (c == s_xPiece || c == s_xKingPiece)
                nXPieces++;
        }
    }

    if (nOPieces == 0 || nXPieces == 0)
    {
        PrintBoard();
        m_isRunning = false;
        return true;
    }

    // Case 2: No more valid moves for one side
    return false;
}

const Board& Game::GetBoard() const
{
    return m_board;
}

bool Game::IsGameRunning() const
{
    return m_isRunning;
}

PlayerSide Game::GetCurrentPlayerTurn() const
{
    return m_curTurn;
}

Coordinates Game::GetCoordinates(const string& input) const
{
    Coordinates coord;
    coord.row = m_size - atoi(input.substr(1).c_str());
    coord.col = input[0] - 'a';

    return coord;
}

void Game::PrintBoard() const
{
    cout << endl;
    string topRow = "  ";
    for (int i = 0; i < m_size; i++)
    {
        topRow += char('a' + i);
        topRow += " ";
    }
    cout << topRow << endl;
    int colNo = m_size;
    for (const auto& row : m_board)
    {
        cout << colNo << " ";

        // Add padding
        string rowStr;
        for (char c : row)
        {
            rowStr += c;
            rowStr += " ";
        }

        cout << rowStr << colNo-- << endl;
    }
    cout << topRow << endl << endl;
}

bool Game::ProcessInput(const vector<string>& inputs)
{
    if (!ValidateInputs(inputs) || inputs.size() < 2)
    {
        return false;
    }

    // Locate piece
    Coordinates piece = GetCoordinates(inputs[0]);
    const char pieceSymbol = Get(piece);

    // Validate piece
    switch (m_curTurn)
    {
    case PlayerSide::OPlayer:
    {
        if (pieceSymbol != s_oPiece && pieceSymbol != s_oKingPiece)
        {
            cout << "Not a valid o piece" << endl;
            return false;
        }
        break;
    }
    case PlayerSide::XPlayer:
    {
        if (pieceSymbol != s_xPiece && pieceSymbol != s_xKingPiece)
        {
            cout << "Not a valid x piece" << endl;
            return false;
        }
        break;
    }
    }

    // Move piece
    for (size_t i = 1; i < inputs.size(); ++i)
    {
        const auto& move = inputs[i];
        Coordinates dest = GetCoordinates(move);
        if (Get(dest) != s_emptyPiece)
        {
            cout << "Invalid destination" << endl;
            return false;
        }

        if (!MovePiece(piece, dest))
        {
            cout << "Unable to move from ";
            cout << inputs[i-1] << " to ";
            cout << move << endl;

            return false;
        }

        if (IsCapture(piece, dest))
        {
            piece = dest;
        }
        else
        {
            break;
        }
    }

    if (!CheckWinCondition())
    {
        NextTurn();
    }

    return true;
}

//------------------------------------------------------------------------
// Game Implementation - Private API
//------------------------------------------------------------------------
bool Game::IsCapture(const Coordinates& origin, const Coordinates& dest) const
{
    // 1 step means a move, 2 steps means a capture
    const int nSteps = abs(dest.row - origin.row);
    if (nSteps == 2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Game::MovePiece(const Coordinates& origin, const Coordinates& dest)
{
    if (!CanMove(origin, dest))
    {
        return false;
    }

    if (IsCapture(origin, dest))
    {
        return HandleCapture(origin, dest);
    }
    else
    {
        return HandleMove(origin, dest);
    }
}

void Game::Set(const Coordinates& coord, char c)
{
    m_board[coord.row][coord.col] = c;
}

char Game::Get(const Coordinates& coord) const
{
    return m_board[coord.row][coord.col];
}

void Game::NextTurn()
{
    switch (m_curTurn)
    {
        case PlayerSide::OPlayer:
        {
            m_curTurn = PlayerSide::XPlayer;
            break;
        }
        case PlayerSide::XPlayer:
        {
            m_curTurn = PlayerSide::OPlayer;
            break;
        }
    }
}

bool Game::HandleMove(const Coordinates& origin, const Coordinates& dest)
{
    Set(dest, Get(origin));
    Set(origin, s_emptyPiece);

    // Handle Promotion
    if (dest.row == 0 && Get(dest) == s_oPiece)
    {
        Set(dest, s_oKingPiece);
    }
    if (dest.row == m_size - 1 && Get(dest) == s_xPiece)
    {
        Set(dest, s_xKingPiece);
    }

    return true;
}

bool Game::HandleCapture(const Coordinates& origin, const Coordinates& dest)
{
    Coordinates pos;

    // Right direction
    if (dest.col > origin.col)
    {
        pos.col = dest.col - 1;
    }
    else // Left direction
    {
        pos.col = dest.col + 1;
    }

    switch (Get(origin))
    {
        case s_oPiece:
        {
            pos.row = dest.row + 1;
            const char c = Get(pos);
            if (c != s_xPiece && c != s_xKingPiece)
                return false;
            else
                Set(pos, s_emptyPiece);
            break;
        }
        case s_xPiece:
        {
            pos.row = dest.row - 1;
            const char c = Get(pos);
            if (c != s_oPiece && c != s_oKingPiece)
                return false;
            else
                Set(pos, s_emptyPiece);
            break;
        }
        case s_oKingPiece:
        {
            pos.row = dest.row > origin.row ? dest.row - 1 : dest.row + 1;
            const char c = Get(pos);
            if (c != s_xPiece && c != s_xKingPiece)
                return false;
            else
                Set(pos, s_emptyPiece);
            break;
        }
        case s_xKingPiece:
        {
            pos.row = dest.row > origin.row ? dest.row - 1 : dest.row + 1;
            const char c = Get(pos);
            if (c != s_oPiece && c != s_oKingPiece)
                return false;
            else
                Set(pos, s_emptyPiece);
            break;
        }
    }

    return HandleMove(origin, dest);
}

bool Game::CanMove(const Coordinates& origin, const Coordinates& dest) const
{
    if (!IsReachable(origin, dest, 2) || Get(dest) != s_emptyPiece)
    {
        return false;
    }

    const char c = Get(origin);
    // As king, the piece can move forwards and backwards
    if (c == s_xKingPiece || c == s_oKingPiece)
    {
        return true;
    }
    else if (c == s_oPiece)// As regular piece
    {
        return dest.row < origin.row;
    }
    else if (c == s_xPiece)
    {
        return dest.row > origin.row;
    }
    else
    {
        return false;
    }
}

bool Game::ValidateInputs(const vector<string>& inputs) const
{
    for (const auto& input : inputs)
    {
        if (!ValidateFormat(input) || !ValidateValues(input))
            return false;
    }

    return true;
}

bool Game::ValidateFormat(const string& input) const
{
    if (input.size() < 2)
    {
        return false;
    }

    if (!islower(input[0]))
    {
        return false;
    }

    for (size_t i = 1; i < input.size(); ++i)
    {
        if (!isdigit(input[i]))
        {
            return false;
        }
    }

    return true;
}

bool Game::ValidateValues(const string& input) const
{
    const int col = input[0] - 'a';
    const int row = m_size - atoi(input.substr(1).c_str());
    
    if (col < 0 || col >= m_size)
    {
        cerr << "Col out of bounds: " << col << endl;
        return false;
    }

    if (row < 0 || row >= m_size)
    {
        cerr << "Row out of bounds: " << row << endl;
        return false;
    }

    return true;
}

inline bool Game::IsReachable(const Coordinates& a, const Coordinates& b, int steps) const
{
    const auto rowDiff = abs(a.row - b.row);
    const auto colDiff = abs(a.col - b.col);
    return rowDiff <= steps && colDiff <= steps && rowDiff == colDiff;
}
