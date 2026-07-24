#include <QApplication>
#include "WindowManager.h"
#include <QIcon>
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QIcon appIcon(":/app/app_icon.png");
    if (!appIcon.isNull()) {
        app.setWindowIcon(appIcon);
        qDebug() << "✅ App icon loaded successfully!";
    } else {
        qDebug() << "❌ Failed to load app icon!";
    }
    app.setApplicationName("PlaTwo Game Hub");
    app.setOrganizationName("PlaTwo");
    app.setApplicationVersion("1.0.0");
    
    app.setStyle("Fusion");
    
    WindowManager windowManager;
        
    return app.exec();
}