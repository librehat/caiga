#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //global QSetting
    QCoreApplication::setOrganizationName("SMSE SEU");
    QCoreApplication::setOrganizationDomain("smse.seu.edu.cn");
    QCoreApplication::setApplicationName("CAIGA");

#ifdef _WIN32
    //Windows should use packaged theme since its lacking of **theme**
    QIcon::setThemeName("Oxygen");
    //UI tweaks for Chinese fonts in Windows
    if (QLocale::system().country() == QLocale::China) {
        a.setFont(QFont("Microsoft Yahei", 9, QFont::Normal, false));
    }
#endif

    //Translations
    QTranslator coreTranslator;
    QTranslator caigaTranslator;
    coreTranslator.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    caigaTranslator.load(QLocale::system(), "caiga", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&coreTranslator);
    a.installTranslator(&caigaTranslator);

    //Main GUI
    MainWindow w;
    w.show();

    return a.exec();
}
