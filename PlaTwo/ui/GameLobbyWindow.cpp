#include "GameLobbyWindow.h"
#include "ui_GameLobbyWindow.h"
#include <QMessageBox>
#include <QCheckBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QPropertyAnimation>
GameLobbyWindow::GameLobbyWindow(const QString& _gameType, AuthManager* _authManager,
                                 const QString& _username, QWidget* parent)
    :QMainWindow(parent)
    ,gameType(_gameType)
    ,gameDisplayName(getGameDisplayName())
    ,ui(new Ui::GameLobbyWindow)
    ,authManager(_authManager)
    ,username(_username)
    ,selectedColor(getDefaultColor())
{
    ui->setupUi(this);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(350);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    setWindowIcon(QIcon(":/app/app/app_icon.png"));
    setupUI();
    setupGameSpecificUI();
    setupConnections();
    loadHistory();
    loadSavedGames();

}
GameLobbyWindow::~GameLobbyWindow() {
    delete ui;
}
QString GameLobbyWindow::getGameDisplayName() const {
    if(gameType == "DotsAndBoxes") return "Dots and Boxes";
    if(gameType == "Morris") return "Nine Men's Morris";
    if(gameType == "Fanorona") return "Fanorona";
    return gameType;
}
QColor GameLobbyWindow::getDefaultColor() const {
    if(gameType == "DotsAndBoxes") return QColor(52,152,219);
    if(gameType == "Morris") return QColor(46,204,113);
    if(gameType == "Fanorona") return QColor(231,76,60);
    return QColor(52,152,219);
}
QString GameLobbyWindow::getSavesDirectory() const {
    return "data/" + username + "/saves/";
}
QJsonObject GameLobbyWindow::loadSaveFile(const QString& filePath) {
    QFile file(filePath);
    if(!file.exists()) return QJsonObject();
    if(!file.open(QIODevice::ReadOnly)) return QJsonObject();

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()) return QJsonObject();
    return doc.object();
}
void GameLobbyWindow::setupUI() {
    setWindowTitle(gameDisplayName + " - Game Looby");
    setFixedSize(750,650);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    ui->labelTitle->setText( gameDisplayName + "Lobby");
    ui->tableWidgetHistory->setColumnCount(5);
    QStringList headers = {"Opponent", "Date", "Role", "Winner", "Score"};
    ui->tableWidgetHistory->setHorizontalHeaderLabels(headers);
    ui->tableWidgetHistory->horizontalHeader()->setStretchLastSection(true); // پر کردن فضای خالی
    ui->tableWidgetHistory->setAlternatingRowColors(true); // رنگ کردن یکی در میان
    ui->tableWidgetHistory->setEditTriggers(QAbstractItemView::NoEditTriggers); // جلوگیری از ادیت
    ui->tableWidgetHistory->setSelectionBehavior(QAbstractItemView::SelectRows);// یک ردیف کامل انتخاب شود

    ui->labelColorPreview->setStyleSheet(
        "background-color: " + selectedColor.name() + ";"
        "border-radius: 22px;"
        "border: 2px solid #2c3e50;"
        );
    ui->labelStatsTotal->setText("Total: 0");
    ui->labelStatsWins->setText("🏆 Wins: 0");
    ui->labelStatsLosses->setText("❌ Losses: 0");
    ui->labelStatsDraws->setText("⚖️ Draws: 0");
    ui->labelStatsWinRate->setText("📈 Rate: 0%");

    ui->labelSaveInfo->setText("📁 Select a save file to see details");

    ui->pushButtonLoadGame->setEnabled(false);
    ui->pushButtonDeleteSave->setEnabled(false);

}
void GameLobbyWindow::setupGameSpecificUI() {
    QLayout * layout = ui->widgetConfigContent->layout();
    if(layout) {
        QLayoutItem * item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }
    QVBoxLayout * configLayout = new QVBoxLayout(ui->widgetConfigContent);
    configLayout->setSpacing(10);
    configLayout->setContentsMargins(0,0,0,0);
    if(gameType == "DotsAndBoxes") {
        QLabel * labelBoardSize = new QLabel("Board Size:", this);
        labelBoardSize->setStyleSheet("font-weight: bold; color: #34495e;");
        configLayout->addWidget(labelBoardSize);
        QComboBox * comboBoardSize = new QComboBox(this);
        comboBoardSize->addItem("6x6", 6);
        comboBoardSize->addItem("8x8", 8);
        comboBoardSize->setObjectName("comboBoardSize");
        comboBoardSize->setStyleSheet(
            "QComboBox {"
            "   border: 1px solid #dcdde1;"
            "   border-radius: 6px;"
            "   padding: 5px 10px;"
            "}"
            "QComboBox: focus {"
            "   border: 2px solid #3498db;"
            "}");
        configLayout->addWidget(comboBoardSize);
    } else if(gameType == "Morris") {
        QLabel * labelBoard = new QLabel("Standard Board(24 positions)", this);
        labelBoard->setStyleSheet("color: #7f8c8d; font-size: 12px;");
        configLayout->addWidget(labelBoard);
        QCheckBox * checkFlying = new QCheckBox("Enable Flying Phase", this);
        checkFlying->setChecked(true);
        checkFlying->setObjectName("checkFlying");
        checkFlying->setStyleSheet("font-size: 13px; color: #34495e;");
        configLayout->addWidget(checkFlying);
    }
    QLabel * labelTimeLimit = new QLabel("Time Limit(per player) :", this);
    labelTimeLimit->setStyleSheet("font-weight: bold; color: #34495e; margin-top: 5px;");
    configLayout->addWidget(labelTimeLimit);

    QComboBox * comboTimeLimit = new QComboBox(this);
    comboTimeLimit->addItem("No Limit", 0);
    comboTimeLimit->addItem("1 Minute", 60);
    comboTimeLimit->addItem("2 Minutes" , 120);
    comboTimeLimit->addItem("5 Minutes", 300);
    comboTimeLimit->setObjectName("comboTimeLimit");
    comboTimeLimit->setStyleSheet(
    "QComboBox {"
    "   border: 1px solid #dcdde1;"
    "   border-radius: 6px;"
    "   padding: 5px 10px;"
    "}"
    "QComboBox: focus {"
    "   border: 2px solid #3498db;"
    "}");
    configLayout->addWidget(comboTimeLimit);
    configLayout->addStretch();

}
void GameLobbyWindow::setupConnections() {
    connect(ui->pushButtonStartGame, &QPushButton::clicked, this, &GameLobbyWindow::onStartGameClicked);
    connect(ui->pushButtonLoadGame, &QPushButton::clicked, this, &GameLobbyWindow::onLoadGameClicked);
    connect(ui->pushButtonDeleteSave, &QPushButton::clicked, this, &GameLobbyWindow::onDeleteSaveClicked);
    connect(ui->pushButtonBack, &QPushButton::clicked, this, &GameLobbyWindow::onBackClicked);
    connect(ui->pushButtonRefresh, &QPushButton::clicked, this, &GameLobbyWindow::onRefreshHistoryClicked);
    connect(ui->pushButtonChooseColor, &QPushButton::clicked, this, &GameLobbyWindow::onChooseColorClicked);
    connect(ui->listWidgetSavedGames, &QListWidget::itemClicked, this, &GameLobbyWindow::onSaveItemClicked);
    connect(ui->pushButtonJoinGame, &QPushButton::clicked, this, &GameLobbyWindow::onJoinGameClicked);
}
void GameLobbyWindow::loadHistory() {
    ui->tableWidgetHistory->setRowCount(0);
    QList<GameHistory> histories = historyManager.loadHistories(username,gameType);
    if(histories.isEmpty()){
        ui->tableWidgetHistory->setRowCount(1);
        ui->tableWidgetHistory->setItem(0,0, new QTableWidgetItem("No games played yet"));
        ui->tableWidgetHistory->setSpan(0,0,1,5);
        updateStats();
        return;
    }
    ui->tableWidgetHistory->setRowCount(histories.size());
    for(int i=0 ; i < histories.size() ; i++) {
        addHistoryToTable(histories[i],i);
    }
    updateStats();
}
void GameLobbyWindow::addHistoryToTable(const GameHistory& history, int row) {
    QStringList players = history.getPlayers();
    QString opponent = players.size() > 1 ? players[1] : "Unknown";
    QString role = (players.size() > 0 && players[0] == username) ? "First" : "Second";
    QString winner = history.getWinner();
    if(winner == username) {
        winner = "✅ You";
    } else if (winner == "Draw") {
        winner = "⚖ Draw";
    } else {
        winner = "❌ "+ winner;
    }
    QVector<int> scores = history.getScores();
    QString scoreText = QString::number(scores.value(0,0)) + "-" + QString::number(scores.value(1,0));
    ui->tableWidgetHistory->setItem(row, 0, new QTableWidgetItem(opponent));
    ui->tableWidgetHistory->setItem(row, 1, new QTableWidgetItem(history.getStartTime().toString("yyyy/MM//dd HH:mm")));
    ui->tableWidgetHistory->setItem(row, 2, new QTableWidgetItem(role));
    ui->tableWidgetHistory->setItem(row, 3, new QTableWidgetItem(winner));
    ui->tableWidgetHistory->setItem(row, 4, new QTableWidgetItem(scoreText));

}
void GameLobbyWindow::updateStats() {
    int total = historyManager.getTotalGamesCount(username, gameType);
    int wins = historyManager.getWinCount(username, gameType);
    int losses = historyManager.getLossCount(username, gameType);
    int draws = historyManager.getDrawCount(username, gameType);
    double winRate = historyManager.getWinRate(username, gameType);

    ui->labelStatsTotal->setText("Total: " + QString::number(total));
    ui->labelStatsWins->setText("🏆 Wins: " + QString::number(wins));
    ui->labelStatsLosses->setText("❌ Losses: " + QString::number(losses));
    ui->labelStatsDraws->setText("⚖️ Draws: " + QString::number(draws));
    ui->labelStatsWinRate->setText("📈 Rate: " + QString::number(winRate, 'f', 1) + "%");

}
void GameLobbyWindow::loadSavedGames() {
    ui->listWidgetSavedGames->clear();
    clearSaveSelection();

    QDir saveDir(getSavesDirectory());
    if(!saveDir.exists()) {
        ui->listWidgetSavedGames->addItem("No saved games found !");
        return;
    }
    QStringList filterFormat;
    filterFormat << "*.json";
    QStringList saveFiles = saveDir.entryList(filterFormat, QDir::Files, QDir::Time);
    if(saveFiles.isEmpty()) {
        ui->listWidgetSavedGames->addItem("No saved games found !");
        return;
    }
    bool found = false;
    for(const QString& fileName: saveFiles) {
        QString filePath = getSavesDirectory() + fileName;
        QJsonObject saveData = loadSaveFile(filePath);
        if(saveData.isEmpty()) continue;
        QString saveGameType = saveData["gameType"].toString();
        if(saveGameType != gameType) {
            continue;
        }
        QString displayText = fileName;
        QString saveTime = saveData["saveTime"].toString();
        QString currentPlayer = saveData["currentPlayer"].toString();
        if(!saveTime.isEmpty()) {
            displayText += "-" +saveTime;
        }
        if(!currentPlayer.isEmpty()) {
            displayText += "(" + currentPlayer + "'s turn";
        }
        QListWidgetItem * item = new QListWidgetItem(displayText, ui->listWidgetSavedGames);
        item->setData(Qt::UserRole, filePath);
        item->setData(Qt::UserRole + 1 , saveData["playerColor"].toString());

        if(!currentPlayer.isEmpty()) {
            item->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
        }

    }
    if(!found) ui->listWidgetSavedGames->addItem("No saved games for this game type !");

}
void GameLobbyWindow::clearSaveSelection() {
    selectSaveFile.clear();
    ui->pushButtonLoadGame->setEnabled(false);
    ui->pushButtonDeleteSave->setEnabled(false);
    ui->labelSaveInfo->setText("📁 Select a save file to see details");
}
void GameLobbyWindow::onSaveItemClicked(QListWidgetItem * item) {
    if(!item || item->text().contains("No saved games found")) {
        clearSaveSelection();
        return;
    }
    QString filePath = item->data(Qt::UserRole).toString();
    selectSaveFile = filePath;

    QJsonObject saveData = loadSaveFile(filePath);
    if(saveData.isEmpty()) {
        clearSaveSelection();
        return;
    }
    updateSaveInfo(saveData);
    ui->pushButtonLoadGame->setEnabled(true);
    ui->pushButtonDeleteSave->setEnabled(true);
}
void GameLobbyWindow::updateSaveInfo(const QJsonObject& saveData) {
    QString info;
    info += "📁" + QFileInfo(selectSaveFile).fileName() + "\n";
    info += "─────────────────────\n";

    info +="🎮 Game: " + saveData["gameType"].toString() + "\n";

    QString saveTime = saveData["saveTime"].toString();
    if(!saveTime.isEmpty()){
        info += "💾 Saved: "+ saveTime + "\n";
    }
    QString currentPlayer = saveData["currentPlayer"].toString();
    if(!currentPlayer.isEmpty()) {
        info += "🎯 Turn: "+ currentPlayer + "\n";
    }
    QString lastMove = saveData["lastMove"].toString();
    if(!lastMove.isEmpty()) {
        info += "📝 Last Move: "+ lastMove + "\n";
    }
    int playerScore = saveData["playerScore"].toInt();
    int opponentScore = saveData["opponentScore"].toInt();
    info += "⭐ Score: " + QString::number(playerScore)+ "-"+QString::number(opponentScore) + "\n";
    int moveCount = saveData["moveCount"].toInt();
    info += "📊 Moves: " + QString::number(moveCount);
    ui->labelSaveInfo->setText(info);
}
GameConfig GameLobbyWindow::createConfig() {
    GameConfig config;
    config.setGameType(gameType);
    QComboBox * comboTimeLimit = ui->widgetConfigContent->findChild<QComboBox*>("comboTimeLimit");
    if(comboTimeLimit) {
        int timeLimit = comboTimeLimit->currentData().toInt();
        config.setHasTimeLimit(timeLimit > 0);
        config.setTimeLimit(timeLimit);
    }
    if(gameType == "DotsAndBoxes") {
        QComboBox * comboBoardSize = ui->widgetConfigContent->findChild<QComboBox*>("comboBoardSize");
        if(comboBoardSize) {
            int size = comboBoardSize->currentData().toInt();
            config.setDotsAndBoxesRows(size);
            config.setDotsAndBoxesColumns(size);
        }
    } else if(gameType == "Morris") {
        QCheckBox * checkFlying = ui->widgetConfigContent->findChild<QCheckBox*>("checkFlying");
        if(checkFlying) {
            config.setUseFlyingPhase(checkFlying->isChecked());
        }
    }
    return config;
}
void GameLobbyWindow::onChooseColorClicked() {
    QColor color = QColorDialog::getColor(selectedColor, this, "Select Your Color");
    if(color.isValid()) {
        selectedColor = color;
        ui->labelColorPreview->setStyleSheet(
            "background-color: " + color.name() + ";"
            "border-radius: 22px;"
            "border: 2px solid #2c3e50;"
            );
    }
}
void GameLobbyWindow::onStartGameClicked() {
    GameConfig config = createConfig();
    if (config.getGameType().isEmpty() || config.getGameType() == "Unknown") {
        QMessageBox::warning(this, "Error",
                             "Game type is not selected!\n"
                             "Please select a game type.");
        return;
    }
    if (config.getHasTimeLimit() && config.getTimeLimit() <= 0) {
        QMessageBox::warning(this, "Error",
                             "Invalid time limit!\n"
                             "Please select a valid time limit.");
        return;
    }
    emit gameStarted(config, selectedColor);
    this->close();
}
void GameLobbyWindow::onLoadGameClicked() {
    if(selectSaveFile.isEmpty()){
        QMessageBox::warning(this, "Error", "No save file selected!");
        return;
    }
    QJsonObject saveData = loadSaveFile(selectSaveFile);
    if(saveData.isEmpty()){
        QMessageBox::warning(this, "Error", "Failed to read save file");
        return;
    }
    QString playerColorStr = saveData["playerColor"].toString();
    QColor playerColor = QColor(playerColorStr);
    if(!playerColor.isValid()) {
        playerColor = selectedColor;
    }
    QString info = ui->labelSaveInfo->text();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Load Game", "Do you want to load this saved game?\n\n" + info, QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::No) {
        return;
    }
    emit gameLoaded(selectSaveFile, playerColor);
    this->close();
}
void GameLobbyWindow::onDeleteSaveClicked() {
    if(selectSaveFile.isEmpty()) {
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete save", "Are you sure you want to delete saved game?\n\n" + QFileInfo(selectSaveFile).fileName(), QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::No) {
        return;
    }
    QFile file(selectSaveFile);
    if(file.remove()) {
        QMessageBox::information(this, "Success", "Save file deleted successfully !");
        clearSaveSelection();
        loadSavedGames();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete save file!");
    }
}
void GameLobbyWindow::onRefreshHistoryClicked() {
    loadHistory();
    loadSavedGames();
    QMessageBox::information(this, "Refreshed", "History and saved games refreshed!");
}
void GameLobbyWindow::onBackClicked() {
    emit backToMenu();
    this->close();
}
void GameLobbyWindow::onJoinGameClicked() {
    QString ip = ui->lineEditServerIP->text().trimmed();
    QString portStr = ui->lineEditServerPort->text().trimmed();
    if(ip.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter server IP address !");
        ui->lineEditServerIP->setFocus();
        return;
    }
    if(portStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter server port!");
        ui->lineEditServerPort->setFocus();
        return;
    }
    bool ok;
    int port = portStr.toInt(&ok);
    if(!ok || port < 1024 || port > 65535) {
        QMessageBox::warning(this, "Error", "Invalid port! (1024-65535)");
        ui->lineEditServerPort->setFocus();
        ui->lineEditServerPort->selectAll();
        return;
    }
    QColor playerColor = selectedColor;
    emit joinGameRequested(ip, port, playerColor);
    this->close();

}