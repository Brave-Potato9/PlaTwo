#include "MainMenuWindow.h"
#include "ui_MainMenuWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QPropertyAnimation>
MainMenuWindow::MainMenuWindow(AuthManager* _authManager, const QString& _username, QWidget* _parent)
    :QMainWindow(_parent)
    ,ui(new Ui::MainMenuWindow)
    ,authManager(_authManager)
    ,username(_username)

{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/app/app/app_icon.png"));
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(350);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
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
    ui->labelUsername->setText("👤 "+username);
    Player player = authManager->getCurrentPlayerData();
    ui->labelEmail->setText("📧"+player.getEmail());
    ui->labelName->setText(player.getName());
}
void MainMenuWindow::setupGameButtons() {
    ui->pushButtonDotsAndBoxes->setStyleSheet(
        "QPushButton {"
        "    border-image: url(:/icons/icons/dotsandboxes_icon.png);"
        "    font: 15pt Haettenschweiler;"
        "    padding-top: 120px;"
        "    color: rgba(42,42,42,80);"
        "    border: none;"
        "    border-radius: 10px;"
        "    background-color: transparent;"
        "}"
        "QPushButton:hover {"
        "    border: 2px solid #e74c3c;"
        "    background-color: rgba(231, 76, 60, 0.2);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(231, 76, 60, 0.4);"
        "}"
        );
    ui->pushButtonDotsAndBoxes->setText("Dots and Boxes");
    ui->pushButtonMorris->setStyleSheet(
        "QPushButton {"
        "    border-image: url(:/icons/icons/morris_icon.png);"
        "    font: 15pt Haettenschweiler;"
        "    padding-top: 120px;"
        "    border: none;"
        "    border-radius: 10px;"
        "    background-color: transparent;"
        "}"
        "QPushButton:hover {"
        "    border: 2px solid #e74c3c;"
        "    background-color: rgba(231, 76, 60, 0.2);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(231, 76, 60, 0.4);"
        "}"
        );
    ui->pushButtonMorris->setText("Nine Men's Morris");
    ui->pushButtonFanorona->setStyleSheet(
        "QPushButton {"
        "    border-image: url(:/icons/icons/fanorona_icon.png);"
        "    font: 20pt Haettenschweiler;"
        "    padding-top: 120px;"
        "    border: none;"
        "    border-radius: 10px;"
        "    background-color: transparent;"
        "}"
        "QPushButton:hover {"
        "    border: 2px solid #e74c3c;"
        "    background-color: rgba(231, 76, 60, 0.2);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(231, 76, 60, 0.4);"
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

