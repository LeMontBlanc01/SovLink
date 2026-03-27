#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QTextStream>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QRandomGenerator>

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    void messageManualInput(); // Pour lire la console

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void processPendingDatagrams();

private:
    QTcpSocket *m_socket;
    QString m_myPubKey;
    QString m_targetPubKey;
    bool m_registered;
    QUdpSocket *m_udpListener;
};

#endif
