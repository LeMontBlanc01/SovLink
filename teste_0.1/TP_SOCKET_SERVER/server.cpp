#include "server.h"
#include <QDataStream>
#include <QDebug>

Server::Server() : m_blockSize(0) {
    m_tcp_server = new QTcpServer(this);

    // On lance l'écoute sur toutes les interfaces, port 8080 (par exemple)
    if (!m_tcp_server->listen(QHostAddress::Any, 8080)) {
        qDebug() << "Erreur : Le serveur n'a pas pu démarrer !";
    } else {
        qDebug() << "Serveur demarre sur le port 8080. En attente de cles publiques...";
        // Connexion du signal de nouvelle connexion au slot
        // Initialisation de la balise UDP
                m_udpBeacon = new QUdpSocket(this);
                m_beaconTimer = new QTimer(this);

                // On connecte le timer à la fonction d'envoi
                connect(m_beaconTimer, &QTimer::timeout, this, &Server::sendBeacon);

                // On démarre le timer (2000 ms = 2 secondes)
                m_beaconTimer->start(2000);
        connect(m_tcp_server, &QTcpServer::newConnection, this, &Server::connexionClient);
        qDebug() << "Balise UDP active : Recherche automatique activée.";
    }
}

void Server::connexionClient() {
    // On récupère la socket du nouveau client
    QTcpSocket *nouveauClient = m_tcp_server->nextPendingConnection();

    // On connecte ses signaux à nos slots
    connect(nouveauClient, &QTcpSocket::readyRead, this, &Server::lireTexte);
    connect(nouveauClient, &QTcpSocket::disconnected, this, &Server::clientDeconnecte);

    qDebug() << "Nouveau client connecté (Socket temporaire). En attente d'enregistrement...";
}

void Server::lireTexte() {
    // On identifie quel socket a envoyé des données
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);

    // On boucle tant qu'il y a des données à lire
    while (socket->bytesAvailable() > 0) {

        // Si le client n'est pas encore dans la Map, le premier message est sa clé publique
        if (!m_clients.values().contains(socket)) {
            QString maClePublique;
            in >> maClePublique; // On lit la clé envoyée par le client

            if (!maClePublique.isEmpty()) {
                m_clients.insert(maClePublique, socket);
                qDebug() << "Client enregistré avec la clé :" << maClePublique;
            }
            return;
        }

        // Si le client est déjà enregistré, il envoie un message pour quelqu'un
        QString cleDestinataire;
        QByteArray messageChiffre;

        in >> cleDestinataire >> messageChiffre;

        if (m_clients.contains(cleDestinataire)) {
            qDebug() << "Relais du message vers :" << cleDestinataire;
            envoiTexte(m_clients.value(cleDestinataire), messageChiffre);
        } else {
            qDebug() << "Erreur : Destinataire" << cleDestinataire << "introuvable.";
            // Optionnel : renvoyer un message d'erreur à l'expéditeur ici
        }
    }
}

void Server::envoiTexte(QTcpSocket* destinataire, const QByteArray& data) {
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);

    // On écrit les données chiffrées dans le paquet
    out << data;

    destinataire->write(paquet);
}

void Server::clientDeconnecte() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // On cherche la clé associée à ce socket pour la supprimer
    QString cleASupprimer = m_clients.key(socket);

    if (!cleASupprimer.isEmpty()) {
        m_clients.remove(cleASupprimer);
        qDebug() << "Client déconnecté. Clé supprimée :" << cleASupprimer;
    }

    socket->deleteLater();
}
void Server::sendBeacon() {
    QByteArray datagram = "SOVLINK_SERVER_ACTIVE";
    // On envoie sur l'adresse de broadcast (255.255.255.255) sur le port 9999
    m_udpBeacon->writeDatagram(datagram, QHostAddress::Broadcast, 9999);
}
