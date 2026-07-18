#ifndef MORRISGAME_H
#define MORRISGAME_H

#include "../../core/Game.h"
#include "MorrisBoard.h"

class MorrisGame : public Game
{
    Q_OBJECT

private:
    MorrisBoard board;

    bool waitingForRemoval;
    QList<int> removablePieces;

    //helper_methods
    bool handlePlacement(const Move& move);
    bool handleMovement(const Move& move);
    bool handleRemoval(const Move& move);
    void checkAndHandleMill(int pos, int player);
    QList<Move> generatePossibleMoves() const;
    void clearRemovalState();
    void onGameStart() override;
    void onGameEnd(const QString& winner) override;

public:
    explicit MorrisGame(QObject* parent = nullptr);

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
    QString getGameType() const override ;

    //specialized_method
    MorrisBoard* getBoard() ;
    const MorrisBoard* getBoard() const ;

};

#endif // MORRISGAME_H