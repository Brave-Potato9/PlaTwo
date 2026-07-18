#include "MorrisBoard.h"

//static
QVector<QList<int>> MorrisBoard::adjacency;
QVector<QList<int>> MorrisBoard::mills;
bool MorrisBoard::staticDataInitialized = false;

//constructors
MorrisBoard::MorrisBoard()
    : positions(PositionsNumber, State::Empty)
    , placedPieces{0, 0}
    , phase(Phase::Placement)
{
    initializeStaticData();
    reset();
}

//helper_method
void MorrisBoard::initializeStaticData()
{
    if (staticDataInitialized)
    {
        return;
    }

    adjacency.resize(PositionsNumber);
    mills.clear();

    auto addRingConnections = [](int start)
    {
        for (int i = 0; i < 8; ++i)
        {
            int next = (i + 1) % 8;
            adjacency[start + i].append(start + next);
            adjacency[start + next].append(start + i);
        }
    };

    addRingConnections(0);
    addRingConnections(8);
    addRingConnections(16);


    for (int i = 0; i < 8; ++i)
    {
        int outer = i;
        int middle = 8 + i;
        int inner = 16 + i;
        adjacency[outer].append(middle);
        adjacency[middle].append(outer);
        adjacency[middle].append(inner);
        adjacency[inner].append(middle);
    }

    auto addMill = [](int a, int b, int c)
    {
        QList<int> mill = {a, b, c};
        mills.append(mill);
    };


    addMill(0, 1, 2);
    addMill(2, 3, 4);
    addMill(4, 5, 6);
    addMill(6, 7, 0);

    addMill(8, 9, 10);
    addMill(10, 11, 12);
    addMill(12, 13, 14);
    addMill(14, 15, 8);


    addMill(16, 17, 18);
    addMill(18, 19, 20);
    addMill(20, 21, 22);
    addMill(22, 23, 16);

    for (int i = 0; i < 8; ++i)
    {
        int outer = i;
        int middle = 8 + i;
        int inner = 16 + i;
        addMill(outer, middle, inner);
    }

    staticDataInitialized = true;
}

bool MorrisBoard::isValidPosition(int pos) const
{
    return (pos >= 0 && pos < PositionsNumber);
}

bool MorrisBoard::isAdjacent(int from, int to) const
{
    if (!isValidPosition(from) || !isValidPosition(to))
    {
        return false;
    }

    return adjacency[from].contains(to);
}

bool MorrisBoard::isInMill(int pos, int player, QList<int>& millPositions) const
{
    State playerState = (player == 0) ? State::Player1 : State::Player2;
    if (positions[pos] != playerState)
    {
        return false;
    }

    for (const auto& mill : mills)
    {
        if (mill.contains(pos))
        {
            bool allSame = true;
            for (int p : mill)
            {
                if (positions[p] != playerState)
                {
                    allSame = false;
                    break;
                }
            }

            if (allSame)
            {
                millPositions = mill;
                return true;
            }
        }
    }
    return false;
}

void MorrisBoard::updatePhase()
{
    if (phase == Phase::Placement)
    {
        if (placedPieces[0] >= maxPieces && placedPieces[1] >= maxPieces)
        {
            phase = Phase::Movement;
        }
    }
    else if (phase == Phase::Movement)
    {
        if (getPiecesCount(0) == 3 || getPiecesCount(1) == 3)
        {
            phase = Phase::Flying;
        }
    }
}

//reset
void MorrisBoard::reset()
{
    for (int i = 0; i < PositionsNumber; ++i)
    {
        positions[i] = State::Empty;
    }

    placedPieces[0] = 0;
    placedPieces[1] = 0;
    phase = Phase::Placement;
}

//board_main_management
bool MorrisBoard::placePiece(int pos, int player)
{
    if (!isValidPosition(pos))
    {
        return false;
    }

    if (positions[pos] != State::Empty)
    {
        return false;
    }

    if (placedPieces[player] >= maxPieces)
    {
        return false;
    }

    if (phase != Phase::Placement)
    {
        return false;
    }

    positions[pos] = (player == 0) ? State::Player1 : State::Player2;
    placedPieces[player]++;
    updatePhase();
    return true;
}

bool MorrisBoard::movePiece(int from, int to, int player)
{
    if (!isValidPosition(from) || !isValidPosition(to))
    {
        return false;
    }

    State playerState = (player == 0) ? State::Player1 : State::Player2;
    if (positions[from] != playerState)
    {
        return false;
    }

    if (positions[to] != State::Empty)
    {
        return false;
    }

    if (phase == Phase::Movement)
    {
        if (!isAdjacent(from, to))
        {
            return false;
        }
    }
    else if (phase == Phase::Flying)
    {

    }
    else
    {
        return false;
    }

    positions[from] = State::Empty;
    positions[to] = playerState;
    updatePhase();
    return true;
}

