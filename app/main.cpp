#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //global QSetting
    QCoreApplication::setOrganizationName("SMSE SEU");
    QCoreApplication::setOrganizationDomain("smse.seu.edu.cn");
    QCoreApplication::setApplicationName("CAIGA");

    //Main GUI
    MainWindow w;
    w.show();
    return a.exec();
}
