#ifndef FANORONAGAME_H
#define FANORONAGAME_H

#include "../../core/Game.h"
#include "FanoronaBoard.h"

class FanoronaGame : public Game
{
    Q_OBJECT

private:
    FanoronaBoard board;

    bool waitingForCapture;
    QPoint lastMoveFrom;
    QPoint lastMoveTo;
    int currentCapturePlayer;
    QList<QPoint> captureDirections;

    bool handleMovement(const Move& move);
    bool handleCapture(const Move& move);
    void checkAndApplyCaptures(const Move& move);
    void applyCaptures(int row, int col, int player, const QList<QPoint>& directions);
    QList<Move> generatePossibleMoves() const;
    bool hasAnyValidMove(int player) const;
    void clearCaptureState();
    void onGameStart() override;
    void onGameEnd(const QString& winner) override;

public:
    explicit FanoronaGame(QObject* parent = nullptr);

    //main_methods
    void initialize(const GameConfig& config) override;
    bool makeMove(const Move& move) override;
    bool isValidMove(const Move& move) const override;
    QList<Move> getPossibleMoves() const override;
    bool checkGameOver() override;
    QString getWinner() const override;

    //supplementry_methods
    QJsonObject getBoardState() const override;
    void resetGame() override;
    QString getGameType() const override;

    //specialized_method
    FanoronaBoard* getBoard();
    const FanoronaBoard* getBoard() const;

};

#endif // FANORONAGAME_H