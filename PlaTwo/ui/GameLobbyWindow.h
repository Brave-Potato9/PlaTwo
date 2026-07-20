#ifndef GAMELOBBYWINDOW_H
#define GAMELOBBYWINDOW_H
#include <QMainWindow>
#include <QComboBox>
#include <QSpinBox>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include "../auth/AuthManager.h"
#include "../core/GameHistoryManager.h"
#include "../utils/GameConfig.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameLobbyWindow; }
QT_END_NAMESPACE

class GameLobbyWindow: public QMainWindow
{
    Q_OBJECT
private:
    QString gameType;
    QString gameDisplayName;
    Ui::GameLobbyWindow * ui;
    AuthManager* authManager;
    QString username;
    GameHistoryManager historyManager;
    QColor selectedColor;
    QString selectSaveFile;
    void setupUI();
    void setupConnections();
    void setupGameSpecificUI();
    void loadHistory();
    void loadSavedGames();
    void addHistoryToTable(const GameHistory& history, int row);
    void updateStats();
    GameConfig createConfig();
    QString getGameDisplayName() const;
    QColor getDefaultColor() const;
    QString getSavesDirectory() const;
    QJsonObject loadSaveFile(const QString& filePath);
    void updateSaveInfo(const QJsonObject& saveDate);
    void clearSaveSelection();
public:
    explicit GameLobbyWindow(const QString& gameType, AuthManager* authManager, const QString& username, QWidget* parent = nullptr);
    ~GameLobbyWindow();
signals:
    void gameStarted(const GameConfig& config, const QColor& playerColor);
    void gameLoaded(const QString& saveFile, const QColor& playerColor);
    void backToMenu();
    void joinGameRequested( const QString& ip, int port, const QColor& playerColor);
private slots:
    void onStartGameClicked();
    void onLoadGameClicked();
    void onDeleteSaveClicked();
    void onBackClicked();
    void onRefreshHistoryClicked();
    void onChooseColorClicked();
    void onJoinGameClicked();

    void onSaveItemClicked(QListWidgetItem * item);


};

#endif // GAMELOBBYWINDOW_H
