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
void Game::InitializeBoard()
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
}

bool Game::InitializeCustomBoard(Board&& board)
{
    // We allow the function to accept a custom state
    // to easily test and simulate certain board states
    if (board.size() == m_size
        && board.size() > 0
        && board.front().size() == m_size)
    {
        m_board = move(board);
        return true;
    }

    return false;
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

    if (nOPieces == 0)
    {
        m_winner = PlayerSide::XPlayer;
        return true;
    }
    else if (nXPieces == 0)
    {
        m_winner = PlayerSide::OPlayer;
        return true;
    }

    // Case 2: No more valid moves for O side
    bool hasValidMovesForO = false;
    if (!hasValidMovesForO)
    {
        for (size_t i = 0; i < m_board.size(); ++i)
        {
            for (size_t j = 0; j < m_board[i].size(); ++j)
            {
                const char c = m_board[i][j];
                if (c != s_oPiece && c != s_oKingPiece)
                {
                    continue;
                }

                Coordinates origin;
                origin.row = i;
                origin.col = j;
                if (GetPossibleMoves(origin).size() > 0)
                {
                    hasValidMovesForO = true;
                    break;
                }
            }

            if (hasValidMovesForO)
                break;
        }
    }
    if (!hasValidMovesForO)
    {
        m_winner = PlayerSide::XPlayer;
        return true;
    }

    // Case 2: No more valid moves for X side
    bool hasValidMovesForX = false;
    if (!hasValidMovesForX)
    {
        for (size_t i = 0; i < m_board.size(); ++i)
        {
            for (size_t j = 0; j < m_board[i].size(); ++j)
            {
                const char c = m_board[i][j];
                if (c != s_xPiece && c != s_xKingPiece)
                {
                    continue;
                }

                Coordinates origin;
                origin.row = i;
                origin.col = j;
                if (GetPossibleMoves(origin).size() > 0)
                {
                    hasValidMovesForX = true;
                    break;
                }
            }

            if (hasValidMovesForX)
                break;
        }
    }
    if (!hasValidMovesForX)
    {
        m_winner = PlayerSide::OPlayer;
        return true;
    }

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

PlayerSide Game::GetWinner() const
{
    return m_winner;
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

    m_isRunning = !CheckWinCondition();
    if (m_isRunning)
    {
        NextTurn();
    }
    else
    {
        // Print final baord
        PrintBoard();
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
    if (coord.IsValid())
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

bool Game::ValidateCoordinates(const Coordinates& coord) const
{
    if (coord.col < 0 || coord.col >= m_size)
    {
        return false;
    }

    if (coord.row < 0 || coord.row >= m_size)
    {
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

vector<Coordinates> Game::GetPossibleMoves(const Coordinates& origin) const
{
    vector<Coordinates> moves;

    // Move and capture regular pieces
    Coordinates leftMove, rightMove, leftCapture, rightCapture;

    // Move and capture for king pieces
    Coordinates left2Move, right2Move, left2Capture, right2Capture;

    switch (Get(origin))
    {
        case s_oPiece:
        {
            // Move left
            leftMove.col = origin.col - 1;
            leftMove.row = origin.row - 1;

            // Move right
            rightMove.col = origin.col + 1;
            rightMove.row = origin.row - 1;

            // Capture left
            leftCapture.col = origin.col - 2;
            leftCapture.row = origin.row - 2;

            // Capture right
            rightCapture.col = origin.col + 2;
            rightCapture.row = origin.row - 2;

            break;
        }
        case s_xPiece:
        {
            // Move left
            leftMove.col = origin.col + 1;
            leftMove.row = origin.row + 1;

            // Move right
            rightMove.col = origin.col - 1;
            rightMove.row = origin.row + 1;

            // Capture left
            leftCapture.col = origin.col + 2;
            leftCapture.row = origin.row + 2;

            // Capture right
            rightCapture.col = origin.col - 2;
            rightCapture.row = origin.row + 2;

            break;
        }
        case s_oKingPiece:
        case s_xKingPiece:
        {
            // Move left
            leftMove.col = origin.col - 1;
            leftMove.row = origin.row - 1;

            // Move backwards left
            left2Move.col = origin.col + 1;
            left2Move.row = origin.row + 1;

            // Move right
            rightMove.col = origin.col + 1;
            rightMove.row = origin.row - 1;

            // Move backwards right
            right2Move.col = origin.col - 1;
            right2Move.row = origin.row + 1;

            // Capture left
            leftCapture.col = origin.col - 2;
            leftCapture.row = origin.row - 2;

            left2Capture.col = origin.col + 2;
            left2Capture.row = origin.row + 2;

            // Capture right
            rightCapture.col = origin.col + 2;
            rightCapture.row = origin.row - 2;

            right2Capture.col = origin.col - 2;
            right2Capture.row = origin.row + 2;

            break;
        }
    }

    if (leftMove.IsValid() && ValidateCoordinates(leftMove) && CanMove(origin, leftMove))
        moves.push_back(move(leftMove));

    if (rightMove.IsValid() && ValidateCoordinates(rightMove) && CanMove(origin, rightMove))
        moves.push_back(move(rightMove));

    if (leftCapture.IsValid() && ValidateCoordinates(leftCapture) && CanMove(origin, leftCapture))
        moves.push_back(move(leftCapture));

    if (rightCapture.IsValid() && ValidateCoordinates(rightCapture) && CanMove(origin, rightCapture))
        moves.push_back(move(rightCapture));

    if (left2Move.IsValid() && ValidateCoordinates(left2Move) && CanMove(origin, left2Move))
        moves.push_back(move(left2Move));

    if (right2Move.IsValid() && ValidateCoordinates(right2Move) && CanMove(origin, right2Move))
        moves.push_back(move(right2Move));

    if (left2Capture.IsValid() && ValidateCoordinates(left2Capture) && CanMove(origin, left2Capture))
        moves.push_back(move(left2Capture));

    if (right2Capture.IsValid() && ValidateCoordinates(right2Capture) && CanMove(origin, right2Capture))
        moves.push_back(move(right2Capture));

    return moves;
}
