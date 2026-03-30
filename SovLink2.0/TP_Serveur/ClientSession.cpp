#include "ClientSession.h"
#include "Server.h"
#include <iostream>

namespace sovlink {

ClientSession::ClientSession(boost::asio::ip::tcp::socket socket, Server& server)
    : m_socket(std::move(socket))
    , m_server(server)
{}

ClientSession::~ClientSession() {
    disconnect();
}

void ClientSession::start() {
    doReadHeader();
}

// ─── Lecture : en-tête (8 bytes) ────────────────────────────────────────────

void ClientSession::doReadHeader() {
    m_readBuf.resize(8);
    auto self = shared_from_this();
    boost::asio::async_read(
        m_socket,
        boost::asio::buffer(m_readBuf, 8),
        [this, self](boost::system::error_code ec, size_t /*bytes*/)
        {
            if (ec) {
                std::cerr << "[Session] Erreur lecture en-tête : " << ec.message() << "\n";
                disconnect();
                return;
            }

            uint32_t type = ((uint32_t)m_readBuf[0] << 24)
                          | ((uint32_t)m_readBuf[1] << 16)
                          | ((uint32_t)m_readBuf[2] <<  8)
                          |  (uint32_t)m_readBuf[3];

            uint32_t len  = ((uint32_t)m_readBuf[4] << 24)
                          | ((uint32_t)m_readBuf[5] << 16)
                          | ((uint32_t)m_readBuf[6] <<  8)
                          |  (uint32_t)m_readBuf[7];

            if (len > 1024 * 1024) { // Protection anti-flood : 1 Mo max
                std::cerr << "[Session] Payload trop grand, déconnexion\n";
                disconnect();
                return;
            }

            doReadPayload(type, len);
        }
    );
}

// ─── Lecture : payload ───────────────────────────────────────────────────────

void ClientSession::doReadPayload(uint32_t type, uint32_t length) {
    m_readBuf.resize(length);
    auto self = shared_from_this();

    if (length == 0) {
        Packet pkt;
        pkt.type = static_cast<PacketType>(type);
        handlePacket(pkt);
        doReadHeader();
        return;
    }

    boost::asio::async_read(
        m_socket,
        boost::asio::buffer(m_readBuf, length),
        [this, self, type](boost::system::error_code ec, size_t /*bytes*/)
        {
            if (ec) {
                std::cerr << "[Session] Erreur lecture payload : " << ec.message() << "\n";
                disconnect();
                return;
            }

            Packet pkt;
            pkt.type    = static_cast<PacketType>(type);
            pkt.payload = m_readBuf;
            handlePacket(pkt);
            doReadHeader();
        }
    );
}

// ─── Traitement des paquets ──────────────────────────────────────────────────

void ClientSession::handlePacket(const Packet& pkt) {
    switch (pkt.type) {

    case PacketType::HANDSHAKE: {
        auto fields = proto::parseSimpleJson(pkt.payload);
        auto it = fields.find("pubkey");
        if (it == fields.end()) {
            std::cerr << "[Session] HANDSHAKE sans pubkey\n";
            disconnect();
            return;
        }
        m_pubkeyHex  = it->second;
        m_registered = true;
        m_server.registerClient(m_pubkeyHex, shared_from_this());

        // Répondre : HANDSHAKE_OK + liste des utilisateurs
        Packet ok;
        ok.type    = PacketType::HANDSHAKE_OK;
        ok.payload = proto::makeHandshakeOk("server", m_server.connectedUsers());
        send(ok);
        break;
    }

    case PacketType::MESSAGE: {
        if (!m_registered) { disconnect(); return; }
        auto fields = proto::parseSimpleJson(pkt.payload);

        auto itTo   = fields.find("to");
        auto itFrom = fields.find("from");
        auto itData = fields.find("data");

        if (itTo == fields.end() || itFrom == fields.end() || itData == fields.end()) {
            std::cerr << "[Session] MESSAGE mal formé\n";
            return;
        }

        // Décoder les données hex → bytes
        std::vector<uint8_t> encData;
        try {
            encData = Crypto::fromHex(itData->second);
        } catch (...) {
            std::cerr << "[Session] DATA hex invalide\n";
            return;
        }

        bool delivered = m_server.deliverMessage(itTo->second, itFrom->second, encData);
        if (!delivered) {
            // Destinataire inconnu : envoyer une erreur
            Packet err;
            err.type    = PacketType::ERROR_MSG;
            std::string msg = "{\"error\":\"Destinataire non connecté\"}";
            err.payload = std::vector<uint8_t>(msg.begin(), msg.end());
            send(err);
        }
        break;
    }

    case PacketType::DISCONNECT:
        std::cout << "[Session] Déconnexion propre du client\n";
        disconnect();
        break;

    default:
        std::cerr << "[Session] Type de paquet inconnu : "
                  << static_cast<uint32_t>(pkt.type) << "\n";
        break;
    }
}

// ─── Envoi asynchrone ────────────────────────────────────────────────────────

void ClientSession::send(const Packet& pkt) {
    auto data = std::make_shared<std::vector<uint8_t>>(pkt.serialize());
    auto self = shared_from_this();

    boost::asio::post(m_socket.get_executor(), [this, self, data]() {
        bool idle;
        {
            std::lock_guard<std::mutex> lock(m_writeMutex);
            idle = m_writeQueue.empty();
            m_writeQueue.push_back(*data);
        }
        if (idle) doWrite();
    });
}

void ClientSession::doWrite() {
    std::vector<uint8_t> front;
    {
        std::lock_guard<std::mutex> lock(m_writeMutex);
        if (m_writeQueue.empty()) return;
        front = m_writeQueue.front();
    }

    auto self = shared_from_this();
    boost::asio::async_write(
        m_socket,
        boost::asio::buffer(front),
        [this, self](boost::system::error_code ec, size_t /*bytes*/)
        {
            if (ec) {
                std::cerr << "[Session] Erreur écriture : " << ec.message() << "\n";
                disconnect();
                return;
            }
            std::lock_guard<std::mutex> lock(m_writeMutex);
            m_writeQueue.pop_front();
            if (!m_writeQueue.empty()) doWrite();
        }
    );
}

// ─── Déconnexion ─────────────────────────────────────────────────────────────

void ClientSession::disconnect() {
    if (m_registered) {
        m_registered = false;
        m_server.unregisterClient(m_pubkeyHex);
    }
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

} // namespace sovlink