bool MorrisBoard::removePiece(int pos, int player)
{
    if (!isValidPosition(pos))
    {
        return false;
    }

    int opponent = 1 - player;
    State opponentState = (opponent == 0) ? State::Player1 : State::Player2;
    if (positions[pos] != opponentState)
    {
        return false;
    }

    QList<int> millPositions;
    bool inMill = isInMill(pos, opponent, millPositions);
    if (inMill)
    {
        int totalOpponent = getPiecesCount(opponent);
        int inMillCount = 0;
        for (int i = 0; i < PositionsNumber; ++i)
        {
            if (positions[i] == opponentState && isMill(i, opponent))
            {
                inMillCount++;
            }
        }
        if (inMillCount < totalOpponent)
        {
            return false;
        }
    }

    positions[pos] = State::Empty;
    updatePhase();
    return true;
}

bool MorrisBoard::isMill(int pos, int player) const
{
    QList<int> dummy;
    return isInMill(pos, player, dummy);
}

bool MorrisBoard::isMillFormed(int pos, int player, QList<int>& millPositions) const
{
    return isInMill(pos, player, millPositions);
}

//state_management
MorrisBoard::State MorrisBoard::getCellState(int pos) const
{
    if (!isValidPosition(pos))
    {
        return State::Empty;
    }

    return positions[pos];
}

int MorrisBoard::getPiecesCount(int player) const
{
    int count = 0;
    State target = (player == 0) ? State::Player1 : State::Player2;
    for (int i = 0; i < PositionsNumber; ++i)
    {
        if (positions[i] == target)
        {
            count++;
        }
    }

    return count;
}

bool MorrisBoard::isBoardFull() const
{
    return placedPieces[0] >= maxPieces && placedPieces[1] >= maxPieces;
}

//valiedation_management
QList<int> MorrisBoard::getValidMoves(int fromPos) const
{
    QList<int> valid;
    if (!isValidPosition(fromPos))
    {
        return valid;
    }

    State state = positions[fromPos];
    if (state == State::Empty)
    {
        return valid;
    }

    int player = (state == State::Player1) ? 0 : 1;

    if (phase == Phase::Movement)
    {
        for (int neighbor : adjacency[fromPos])
        {
            if (positions[neighbor] == State::Empty)
            {
                valid.append(neighbor);
            }
        }
    }
    else if (phase == Phase::Flying)
    {
        for (int i = 0; i < PositionsNumber; ++i)
        {
            if (i != fromPos && positions[i] == State::Empty)
            {
                valid.append(i);
            }
        }
    }
    return valid;
}

QList<int> MorrisBoard::getValidPlacements(int player) const
{
    QList<int> valid;
    if (phase != Phase::Placement)
    {
        return valid;
    }

    if (placedPieces[player] >= maxPieces)
    {
        return valid;
    }

    for (int i = 0; i < PositionsNumber; ++i)
    {
        if (positions[i] == State::Empty)
        {
            valid.append(i);
        }
    }

    return valid;
}

QList<int> MorrisBoard::getRemovablePieces(int player) const
{
    QList<int> removable;
    int opponent = 1 - player;
    State opponentState = (opponent == 0) ? State::Player1 : State::Player2;

    for (int i = 0; i < PositionsNumber; ++i)
    {
        if (positions[i] == opponentState && !isMill(i, opponent))
        {
            removable.append(i);
        }
    }

    if (removable.isEmpty())
    {
        for (int i = 0; i < PositionsNumber; ++i)
        {
            if (positions[i] == opponentState)
            {
                removable.append(i);
            }
        }
    }

    return removable;
}

bool MorrisBoard::hasValidMoves(int player) const
{
    if (phase == Phase::Placement)
    {
        return !getValidPlacements(player).isEmpty();
    }
    else
    {
        State playerState = (player == 0) ? State::Player1 : State::Player2;
        for (int i = 0; i < PositionsNumber; ++i)
        {
            if (positions[i] == playerState)
            {
                if (!getValidMoves(i).isEmpty())
                {
                    return true;
                }
            }
        }

        return false;
    }
}

//working_with_JSON
QJsonObject MorrisBoard::toJson() const
{
    QJsonObject obj;
    QJsonArray posArray;
    for (int i = 0; i < PositionsNumber; ++i)
    {
        int val = 0;
        if (positions[i] == State::Player1)
        {
            val = 1;
        }
        else if (positions[i] == State::Player2)
        {
            val = 2;
        }

        posArray.append(val);
    }

    obj["positions"] = posArray;
    obj["placedP1"] = placedPieces[0];
    obj["placedP2"] = placedPieces[1];
    obj["phase"] = static_cast<int>(phase);
    return obj;
}

MorrisBoard MorrisBoard::fromJson(const QJsonObject& obj)
{
    MorrisBoard board;
    QJsonArray posArray = obj["positions"].toArray();
    for (int i = 0; i < posArray.size() && i < PositionsNumber; ++i)
    {
        int val = posArray[i].toInt();
        if (val == 1)
        {
            board.positions[i] = State::Player1;
        }
        else if (val == 2)
        {
            board.positions[i] = State::Player2;
        }
        else
        {
            board.positions[i] = State::Empty;
        }
    }

    board.placedPieces[0] = obj["placedP1"].toInt(0);
    board.placedPieces[1] = obj["placedP2"].toInt(0);
    board.phase = static_cast<Phase>(obj["phase"].toInt(0));
    board.updatePhase();
    return board;
}