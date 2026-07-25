#include "GameBoardWidget.h"
#include <QDebug>
#include <cmath>
#include "../games/dotsandboxes/DotsAndBoxesGame.h"
#include "../games/morris/MorrisGame.h"
#include "../games/fanorona/FanoronaGame.h"
GameBoardWidget::GameBoardWidget(QWidget* parent)
    : QWidget(parent)
    , m_game(nullptr)
    , m_gameSession(nullptr)
    , m_boardRows(6)
    , m_boardCols(6)
    , m_cellSize(50)
    , m_player1Color(Qt::blue)
    , m_player2Color(Qt::red)
    , m_backgroundColor(Qt::white)
    , m_gridColor(QColor(200, 200, 200))
    , m_highlightColor(QColor(52, 152, 219, 80))
    , m_highlightEnabled(true)
    , m_hoverEnabled(true)
    , selectedMorrisPosition(-1)
    , selectedFanoronaCell(-1,-1)
{
    setWindowIcon(QIcon(":/app/app/app_icon.png"));
    setMinimumSize(400, 400);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
GameBoardWidget::GameBoardWidget(GameSession* _gameSession, QWidget* parent)
    : QWidget(parent)
    , m_game(_gameSession->getGame())
    , m_gameSession(_gameSession)
    , m_boardRows(6)
    , m_boardCols(6)
    , m_cellSize(50)
    , m_player1Color(Qt::blue)
    , m_player2Color(Qt::red)
    , m_backgroundColor(Qt::white)
    , m_gridColor(QColor(200, 200, 200))
    , m_highlightColor(QColor(52, 152, 219, 80))
    , m_highlightEnabled(true)
    , m_hoverEnabled(true)
    , selectedMorrisPosition(-1)
    , selectedFanoronaCell(-1,-1)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    MorrisGame * check = qobject_cast<MorrisGame*>(m_game);
    if(check != nullptr ) {
        initializeMorrisPositions();
    }

    calculateBoardOffset();
}
GameBoardWidget::~GameBoardWidget() {

}
void GameBoardWidget::initializeMorrisPositions()
{
    int x1 = m_boardOffset.x();
    int x2 = x1 + m_cellSize * 3;
    int x3 = x1 + m_cellSize * 6;

    int y1 = m_boardOffset.y();
    int y2 = y1 + m_cellSize * 3;
    int y3 = y1 + m_cellSize * 6;


    morrisPositions =
        {
            // outer square
            QPoint(x1,y1), //0
            QPoint(x2,y1), //1
            QPoint(x3,y1), //2

            QPoint(x3,y2), //3
            QPoint(x3,y3), //4
            QPoint(x2,y3), //5

            QPoint(x1,y3), //6
            QPoint(x1,y2), //7


            // middle square
            QPoint(x1+ m_cellSize,y1+m_cellSize), //8
            QPoint(x2,y1+m_cellSize),             //9
            QPoint(x3-m_cellSize,y1+m_cellSize),  //10

            QPoint(x3-m_cellSize,y2),             //11
            QPoint(x3-m_cellSize,y3-m_cellSize),  //12
            QPoint(x2,y3-m_cellSize),             //13

            QPoint(x1+m_cellSize,y3-m_cellSize),  //14
            QPoint(x1+m_cellSize,y2),             //15


            // inner square
            QPoint(x1+2*m_cellSize,y1+2*m_cellSize), //16
            QPoint(x2,y1+2*m_cellSize),              //17
            QPoint(x3-2*m_cellSize,y1+2*m_cellSize), //18

            QPoint(x3-2*m_cellSize,y2),              //19
            QPoint(x3-2*m_cellSize,y3-2*m_cellSize), //20
            QPoint(x2,y3-2*m_cellSize),              //21

            QPoint(x1+2*m_cellSize,y3-2*m_cellSize), //22
            QPoint(x1+2*m_cellSize,y2)               //23
        };
}
void GameBoardWidget::setGame(Game * game) {
    m_game = game;
    MorrisGame* morrisGame = qobject_cast<MorrisGame*>(game);
    if (morrisGame) {
        calculateBoardOffset();
        initializeMorrisPositions();
    }

    update();
}
void GameBoardWidget::setGameSession(GameSession* _gameSession) {
    m_gameSession = _gameSession;
}
void GameBoardWidget::setBoardSize(int rows, int cols) {
    m_boardRows = rows;
    m_boardCols = cols;
    calculateBoardOffset();
    update();
}
void GameBoardWidget::setPlayer1Color(const QColor& color) {
    m_player1Color = color;
    update();
}
void GameBoardWidget::setPlayer2Color(const QColor& color) {
    m_player2Color = color;
    update();
}
void GameBoardWidget::setCellSize(int size) {
    m_cellSize = size;
    update();
}
void GameBoardWidget::setHighlightEnabled(bool enabled) {
    m_highlightEnabled = enabled;
    if(!enabled) {
        clearHighlights();
    }
    update();
}
void GameBoardWidget::setHoverEnabled(bool enabled) {
    m_hoverEnabled = enabled;
    update();
}
void GameBoardWidget::highlightValidMoves(const QList<Move>& moves) {
    m_highlightedMoves = moves;
    update();
}
void GameBoardWidget::clearHighlights() {
    m_highlightedMoves.clear();
    update();
}
void GameBoardWidget::highlightLastMove(const Move& move) {
    m_lastMove = move;
    update();
}
void GameBoardWidget::clearLastMove() {
    m_lastMove = Move();
    update();
}
void GameBoardWidget::calculateBoardOffset() {
    int boardWidth = m_boardCols * m_cellSize;
    int boardHeight = m_boardRows * m_cellSize;

    m_boardOffset.setX((width() - boardWidth) / 2 );
    m_boardOffset.setY((height() - boardHeight) / 2);

}
void GameBoardWidget::paintEvent(QPaintEvent * event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //prevention from transform to pixel
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    drawBackground(painter);

    if(m_game) {
        drawGameBoard(painter);
        drawHighlights(painter);
        drawHover(painter);
        drawLastMove(painter);
    } else {
        painter.setPen(QColor(150, 150, 150));
        painter.setFont(QFont("Arial", 16));
        painter.drawText(rect(), Qt::AlignCenter, "No game selected");
    }
}
void GameBoardWidget::mousePressEvent(QMouseEvent* event) {
    if(!m_game || m_game->isFinished()) {
        return;
    }
    QPoint pos = event->pos();
    QString gameType = m_game->getGameType();
    Move move;
    if(gameType == "DotsAndBoxes") {
        handleDotsAndBoxesClick(pos);
        return;
    } else if(gameType == "Fanorona") {
        handleFanoronaClick(pos);
        return;
    } else if( gameType == "Morris") {
        handleMorrisClick(pos);
        return;
    }
    emit boardClicked(pos);
}
void GameBoardWidget::mouseMoveEvent(QMouseEvent* event) {
    m_hoverPosition = event->pos();

    if(m_hoverEnabled) {
        update();
    }
}
void GameBoardWidget::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    m_hoverPosition = QPoint(-1,-1);
    update();
}
void GameBoardWidget::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event);
    calculateBoardOffset();
    if (qobject_cast<MorrisGame*>(m_game)) {
        initializeMorrisPositions();
    }
    update();
}
void GameBoardWidget::drawBackground(QPainter& painter) {
    painter.fillRect(rect(), m_backgroundColor);
}
void GameBoardWidget::drawGameBoard(QPainter& painter) {
    if (!m_game) {
        painter.drawText(rect(), Qt::AlignCenter, "No game loaded");
        return;
    }
    QString gameType = m_game->getGameType();

    if(gameType == "DotsAndBoxes") {
        drawDotsAndBoxes(painter);
    } else if(gameType == "Morris") {
        drawMorris(painter);
    } else if(gameType == "Fanorona") {
        drawFanorona(painter);
    }
}
void GameBoardWidget::drawDotsAndBoxes(QPainter &painter)
{
    auto *game = dynamic_cast<DotsAndBoxesGame*>(m_game);

    if (!game)
        return;

    const DotsAndBoxesBoard *board = game->getBoard();

    const int rows = board->getRows();
    const int cols = board->getColumns();

    // squares

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            auto owner = board->getBoxOwner(r,c);

            if(owner == DotsAndBoxesBoard::State::Empty)
                continue;

            QRect rect(
                m_boardOffset.x() + c * m_cellSize,
                m_boardOffset.y() + r * m_cellSize,
                m_cellSize,
                m_cellSize
                );

            QColor color =
                (owner == DotsAndBoxesBoard::State::Player1)
                    ? m_player1Color
                    : m_player2Color;

            color.setAlpha(70);

            painter.fillRect(rect,color);
        }
    }

    // horizontal_lines

    QPen linePen;
    linePen.setWidth(4);

    for(int r=0;r<=rows;r++)
    {
        for(int c=0;c<cols;c++)
        {
            QPoint p1(
                m_boardOffset.x()+c*m_cellSize,
                m_boardOffset.y()+r*m_cellSize);

            QPoint p2(
                m_boardOffset.x()+(c+1)*m_cellSize,
                m_boardOffset.y()+r*m_cellSize);

            if(board->getHorizontalLine(r,c))
            {
                linePen.setColor(Qt::black);
            }
            else
            {
                linePen.setColor(QColor(180,180,180));
            }

            painter.setPen(linePen);
            painter.drawLine(p1,p2);
        }
    }

    // vertical_lines

    for(int r=0;r<rows;r++)
    {
        for(int c=0;c<=cols;c++)
        {
            QPoint p1(
                m_boardOffset.x()+c*m_cellSize,
                m_boardOffset.y()+r*m_cellSize);

            QPoint p2(
                m_boardOffset.x()+c*m_cellSize,
                m_boardOffset.y()+(r+1)*m_cellSize);

            if(board->getVerticalLine(r,c))
            {
                linePen.setColor(Qt::black);
            }
            else
            {
                linePen.setColor(QColor(180,180,180));
            }

            painter.setPen(linePen);
            painter.drawLine(p1,p2);
        }
    }

    // dots

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    const int radius = 5;

    for(int r=0;r<=rows;r++)
    {
        for(int c=0;c<=cols;c++)
        {
            QPoint center(
                m_boardOffset.x()+c*m_cellSize,
                m_boardOffset.y()+r*m_cellSize);

            painter.drawEllipse(center,radius,radius);
        }
    }
}
void GameBoardWidget::drawMorris(QPainter& painter)
{
    if(morrisPositions.isEmpty()) return;
    auto* game = dynamic_cast<MorrisGame*>(m_game);
    if(!game) return;

    MorrisBoard* board = game->getBoard();

    QPen linePen(Qt::black);
    linePen.setWidth(2);
    painter.setPen(linePen);
    painter.setBrush(Qt::NoBrush);

    auto drawRing = [&](int start) {
        for (int i = 0; i < 8; ++i) {
            int next = start + (i + 1) % 8;
            painter.drawLine(morrisPositions[start + i], morrisPositions[next]);
        }
    };

    drawRing(0);
    drawRing(8);
    drawRing(16);

    painter.drawLine(morrisPositions[1],  morrisPositions[9]);
    painter.drawLine(morrisPositions[9],  morrisPositions[17]);

    painter.drawLine(morrisPositions[3],  morrisPositions[11]);
    painter.drawLine(morrisPositions[11], morrisPositions[19]);

    painter.drawLine(morrisPositions[5],  morrisPositions[13]);
    painter.drawLine(morrisPositions[13], morrisPositions[21]);

    painter.drawLine(morrisPositions[7],  morrisPositions[15]);
    painter.drawLine(morrisPositions[15], morrisPositions[23]);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    for (int i = 0; i < 24; ++i) {
        painter.drawEllipse(morrisPositions[i], 4, 4);
    }

    for(int i=0;i<24;i++)
    {
        QPoint p = morrisPositions[i];
        auto state = board->getCellState(i);

        if(state == MorrisBoard::State::Player1)
        {
            painter.setBrush(m_player1Color);
            painter.setPen(Qt::black);
            painter.drawEllipse(p, 15, 15);
        }
        else if(state == MorrisBoard::State::Player2)
        {
            painter.setBrush(m_player2Color);
            painter.setPen(Qt::black);
            painter.drawEllipse(p, 15, 15);
        }

        if(i == selectedMorrisPosition)
        {
            painter.setPen(QPen(m_highlightColor,3));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(p, 20, 20);
        }
    }
}
void GameBoardWidget::drawFanorona(QPainter& painter)
{
    int cols = 9;
    int rows = 5;

    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);

    // horizontal_lines
    for (int r = 0; r < rows; ++r) {
        QPoint p1 = getCellCenter(r, 0);
        QPoint p2 = getCellCenter(r, cols - 1);
        painter.drawLine(p1, p2);
    }

    // vertical_lines
    for (int c = 0; c < cols; ++c) {
        QPoint p1 = getCellCenter(0, c);
        QPoint p2 = getCellCenter(rows - 1, c);
        painter.drawLine(p1, p2);
    }
    for (int r = 0; r < rows - 1; ++r) {
        for (int c = 0; c < cols - 1; ++c) {
            if ((r + c) % 2 == 0) {
                // \ diagonal
                painter.drawLine(getCellCenter(r, c), getCellCenter(r + 1, c + 1));
            } else {
                // / diagonal
                painter.drawLine(getCellCenter(r, c + 1), getCellCenter(r + 1, c));
            }
        }
    }

    // pieces
    FanoronaBoard* board = nullptr;
    if (m_game->getGameType() == "Fanorona") {
        FanoronaGame* game = qobject_cast<FanoronaGame*>(m_game);
        if (game) {
            board = game->getBoard();
        }
    }
    if (selectedFanoronaCell.x() != -1)
    {
        painter.setPen(QPen(m_highlightColor, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(
            getCellCenter(selectedFanoronaCell.x(), selectedFanoronaCell.y()),
            18, 18);
    }
    if (!board) return;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            auto state = board->getCellState(r, c);
            if (state == FanoronaBoard::State::Empty) continue;

            QPoint center = getCellCenter(r, c);
            int pieceRadius = 12;

            painter.setBrush(state == FanoronaBoard::State::Player1 ? m_player1Color : m_player2Color);
            painter.setPen(Qt::black);
            painter.drawEllipse(center, pieceRadius, pieceRadius);
        }
    }
}
void GameBoardWidget::drawHover(QPainter& painter)
{
    if (!m_hoverEnabled || m_hoverPosition.isNull()) {
        return;
    }

    int col = m_hoverPosition.x() / m_cellSize;
    int row = m_hoverPosition.y() / m_cellSize;

    QRect hoverRect(col * m_cellSize, row * m_cellSize, m_cellSize, m_cellSize);

    painter.fillRect(hoverRect, QColor(255, 255, 0, 80));
}
void GameBoardWidget::drawLastMove(QPainter& painter) {
    if(m_lastMove.getMoveType() == Move::Type::Unknown) {
        return;
    }
    painter.setPen(QPen(QColor(255, 0, 0, 100), 3));
    painter.setBrush(Qt::NoBrush);

    QString gameType = m_game->getGameType();
    if(gameType == "DotsAndBoxes") {
        int row = m_lastMove.getRow();
        int col = m_lastMove.getColumn();
        bool horizontal = m_lastMove.isHorizontal();

        QPoint p1,p2;
        if(horizontal) {
            p1 = getCellCenter(row, col);
            p2 = getCellCenter(row, col+1);
        } else {
            p1 = getCellCenter(row, col);
            p2 = getCellCenter(row + 1, col);
        }
        painter.drawLine(p1, p2);
    }
}
QRect GameBoardWidget::getCellRect(int row, int col) const {
    int x = m_boardOffset.x() + col * m_cellSize;
    int y = m_boardOffset.y() + row * m_cellSize;
    return QRect(x, y, m_cellSize, m_cellSize);
}
QPoint GameBoardWidget::getCellCenter(int row, int col) const {
    QRect rect = getCellRect(row, col);
    return rect.center();
}
bool GameBoardWidget::isValidCell(int row, int col) const {
    return (row >= 0 && row <= m_boardRows && col>= 0 && col <= m_boardCols);
}
QPoint GameBoardWidget::getCellAt(const QPoint& pos) const {
    int col = (pos.x() - m_boardOffset.x()) / m_cellSize;
    int row = (pos.y() - m_boardOffset.y()) / m_cellSize;

    if (row >= 0 && row <= m_boardRows && col >= 0 && col <= m_boardCols) {
        return QPoint(col, row);
    }
    return QPoint(-1, -1);

}
void GameBoardWidget::handleDotsAndBoxesClick(const QPoint& pos) {
    auto * game = dynamic_cast<DotsAndBoxesGame*>(m_game);
    if(!game) {
        return;
    }
    const int maxSapce = 12;
    QPoint cell = getCellAt(pos);
    if(cell.x() < 0 || cell.y() < 0) {
        return;
    }
    int row = cell.y();
    int col = cell.x();
    // relative location
    int localX = (pos.x() - m_boardOffset.x()) % m_cellSize;
    int localY = (pos.y() - m_boardOffset.y()) % m_cellSize;

    int dTop = localY;
    int dBottom = m_cellSize - localY;
    int dLeft = localX;
    int dRight = m_cellSize - localX;

    int best = qMin(qMin(dTop, dBottom), qMin(dLeft, dRight));
    bool horizontal;
    int lineRow = row;
    int lineCol = col;

    if(best == dTop ) {
        horizontal = true;
    } else if( best == dBottom) {
        horizontal = true;
        lineRow++;
    } else if(best == dLeft) {
        horizontal = false;
    } else {
        horizontal = false;
        lineCol++;
    }
    if( best > maxSapce ) {
        return;
    }
    if(!game->getBoard()->isValidLine(lineRow, lineCol, horizontal)) {
        return;
    }
    if(game->getBoard()->isLineDrawn(lineRow, lineCol, horizontal)) {
        return;
    }
    QString username;
    if (m_gameSession) {
        username = m_gameSession->getCurrentPlayer();
    } else {
        int idx = game->getCurrentPlayerIndex();
        username = (idx == 0) ? "Player1" : "Player2";
    }

    Move move(username, lineRow, lineCol, horizontal);
    if(game->makeMove(move)) {
        m_lastMove = move;
        update();
    }

}
int GameBoardWidget::getMorrisPosition(const QPoint& pos) const
{
    const int threshold = 25;

    int selected = -1;
    double minDistance = INT_MAX;


    for(int i = 0; i < morrisPositions.size(); i++)
    {
        double distance =
            QLineF(pos,
                   morrisPositions[i])
                .length();


        if(distance < minDistance)
        {
            minDistance = distance;
            selected = i;
        }
    }


    if(minDistance > threshold)
        return -1;


    return selected;
}
void GameBoardWidget::handleMorrisClick(const QPoint& pos) {
    if (morrisPositions.isEmpty()) return;

    auto * game = dynamic_cast<MorrisGame*> (m_game);
    if(!game) {
        return;
    }
    int clickedPostion = getMorrisPosition(pos);
    if(clickedPostion == -1) {
        selectedMorrisPosition = -1;
        update();
        return;
    }
    QString username;
    if (m_gameSession) {
        username = m_gameSession->getCurrentPlayer();
    } else {
        int idx = game->getCurrentPlayerIndex();
        username = (idx == 0) ? "Player1" : "Player2";
    }

    MorrisBoard::Phase phase = game->getBoard()->getPhase();

    // delete
    if(game->isWaitingForRemoval()) {
        Move move(username, Move::Type::Removal);
        move.setMorrisData(-1, clickedPostion, true);

        if(game->makeMove(move)) {
            selectedMorrisPosition = -1;
            m_lastMove = move;
            update();
        }
        return;
    }

    // placement
    if(phase == MorrisBoard::Phase::Placement) {
        Move move(username, QPoint(clickedPostion, 0));

        move.setMorrisData(-1, clickedPostion, false);

        if(game->makeMove(move)) {
            selectedMorrisPosition = -1;
            m_lastMove = move;
            update();
        }
        return;
    }

    // moving
    if(selectedMorrisPosition == -1) {
        MorrisBoard::State state = game->getBoard()->getCellState(clickedPostion);
        int currentPlayer = game->getCurrentPlayerIndex();

        MorrisBoard::State playerState = currentPlayer == 0 ? MorrisBoard::State::Player1 : MorrisBoard::State::Player2;
        if(state == playerState) {
            selectedMorrisPosition = clickedPostion;
            update();
        }
        return;
    }
    Move move(username, selectedMorrisPosition, clickedPostion);
    move.setMorrisData(selectedMorrisPosition, clickedPostion, false);

    if (game->isValidMove(move)) {
        if (game->makeMove(move)) {
            selectedMorrisPosition = -1;
            update();
        }
    } else {
        selectedMorrisPosition = -1;
        update();
    }

}
void GameBoardWidget::handleFanoronaClick(const QPoint& pos) {
    auto* game = dynamic_cast<FanoronaGame*>(m_game);
    if (!game) return;

    QPoint cell = getCellAt(pos);
    if (cell.x() < 0 || cell.y() < 0) return;

    int col = cell.x();
    int row = cell.y();
    QString username;
    if (m_gameSession) {
        username = m_gameSession->getCurrentPlayer();
    } else {
        int idx = game->getCurrentPlayerIndex();
        username = (idx == 0) ? "Player1" : "Player2";
    }
    if (game->isWaitingForCapture())
    {
        QPoint captureFrom = game->getLastMoveTo();

        int dx = row - captureFrom.x();
        int dy = col - captureFrom.y();
        int ddx = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
        int ddy = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

        Move move(username, Move::Type::Capture);
        move.setData("captureRow", captureFrom.x());
        move.setData("captureCol", captureFrom.y());
        move.setData("directionX", ddx);
        move.setData("directionY", ddy);

        emit moveSelected(move);
        return;
    }
    // click on own piece(start)
    if (selectedFanoronaCell.x() == -1)
    {
        auto state = game->getBoard()->getCellState(row, col);
        int currentPlayer = game->getCurrentPlayerIndex();
        auto myState = (currentPlayer == 0) ? FanoronaBoard::State::Player1
                                            : FanoronaBoard::State::Player2;
        if (state == myState)
        {
            selectedFanoronaCell = QPoint(row, col);
            update();
        }
        return;
    }
    // second click ( select goal)
    QPoint from = selectedFanoronaCell;
    QPoint to(row, col);
    selectedFanoronaCell = QPoint(-1, -1);

    if (from == to)
    {
        update();
        return;
    }

    Move move(username, from, to);
    emit moveSelected(move);
    update();
}
void GameBoardWidget::drawHighlights(QPainter& painter)
{
    if (!m_highlightEnabled || m_highlightedMoves.isEmpty()) {
        return;
    }

    painter.setBrush(m_highlightColor);
    painter.setPen(Qt::NoPen);

    for (const Move& move : m_highlightedMoves) {
        QString gameType = m_game->getGameType();

        if (gameType == "DotsAndBoxes") {
            int row = move.getRow();
            int col = move.getColumn();
            bool horizontal = move.isHorizontal();

            QPoint p1, p2;
            if (horizontal) {
                p1 = getCellCenter(row, col);
                p2 = getCellCenter(row, col + 1);
            } else {
                p1 = getCellCenter(row, col);
                p2 = getCellCenter(row + 1, col);
            }

            QLineF line(p1, p2);
            QPolygonF rect;
            QPointF normal = line.normalVector().unitVector().p2() - line.p1();
            normal *= 8;

            rect << line.p1() - normal
                 << line.p2() - normal
                 << line.p2() + normal
                 << line.p1() + normal;

            painter.drawPolygon(rect);

        } else if (gameType == "Morris") {

            int from = move.getFrom();
            int to = move.getTo();

            if(to >=0 && to < 24)
            {
                painter.drawEllipse(
                    morrisPositions[to],
                    22,
                    22);
            }

            if(from >=0 && from < 24)
            {
                painter.setPen(
                    QPen(m_highlightColor,3));

                painter.setBrush(Qt::NoBrush);


                painter.drawEllipse(
                    morrisPositions[from],
                    25,
                    25);
            }

        } else if (gameType == "Fanorona") {

            QPoint to = move.getToPoint();
            QRect rect = getCellRect(to.x(), to.y());
            painter.drawRect(rect);
        }
    }
}
