#ifndef DOTSANDBOXESGAME_H
#define DOTSANDBOXESGAME_H

#include "../../core/Game.h"
#include "DotsAndBoxesBoard.h"

class DotsAndBoxesGame: public Game
{
    Q_OBJECT
protected:


private:
    DotsAndBoxesBoard board;
    int totalBoxes;
    int completedBoxes;

    //helper_method
    void handleCompletedBoxes(const Move& move);
    QList<Move> generatePossibleMoves() const;
    void onGameStart() override;
    void onGameEnd(const QString& winner) override;

public:
    //constructor
    explicit DotsAndBoxesGame(QObject* parent = nullptr);

    //main_method
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
    DotsAndBoxesBoard* getBoard() ;
    const DotsAndBoxesBoard* getBoard() const ;
    void setBoardSize(int rows, int columns);
    int getTotalBoxes() const ;
    int getCompletedBoxes() const ;
    double getCompletionPercentage() const;
};

#endif // DOTSANDBOXESGAME_H
