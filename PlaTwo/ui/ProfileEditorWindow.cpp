#include "ProfileEditorWindow.h"
#include "ui_ProfileEditorWindow.h"
#include <QMessageBox>
#include <QPropertyAnimation>
ProfileEditorWindow::ProfileEditorWindow(AuthManager* authManager, const QString& _username, QWidget * parent)
    : QMainWindow(parent)
    , ui(new Ui::ProfileEditorWindow)
    , authManager(authManager)
    , username(_username)
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
    loadPlayerData();

}
ProfileEditorWindow::~ProfileEditorWindow() {
    delete ui;
}
void ProfileEditorWindow::setupUI() {
    setWindowTitle("Edit Profile - PlaTwo");
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(625,565);
    ui->lineEditUsername->setEnabled(false);
    ui->labelError->setVisible(false);
    ui->labelPasswordError->setVisible(false);
    ui->lineEditOldPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditNewPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditConfirmPassword->setEchoMode(QLineEdit::Password);

}
void ProfileEditorWindow::setupConnections() {
    connect(ui->pushButtonSaveChange, &QPushButton::clicked, this, &ProfileEditorWindow::onSaveClicked);
    connect(ui->pushButtonChangePassword, &QPushButton::clicked, this, &ProfileEditorWindow::onChangePasswordClicked);
    connect(ui->BackButton, &QPushButton::clicked, this, &ProfileEditorWindow::onCancelClicked);
}
void ProfileEditorWindow::loadPlayerData() {
    Player player;
    if(authManager->getPlayerManager().findPlayer(username, player)) {
        ui->lineEditUsername->setText(player.getUsername());
        ui->lineEditName->setText(player.getName());
        ui->lineEditPhone->setText(player.getPhoneNumber());
        ui->lineEditEmail->setText(player.getEmail());
    }
}
void ProfileEditorWindow::setSaveButtonState(bool enabled, const QString& text) {
    ui->pushButtonSaveChange->setEnabled(enabled);
    if(!text.isEmpty()) {
        ui->pushButtonSaveChange->setText(text);
    } else {
        ui->pushButtonSaveChange->setText(enabled ? "Save Changes " : "Saving...");
    }
}
bool ProfileEditorWindow::validateInputs() {
    clearError();

    QString name = ui->lineEditName->text().trimmed();
    if(name.isEmpty()) {
        showError("Please enter your full name");
        ui->lineEditName->setFocus();
        return false;
    }
    QString phone = ui->lineEditPhone->text().trimmed();
    if(!phone.isEmpty() && !Validator::isValidPhone(phone)) {
        showError("Phone number must be 11 digits and start with 09");
        ui->lineEditPhone->setFocus();
        return false;
    }
    QString email = ui->lineEditEmail->text().trimmed();
    if(!email.isEmpty() && !Validator::isValidEmail(email)) {
        showError("Invalid email addresss");
        return false;
    }
    return true;
}
void ProfileEditorWindow::showError(const QString& message) {
    ui->labelError->setText("⚠ "+ message);
    ui->labelError->setVisible(true);
    ui->labelError->setStyleSheet("color: #ef4444; font-size: 7px;");
}
void ProfileEditorWindow::clearError() {
    ui->labelError->setVisible(false);
    ui->labelError->clear();
}
void ProfileEditorWindow::onSaveClicked() {
    if(!validateInputs()) {
        return;
    }
    setSaveButtonState(false);
    Player player;
    if(!authManager->getPlayerManager().findPlayer(username, player)) {
        setSaveButtonState(true);
        showError("Player not found!");
        return;
    }
    player.setName(ui->lineEditName->text().trimmed());
    player.setPhoneNumber(ui->lineEditPhone->text().trimmed());
    player.setEmail(ui->lineEditEmail->text().trimmed());
    if(authManager->getPlayerManager().updatePlayer(player)) {
        setSaveButtonState(true);
        QMessageBox::information(this, "Success", "Profile updated successfully!");
        emit profileUpdated();
        this->close();
    } else {
        setSaveButtonState(true);
        showError("Failed to update profile!");
    }
}
void ProfileEditorWindow::onChangePasswordClicked() {
    QString oldPassword = ui->lineEditOldPassword->text();
    QString newPassword = ui->lineEditNewPassword->text();
    QString confirmPassword = ui->lineEditConfirmPassword->text();

    ui->labelPasswordError->setVisible(false);

    if(oldPassword.isEmpty()) {
        ui->labelPasswordError->setText("⚠ Please enter current password");
        ui->labelPasswordError->setVisible(true);
        ui->labelPasswordError->setStyleSheet("color: #ef4444; font-size: 7px;");
        ui->lineEditOldPassword->setFocus();
        return;
    }
    if(newPassword.isEmpty()) {
        ui->labelPasswordError->setText("⚠ Password must be at least 8 characters");
        ui->labelPasswordError->setVisible(true);
        ui->lineEditOldPassword->setFocus();
        return;
    }
    if(!Validator::isValidPassword(newPassword)) {
        ui->labelPasswordError->setText("⚠ Password must have digits,A-a,#!");
        ui->labelPasswordError->setVisible(true);
        ui->lineEditNewPassword->setFocus();
        return;
    }
    if(newPassword != confirmPassword) {
        ui->labelPasswordError->setText("⚠ Passwords do not match");
        ui->labelPasswordError->setVisible(true);
        ui->lineEditConfirmPassword->setFocus();
        return;
    }
    if(authManager->changePassword(username, oldPassword, newPassword)) {
        QMessageBox::information(this, "Success", "Password changed successfully!");
        ui->lineEditOldPassword->clear();
        ui->lineEditNewPassword->clear();
        ui->lineEditConfirmPassword->clear();
        ui->labelPasswordError->setVisible(false);
    } else {
        ui->labelPasswordError->setText("⚠ Current Password is incorrect");
        ui->labelPasswordError->setVisible(true);
        ui->labelPasswordError->setStyleSheet("color: #ef4444; font-size: 7px;");
        ui->lineEditOldPassword->setFocus();
        ui->lineEditOldPassword->selectAll();
    }

}
void ProfileEditorWindow::onCancelClicked() {
    emit closed();
    this->close();
}
