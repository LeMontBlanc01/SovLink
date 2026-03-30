#include "server_test.h"
#include <QDataStream>
#include <QDebug>

Server::Server() {
    m_tcp_server = new QTcpServer(this);

    if (!m_tcp_server->listen(QHostAddress::Any, 8080)) {
        qDebug() << "Erreur : Le serveur n'a pas pu demarrer !";
        return;
    }

    qDebug() << "=== SovLink Relay Server ===";
    qDebug() << "Serveur demarre sur le port 8080.";

    m_udpBeacon  = new QUdpSocket(this);
    m_beaconTimer = new QTimer(this);
    connect(m_beaconTimer, &QTimer::timeout, this, &Server::sendBeacon);
    m_beaconTimer->start(2000);

    connect(m_tcp_server, &QTcpServer::newConnection, this, &Server::connexionClient);
    qDebug() << "Balise UDP active sur port 9999.";
    qDebug() << "En attente de clients...";
}

void Server::connexionClient() {
    QTcpSocket *sock = m_tcp_server->nextPendingConnection();
    connect(sock, &QTcpSocket::readyRead,    this, &Server::lireTexte);
    connect(sock, &QTcpSocket::disconnected, this, &Server::clientDeconnecte);
    m_pendingSockets.insert(sock);
    qDebug() << "[+] Nouvelle connexion depuis" << sock->peerAddress().toString();
}

void Server::lireTexte() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);

    while (socket->bytesAvailable() > 0) {

        if (m_blockSizes[socket] == 0) {
            if (socket->bytesAvailable() < (int)sizeof(quint32)) return;
            in >> m_blockSizes[socket];
        }

        if (socket->bytesAvailable() < m_blockSizes[socket]) return;
        m_blockSizes[socket] = 0;

        if (m_pendingSockets.contains(socket)) {
            QString clePublique;
            in >> clePublique;
            if (!clePublique.isEmpty()) {
                m_clients.insert(clePublique, socket);
                m_pendingSockets.remove(socket);
                qDebug() << "[✓] Client enregistré :" << clePublique.left(20);
            }
            // FIX 2 : pas de return ici, on continue la boucle
            continue;
        }

        QString    cleDestinataire;
        QByteArray payload;
        in >> cleDestinataire >> payload;

        if (cleDestinataire.isEmpty() || payload.isEmpty()) return;

        if (m_clients.contains(cleDestinataire)) {
            qDebug() << "[→] Relais vers :" << cleDestinataire.left(20);
            envoiTexte(m_clients.value(cleDestinataire), payload);
        } else {
            qDebug() << "[!] Destinataire introuvable";
        }
    }
}

void Server::envoiTexte(QTcpSocket* dest, const QByteArray& data) {
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);

    out << quint32(0);      // placeholder taille
    out << data;
    // Écrit la vraie taille au début
    out.device()->seek(0);
    out << quint32(paquet.size() - sizeof(quint32));

    dest->write(paquet);
}

void Server::clientDeconnecte() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_pendingSockets.remove(socket);
    m_blockSizes.remove(socket);
    QString cle = m_clients.key(socket);
    if (!cle.isEmpty()) {
        m_clients.remove(cle);
        qDebug() << "[-] Client déconnecté :" << cle.left(20) << "...";
        qDebug() << "    Clients restants :" << m_clients.size();
    }
    socket->deleteLater();
}

void Server::sendBeacon() {
    QByteArray datagram = "SOVLINK_SERVER_ACTIVE";
    m_udpBeacon->writeDatagram(datagram, QHostAddress::Broadcast, 9999);
}
