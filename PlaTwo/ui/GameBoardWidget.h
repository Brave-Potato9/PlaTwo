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
    // construtor_destructor
    explicit GameBoardWidget(QWidget* parent = nullptr);
    explicit GameBoardWidget(GameSession* gameSession, QWidget* parent = nullptr);
    ~GameBoardWidget();

    // basic_board_settings
    void setGame(Game* game);
    void setCellSize(int size);
    void setBoardSize(int rows, int cols);

    // setters
    void setGameSession(GameSession* gameSession);
    void setPlayer1Color(const QColor& color);
    void setPlayer2Color(const QColor& color);
    void setHighlightEnabled(bool enabled);
    void setHoverEnabled(bool enabled);

    // manage_highlights
    void highlightValidMoves(const QList<Move>& moves);
    void clearHighlights();
    void highlightLastMove(const Move& move);

    void clearLastMove();


    bool isGameActive() const { return m_game != nullptr; }
    int getRows() const { return m_boardRows; }
    int getCols() const { return m_boardCols; }

signals:
    void boardClicked(const QPoint& pos);
    void moveSelected(const Move& move);

protected:
    // draw
    void paintEvent(QPaintEvent* event) override;
    // work_with_mouse
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
    QPoint selectedFanoronaCell;


    QPoint m_boardOffset;
    // calculate_utils
    void calculateBoardOffset();
    QRect getCellRect(int row, int col) const;
    QPoint getCellCenter(int row, int col) const;
    bool isValidCell(int row, int col) const;
    QPoint getCellAt(const QPoint& pos) const;


    // draw
    void drawBackground(QPainter& painter);
    void drawGameBoard(QPainter& painter);

    // draw_games
    void drawDotsAndBoxes(QPainter& painter);
    void drawMorris(QPainter& painter);
    void drawFanorona(QPainter& painter);

    // manage_highlights
    void drawHighlights(QPainter& painter);

    void drawHover(QPainter& painter);

    // draw_last_move
    void drawLastMove(QPainter& painter);

    // process_click
    void handleDotsAndBoxesClick(const QPoint& pos);
    void handleMorrisClick(const QPoint& pos);
    void handleFanoronaClick(const QPoint& pos);
    // morris_utils
    int getMorrisPosition(const QPoint& pos) const;
    void initializeMorrisPositions();
};

#endif // GAMEBOARDWIDGET_H