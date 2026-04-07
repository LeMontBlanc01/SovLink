#include "routeurcisco.h"
#include <QHostAddress>

routeurcisco::routeurcisco(QObject *parent) : QObject{parent} {
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &routeurcisco::onNewConnection);

    if(!server->listen(QHostAddress::Any, 12345)) {
        qDebug() << "Erreur :" << server->errorString();
    } else {
        qDebug() << "Routeur lancé sur le port 12345...";
    }
}

void routeurcisco::onNewConnection() {
    QTcpSocket *socketClient = server->nextPendingConnection();
    if (socketClient) {
        // Création de l'ID unique (IP:Port)
        QString id = socketClient->peerAddress().toString() + ":" + QString::number(socketClient->peerPort());

        // On ajoute le client à notre table de routage
        tableRoutage.insert(id, socketClient);
        // --- NOUVEAU : Envoi du message de bienvenue ---
        QJsonObject welcome;
        welcome["type"] = "welcome"; // On précise le type de message
        welcome["votre_id"] = id;
        welcome["message"] = "Bienvenue sur le routeur Cisco !";

        socketClient->write(QJsonDocument(welcome).toJson(QJsonDocument::Compact));
        // -----------------------------------------------

        qDebug() << "Nouveau client enregistré :" << id;

        connect(socketClient, &QTcpSocket::readyRead, this, &routeurcisco::lire_message);
        connect(socketClient, &QTcpSocket::disconnected, this, &routeurcisco::auDeconnecter);
    }
}

void routeurcisco::lire_message() {
    QTcpSocket *socketEmetteur = qobject_cast<QTcpSocket *>(sender());
    if (!socketEmetteur) return;

    // On lit les données JSON
    QByteArray data = socketEmetteur->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Erreur : Format JSON invalide reçu.";
        return;
    }

    QJsonObject json = doc.object();
    QString dest = json["destinataire"].toString();
    QString msg  = json["message"].toString();

    // On récupère l'ID de celui qui envoie pour l'ajouter au message
    QString sourceId = socketEmetteur->peerAddress().toString() + ":" + QString::number(socketEmetteur->peerPort());
    json["source"] = sourceId; // On certifie la source

    qDebug() << "Routage demandé vers :" << dest;

    // --- LOGIQUE DE ROUTAGE ---
    if (tableRoutage.contains(dest)) {
        QTcpSocket *socketDest = tableRoutage.value(dest);

        // On renvoie le JSON modifié au destinataire
        socketDest->write(QJsonDocument(json).toJson(QJsonDocument::Compact));
        socketDest->flush();

        qDebug() << "Message transmis avec succès.";
    } else {
        qDebug() << "Échec : Destinataire" << dest << "introuvable.";
        // Optionnel : renvoyer un message d'erreur à l'émetteur ici
    }
}

void routeurcisco::auDeconnecter() {
    QTcpSocket *socketClient = qobject_cast<QTcpSocket *>(sender());
    if (socketClient) {
        // On retrouve l'ID pour le supprimer de la map
        QString id = socketClient->peerAddress().toString() + ":" + QString::number(socketClient->peerPort());
        tableRoutage.remove(id);

        qDebug() << "Client déconnecté et retiré de la table :" << id;
        socketClient->deleteLater();
    }
}

// Cette fonction n'est plus strictement nécessaire si tu routes tout dans lire_message
void routeurcisco::envoyerMessage(const QString &message) {
    QTcpSocket *socketClient = qobject_cast<QTcpSocket *>(sender());



    if (socketClient->state() == QAbstractSocket::ConnectedState) {

        socketClient->write(message.toUtf8());

        socketClient->flush();

        qDebug() << "Message envoyé :" << message;



    } else {

        qDebug() << "Erreur : Impossible d'envoyer, état actuel :" << socketClient->state();

    }


}
