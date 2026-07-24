#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../auth/AuthManager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE
class LoginWindow : public QMainWindow
{
    Q_OBJECT
private:
    AuthManager * authManager;
    Ui::LoginWindow * ui;
    // helper_methods
    void setUpUI();
    void setUpConnections();
    void setLoginButtonState(bool enabled, const QString& text = "");
    void saveRememberMe(const QString& username);
    void clearRememberMe();
    void clearFields();
public:
    //constructor_destructor
    LoginWindow(AuthManager * authManager, QWidget * parent = nullptr);
    ~LoginWindow();
    void checkRememberMe();
signals:
    void signupRequested();
    void forgotPasswordRequested();
    void loginSuccess(const QString& username);
private slots:
    //button_slots
    void onLoginClicked();
    void onSignupClicked();
    void onForgotPasswordClicked();
    void onExitClicked();

    //auth_slots
    void onLoginSuccess(const QString& username);
    void onLoginFailed(const QString& reason);
};

#endif // LOGINWINDOW_H
