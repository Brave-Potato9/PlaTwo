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
    QComboBox* comboBoardSizeOfDAB = nullptr;
    // setup_UI
    void setupUI();
    void setupConnections();
    void setupGameSpecificUI();
    // games_history
    void loadHistory();
    void updateStats();
    void addHistoryToTable(const GameHistory& history, int row);
    // manage_saves
    void loadSavedGames();
    void updateSaveInfo(const QJsonObject& saveDate);
    void clearSaveSelection();
    QString getSavesDirectory() const;
    QJsonObject loadSaveFile(const QString& filePath);
    // game settings
    GameConfig createConfig();
    QColor getDefaultColor() const;
    QString getGameDisplayName() const;
    
    
public:
    // constructor_destructor
    explicit GameLobbyWindow(const QString& gameType, AuthManager* authManager, const QString& username, QWidget* parent = nullptr);
    ~GameLobbyWindow();
signals:
    void gameStarted(const GameConfig& config, const QColor& playerColor);
    void gameLoaded(const QString& saveFile, const QColor& playerColor, int port);
    void backToMenu();
    void joinGameRequested( const QString& ip, int port, const QColor& playerColor, const QString& gameType);
private slots:
    // start/connect
    void onStartGameClicked();
    void onJoinGameClicked();
    // manage_saves
    void onLoadGameClicked();
    void onDeleteSaveClicked();
    void onSaveItemClicked(QListWidgetItem * item);
    // game_history
    void onRefreshHistoryClicked();
    // game_settings
    void onChooseColorClicked();
    // back_to_menu
    void onBackClicked();




};

#endif // GAMELOBBYWINDOW_H
