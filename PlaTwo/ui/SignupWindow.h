#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H
#include <QObject>
#include <QMainWindow>
#include "auth/AuthManager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class SignupWindow; }
QT_END_NAMESPACE
class SignupWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::SignupWindow *ui;
    AuthManager* authManager;
    //helper_Methods
    void setUpConnections();
    bool validateInputs();
    void clearFields();
    void showError(const QString& field, const QString& message);
    void clearErrors();
    void setSignupButtonState(bool enabled, const QString& text = "");
public:
    // constructor_destructor
    explicit SignupWindow(AuthManager * authManager, QWidget* parent = nullptr);
    ~SignupWindow();
signals:
    void signupCompleted(const QString& username);
    void canceled();
private slots:
    void onSignupClicked();
    void onCancelClicked();
    void onLoginClicked();
    void onSignupSuccess(const QString& username);
    void onSignupFailed(const QString& reason);
};

#endif // SIGNUPWINDOW_H
