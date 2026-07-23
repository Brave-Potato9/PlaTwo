#ifndef DOTSANDBOXESBOARD_H
#define DOTSANDBOXESBOARD_H

#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QPair>

class DotsAndBoxesBoard
{
public:
    enum class State
    {
        Player1,
        Player2,
        Empty
    };

private:
    int rows;
    int columns;
    QVector<QVector<bool>> horizontalLines;
    QVector<QVector<bool>> verticalLines;
    QVector<QVector<DotsAndBoxesBoard::State>> boxOwners;

    //helper_method
    void markBoxOwner(int row, int column, int player);
    DotsAndBoxesBoard::State intToState(int state) const;

public:
    //constructor
    DotsAndBoxesBoard(int _rows = 6, int _columns = 6);

    //line_management_methods
    bool drawLine(int row, int column, bool isHorizontal, int player);
    bool isLineDrawn(int row, int column, bool isHorizontal) const;
    bool isValidLine(int row, int column, bool isHorizontal) const;

    //box_management_methods
    QList<QPair<int,int>> getCompletedBoxes(int row, int column, bool isHorizontal, int player);
    bool isBoxComplete(int row, int column) const;
    DotsAndBoxesBoard::State getBoxOwner(int row, int column) const;
    int getCompletedBoxesCount() const;
    int getTotalBoxes() const;
    bool isBoardFull() const;

    //score_methods
    int getScore(int player) const;
    QVector<int> getScores() const; //[0] -> player1 / [1] -> player2

    //working_with_JSON
    QJsonObject toJson() const;
    static DotsAndBoxesBoard fromJson(const QJsonObject& obj);

    //getters
    int getRows() const ;
    int getColumns() const ;
    bool getHorizontalLine(int row, int column) const;
    bool getVerticalLine(int row, int column) const;

    //reset
    void reset(int newRows = 6, int newColumns = 6);
};

#endif // DOTSANDBOXESBOARD_H
