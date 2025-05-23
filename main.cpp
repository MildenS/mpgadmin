#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    //qDebug() << ":/" + QApplication::applicationName() + "_" + QLocale::system().name();
    translator.load(":/" + QApplication::applicationName() + "_" + QLocale::system().name());
    bool is_installed = a.installTranslator(&translator);
    //qDebug() << is_installed;
    StartWindow w;
    w.show();
    return a.exec();
}
