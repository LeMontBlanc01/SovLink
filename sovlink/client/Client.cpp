#include "Client.h"
#include <QDebug>

namespace sovlink {

Client::Client(QObject* parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
{
    // Générer la paire de clés pour cette session
    Crypto::generateKeyPair(m_myPub, m_mySec);
    m_myPubHex = QString::fromStdString(Crypto::toHex(m_myPub));

    qRegisterMetaType<sovlink::Message>("sovlink::Message");

    connect(m_socket, &QTcpSocket::connected,
            this,     &Client::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this,     &Client::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,
            this,     &Client::onReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred,
            this,     &Client::onSocketError);
}

// ─── Connexion / déconnexion ─────────────────────────────────────────────────

void Client::connectToServer(const QString& host, quint16 port) {
    m_socket->connectToHost(host, port);
}

void Client::disconnectFromServer() {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        // Envoyer DISCONNECT proprement
        Packet pkt;
        pkt.type = PacketType::DISCONNECT;
        auto data = pkt.serialize();
        m_socket->write(reinterpret_cast<const char*>(data.data()),
                        static_cast<qint64>(data.size()));
        m_socket->flush();
    }
    m_socket->disconnectFromHost();
}

bool Client::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

QStringList Client::onlineUsers() const {
    return m_onlineUsers;
}

// ─── Slots réseau ─────────────────────────────────────────────────────────────

void Client::onSocketConnected() {
    qDebug() << "[Client] Connecté au serveur, envoi du handshake";

    // Envoyer notre clé publique
    Packet pkt;
    pkt.type    = PacketType::HANDSHAKE;
    pkt.payload = proto::makeHandshake(m_myPubHex.toStdString());
    auto data   = pkt.serialize();
    m_socket->write(reinterpret_cast<const char*>(data.data()),
                    static_cast<qint64>(data.size()));
}

void Client::onSocketDisconnected() {
    qDebug() << "[Client] Déconnecté";
    m_onlineUsers.clear();
    emit disconnected();
}

void Client::onSocketError(QAbstractSocket::SocketError /*err*/) {
    emit connectionError(m_socket->errorString());
}

void Client::onReadyRead() {
    m_readBuffer.append(m_socket->readAll());

    // Traiter tous les paquets complets dans le buffer
    while (true) {
        std::vector<uint8_t> buf(
            reinterpret_cast<const uint8_t*>(m_readBuffer.constData()),
            reinterpret_cast<const uint8_t*>(m_readBuffer.constData()) + m_readBuffer.size()
        );

        Packet pkt;
        size_t consumed = 0;
        if (!Packet::deserialize(buf, pkt, consumed)) break;

        m_readBuffer.remove(0, static_cast<qsizetype>(consumed));
        processPacket(pkt);
    }
}

// ─── Traitement des paquets ───────────────────────────────────────────────────

void Client::processPacket(const Packet& pkt) {
    switch (pkt.type) {
    case PacketType::HANDSHAKE_OK: handleHandshakeOk(pkt); break;
    case PacketType::DELIVER:      handleDeliver(pkt);      break;
    case PacketType::USER_LIST:    handleUserList(pkt);     break;
    case PacketType::ERROR_MSG:    handleError(pkt);        break;
    default:
        qWarning() << "[Client] Paquet inconnu :" << static_cast<int>(pkt.type);
    }
}

void Client::handleHandshakeOk(const Packet& pkt) {
    auto fields = proto::parseSimpleJson(pkt.payload);

    // Mettre à jour la liste des utilisateurs
    auto itUsers = fields.find("users");
    if (itUsers != fields.end()) {
        auto users = proto::parseUserList(itUsers->second);
        m_onlineUsers.clear();
        for (auto& u : users) {
            QString qU = QString::fromStdString(u);
            if (qU != m_myPubHex) // Ne pas s'inclure soi-même
                m_onlineUsers.append(qU);

            // Mémoriser la clé publique
            m_knownKeys[u] = Crypto::fromHex(u);
        }
    }

    emit connected();
    emit userListUpdated(m_onlineUsers);
}

void Client::handleDeliver(const Packet& pkt) {
    auto fields = proto::parseSimpleJson(pkt.payload);

    auto itFrom = fields.find("from");
    auto itData = fields.find("data");
    if (itFrom == fields.end() || itData == fields.end()) return;

    const std::string& fromHex = itFrom->second;

    // Récupérer ou mémoriser la clé publique de l'expéditeur
    auto& senderPub = m_knownKeys[fromHex];
    if (senderPub.empty()) {
        try { senderPub = Crypto::fromHex(fromHex); }
        catch (...) { qWarning() << "[Client] Clé expéditeur invalide"; return; }
    }

    // Décoder et déchiffrer
    std::vector<uint8_t> encData;
    try { encData = Crypto::fromHex(itData->second); }
    catch (...) { qWarning() << "[Client] Data hex invalide"; return; }

    std::string plaintext = Crypto::decrypt(encData, senderPub, m_mySec);
    if (plaintext.empty()) {
        qWarning() << "[Client] Déchiffrement échoué";
        return;
    }

    Message msg;
    msg.fromFull = QString::fromStdString(fromHex);
    msg.fromId   = msg.fromFull.left(16);
    msg.text     = QString::fromStdString(plaintext);
    msg.isOwn    = false;

    emit messageReceived(msg);
}

void Client::handleUserList(const Packet& pkt) {
    auto fields = proto::parseSimpleJson(pkt.payload);

    auto itUsers = fields.find("users");
    if (itUsers == fields.end()) return;

    auto users = proto::parseUserList(itUsers->second);
    m_onlineUsers.clear();
    for (auto& u : users) {
        QString qU = QString::fromStdString(u);
        if (qU != m_myPubHex)
            m_onlineUsers.append(qU);
        m_knownKeys[u] = Crypto::fromHex(u);
    }
    emit userListUpdated(m_onlineUsers);
}

void Client::handleError(const Packet& pkt) {
    std::string msg(pkt.payload.begin(), pkt.payload.end());
    emit deliveryError(QString::fromStdString(msg));
}

// ─── Envoi d'un message chiffré ──────────────────────────────────────────────

bool Client::sendMessage(const QString& recipientPubHex, const QString& text) {
    if (!isConnected()) return false;

    std::string recHex = recipientPubHex.toStdString();

    // Récupérer/mémoriser la clé publique du destinataire
    auto& recPub = m_knownKeys[recHex];
    if (recPub.empty()) {
        try { recPub = Crypto::fromHex(recHex); }
        catch (...) {
            emit deliveryError("Clé publique du destinataire invalide");
            return false;
        }
    }

    // Chiffrer
    std::vector<uint8_t> encrypted;
    try {
        encrypted = Crypto::encrypt(text.toStdString(), recPub, m_mySec);
    } catch (const std::exception& e) {
        emit deliveryError(QString("Erreur chiffrement : ") + e.what());
        return false;
    }

    // Construire et envoyer le paquet MESSAGE
    Packet pkt;
    pkt.type    = PacketType::MESSAGE;
    pkt.payload = proto::makeMessage(recHex,
                                     m_myPubHex.toStdString(),
                                     encrypted);
    auto data = pkt.serialize();
    m_socket->write(reinterpret_cast<const char*>(data.data()),
                    static_cast<qint64>(data.size()));

    // Afficher le message dans notre propre UI
    Message msg;
    msg.fromFull = m_myPubHex;
    msg.fromId   = m_myPubHex.left(16);
    msg.text     = text;
    msg.isOwn    = true;
    emit messageReceived(msg);

    return true;
}

} // namespace sovlink
