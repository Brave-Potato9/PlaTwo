#include "MainMenuWindow.h"
#include "ui_MainMenuWindow.h"
#include <QMessageBox>
#include <QDebug>
MainMenuWindow::MainMenuWindow(AuthManager* _authManager, const QString& _username, QWidget* _parent)
    :QMainWindow(_parent)
    ,ui(new Ui::MainMenuWindow)
    ,authManager(_authManager)
    ,username(_username)

{
    ui->setupUi(this);
    setupUI();
    setupConnections();

}
MainMenuWindow::~MainMenuWindow() {
    delete ui;
}
void MainMenuWindow::setupUI() {
    setWindowTitle("PlaTwo - Main Menu");
    setMinimumSize(700,500);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint );
    setupGameButtons();
}
void MainMenuWindow::setupGameButtons() {
    ui->pushButtonDotsAndBoxes->setStyleSheet(
        "QPushButton {"
        "   color: rgba(42,42,42,200);"
        "   border-radius: 10px;"
        "   padding-top: 125px;"
        "   font: 15pt Haettenschweiler"
        "}"
        "QPushButton:hover {"
        "   color: rgba(42,42,42,300);"
        "}"
        );
    ui->pushButtonDotsAndBoxes->setText("Dots and Boxes");
    ui->pushButtonMorris->setStyleSheet(
        "QPushButton {"
        "   color: rgba(42,42,42,200);"
        "   border-radius: 10px;"
        "   padding-top: 125px;"
        "   font: 15pt Haettenschweiler"
        "}"
        "QPushButton:hover {"
        "   color: rgba(42,42,42,300);"
        "}"
        );
    ui->pushButtonMorris->setText("Nine Men's Morris");
    ui->pushButtonFanorona->setStyleSheet(
        "QPushButton {"
        "   color: rgba(42,42,42,200);"
        "   border-radius: 10px;"
        "   padding-top: 125px;"
        "   font: 15pt Haettenschweiler"
        "}"
        "QPushButton:hover {"
        "   color: rgba(42,42,42,300);"
        "}"
        );
    ui->pushButtonFanorona->setText("Fanorona");

    ui->pushButtonProfile->setStyleSheet(
        "QPushButton {"
        "   background-color: #9b59b6;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #8e44ad;"
        "}"
    );
    ui->pushButtonLogout->setStyleSheet(
        "QPushButton {"
        "   background-color: #e67e22;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #d35400;"
        "}"
        );
    ui->pushButtonExit->setStyleSheet(
        "QPushButton {"
        "   background-color: #e74c3c;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c0392b;"
        "}"
        );
}
void MainMenuWindow::setupConnections() {
    connect(ui->pushButtonDotsAndBoxes, &QPushButton::clicked, this, &MainMenuWindow::onDotsAndBoxesClicked);
    connect(ui->pushButtonMorris, &QPushButton::clicked, this, &MainMenuWindow::onMorrisClicked);
    connect(ui->pushButtonFanorona, &QPushButton::clicked, this, &MainMenuWindow::onFanoronaClicked);
    connect(ui->pushButtonProfile, &QPushButton::clicked, this, &MainMenuWindow::onProfileClicked);
    connect(ui->pushButtonLogout, &QPushButton::clicked, this, &MainMenuWindow::onLogoutClicked);
    connect(ui->pushButtonExit, &QPushButton::clicked, this, &MainMenuWindow::onExitClicked);

}
void MainMenuWindow::setUsername(const QString& _username) {
    username = _username;
}
void MainMenuWindow::onDotsAndBoxesClicked() {
    emit gameSelected("DotsAndBoxes", username);
}
void MainMenuWindow::onMorrisClicked() {
    emit gameSelected("Morris", username);
}
void MainMenuWindow::onFanoronaClicked() {
    emit gameSelected("Fanorona", username);
}
void MainMenuWindow::onProfileClicked() {
    emit profileEditRequested(username);
}
void MainMenuWindow::onLogoutClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Logout",
    "Are you sure you want to logout?",QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        emit logoutRequested();
        this->close();
    }
}
void MainMenuWindow::onExitClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit",
    "Are you sure you want to exit the application?",QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}

