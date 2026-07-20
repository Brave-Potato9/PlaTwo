#ifndef FORGOTPASSWORDWINDOW_H
#define FORGOTPASSWORDWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "../auth/AuthManager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class ForgotPasswordWindow; }
QT_END_NAMESPACE

class ForgotPasswordWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::ForgotPasswordWindow *ui;
    AuthManager * authManager;

    //helper_methods
    void setupConnections();
    bool validateInputs();
    void clearFields();
    void showError(const QString& field, const QString& message);
    void clearErrors();
    void setResetButtonState(bool enabled, const QString& text = "" );
public:
    // constructor_destructor
    explicit ForgotPasswordWindow(AuthManager * authManager, QWidget * parent = nullptr);
    ~ForgotPasswordWindow();

signals:
    void passwordReset();
    void canceled();
private slots:
    void onResetClicked();
    void onCancelClicked();


};

#endif // FORGOTPASSWORDWINDOW_H
