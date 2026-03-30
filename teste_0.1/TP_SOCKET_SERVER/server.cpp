#include "server.h"
#include <QDataStream>
#include <QDebug>

Server::Server() : m_blockSize(0) {
    m_tcp_server = new QTcpServer(this);

    if (!m_tcp_server->listen(QHostAddress::Any, 8080)) {
        qDebug() << "Erreur : Le serveur n'a pas pu demarrer !";
    } else {
        qDebug() << "Serveur demarre sur le port 8080.";
        m_udpBeacon = new QUdpSocket(this);
        m_beaconTimer = new QTimer(this);
        connect(m_beaconTimer, &QTimer::timeout, this, &Server::sendBeacon);
        m_beaconTimer->start(2000);
        connect(m_tcp_server, &QTcpServer::newConnection, this, &Server::connexionClient);
        qDebug() << "Balise UDP active.";
    }
}

void Server::connexionClient() {
    QTcpSocket *nouveauClient = m_tcp_server->nextPendingConnection();
    connect(nouveauClient, &QTcpSocket::readyRead, this, &Server::lireTexte);
    connect(nouveauClient, &QTcpSocket::disconnected, this, &Server::clientDeconnecte);

    // On marque ce socket comme "pas encore enregistré"
    m_pendingSockets.insert(nouveauClient);
    qDebug() << "Nouveau client connecte. En attente d'enregistrement...";
}



void Server::lireTexte() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);

    while (socket->bytesAvailable() > 0) {

        // Ce socket n'est pas encore enregistré → premier message = sa clé
        if (m_pendingSockets.contains(socket)) {
            QString maClePublique;
            in >> maClePublique;
            if (!maClePublique.isEmpty()) {
                m_clients.insert(maClePublique, socket);
                m_pendingSockets.remove(socket);
                qDebug() << "Client enregistre avec la cle :" << maClePublique;
            }
            return;
        }

        // Socket déjà enregistré → c'est un vrai message à relayer
        QString cleDestinataire;
        QByteArray messageChiffre;
        in >> cleDestinataire >> messageChiffre;

        if (cleDestinataire.isEmpty()) return;

        if (m_clients.contains(cleDestinataire)) {
            qDebug() << "Relais vers :" << cleDestinataire;
            envoiTexte(m_clients.value(cleDestinataire), messageChiffre);
        } else {
            qDebug() << "Destinataire introuvable :" << cleDestinataire;
        }
    }
}

void Server::envoiTexte(QTcpSocket* destinataire, const QByteArray& data) {
    // On n'utilise plus QDataStream ici pour l'envoi final
    // On envoie directement les octets au socket
    destinataire->write(data);
    destinataire->flush(); // Force l'envoi immédiat
}

void Server::clientDeconnecte() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_pendingSockets.remove(socket);
    QString cleASupprimer = m_clients.key(socket);
    if (!cleASupprimer.isEmpty()) {
        m_clients.remove(cleASupprimer);
        qDebug() << "Client deconnecte. Cle supprimee :" << cleASupprimer;
    }
    socket->deleteLater();
}

void Server::sendBeacon() {
    QByteArray datagram = "SOVLINK_SERVER_ACTIVE";
    m_udpBeacon->writeDatagram(datagram, QHostAddress::Broadcast, 9999);
}
