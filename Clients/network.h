#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpsocket>
class network : public QObject
{
    Q_OBJECT
public:
   explicit network(QObject *parent = nullptr);
    void connect_to_server();
    void envoyerMessage(const QString &message);
    void recevoirMessage();
    void sonnerie2(QString message);
    void connexionAuServeur(QString ip, int port);
private:
    QTcpSocket *connexion;
    QTcpSocket *socketClient; // C'est ici qu'on le déclare !
    network *instanceDeNetwork;
signals:
    void messageRecuPourInterface(QString message);
};

#endif // NETWORK_H
