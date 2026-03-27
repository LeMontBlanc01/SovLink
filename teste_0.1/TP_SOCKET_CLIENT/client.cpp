#include "client.h"
#include <QCoreApplication>
#include <QDebug>

#include "client.h"
#include <QDebug>

Client::Client(QObject *parent) : QObject(parent) {
    // 1. Initialisation du socket TCP (La ligne de vie du chat)
    m_socket = new QTcpSocket(this);

    // Connexion des signaux TCP pour réagir aux événements réseau
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);

    // 2. Configuration de l'écouteur UDP (Le radar pour trouver le serveur)
    m_udpListener = new QUdpSocket(this);

    // On écoute sur le port 9999.
    // ShareAddress permet de lancer plusieurs clients sur le même PC pour tes tests.
    bool bindOk = m_udpListener->bind(9999, QUdpSocket::ShareAddress);

    if (bindOk) {
        qDebug() << "Radar UDP actif. En attente du signal du serveur SovLink...";
        // Dès qu'un paquet UDP arrive, on appelle la fonction de recherche
        connect(m_udpListener, &QUdpSocket::readyRead, this, &Client::processPendingDatagrams);
    } else {
        qDebug() << "Erreur critique : Impossible d'écouter le port UDP 9999 (Port peut-être déjà utilisé).";
    }
}

void Client::onConnected() {
        qDebug() << "Connexion TCP etablie avec le serveur !";

        // On envoie notre "nom" ou "clé publique" pour que le serveur nous enregistre
        QDataStream out(m_socket);
        QString monID = "Session_User_" + QString::number(QRandomGenerator::global()->generate());
        out << monID;

        qDebug() << "Enregistre sur le serveur en tant que :" << monID;
    }


void Client::onReadyRead() {
    QDataStream in(m_socket);
    QByteArray messageData;

    // On lit le message relayé par le serveur
    in >> messageData;
    qDebug() << "\n[Message reçu] :" << QString::fromUtf8(messageData);
    qDebug() << "Votre réponse :";
}

// Cette fonction doit être appelée dans le main.cpp pour permettre la saisie
void Client::messageManualInput() {
    QTextStream input(stdin);
    QString line = input.readLine();

    if (!line.isEmpty() && m_socket->isOpen()) {
        QDataStream out(m_socket);
        // On respecte le format attendu par le serveur : [Cible] [Message]
        out << m_targetPubKey << line.toUtf8();
        qDebug() << "[Moi] :" << line;
    }
}

void Client::onDisconnected() {
    qDebug() << "Déconnecté du serveur.";
    qApp->quit();
}
void Client::processPendingDatagrams() {
    while (m_udpListener->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpListener->pendingDatagramSize());
        QHostAddress senderIP;

        m_udpListener->readDatagram(datagram.data(), datagram.size(), &senderIP);

        if (datagram == "SOVLINK_SERVER_ACTIVE") {
            qDebug() << "Serveur detecte a l'adresse :" << senderIP.toString();

            // On arrête de chercher (on ferme le radar UDP)
            m_udpListener->close();

            // On lance la vraie connexion TCP vers l'IP trouvée
            m_socket->connectToHost(senderIP, 8080);
            break;
        }
    }
}
