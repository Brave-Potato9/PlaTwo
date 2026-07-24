#include "ForgotPasswordWindow.h"
#include "ui_ForgotPasswordWindow.h"
#include <QMessageBox>
#include "../utils/Validator.h"
#include <QPropertyAnimation>
ForgotPasswordWindow::ForgotPasswordWindow(AuthManager* authManager, QWidget * parent) : QMainWindow(parent), ui(new Ui::ForgotPasswordWindow), authManager(authManager) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setWindowIcon(QIcon(":/app/app/app_icon.png"));
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(350);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    setupConnections();
    ui->labelPhoneError->setVisible(false);
    ui->labelConfirmPassError->setVisible(false);
    ui->labelNewPasswordError->setVisible(false);
    ui->lineEditNewPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditConfirmPassword->setEchoMode(QLineEdit::Password);
}
ForgotPasswordWindow::~ForgotPasswordWindow() {
    delete ui;
}
void ForgotPasswordWindow::setupConnections() {
    // connect_buttons
    connect(ui->pushButtonResetPassword, &QPushButton::clicked, this, &ForgotPasswordWindow::onResetClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &ForgotPasswordWindow::onCancelClicked);

    // Enter_in_fields
    connect(ui->lineEditPhoneNumber, &QLineEdit::returnPressed, this, &ForgotPasswordWindow::onResetClicked);
    connect(ui->lineEditNewPassword, &QLineEdit::returnPressed, this, &ForgotPasswordWindow::onResetClicked);
    connect(ui->lineEditConfirmPassword, &QLineEdit::returnPressed, this, &ForgotPasswordWindow::onResetClicked);
}
void ForgotPasswordWindow::setResetButtonState(bool enabled, const QString& text) {
    ui->pushButtonResetPassword->setEnabled(enabled);
    if(!text.isEmpty()) {
        ui->pushButtonResetPassword->setText(text);
    } else {
        ui->pushButtonResetPassword->setText(enabled ? "Reset Password" : "Changing Password...");
    }
}
bool ForgotPasswordWindow::validateInputs() {
    clearErrors();
    // validate_phone
    QString phone = ui->lineEditPhoneNumber->text().trimmed();
    if(phone.isEmpty()) {
        showError("phone", "Please Enter your phone number");
        return false;
    }
    if(!Validator::isValidPhone(phone)) {
        showError("phone", "Phone number must be 11 digits and starts with 09");
        return false;
    }

    // validate_New_Password
    QString newPassword = ui->lineEditNewPassword->text();
    if(newPassword.isEmpty()) {
        showError("password", "Please enter new password");
        return false;
    }
    if(!Validator::isValidPassword(newPassword)) {
        showError("password", "Password must be at least 8 characters\n(At least 0-9,A-a,#!,etc");
        return false;
    }

    // validate_confirm_password
    QString confirmPassword = ui->lineEditConfirmPassword->text();
    if(newPassword != confirmPassword) {
        showError("confirmPassword", "Password and confirmPassword are not same");
        return false;
    }
    return true;
}
void ForgotPasswordWindow::showError(const QString& field, const QString& message) {
    QString style =
    "QLineEdit {"
    "border: 2px solid #ef4444;"
    "border-radius: 7px;"
    "padding: 4px 9px;"
    "font-size: 9px;"
    "}";
    if(field == "phone") {
        ui->lineEditPhoneNumber->setStyleSheet(style);
        ui->labelPhoneError->setText(message);
        ui->labelPhoneError->setVisible(true);
        ui->lineEditPhoneNumber->setFocus();
    } else if(field == "password") {
        ui->lineEditNewPassword->setStyleSheet(style);
        ui->labelNewPasswordError->setText(message);
        ui->labelNewPasswordError->setVisible(true);
        ui->lineEditNewPassword->setFocus();
    } else if(field == "confirmPassword") {
        ui->lineEditConfirmPassword->setStyleSheet(style);
        ui->labelConfirmPassError->setText(message);
        ui->labelConfirmPassError->setVisible(true);
        ui->lineEditConfirmPassword->setFocus();
    }
}
void ForgotPasswordWindow::clearErrors() {
    QString normalStyle = "QLineEdit {"
    "border: 1px solid #dcdde1;"
    "border-radius: 7px;"
    "padding: 4px 9px;"
    "font-size: 9px;"
    "}"
    "QLineEdit:focus {"
    "border: 2px solid #855ead;"
    "}";
    ui->lineEditPhoneNumber->setStyleSheet(normalStyle);
    ui->lineEditNewPassword->setStyleSheet(normalStyle);
    ui->lineEditConfirmPassword->setStyleSheet(normalStyle);
    ui->labelPhoneError->setVisible(false);
    ui->labelConfirmPassError->setVisible(false);
    ui->labelNewPasswordError->setVisible(false);
}
void ForgotPasswordWindow::clearFields() {
    ui->lineEditPhoneNumber->clear();
    ui->lineEditNewPassword->clear();
    ui->lineEditConfirmPassword->clear();
    clearErrors();
}
void ForgotPasswordWindow::onResetClicked() {
    if(!validateInputs()) {
        return;
    }
    setResetButtonState(false);
    QString phone = ui->lineEditPhoneNumber->text().trimmed();
    QString newPassword = ui->lineEditNewPassword->text();
    if(authManager->resetPassword(phone, newPassword)) {
        QMessageBox::information(this, "Successful", "Password changed !");
        clearFields();
        emit passwordReset();
        this->close();
    } else {
        setResetButtonState(true);
        showError("phone", "Phone number not found!");
    }
}
void ForgotPasswordWindow::onCancelClicked() {
    emit canceled();
    this->close();
}