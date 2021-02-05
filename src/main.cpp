#include "mainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc,char** argv)
{
    QApplication* app=new QApplication(argc,argv);
    mainWindow* wnd=new mainWindow();
    wnd->show();
    return app->exec();
}
