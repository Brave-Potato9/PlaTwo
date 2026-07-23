#include <QApplication>
#include "WindowManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("PlaTwo Game Hub");
    app.setOrganizationName("PlaTwo");
    app.setApplicationVersion("1.0.0");
    
    app.setStyle("Fusion");
    
    WindowManager windowManager;
        
    return app.exec();
}