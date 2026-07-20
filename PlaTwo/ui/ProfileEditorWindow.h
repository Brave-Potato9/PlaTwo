#ifndef PROFILEEDITORWINDOW_H
#define PROFILEEDITORWINDOW_H

#include <QMainWindow>
#include <QObject>

#include "../auth/AuthManager.h"
#include "../utils/Validator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ProfileEditorWindow; }
QT_END_NAMESPACE

class ProfileEditorWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::ProfileEditorWindow *ui;
    AuthManager * authManager;
    QString username;

    // helper_methods
    void setupUI();
    void setupConnections();
    void loadPlayerData();
    bool validateInputs();
    void showError(const QString& message);
    void clearError();
    void setSaveButtonState(bool enabled, const QString& text = "");

public:
    explicit ProfileEditorWindow(AuthManager * authManager, const QString& username, QWidget * parent = nullptr);
    ~ProfileEditorWindow();

signals:
    void closed();
    void profileUpdated();

private slots:
    void onSaveClicked();
    void onChangePasswordClicked();
    void onCancelClicked();
};

#endif // PROFILEEDITORWINDOW_H