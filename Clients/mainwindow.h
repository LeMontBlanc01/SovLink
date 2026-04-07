#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QObject>
#include <QMainWindow>
#include <QTcpSocket>
#include "network.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sonnerie1();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    network *instanceDeNetwork;

private slots:
    void on_Bouton_send_clicked();



};
#endif // MAINWINDOW_H
