#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("PersonalLibrary");
    app.setOrganizationName("CourseWork");
    app.setApplicationVersion("1.0");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
