#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QDataStream>
#include <QUdpSocket>
#include <QTimer>
#include <QSet>

// Les classes forward-declared
class QNetworkSession;

class Server : public QObject {
    Q_OBJECT

public:
    Server();

private slots:
    void connexionClient(); // Anciennement ton slot pour gérer l'arrivée d'un client
    void lireTexte();       // Anciennement ton slot pour recevoir les données
    void clientDeconnecte(); // À AJOUTER : pour nettoyer la Map quand un client part
    void sendBeacon();

private:
    // MODIFICATION : On n'envoie plus à "un" client, mais à un client spécifique via sa clé
    void envoiTexte(QTcpSocket* destinataire, const QByteArray& data);


private:
    QTcpServer *m_tcp_server;

    // --- MODIFICATIONS MAJEURES POUR TON PROJET ---

    // 1. On remplace m_socket_client par une MAP
    // Clé (QString) : La clé publique (pseudo)
    // Valeur (QTcpSocket*) : Le lien vers le socket réel
    QMap<QString, QTcpSocket*> m_clients;
    QSet<QTcpSocket*> m_pendingSockets;
    // 2. Pour le protocole de lecture (évite de couper les messages)
    quint16 m_blockSize;

    // 3. Ta session réseau
    QNetworkSession *m_network_session;
    QUdpSocket *m_udpBeacon;
    QTimer *m_beaconTimer;

};

#endif
