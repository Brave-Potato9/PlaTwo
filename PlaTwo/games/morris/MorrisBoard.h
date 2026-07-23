#ifndef MORRISBOARD_H
#define MORRISBOARD_H

#include <QVector>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

class MorrisBoard
{
public:
    enum class State
    {
        Empty,
        Player1,
        Player2
    };

    enum class Phase
    {
        Placement,
        Movement,
        Flying
    };

    static const int PositionsNumber = 24;
    static const int maxPieces = 9;

    static QVector<QList<int>> adjacency;
    static QVector<QList<int>> mills;
    static bool staticDataInitialized;

private:
    QVector<State> positions;
    int placedPieces[2];

    Phase phase;

    //helper_methods
    void initializeBoard();
    bool isValidPosition(int pos) const;
    bool isInMill(int pos, int player, QList<int>& millPositions) const;
    void updatePhase();
    static void initializeStaticData();


public:
    //constructors
    MorrisBoard();

    //helper_methods
    bool isAdjacent(int from, int to) const;

    //board_main_management
    bool placePiece(int pos, int player);
    bool movePiece(int from, int to, int player);
    bool removePiece(int pos, int player);
    bool isMill(int pos, int player) const;
    bool isMillFormed(int pos, int player, QList<int>& millPositions) const;

    //state_management
    State getCellState(int pos) const;
    int getPiecesCount(int player) const;
    int getPlacedCount(int player) const;
    Phase getPhase() const;
    bool isBoardFull() const;

    //valiedation_management
    QList<int> getValidMoves(int fromPos) const;
    QList<int> getValidPlacements(int player) const;
    QList<int> getRemovablePieces(int player) const;
    bool hasValidMoves(int player) const;

    //working_with_JSON
    QJsonObject toJson() const;
    static MorrisBoard fromJson(const QJsonObject& obj);

    //reset
    void reset();

};

#endif // MORRISBOARD_H