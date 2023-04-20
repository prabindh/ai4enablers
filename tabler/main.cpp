/*
 * Part of ai4enablers project. 2023
 *
*/
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
