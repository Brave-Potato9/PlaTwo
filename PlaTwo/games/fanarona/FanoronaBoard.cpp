#include "FanoronaBoard.h"
#include <QDebug>

//static_datas
const int FanoronaBoard::rows = 5;
const int FanoronaBoard::column = 9;
const QList<QPoint> FanoronaBoard::direction = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},
    {1, -1},  {1, 0},  {1, 1}
};

//constructors
FanoronaBoard::FanoronaBoard()
    : board(rows, QVector<State>(column, State::Empty))
{
    reset();
}

//helper_methods
bool FanoronaBoard::isValidPosition(int _row, int _column) const
{
    return (_row >= 0 && _row < rows && _column >= 0 && _column < column);
}

bool FanoronaBoard::isEmpty(int _row, int _column) const
{
    if (!isValidPosition(_row, _column))
    {
        return false;
    }

    return board[_row][_column] == State::Empty;
}

bool FanoronaBoard::isPlayerPiece(int _row, int _column, int player) const
{
    if (!isValidPosition(_row, _column))
    {
        return false;
    }

    State target = (player == 0) ? State::Player1 : State::Player2;

    return board[_row][_column] == target;
}

bool FanoronaBoard::isOpponentPiece(int _row, int _column, int player) const
{
    if (!isValidPosition(_row, _column))
    {
        return false;
    }

    int opponent = 1 - player;
    State target = (opponent == 0) ? State::Player1 : State::Player2;

    return board[_row][_column] == target;
}

bool FanoronaBoard::canCapture(int _row,
                               int _column,
                               int player,
                               const QPoint& _direction) const
{
    int opponent = 1 - player;
    State opponentState = (opponent == 0) ? State::Player1 : State::Player2;

    int r = _row + _direction.x();
    int c = _column + _direction.y();

    if (!isValidPosition(r, c))
    {
        return false;
    }

    if (board[r][c] != opponentState)
    {
        return false;
    }


    while (isValidPosition(r, c))
    {
        if (board[r][c] == opponentState)
        {
            r += _direction.x();
            c += _direction.y();
        }
        else if (board[r][c] == State::Empty)
        {

            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

QList<QPair<int,int>> FanoronaBoard::getCapturesInDirection(int _row,
                                                            int _column,
                                                            int player,
                                                            const QPoint& _direction) const
{
    QList<QPair<int,int>> captures;
    int opponent = 1 - player;
    State opponentState = (opponent == 0) ? State::Player1 : State::Player2;

    int r = _row + _direction.x();
    int c = _column + _direction.y();

    if (!isValidPosition(r, c))
    {
        return captures;
    }

    if (board[r][c] != opponentState)
    {
        return captures;
    }

    while (isValidPosition(r, c) && board[r][c] == opponentState)
    {
        captures.append({r, c});
        r += _direction.x();
        c += _direction.y();
    }

    if (isValidPosition(r, c) && board[r][c] != State::Empty)
    {
        captures.clear();
    }

    return captures;
}

void FanoronaBoard::clearBoard()
{
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < column; ++c)
        {
            board[r][c] = State::Empty;
        }
    }
}

//reset
void FanoronaBoard::reset()
{
    clearBoard();

    for (int c = 0; c < column; ++c)
    {
        board[0][c] = State::Player1;
        board[1][c] = State::Player1;
        board[3][c] = State::Player2;
        board[4][c] = State::Player2;

        if(c == 0 || c == 2 || c == 5 || c == 7)
        {
            board[2][c] = State::Player1;
        }
        else
        {
            board[2][c] = State::Player2;
        }
    }
}

//board_management
bool FanoronaBoard::movePiece(int fromRow, int fromCol,
                              int toRow, int toCol, int player)
{
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol))
    {
        return false;
    }

    if ((fromRow + fromCol) % 2 == 1 && (toRow + toCol) % 2 == 1)
    {
        return false;
    }

    State playerState = (player == 0) ? State::Player1 : State::Player2;

    if (board[fromRow][fromCol] != playerState)
    {
        return false;
    }

    if (board[toRow][toCol] != State::Empty)
    {
        return false;
    }

    int dr = toRow - fromRow;
    int dc = toCol - fromCol;
    QPoint dir(dr, dc);

    if (!direction.contains(dir))
    {
        return false;
    }

    board[fromRow][fromCol] = State::Empty;
    board[toRow][toCol] = playerState;
    return true;
}

