#include "network.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

network::network(QObject *parent)
    : QObject{parent} // On termine la liste d'initialisation ici
{
    // On initialise la socket à l'intérieur des accolades
    socketClient = new QTcpSocket(this);

    // Témoin de réussite
    connect(socketClient, &QTcpSocket::connected, []() {
        qDebug() << "✅ RÉSEAU : Connecté au serveur !";
    });

    // Témoin d'échec
    connect(socketClient, &QTcpSocket::errorOccurred, [](QAbstractSocket::SocketError error) {
        qDebug() << "❌ RÉSEAU : Erreur de connexion ! Code :" << error;
    });

    // Si tu veux utiliser "connexion" comme nom, garde celui-là,
    // mais n'utilise qu'une seule variable !

    connect(socketClient, &QTcpSocket::readyRead, this, [this]() {
        recevoirMessage();
    });
}

void network::connect_to_server() {
    // On force la fermeture d'une éventuelle connexion précédente
    if (socketClient->state() != QAbstractSocket::UnconnectedState) {
        socketClient->abort();
    }

    qDebug() << "Tentative de connexion vers 127.0.0.1...";
    socketClient->connectToHost("127.0.0.1", 12345);
}

void network::envoyerMessage(const QString &message) {
    if (socketClient->state() == QAbstractSocket::ConnectedState) {
        socketClient->write(message.toUtf8());
        socketClient->flush();
        qDebug() << "Message envoyé :" << message;
    } else {
        qDebug() << "Erreur : Impossible d'envoyer, état actuel :" << socketClient->state();
    }
}

void network::recevoirMessage() {
    QTcpSocket *socketClient = qobject_cast<QTcpSocket *>(sender());
    if (!socketClient) return;

    QByteArray data = socketClient->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    QString contenu;

    if (!doc.isNull()) {
        // 1. On récupère le message de bienvenue
        QString msg = json["message"].toString();

        // 2. On récupère l'ID s'il existe dans le JSON
        QString id = json["votre_id"].toString();

        if (!id.isEmpty()) {
            contenu = msg + "\nVotre ID est : " + id; // On combine les deux !
        } else {
            contenu = msg;
        }
    } else {
        contenu = QString::fromUtf8(data).trimmed();
    }

    if (!contenu.isEmpty()) {
        QTimer::singleShot(100, this, [this, contenu]() {
            emit messageRecuPourInterface(contenu);
        });
    }
}
bool network::connexionAuServeur(QString ip, int port) {
    socketClient->connectToHost(ip, port);
    return socketClient->waitForConnected(3000);
}
void network::sonnerie2(QString message) {
    if (socketClient->state() == QAbstractSocket::ConnectedState) {
        // On envoie le message au serveur
        socketClient->write(message.toUtf8());
    }
}
