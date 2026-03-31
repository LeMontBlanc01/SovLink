#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QDataStream>
#include <QUdpSocket>
#include <QTimer>

class QNetworkSession;

class Server : public QObject {
    Q_OBJECT

public:
    Server();

private slots:
    void connexionClient();
    void lireTexte();
    void clientDeconnecte();
    void sendBeacon();

private:
    void envoiTexte(QTcpSocket* destinataire, const QByteArray& data);

private:
    QTcpServer       *m_tcp_server;
    QMap<QString, QTcpSocket*> m_clients;   // clePublique -> socket
    QSet<QTcpSocket*>          m_pendingSockets;
    QMap<QTcpSocket*, quint16> m_blockSizes;
    QNetworkSession  *m_network_session = nullptr;
    QUdpSocket       *m_udpBeacon;
    QTimer           *m_beaconTimer;
};

#endif
