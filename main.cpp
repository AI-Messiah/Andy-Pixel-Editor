#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setMinimumSize(1040, 784);
    w.setMaximumSize(1040, 784);
    w.show();
    return a.exec();
}
