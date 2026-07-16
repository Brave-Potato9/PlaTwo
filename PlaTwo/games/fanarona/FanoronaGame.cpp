#include "FanoronaGame.h"

//constructor
FanoronaGame::FanoronaGame(QObject* parent)
    : Game(parent)
    , waitingForCapture(false)
    , currentCapturePlayer(-1)
{
    gameType = "Fanorona";
}

//main_methods
void FanoronaGame::initialize(const GameConfig& config)
{
    board.reset();
    scores[0] = 0;
    scores[1] = 0;
    moveHistory.clear();
    isGameOver = false;
    winner = "";
    state = State::Idle;
    currentPlayerIndex = 0;
    waitingForCapture = false;
    captureDirections.clear();
    currentCapturePlayer = -1;
}

bool FanoronaGame::makeMove(const Move& move)
{
    if (isGameOver)
    {
        emit invalidMoveAttempted(move, "Game is already over!");
        return false;
    }

    if (waitingForCapture)
    {
        return handleCapture(move);
    }
    else
    {
        return handleMovement(move);
    }
}

bool FanoronaGame::isValidMove(const Move& move) const
{
    if (isGameOver)
    {
        return false;
    }

    if (waitingForCapture)
    {
        if (move.getMoveType() != Move::Type::Capture)
        {
            return false;
        }

        int row = move.getData("captureRow").toInt();
        int col = move.getData("captureCol").toInt();
        int dx = move.getData("directionX").toInt();
        int dy = move.getData("directionY").toInt();
        QPoint dir(dx, dy);

        if (row != lastMoveTo.x() || col != lastMoveTo.y())
        {
            return false;
        }

        return captureDirections.contains(dir);
    }

    int player = currentPlayerIndex;
    QPoint from = move.getFromPoint();
    QPoint to = move.getToPoint();

    if (!board.isInBounds(from.x(), from.y()) || !board.isInBounds(to.x(), to.y()))
    {
        return false;
    }

    if (!board.isPlayerPiece(from.x(), from.y(), player))
    {
        return false;
    }

    if (!board.isEmpty(to.x(), to.y()))
    {
        return false;
    }

    int dr = to.x() - from.x();
    int dc = to.y() - from.y();
    QPoint dir(dr, dc);

    if (!FanoronaBoard::direction.contains(dir))
    {
        return false;
    }

    if ((from.x() + from.y()) % 2 == 1 && (to.x() + to.y()) % 2 == 1)
    {
        return false;
    }

    return true;
}

QList<Move> FanoronaGame::getPossibleMoves() const
{
    return generatePossibleMoves();
}

bool FanoronaGame::checkGameOver()
{
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

QString FanoronaGame::getWinner() const
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
    if (p0 == 0 && p1 == 0)
    {
        return "Draw";
    }
    if (p0 == 0)
    {
        return "Player 2";
    }
    if (p1 == 0)
    {
        return "Player 1";
    }

    return "Unknown";
}

//supplementry_methods
QJsonObject FanoronaGame::getBoardState() const
{
    return board.toJson();
}

void FanoronaGame::resetGame()
{
    board.reset();
    scores[0] = 0;
    scores[1] = 0;
    moveHistory.clear();
    isGameOver = false;
    winner = "";
    state = State::Idle;
    currentPlayerIndex = 0;
    waitingForCapture = false;
    captureDirections.clear();
    currentCapturePlayer = -1;
    emit gameReset();
}

QString FanoronaGame::getGameType() const
{
    return "Fanorona";
}

//specialized_method
FanoronaBoard* FanoronaGame::getBoard()
{
    return &board;
}

const FanoronaBoard* FanoronaGame::getBoard() const
{
    return &board;
}

//helper_methods
void FanoronaGame::onGameStart()
{
    board.reset();
    scores[0] = 0;
    scores[1] = 0;
    isGameOver = false;
    winner = "";
    currentPlayerIndex = 0;
    waitingForCapture = false;
    captureDirections.clear();
    currentCapturePlayer = -1;
}

void FanoronaGame::onGameEnd(const QString& winner)
{
    this->winner = winner;
}

