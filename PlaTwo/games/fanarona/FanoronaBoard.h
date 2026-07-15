#ifndef FANORONABOARD_H
#define FANORONABOARD_H

#include <QVector>
#include <QPoint>
#include <QList>
#include <QPair>
#include <QJsonObject>
#include <QJsonArray>

class FanoronaBoard
{
public:
    enum class State
    {
        Empty,
        Player1,
        Player2
    };

private:
    static const int rows;
    static const int column;
    QVector<QVector<State>> board;

    static const QList<QPoint> direction;

    //helper_methods
    bool isValidPosition(int row, int col) const;
    bool isEmpty(int row, int col) const;
    bool isPlayerPiece(int row, int col, int player) const;
    bool isOpponentPiece(int row, int col, int player) const;
    bool canCapture(int row, int col, int player, const QPoint& dir) const;
    QList<QPair<int,int>> getCapturesInDirection(int row,
                                                  int col,
                                                  int player,
                                                  const QPoint& dir) const;
    void clearBoard();

public:

    //constructors
    FanoronaBoard();

    //board_management
    bool movePiece(int fromRow, int fromCol, int toRow, int toCol, int player);
    bool applyCapture(int row, int col, int player, const QPoint& direction);
    bool removePiece(int row, int col, int player);

    //state_management
    State getCellState(int row, int col) const;
    int getPiecesCount(int player) const;
    bool isInBounds(int row, int col) const;
    int getRows() const;
    int getColumns() const;

    //capture_management
    QList<QPair<int,int>> getCaptures(int row, int col, int player) const;
    QList<QPair<int,int>> getCaptureChain(int row, int col, int player,
                                           const QPoint& direction) const;
    bool hasValidMoves(int player) const;

    //working_with_JSON
    QJsonObject toJson() const;
    static FanoronaBoard fromJson(const QJsonObject& obj);

    //reset
    void reset();


};

#endif // FANORONABOARD_H
