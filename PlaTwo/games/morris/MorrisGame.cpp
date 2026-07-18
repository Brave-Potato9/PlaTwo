#include "MorrisGame.h"
#include <QDebug>

//constructor
MorrisGame::MorrisGame(QObject* parent)
    : Game(parent)
    , waitingForRemoval(false)
{
    gameType = "Morris";
}

//main_methods
void MorrisGame::initialize(const GameConfig& config)
{
    board.reset();
    scores[0] = 0;
    scores[1] = 0;
    moveHistory.clear();
    isGameOver = false;
    winner = "";
    state = State::Idle;
    currentPlayerIndex = 0;
    waitingForRemoval = false;
    removablePieces.clear();
}

bool MorrisGame::makeMove(const Move& move)
{
    if (isGameOver)
    {
        emit invalidMoveAttempted(move, "Game is already over!");
        return false;
    }

    if (waitingForRemoval)
    {
        return handleRemoval(move);
    }

    if (move.isPlacementMove())
    {
        return handlePlacement(move);
    }
    else
    {
        return handleMovement(move);
    }
}

bool MorrisGame::isValidMove(const Move& move) const
{
    if (isGameOver)
    {
        return false;
    }

    if (waitingForRemoval)
    {
        if (move.getMoveType() != Move::Type::Removal)
        {
            return false;
        }

        int pos = move.getTo();
        if (!removablePieces.contains(pos))
        {
            return false;
        }

        return true;
    }

    int player = currentPlayerIndex;

    if (move.isPlacementMove())
    {
        int pos = move.getTo();
        if (board.getPhase() != MorrisBoard::Phase::Placement)
        {
            return false;
        }

        if (board.getCellState(pos) != MorrisBoard::State::Empty)
        {
            return false;
        }

        if (board.getPlacedCount(player) >= 9)
        {
            return false;
        }

        return true;
    }
    else
    {
        int from = move.getFrom();
        int to = move.getTo();
        MorrisBoard::State playerState = (player == 0) ? MorrisBoard::State::Player1 : MorrisBoard::State::Player2;
        if (board.getCellState(from) != playerState)
        {
            return false;
        }

        if (board.getCellState(to) != MorrisBoard::State::Empty)
        {
            return false;
        }

        if (board.getPhase() == MorrisBoard::Phase::Movement)
        {
            return board.isAdjacent(from, to);
        }
        else if (board.getPhase() == MorrisBoard::Phase::Flying)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

QList<Move> MorrisGame::getPossibleMoves() const
{
    return generatePossibleMoves();
}

bool MorrisGame::checkGameOver()
{
    if (waitingForRemoval)
    {
        return false;
    }

    int p0 = board.getPiecesCount(0);
    int p1 = board.getPiecesCount(1);


    if (p0 < 3 || p1 < 3)
    {
        isGameOver = true;
        winner = getWinner();
        setState(State::GameOver);
        emit gameLogicOver(winner);
        return true;
    }

    if (!board.hasValidMoves(currentPlayerIndex))
    {
        isGameOver = true;
        winner = (currentPlayerIndex == 0) ? "Player 2" : "Player 1";
        setState(State::GameOver);
        emit gameLogicOver(winner);
        return true;
    }

    return false;
}

QString MorrisGame::getWinner() const
{
    if (!isGameOver)
    {
        return "";
    }

    if (!winner.isEmpty())
    {
        return winner;
    }

    int p0 = board.getPiecesCount(0);
    int p1 = board.getPiecesCount(1);

    if (p0 < 3 && p1 < 3)
    {
        return "Draw";
    }

    if (p0 < 3)
    {
        return "Player 2";
    }

    if (p1 < 3)
    {
        return "Player 1";
    }

    return "Unknown";
}

//supplementry_methods
QJsonObject MorrisGame::getBoardState() const
{
    return board.toJson();
}

void MorrisGame::resetGame()
{
    board.reset();
    scores[0] = 0;
    scores[1] = 0;
    moveHistory.clear();
    isGameOver = false;
    winner = "";
    state = State::Idle;
    currentPlayerIndex = 0;
    waitingForRemoval = false;
    removablePieces.clear();
    emit gameReset();
}

QString MorrisGame::getGameType() const
{
    return "Morris";
}

//specialized_method
MorrisBoard* MorrisGame::getBoard()
{
    return &board;
}

const MorrisBoard* MorrisGame::getBoard() const
{
    return &board;
}

//helper_methods
void MorrisGame::onGameStart()
{
    board.reset();
    scores[0] = 0;
    scores[1] = 0;
    isGameOver = false;
    winner = "";
    currentPlayerIndex = 0;
    waitingForRemoval = false;
    removablePieces.clear();
}

void MorrisGame::onGameEnd(const QString& winner)
{
    this->winner = winner;
}

bool MorrisGame::handlePlacement(const Move& move)
{
    int pos = move.getTo();
    int player = currentPlayerIndex;

    if (board.getPhase() != MorrisBoard::Phase::Placement)
    {
        emit invalidMoveAttempted(move, "Not in placement phase!");
        return false;
    }

    if (board.getCellState(pos) != MorrisBoard::State::Empty)
    {
        emit invalidMoveAttempted(move, "Position is not empty!");
        return false;
    }

    if (board.getPlacedCount(player) >= 9)
    {
        emit invalidMoveAttempted(move, "All pieces already placed!");
        return false;
    }

    if (!board.placePiece(pos, player))
    {
        emit invalidMoveAttempted(move, "Failed to place piece!");
        return false;
    }

    Move recorded = move;
    recorded.setMoveNumber(moveHistory.size() + 1);
    recorded.setIsValidMove(true);
    addMoveToHistory(recorded);

    checkAndHandleMill(pos, player);

    if (!waitingForRemoval)
    {
        switchPlayer();
    }

    emit boardUpdated();
    emit boardStateChanged(getBoardState());

    if (checkGameOver())
    {
        return true;
    }

    return true;
}

bool MorrisGame::handleMovement(const Move& move)
{
    int from = move.getFrom();
    int to = move.getTo();
    int player = currentPlayerIndex;

    MorrisBoard::State playerState = (player == 0) ? MorrisBoard::State::Player1 : MorrisBoard::State::Player2;
    if (board.getCellState(from) != playerState)
    {
        emit invalidMoveAttempted(move, "Not your piece!");
        return false;
    }

    if (board.getCellState(to) != MorrisBoard::State::Empty)
    {
        emit invalidMoveAttempted(move, "Target is not empty!");
        return false;
    }

    if (board.getPhase() == MorrisBoard::Phase::Movement && !board.isAdjacent(from, to))
    {
        emit invalidMoveAttempted(move, "Not adjacent!");
        return false;
    }

    if (!board.movePiece(from, to, player))
    {
        emit invalidMoveAttempted(move, "Failed to move piece!");
        return false;
    }

    Move recorded = move;
    recorded.setMoveNumber(moveHistory.size() + 1);
    recorded.setIsValidMove(true);
    addMoveToHistory(recorded);

    checkAndHandleMill(to, player);

    if (!waitingForRemoval)
    {
        switchPlayer();
    }

    emit boardUpdated();
    emit boardStateChanged(getBoardState());

    if (checkGameOver())
    {
        return true;
    }

    return true;
}

bool MorrisGame::handleRemoval(const Move& move)
{
    if (!waitingForRemoval)
    {
        emit invalidMoveAttempted(move, "No removal needed!");
        return false;
    }

    if (move.getMoveType() != Move::Type::Removal)
    {
        emit invalidMoveAttempted(move, "Removal move required!");
        return false;
    }

    int pos = move.getTo();
    if (!removablePieces.contains(pos))
    {
        emit invalidMoveAttempted(move, "Cannot remove this piece!");
        return false;
    }

    int player = currentPlayerIndex;
    if (!board.removePiece(pos, player))
    {
        emit invalidMoveAttempted(move, "Failed to remove piece!");
        return false;
    }

    Move recorded = move;
    recorded.setMoveNumber(moveHistory.size() + 1);
    recorded.setIsValidMove(true);
    addMoveToHistory(recorded);

    clearRemovalState();

    switchPlayer();

    emit boardUpdated();
    emit boardStateChanged(getBoardState());

    if (checkGameOver())
    {
        return true;
    }

    return true;
}

void MorrisGame::checkAndHandleMill(int pos, int player)
{
    QList<int> millPositions;
    if (board.isMillFormed(pos, player, millPositions))
    {
        waitingForRemoval = true;
        removablePieces = board.getRemovablePieces(player);

        if (removablePieces.isEmpty())
        {
            waitingForRemoval = false;
        }
    }
}

QList<Move> MorrisGame::generatePossibleMoves() const
{
    QList<Move> moves;
    if (isGameOver)
    {
        return moves;
    }

    int player = currentPlayerIndex;
    QString playerName = QString("Player %1").arg(player + 1);

    if (waitingForRemoval)
    {
        for (int pos : removablePieces)
        {
            Move m("", Move::Type::Removal);
            m.setData("to", pos);
            m.setPlayerUsername(playerName);
            m.setDescription(QString("Remove piece at %1").arg(pos));
            moves.append(m);
        }

        return moves;
    }

    if (board.getPhase() == MorrisBoard::Phase::Placement)
    {
        QList<int> validPos = board.getValidPlacements(player);
        for (int pos : validPos)
        {
            Move m("", -1, pos);
            m.setMoveType(Move::Type::Placement);
            m.setPlayerUsername(playerName);
            m.setDescription(QString("Place at %1").arg(pos));
            moves.append(m);
        }
    }
    else
    {
        for (int from = 0; from < 24; ++from)
        {
            MorrisBoard::State state = board.getCellState(from);
            if (state == (player == 0 ? MorrisBoard::State::Player1 : MorrisBoard::State::Player2))
            {
                QList<int> toPositions = board.getValidMoves(from);
                for (int to : toPositions)
                {
                    Move m("", from, to);
                    m.setMoveType(Move::Type::Movement);
                    m.setPlayerUsername(playerName);
                    m.setDescription(QString("Move from %1 to %2").arg(from).arg(to));
                    moves.append(m);
                }
            }
        }
    }

    return moves;
}

void MorrisGame::clearRemovalState()
{
    waitingForRemoval = false;
    removablePieces.clear();
}