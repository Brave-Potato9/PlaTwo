#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
LoginWindow::LoginWindow(AuthManager * authManager, QWidget * parent)
    : QMainWindow(parent)
    , authManager(authManager)
    , ui(new Ui::LoginWindow)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setUpUI();
    setUpConnections();
    checkRememberMe();
}
LoginWindow::~LoginWindow() {
    delete ui;
}
void LoginWindow::setUpUI() {
    setWindowTitle("PlaTwo Game Hub");
    setFixedSize(625, 565);
    ui->lineEditUsername->setFocus();
    ui->pushButtonLogin->setDefault(true);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password); // for mask
}
void LoginWindow::setUpConnections() {
    // connect_buttons
    connect(ui->pushButtonLogin, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(ui->pushButtonSignUp, &QPushButton::clicked, this, &LoginWindow::onSignupClicked);
    connect(ui->forgetPasswordButton, &QPushButton::clicked, this, &LoginWindow::onForgotPasswordClicked);
    connect(ui->exitButton, &QPushButton::clicked, this , &LoginWindow::onExitClicked);

    // connect_Enter_of_lineEdits
    connect(ui->lineEditUsername, &QLineEdit::returnPressed, this,  &LoginWindow::onLoginClicked);
    connect(ui->lineEditPassword, &QLineEdit::returnPressed, this , &LoginWindow::onLoginClicked);

    // connect_AuthManager_signals
    connect(authManager, &AuthManager::loginSuccess, this, &LoginWindow::onLoginSuccess);
    connect(authManager, &AuthManager::loginFailed, this, &LoginWindow::onLoginFailed);
}
void LoginWindow::setLoginButtonState(bool enabled, const QString& text) {
    ui->pushButtonLogin->setEnabled(enabled);
    if(!text.isEmpty()) {
        ui->pushButtonLogin->setText(text);
    } else {
        ui->pushButtonLogin->setText(enabled ? "Login" : "Logggin in...");
    }
}
void LoginWindow::clearFields() {
    QString normalStyle = "QLineEdit {"
    "border: 1px solid #dcdde1;"
    "border-radius: 7px;"
    "padding: 4px 9px;"
    "font-size: 9px;"
    "}"
    "QLineEdit:focus {"
    "border: 2px solid #855ead;"
    "}";
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->lineEditUsername->setStyleSheet(normalStyle);
    ui->lineEditPassword->setStyleSheet(normalStyle);
    ui->lineEditUsername->setFocus();

}
void LoginWindow::checkRememberMe() {
    QSettings settings("Ap project", "PlaTwo");
    if(settings.value("rememberMe", false).toBool()) {
        QString username = settings.value("username").toString();
        QString password = settings.value("password").toString();
        if(!username.isEmpty() && !password.isEmpty()) {
            ui->lineEditUsername->setText(username);
            ui->checkBoxRememberMe->setChecked(true);
            if(authManager->loginWithHash(username, password)) {
                return;
            }
        }
    }
    ui->lineEditUsername->setFocus();
}
void LoginWindow::saveRememberMe(const QString& username) {
    QSettings settings("Ap project", "PlaTwo");
    settings.setValue("rememberMe", true);
    settings.setValue("username", username);
    Player player;
    if(authManager->getPlayerManager().findPlayer(username, player)) {
        settings.setValue("password", player.getPassword());
    }
}
void LoginWindow::clearRememberMe() {
    QSettings settings("Ap project", "PlaTwo");
    settings.remove("rememberMe");
    settings.remove("username");
    settings.remove("password");
}
void LoginWindow::onLoginClicked() {
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();
    if(username.isEmpty()) {
        ui->lineEditUsername->setStyleSheet(
            "border: 2px solid #EF4444; border-radius: 7px; padding: 4px 9px");
        QMessageBox::warning(this, "Error", "Please Enter Username");
        ui->lineEditUsername->setFocus();
        return;
    }
    if(password.isEmpty()) {
        ui->lineEditPassword->setStyleSheet("border: 2px solid #EF4444; border-radius: 7px; padding: 4px 9px");
        QMessageBox::warning(this, "Error", "Please Enter Password");
        ui->lineEditPassword->setFocus();
        return;
    }
    setLoginButtonState(false);
    authManager->login(username, password);
}
void LoginWindow::onExitClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit", "Are you sure to close app?",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
void LoginWindow::onSignupClicked() {
    emit signupRequested();
}
void LoginWindow::onForgotPasswordClicked() {
    emit forgotPasswordRequested();
}

void LoginWindow::onLoginSuccess(const QString& username) {
    setLoginButtonState(true);
    QString normalStyle = "QLineEdit {"
                          "border: 1px solid #dcdde1;"
                          "border-radius: 7px;"
                          "padding: 4px 9px;"
                          "font-size: 9px;"
                          "}"
                          "QLineEdit:focus {"
                          "border: 2px solid #855ead;"
                          "}";
    ui->lineEditUsername->setStyleSheet(normalStyle);
    ui->lineEditPassword->setStyleSheet(normalStyle);
    if(ui->checkBoxRememberMe->isChecked()) {
        saveRememberMe(username);
    } else {
        clearRememberMe();
    }
    QMessageBox::information(this, "Welcome","Welcome "+ username +"!");
    emit loginSuccess(username);
}
void LoginWindow::onLoginFailed(const QString& reason) {
    setLoginButtonState(true);
    QMessageBox::warning(this, "Error", reason);
    ui->lineEditUsername->setStyleSheet(
        "border: 2px solid #ef4444; border-radius: 7px; padding: 4px 9px;"
        );
    ui->lineEditPassword->setStyleSheet(
        "border: 2px solid #ef4444; border-radius: 7px; padding: 4px 9px;"
        );
    ui->lineEditPassword->clear();
    ui->lineEditPassword->setFocus();
}
