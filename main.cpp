#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QIcon icon(":/icons/icons/logo.ico");
    w.setWindowIcon(icon);
    w.setWindowTitle("基于QT的多参数生理信号采集上位机 Ver:0.9.9");
    w.show();
    return a.exec();
}


