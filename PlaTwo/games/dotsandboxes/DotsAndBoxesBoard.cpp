#include "DotsAndBoxesBoard.h"
#include <QDebug>

//------------------------------------ constructor ------------------------------------
DotsAndBoxesBoard::DotsAndBoxesBoard(int _rows, int _columns)
    : rows(_rows)
    , columns(_columns)
{
    reset(_rows, _columns);
}

//------------------------------------ helper_methods ------------------------------------
bool DotsAndBoxesBoard::isValidLine(int row, int column, bool isHorizontal) const
{
    if (isHorizontal)
    {
        return (row >= 0 && row <= rows &&
                column >= 0 && column < columns);
    }
    else
    {
        return (row >= 0 && row < rows &&
                column >= 0 && column <= columns);
    }
}

void DotsAndBoxesBoard::markBoxOwner(int row, int column, int player)
{
    if (row < 0 || row >= rows || column < 0 || column >= columns)
    {
        return;
    }

    if (boxOwners[row][column] == State::Empty)
    {
        boxOwners[row][column] = ((player == 0) ? State::Player1 : State::Player2);
    }
}

DotsAndBoxesBoard::State DotsAndBoxesBoard::intToState(int state) const
{
    if (state == 0)
    {
        return State::Player1;
    }
    else if (state == 1)
    {
        return State::Player2;
    }
    else
    {
        return State::Empty;
    }
}

//------------------------------------ reset ------------------------------------
void DotsAndBoxesBoard::reset(int newRows, int newColumns)
{
    rows = newRows;
    columns = newColumns;

    horizontalLines.resize(rows + 1);
    for (int r = 0; r <= rows; ++r)
    {
        horizontalLines[r].resize(columns, false);
    }

    verticalLines.resize(rows);
    for (int r = 0; r < rows; ++r)
    {
        verticalLines[r].resize(columns + 1, false);
    }

    boxOwners.resize(rows);
    for (int r = 0; r < rows; ++r)
    {
        boxOwners[r].resize(columns, State::Empty);
    }
}

//------------------------------------ line_management_methods ------------------------------------
bool DotsAndBoxesBoard::drawLine(int row, int column, bool isHorizontal, int player)
{
    if (!isValidLine(row, column, isHorizontal))
    {
        return false;
    }

    if (isLineDrawn(row, column, isHorizontal))
    {
        return false;
    }

    if (player < 0 || player > 1)
    {
        return false;
    }

    if (isHorizontal)
    {
        horizontalLines[row][column] = true;
    }
    else
    {
        verticalLines[row][column] = true;
    }

    return true;
}

bool DotsAndBoxesBoard::isLineDrawn(int row, int column, bool isHorizontal) const
{
    if (!isValidLine(row, column, isHorizontal))
    {
        return false;
    }

    if (isHorizontal)
    {
        return horizontalLines[row][column];
    }
    else
    {
        return verticalLines[row][column];
    }
}

//------------------------------------ box_management_methods ------------------------------------
bool DotsAndBoxesBoard::isBoxComplete(int row, int column) const
{
    if (row < 0 || row >= rows ||
        column < 0 || column >= columns)
    {
        return false;
    }

    bool top = horizontalLines[row][column];
    bool bottom = horizontalLines[row + 1][column];
    bool left = verticalLines[row][column];
    bool right = verticalLines[row][column + 1];

    return top && bottom && left && right;
}

QList<QPair<int,int>> DotsAndBoxesBoard::getCompletedBoxes(
    int row, int column, bool isHorizontal, int player)
{
    QList<QPair<int,int>> completedBoxes;

    if (isHorizontal)
    {
        if (row > 0 && isBoxComplete(row - 1, column))
        {
            completedBoxes.append({row - 1, column});
            markBoxOwner(row - 1, column, player);
        }
        if (row < rows && isBoxComplete(row, column))
        {
            completedBoxes.append({row, column});
            markBoxOwner(row, column, player);
        }
    }
    else
    {
        if (column > 0 && isBoxComplete(row, column - 1))
        {
            completedBoxes.append({row, column - 1});
            markBoxOwner(row, column - 1, player);
        }
        if (column < columns && isBoxComplete(row, column))
        {
            completedBoxes.append({row, column});
            markBoxOwner(row, column, player);
        }
    }

    return completedBoxes;
}

