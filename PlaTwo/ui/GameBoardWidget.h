#ifndef GAMEBOARDWIDGET_H
#define GAMEBOARDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include <QPoint>
#include "../core/Game.h"
#include "../core/GameSession.h"
#include "../core/Move.h"

class GameBoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameBoardWidget(QWidget* parent = nullptr);
    explicit GameBoardWidget(GameSession* gameSession, QWidget* parent = nullptr);
    ~GameBoardWidget();

    void setGame(Game* game);
    void setBoardSize(int rows, int cols);
    void setPlayer1Color(const QColor& color);
    void setPlayer2Color(const QColor& color);
    void setCellSize(int size);
    void setHighlightEnabled(bool enabled);
    void setHoverEnabled(bool enabled);

    void highlightValidMoves(const QList<Move>& moves);
    void clearHighlights();
    void highlightLastMove(const Move& move);
    void clearLastMove();


    bool isGameActive() const { return m_game != nullptr; }
    int getRows() const { return m_boardRows; }
    int getCols() const { return m_boardCols; }
    QPoint getCellAt(const QPoint& pos) const;

signals:
    void boardClicked(const QPoint& pos);
    void moveSelected(const Move& move);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Game* m_game;
    GameSession* m_gameSession;
    int m_boardRows;
    int m_boardCols;
    int m_cellSize;

    QColor m_player1Color;
    QColor m_player2Color;
    QColor m_backgroundColor;
    QColor m_gridColor;
    QColor m_highlightColor;


    QList<Move> m_highlightedMoves;
    Move m_lastMove;
    QPoint m_hoverPosition;
    bool m_highlightEnabled;
    bool m_hoverEnabled;
    int selectedMorrisPosition;
    QVector<QPoint> morrisPositions;


    QPoint m_boardOffset;

    void calculateBoardOffset();
    void drawBackground(QPainter& painter);
    void drawGameBoard(QPainter& painter);
    void drawDotsAndBoxes(QPainter& painter);
    void drawMorris(QPainter& painter);
    void drawFanorona(QPainter& painter);
    void drawHighlights(QPainter& painter);
    void drawHover(QPainter& painter);
    void drawLastMove(QPainter& painter);
    QRect getCellRect(int row, int col) const;
    QPoint getCellCenter(int row, int col) const;
    bool isValidCell(int row, int col) const;
    Move createMoveFromClick(const QPoint& pos);
    void handleDotsAndBoxesClick(const QPoint& pos);
    void handleMorrisClick(const QPoint& pos);
    void handleFanoronaClick(const QPoint& pos);
    int getMorrisPosition(const QPoint& pos) const;
    void initializeMorrisPositions();
};

#endif // GAMEBOARDWIDGET_H