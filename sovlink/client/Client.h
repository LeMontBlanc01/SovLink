#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include <vector>
#include <map>
#include "Crypto.h"
#include "Protocol.h"

namespace sovlink {

struct Message {
    QString fromId;    // identifiant court (16 chars hex) de l'expéditeur
    QString fromFull;  // clé publique complète hex de l'expéditeur
    QString text;      // texte déchiffré
    bool    isOwn;     // true si envoyé par nous
};

/**
 * Client — Gère la connexion TCP au serveur SovLink
 *
 * Génère une paire de clés à la création.
 * Émet des signaux Qt pour les événements réseau.
 */
class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject* parent = nullptr);

    /** Se connecte au serveur */
    void connectToServer(const QString& host, quint16 port);

    /** Envoie un message chiffré à un destinataire (clé publique hex) */
    bool sendMessage(const QString& recipientPubHex, const QString& text);

    /** Déconnexion propre */
    void disconnectFromServer();

    // Accesseurs
    QString myPubkeyHex()   const { return m_myPubHex; }
    QString myShortId()     const { return m_myPubHex.left(16); }
    QStringList onlineUsers() const;
    bool isConnected()      const;

signals:
    void connected();
    void disconnected();
    void connectionError(const QString& msg);
    void messageReceived(const sovlink::Message& msg);
    void userListUpdated(const QStringList& users);
    void deliveryError(const QString& msg);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError err);
    void onReadyRead();

private:
    void processPacket(const Packet& pkt);
    void handleHandshakeOk(const Packet& pkt);
    void handleDeliver(const Packet& pkt);
    void handleUserList(const Packet& pkt);
    void handleError(const Packet& pkt);

    QTcpSocket*              m_socket;
    QByteArray               m_readBuffer;

    // Clés de cette session
    Crypto::PublicKey        m_myPub;
    Crypto::SecretKey        m_mySec;
    QString                  m_myPubHex;

    // Cache des clés publiques connues (hex → bytes)
    std::map<std::string, Crypto::PublicKey> m_knownKeys;

    // Liste des utilisateurs connectés (hex complet)
    QStringList              m_onlineUsers;
};

} // namespace sovlink

Q_DECLARE_METATYPE(sovlink::Message)