DotsAndBoxesBoard::State DotsAndBoxesBoard::getBoxOwner(
    int row, int column) const
{
    if (row < 0 || row >= rows ||
        column < 0 || column >= columns)
    {
        return State::Empty;
    }

    return boxOwners[row][column];
}

int DotsAndBoxesBoard::getCompletedBoxesCount() const
{
    int count = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            if (boxOwners[r][c] != State::Empty) count++;
        }
    }

    return count;
}

int DotsAndBoxesBoard::getTotalBoxes() const
{
    return rows * columns;
}

bool DotsAndBoxesBoard::isBoardFull() const
{
    return (getCompletedBoxesCount() == rows * columns);
}

//------------------------------------ score_methods ------------------------------------
int DotsAndBoxesBoard::getScore(int player) const
{
    int count = 0;
    State target = intToState(player);
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            if (boxOwners[r][c] == target)
            {
                count++;
            }
        }
    }

    return count;
}

QVector<int> DotsAndBoxesBoard::getScores() const
{
    QVector<int> scores(2, 0);
    scores[0] = getScore(0);
    scores[1] = getScore(1);
    return scores;
}

//------------------------------------ Working_with_JSON ------------------------------------
QJsonObject DotsAndBoxesBoard::toJson() const
{
    QJsonObject jsonObj;
    jsonObj["rows"] = rows;
    jsonObj["columns"] = columns;

    //horizontal line
    QJsonArray hLines;
    for (int r = 0; r <= rows; ++r)
    {
        QJsonArray row;
        for (int c = 0; c < columns; ++c)
        {
            row.append(horizontalLines[r][c]);
        }
        hLines.append(row);
    }
    jsonObj["horizontalLines"] = hLines;

    //vertical line
    QJsonArray vLines;
    for (int r = 0; r < rows; ++r)
    {
        QJsonArray row;
        for (int c = 0; c <= columns; ++c)
        {
            row.append(verticalLines[r][c]);
        }
        vLines.append(row);
    }
    jsonObj["verticalLines"] = vLines;

    //boxes
    QJsonArray owners;
    for (int r = 0; r < rows; ++r)
    {
        QJsonArray row;
        for (int c = 0; c < columns; ++c)
        {
            if (boxOwners[r][c] == State::Player1)
            {
                row.append(0);
            }
            else if (boxOwners[r][c] == State::Player2)
            {
                row.append(1);
            }
            else
            {
                row.append(-1);
            }
        }
        owners.append(row);
    }
    jsonObj["boxOwners"] = owners;

    return jsonObj;
}

DotsAndBoxesBoard DotsAndBoxesBoard::fromJson(const QJsonObject& obj)
{
    int rows = obj["rows"].toInt(6);
    int columns = obj["columns"].toInt(6);

    DotsAndBoxesBoard board(rows, columns);

    //horizontal line
    QJsonArray hLines = obj["horizontalLines"].toArray();
    for (int r = 0; r < hLines.size() && r <= rows; ++r)
    {
        QJsonArray row = hLines[r].toArray();
        for (int c = 0; c < row.size() && c < columns; ++c)
        {
            board.horizontalLines[r][c] = row[c].toBool();
        }
    }

    //vertical line
    QJsonArray vLines = obj["verticalLines"].toArray();
    for (int r = 0; r < vLines.size() && r < rows; ++r)
    {
        QJsonArray row = vLines[r].toArray();
        for (int c = 0; c < row.size() && c <= columns; ++c)
        {
            board.verticalLines[r][c] = row[c].toBool();
        }
    }

    //boxes
    QJsonArray owners = obj["boxOwners"].toArray();
    for (int r = 0; r < owners.size() && r < rows; ++r)
    {
        QJsonArray row = owners[r].toArray();
        for (int c = 0; c < row.size() && c < columns; ++c)
        {
            int val = row[c].toInt(-1);
            board.boxOwners[r][c] = board.intToState(val);
        }
    }

    return board;
}

//------------------------------------ getters ------------------------------------
int DotsAndBoxesBoard::getRows() const
{
    return rows;
}

int DotsAndBoxesBoard::getColumns() const
{
    return columns;
}

bool DotsAndBoxesBoard::getHorizontalLine(int row, int column) const
{
    if (!isValidLine(row, column, true))
    {
        return false;
    }
    return horizontalLines[row][column];
}

bool DotsAndBoxesBoard::getVerticalLine(int row, int column) const
{
    if (!isValidLine(row, column, false))
    {
        return false;
    }
    return verticalLines[row][column];
}