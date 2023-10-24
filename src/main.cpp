#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TUG w;
    w.show();
    return a.exec();
}

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
