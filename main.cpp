#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QIcon icon(":/icons/icons/logo.ico");
    w.setWindowIcon(icon);
    w.setWindowTitle("SKY多参数串口助手");
    w.show();
    return a.exec();
}


