#include "SignupWindow.h"
#include "ui_SignupWindow.h"
#include <QMessageBox>
#include "../utils/Validator.h"
//------------------------------------ constructor_destructor ------------------------------------
SignupWindow::SignupWindow(AuthManager * authManager, QWidget * parent) : QMainWindow(parent), ui(new Ui::SignupWindow),  authManager(authManager) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setUpConnections();
    ui->labelNameError->setVisible(false);
    ui->labelUsernameError->setVisible(false);
    ui->labelEmailError->setVisible(false);
    ui->labelPhoneError->setVisible(false);
    ui->labelPasswordError->setVisible(false);
    ui->labelConfirmpassError->setVisible(false);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditConfirmPassword->setEchoMode(QLineEdit::Password);
}
SignupWindow::~SignupWindow() {
    delete ui;
}
//------------------------------------ helper_methods ------------------------------------
void SignupWindow::setUpConnections() {
    // connect_buttons
    connect(ui->pushButtonSignUp, &QPushButton::clicked, this, &SignupWindow::onSignupClicked);
    connect(ui->pushButtonLogin, &QPushButton::clicked, this, &SignupWindow::onCancelClicked);
    connect(ui->BackButton, &QPushButton::clicked, this, &SignupWindow::onCancelClicked);

    // Enter_in_fields
    connect(ui->lineEditUsername, &QLineEdit::returnPressed, this, &SignupWindow::onSignupClicked);
    connect(ui->lineEditPassword, &QLineEdit::returnPressed, this, &SignupWindow::onSignupClicked);
    connect(ui->lineEditConfirmPassword, &QLineEdit::returnPressed, this, &SignupWindow::onSignupClicked);
    connect(ui->lineEditName, &QLineEdit::returnPressed, this, &SignupWindow::onSignupClicked);
    connect(ui->lineEditPhone, &QLineEdit::returnPressed, this, &SignupWindow::onSignupClicked);
    connect(ui->lineEditEmail, &QLineEdit::returnPressed, this, &SignupWindow::onSignupClicked);

    // connect_signals_of_authManager
    connect(authManager, &AuthManager::signupSuccess, this, &SignupWindow::onSignupSuccess);
    connect(authManager, &AuthManager::signupFailed, this, &SignupWindow::onSignupFailed);

}
void SignupWindow::setSignupButtonState(bool enabled, const QString& text) {
    ui->pushButtonSignUp->setEnabled(enabled);
    if(!text.isEmpty()) {
        ui->pushButtonSignUp->setText(text);
    } else {
        ui->pushButtonSignUp->setText(enabled ? "Sign up" : "Signing up ...");
    }
}
bool SignupWindow::validateInputs() {
    clearErrors();
    // validate_username
    QString username = ui->lineEditUsername->text().trimmed();
    if(username.isEmpty()) {
        showError("username", "Please Enter Username");
        return false;
    }
    if(username.length() < 3) {
        showError("username", "Username should have 3 characters(at least)");
        return false;
    }
    // validate_Password
    QString password = ui->lineEditPassword->text();
    if(password.isEmpty()) {
        showError("password", "Please Enter Password");
        return false;
    }
    if(password.length() < 8) {
        showError("password", "Password should have 8 characters(at least)");
        return false;
    }

    // validate_repeat_password
    QString confirmPassword = ui->lineEditConfirmPassword->text();
    if(password != confirmPassword) {
        showError("confirmPassword", "Password and repeat it does not match");
        return false;
    }

    // validate_Name
    QString name = ui->lineEditName->text().trimmed();
    if(name.isEmpty()) {
        showError("name", "Please enter name");
        return false;
    }

    // validate_phone
    QString phone = ui->lineEditPhone->text().trimmed();
    if(!phone.isEmpty()) {
        if(!Validator::isValidPhone(phone)) {
            showError("phone", "Phone number should be 11 numbers and starts with 09");
            return false;
        }
    }

    // validate_Email
    QString email = ui->lineEditEmail->text().trimmed();
    if(!email.isEmpty()){
        if(!Validator::isValidEmail(email)) {
            showError("email", "Email address is invalid");
            return false;
        }
    }
    return true;
}
void SignupWindow::showError(const QString& field, const QString& message) {
    QString style = "QLineEdit {"
    "border: 2px solid #ef4444;"
    "border-radius: 7px;"
    "padding: 4px 9px;"
    "font-size: 9px;"
    "}";
    if(field == "username") {
        ui->lineEditUsername->setStyleSheet(style);
        ui->labelUsernameError->setText(message);
        ui->labelUsernameError->setVisible(true);
        ui->lineEditUsername->setFocus();
    } else if (field == "password") {
        ui->lineEditPassword->setStyleSheet(style);
        ui->labelPasswordError->setText(message);
        ui->labelPasswordError->setVisible(true);
        ui->lineEditPassword->setFocus();
    } else if (field == "confirmPassword") {
        ui->lineEditConfirmPassword->setStyleSheet(style);
        ui->labelConfirmpassError->setText(message);
        ui->labelConfirmpassError->setVisible(true);
        ui->lineEditConfirmPassword->setFocus();
    } else if (field == "name") {
        ui->lineEditName->setStyleSheet(style);
        ui->labelNameError->setText(message);
        ui->labelNameError->setVisible(true);
        ui->lineEditName->setFocus();
    } else if (field == "phone") {
        ui->lineEditPhone->setStyleSheet(style);
        ui->labelPhoneError->setText(message);
        ui->labelPhoneError->setVisible(true);
        ui->lineEditPhone->setFocus();
    } else if (field == "email") {
        ui->lineEditEmail->setStyleSheet(style);
        ui->labelEmailError->setText(message);
        ui->labelEmailError->setVisible(true);
        ui->lineEditEmail->setFocus();
    }
}
void SignupWindow::clearErrors() {
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
    ui->lineEditConfirmPassword->setStyleSheet(normalStyle);
    ui->lineEditName->setStyleSheet(normalStyle);
    ui->lineEditPhone->setStyleSheet(normalStyle);
    ui->lineEditEmail->setStyleSheet(normalStyle);
    ui->labelUsernameError->setVisible(false);
    ui->labelPasswordError->setVisible(false);
    ui->labelConfirmpassError->setVisible(false);
    ui->labelNameError->setVisible(false);
    ui->labelPhoneError->setVisible(false);
    ui->labelEmailError->setVisible(false);
}
void SignupWindow::clearFields() {
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->lineEditConfirmPassword->clear();
    ui->lineEditName->clear();
    ui->lineEditPhone->clear();
    ui->lineEditEmail->clear();
    clearErrors();
}
//------------------------------------ slots ------------------------------------
void SignupWindow::onSignupClicked() {
    if(!validateInputs()) {
        return;
    }
    setSignupButtonState(false);
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();
    QString name = ui->lineEditName->text().trimmed();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();
    authManager->signup(username, password, name, phone, email);
}
void SignupWindow::onCancelClicked() {
    emit canceled();
    this->close();
}
void SignupWindow::onLoginClicked() {
    emit canceled();
    this->close();
}
void SignupWindow::onSignupSuccess(const QString& username) {
    setSignupButtonState(true);
    QMessageBox::information(this, "Successful", "Sign up successfully.Sign in with username: " + username);
    clearFields();
    emit signupCompleted(username);
    this->close();
}
void SignupWindow::onSignupFailed(const QString& reason) {
    setSignupButtonState(true);
    QMessageBox::warning(this, "Error", reason);
}