bool FanoronaBoard::applyCapture(int _row, int _column, int player,
                                 const QPoint& direction)
{
    if (!isValidPosition(_row, _column))
    {
        return false;
    }

    if (!isPlayerPiece(_row, _column, player))
    {
        return false;
    }

    QList<QPair<int,int>> captures = getCapturesInDirection(_row, _column,
                                                            player,
                                                            direction);
    if (captures.isEmpty())
    {
        return false;
    }

    for (auto& pos : captures)
    {
        int r = pos.first;
        int c = pos.second;
        board[r][c] = State::Empty;
    }

    return true;
}

bool FanoronaBoard::removePiece(int _row, int _column, int player)
{
    if (!isValidPosition(_row, _column))
    {
        return false;
    }

    int opponent = 1 - player;
    State opponentState = (opponent == 0) ? State::Player1 : State::Player2;

    if (board[_row][_column] != opponentState)
    {
        return false;
    }

    board[_row][_column] = State::Empty;
    return true;
}

//state_management
FanoronaBoard::State FanoronaBoard::getCellState(int _row, int _column) const
{
    if (!isValidPosition(_row, _column))
    {
        return State::Empty;
    }

    return board[_row][_column];
}

int FanoronaBoard::getPiecesCount(int player) const
{
    int count = 0;
    State target = (player == 0) ? State::Player1 : State::Player2;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < column; ++c)
        {
            if (board[r][c] == target)
            {
                count++;
            }
        }
    }

    return count;
}

bool FanoronaBoard::isInBounds(int row, int col) const
{
    return isValidPosition(row, col);
}

int FanoronaBoard::getRows() const
{
    return rows;
}

int FanoronaBoard::getColumns() const
{
    return column;
}

//capture_management
QList<QPair<int,int>> FanoronaBoard::getCaptures(int _row, int _column, int player) const
{
    QList<QPair<int,int>> allCaptures;
    if (!isValidPosition(_row, _column))
    {
        return allCaptures;
    }

    if (!isPlayerPiece(_row, _column, player))
    {
        return allCaptures;
    }

    for (const QPoint& dir : direction)
    {
        QList<QPair<int,int>> captures = getCapturesInDirection(_row, _column, player, dir);
        allCaptures.append(captures);
    }

    return allCaptures;
}

QList<QPair<int,int>> FanoronaBoard::getCaptureChain(int _row,
                                                    int _column,
                                                    int player,
                                                    const QPoint& direction) const
{
    return getCapturesInDirection(_row, _column, player, direction);
}

bool FanoronaBoard::hasValidMoves(int player) const
{
    State playerState = (player == 0) ? State::Player1 : State::Player2;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < column; ++c)
        {
            if (board[r][c] == playerState)
            {
                for (const QPoint& dir : direction)
                {
                    int nr = r + dir.x();
                    int nc = c + dir.y();

                    if (isValidPosition(nr, nc) && board[nr][nc] == State::Empty)
                    {
                        return true;
                    }
                }

                if (!getCaptures(r, c, player).isEmpty())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//working_with_JSON
QJsonObject FanoronaBoard::toJson() const
{
    QJsonObject obj;
    QJsonArray rowsArray;
    for (int r = 0; r < rows; ++r)
    {
        QJsonArray colsArray;
        for (int c = 0; c < column; ++c)
        {
            int val = 0;
            if (board[r][c] == State::Player1)
            {
                val = 1;
            }
            else if (board[r][c] == State::Player2)
            {
                val = 2;
            }

            colsArray.append(val);
        }

        rowsArray.append(colsArray);
    }

    obj["board"] = rowsArray;
    return obj;
}

FanoronaBoard FanoronaBoard::fromJson(const QJsonObject& obj)
{
    FanoronaBoard board;
    QJsonArray rowsArray = obj["board"].toArray();
    for (int r = 0; r < rowsArray.size() && r < rows; ++r)
    {
        QJsonArray colsArray = rowsArray[r].toArray();
        for (int c = 0; c < colsArray.size() && c < column; ++c)
        {
            int val = colsArray[c].toInt();

            if (val == 1)
            {
                board.board[r][c] = State::Player1;
            }
            else if (val == 2)
            {
                board.board[r][c] = State::Player2;
            }
            else
            {
                board.board[r][c] = State::Empty;
            }
        }
    }

    return board;
}