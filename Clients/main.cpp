#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; // Tout le "câblage" va se passer ici, à l'intérieur de 'w'
    w.show();
    return a.exec();
}
