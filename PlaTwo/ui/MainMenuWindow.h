#ifndef MAINMENUWINDOW_H
#define MAINMENUWINDOW_H
#include <QObject>
#include <QMainWindow>
#include "../auth/AuthManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenuWindow; }
QT_END_NAMESPACE

class MainMenuWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainMenuWindow * ui;
    AuthManager* authManager;
    QString username;
    void setupUI();
    void setupConnections();
    void setupGameButtons();
public:
    explicit MainMenuWindow(AuthManager* _authManger, const QString& _username, QWidget* _parent=nullptr);
    ~MainMenuWindow();
signals:
    void gameSelected(const QString& gameType, const QString& username);
    void profileEditRequested(const QString& username);
    void logoutRequested();
public slots:
    void setUsername(const QString& _username);
private slots:
    void onDotsAndBoxesClicked();
    void onMorrisClicked();
    void onFanoronaClicked();
    void onProfileClicked();
    void onLogoutClicked();
    void onExitClicked();

};

#endif // MAINMENUWINDOW_H
