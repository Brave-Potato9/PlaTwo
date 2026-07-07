#include "DotsAndBoxesGame.h"

//------------------------------------ constructor ------------------------------------
DotsAndBoxesGame::DotsAndBoxesGame(QObject* parent)
    : Game(parent)
    , board(6, 6)
    , totalBoxes(36)
    , completedBoxes(0)
{
    gameType = "DotsAndBoxes";
}

//------------------------------------ main_methods ------------------------------------
void DotsAndBoxesGame::initialize(const GameConfig& config)
{
    int rows = qMax(2, config.getDotsAndBoxesRows());
    int columns = qMax(2, config.getDotsAndBoxesColumns());

    board.reset(rows, columns);
    totalBoxes = board.getTotalBoxes();
    completedBoxes = 0;

    scores[0] = 0;
    scores[1] = 0;
    moveHistory.clear();
    isGameOver = false;
    winner = "";
    state = State::Idle;
    currentPlayerIndex = 0;
}

bool DotsAndBoxesGame::makeMove(const Move& move)
{
    if (isGameOver)
    {
        emit invalidMoveAttempted(move, "Game is already over!");
        return false;
    }

    int row = move.getRow();
    int column = move.getColumn();
    bool horizontal = move.isHorizontal();

    //draw line
    if (!board.drawLine(row, column, horizontal, currentPlayerIndex))
    {
        emit invalidMoveAttempted(move, "Failed to draw line!");
        return false;
    }

    //set in history
    Move recorded = move;
    recorded.setMoveNumber(moveHistory.size() + 1);
    recorded.setIsValidMove(true);
    addMoveToHistory(recorded);

    //check completed boxs
    handleCompletedBoxes(recorded);

    //update board
    emit boardUpdated();
    emit boardStateChanged(getBoardState());

    //check game ended
    if (checkGameOver())
    {
        return true;
    }

    return true;
}

bool DotsAndBoxesGame::isValidMove(const Move& move) const
{
    if (isGameOver)
    {
        return false;
    }

    int row = move.getRow();
    int column = move.getColumn();
    bool horizontal = move.isHorizontal();

    return board.isValidLine(row, column, horizontal) && !board.isLineDrawn(row, column, horizontal);
}

QList<Move> DotsAndBoxesGame::getPossibleMoves() const
{
    return generatePossibleMoves();
}

bool DotsAndBoxesGame::checkGameOver()
{
    if (board.isBoardFull())
    {
        isGameOver = true;
        winner = getWinner();
        setState(State::GameOver);
        emit gameLogicOver(winner);
        return true;
    }
    return false;
}

QString DotsAndBoxesGame::getWinner() const
{
    if (!isGameOver)
    {
        return "";
    }

    int score0 = board.getScore(0);
    int score1 = board.getScore(1);

    if (score0 > score1)
    {
        return "Player 1";
    }
    else if (score1 > score0)
    {
        return "Player 2";
    }
    else
    {
        return "Draw";
    }
}

//------------------------------------ supplementry_methods ------------------------------------
QJsonObject DotsAndBoxesGame::getBoardState() const
{
    return board.toJson();
}

void DotsAndBoxesGame::resetGame()
{
    board.reset(board.getRows(), board.getColumns());
    scores[0] = 0;
    scores[1] = 0;
    moveHistory.clear();
    completedBoxes = 0;
    isGameOver = false;
    winner = "";
    state = State::Idle;
    currentPlayerIndex = 0;
    emit gameReset();
}

QString DotsAndBoxesGame::getGameType() const
{
    return "DotsAndBoxes";
}

//------------------------------------ specialized_method ------------------------------------
DotsAndBoxesBoard* DotsAndBoxesGame::getBoard()
{
    return &board;
}

const DotsAndBoxesBoard* DotsAndBoxesGame::getBoard() const
{
    return &board;
}

void DotsAndBoxesGame::setBoardSize(int rows, int cols)
{
    board.reset(rows, cols);
    totalBoxes = board.getTotalBoxes();
    completedBoxes = 0;
}

int DotsAndBoxesGame::getTotalBoxes() const
{
    return totalBoxes;
}

int DotsAndBoxesGame::getCompletedBoxes() const
{
    return completedBoxes;
}

double DotsAndBoxesGame::getCompletionPercentage() const
{
    if (totalBoxes == 0)
    {
        return 0.0;
    }

    return (static_cast<double>(completedBoxes) / totalBoxes) * 100.0;
}

//------------------------------------ helper_method ------------------------------------
void DotsAndBoxesGame::onGameStart()
{
    board.reset(board.getRows(), board.getColumns());
    scores[0] = 0;
    scores[1] = 0;
    isGameOver = false;
    winner = "";
    currentPlayerIndex = 0;
    completedBoxes = 0;
}

void DotsAndBoxesGame::onGameEnd(const QString& winner)
{
    this->winner = winner;
}

void DotsAndBoxesGame::handleCompletedBoxes(const Move& move)
{
    int row = move.getRow();
    int column = move.getColumn();
    bool horizontal = move.isHorizontal();

    auto completed = board.getCompletedBoxes(row,
                                             column,
                                             horizontal,
                                             currentPlayerIndex);

    if (!completed.isEmpty())
    {
        int count = completed.size();
        completedBoxes += count;
        addScore(currentPlayerIndex, count);
        emit scoreUpdated(currentPlayerIndex, getScore(currentPlayerIndex));
    }
    else
    {
        switchPlayer();
    }
}

QList<Move> DotsAndBoxesGame::generatePossibleMoves() const
{
    QList<Move> moves;
    if (isGameOver)
    {
        return moves;
    }

    QString playerName = QString("Player %1").arg(currentPlayerIndex + 1);

    for (int r = 0; r <= board.getRows(); ++r)
    {
        for (int c = 0; c < board.getColumns(); ++c)
        {
            if (!board.getHorizontalLine(r, c))
            {
                Move m(playerName, r, c, true);
                m.setMoveType(Move::Type::Line);
                m.setDescription(QString("Horizontal line at (%1, %2)").arg(r).arg(c));
                moves.append(m);
            }
        }
    }

    for (int r = 0; r < board.getRows(); ++r)
    {
        for (int c = 0; c <= board.getColumns(); ++c)
        {
            if (!board.getVerticalLine(r, c))
            {
                Move m(playerName, r, c, false);
                m.setMoveType(Move::Type::Line);
                m.setDescription(QString("Vertical line at (%1, %2)").arg(r).arg(c));
                moves.append(m);
            }
        }
    }

    return moves;
}