bool FanoronaGame::handleMovement(const Move& move)
{
    int player = currentPlayerIndex;
    QPoint from = move.getFromPoint();
    QPoint to = move.getToPoint();

    if (!board.isInBounds(from.x(), from.y()) ||
        !board.isInBounds(to.x(), to.y()))
    {
        emit invalidMoveAttempted(move, "Out of bounds!");
        return false;
    }

    if (!board.isPlayerPiece(from.x(), from.y(), player))
    {
        emit invalidMoveAttempted(move, "Not your piece!");
        return false;
    }

    if (!board.isEmpty(to.x(), to.y()))
    {
        emit invalidMoveAttempted(move, "Target is not empty!");
        return false;
    }

    if (!board.movePiece(from.x(), from.y(), to.x(), to.y(), player))
    {
        emit invalidMoveAttempted(move, "Invalid movement!");
        return false;
    }

    Move recorded = move;
    recorded.setMoveNumber(moveHistory.size() + 1);
    recorded.setIsValidMove(true);
    addMoveToHistory(recorded);

    lastMoveFrom = from;
    lastMoveTo = to;

    checkAndApplyCaptures(recorded);

    if (!waitingForCapture)
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

bool FanoronaGame::handleCapture(const Move& move)
{
    if (!waitingForCapture)
    {
        emit invalidMoveAttempted(move, "No capture needed!");
        return false;
    }

    if (move.getMoveType() != Move::Type::Capture)
    {
        emit invalidMoveAttempted(move, "Capture move required!");
        return false;
    }

    int row = move.getData("captureRow").toInt();
    int col = move.getData("captureCol").toInt();
    int dx = move.getData("directionX").toInt();
    int dy = move.getData("directionY").toInt();
    QPoint dir(dx, dy);

    if (!captureDirections.contains(dir))
    {
        emit invalidMoveAttempted(move, "Invalid capture direction!");
        return false;
    }

    if (!board.applyCapture(row, col, currentCapturePlayer, dir))
    {
        emit invalidMoveAttempted(move, "Failed to apply capture!");
        return false;
    }

    Move recorded = move;
    recorded.setMoveNumber(moveHistory.size() + 1);
    recorded.setIsValidMove(true);
    addMoveToHistory(recorded);

    clearCaptureState();

    switchPlayer();

    emit boardUpdated();
    emit boardStateChanged(getBoardState());

    if (checkGameOver())
    {
        return true;
    }

    return true;
}

void FanoronaGame::checkAndApplyCaptures(const Move& move)
{
    int player = currentPlayerIndex;
    QPoint to = move.getToPoint();
    int row = to.x();
    int col = to.y();

    QList<QPair<int,int>> captures = board.getCaptures(row, col, player);

    if (!captures.isEmpty())
    {
        waitingForCapture = true;
        currentCapturePlayer = player;

        captureDirections.clear();
        for (const QPoint& dir : FanoronaBoard::direction)
        {
            QList<QPair<int,int>> caps =
                board.getCapturesInDirection(row, col, player, dir);
            if (!caps.isEmpty())
            {
                captureDirections.append(dir);
            }
        }

        if (captureDirections.isEmpty())
        {
            waitingForCapture = false;
            currentCapturePlayer = -1;
        }
    }
}

void FanoronaGame::applyCaptures(int row, int col, int player,
                                 const QList<QPoint>& directions)
{
    for (const QPoint& dir : directions)
    {
        board.applyCapture(row, col, player, dir);
    }
}

QList<Move> FanoronaGame::generatePossibleMoves() const
{
    QList<Move> moves;
    if (isGameOver)
    {
        return moves;
    }

    int currentPlayer = currentPlayerIndex;

    if (waitingForCapture)
    {
        int row = lastMoveTo.x();
        int col = lastMoveTo.y();
        for (const QPoint& dir : captureDirections)
        {
            Move m("", QPoint(0,0), QPoint(0,0));

            QString playerName = QString("Player %1").arg(currentPlayer + 1);

            m.setMoveType(Move::Type::Capture);
            m.setData("captureRow", row);
            m.setData("captureCol", col);
            m.setData("directionX", dir.x());
            m.setData("directionY", dir.y());
            m.setPlayerUsername(playerName);
            m.setDescription(QString("Capture in direction (%1,%2)").arg(dir.x()).arg(dir.y()));
            moves.append(m);
        }

        return moves;
    }

    for (int r = 0; r < board.getRows(); ++r)
    {
        for (int c = 0; c < board.getColumns(); ++c)
        {
            if (board.isPlayerPiece(r, c, currentPlayer))
            {
                QPoint from(r, c);
                for (const QPoint& dir : FanoronaBoard::direction)
                {
                    int nr = r + dir.x();
                    int nc = c + dir.y();
                    if (board.isInBounds(nr, nc) && board.isEmpty(nr, nc))
                    {
                        if (!((r + c) % 2 == 1 && (nr + nc) % 2 == 1))
                        {
                            QPoint to(nr, nc);
                            Move m("", from, to);
                            QString playerName = QString("Player %1").arg(currentPlayer + 1);
                            m.setMoveType(Move::Type::Movement);
                            m.setPlayerUsername(playerName);
                            m.setDescription(QString("Move from (%1,%2) to (%3,%4)").arg(r).arg(c).arg(nr).arg(nc));
                            moves.append(m);
                        }
                    }
                }
            }
        }
    }

    return moves;
}

bool FanoronaGame::hasAnyValidMove(int player) const
{
    return board.hasValidMoves(player);
}

void FanoronaGame::clearCaptureState()
{
    waitingForCapture = false;
    captureDirections.clear();
    currentCapturePlayer = -1;
}