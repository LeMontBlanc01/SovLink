#include "Server.h"
#include "ClientSession.h"
#include <iostream>

namespace sovlink {

Server::Server(boost::asio::io_context& io, uint16_t port)
    : m_io(io)
    , m_acceptor(io, boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::tcp::v4(), port))
{
    Crypto::generateKeyPair(m_serverPub, m_serverSec);
    std::cout << "[SovLink Server] Clé publique : "
              << Crypto::toHex(m_serverPub) << "\n";
}

void Server::start() {
    std::cout << "[SovLink Server] En écoute...\n";
    doAccept();
}

void Server::doAccept() {
    m_acceptor.async_accept(
        [this](boost::system::error_code ec,
               boost::asio::ip::tcp::socket socket)
    {
        if (!ec) {
            std::cout << "[Server] Nouvelle connexion entrante\n";
            auto session = std::make_shared<ClientSession>(
                std::move(socket), *this);
            session->start();
        } else {
            std::cerr << "[Server] Erreur acceptation : " << ec.message() << "\n";
        }
        doAccept(); // Reprendre l'acceptation
    });
}

void Server::registerClient(const std::string& pubkeyHex,
                             std::shared_ptr<ClientSession> session)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_clients[pubkeyHex] = session;
    }
    std::cout << "[Server] Client enregistré : " << pubkeyHex.substr(0, 16) << "...\n";
    broadcastUserList();
}

void Server::unregisterClient(const std::string& pubkeyHex) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_clients.erase(pubkeyHex);
    }
    std::cout << "[Server] Client déconnecté : " << pubkeyHex.substr(0, 16) << "...\n";
    broadcastUserList();
}

std::vector<std::string> Server::connectedUsers() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> users;
    users.reserve(m_clients.size());
    for (auto& [key, _] : m_clients)
        users.push_back(key);
    return users;
}

bool Server::deliverMessage(const std::string& toPubkeyHex,
                             const std::string& fromPubkeyHex,
                             const std::vector<uint8_t>& encryptedData)
{
    std::shared_ptr<ClientSession> target;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_clients.find(toPubkeyHex);
        if (it == m_clients.end()) return false;
        target = it->second;
    }

    // Construire le paquet DELIVER
    Packet pkt;
    pkt.type    = PacketType::DELIVER;
    pkt.payload = proto::makeDeliver(fromPubkeyHex, encryptedData);
    target->send(pkt);

    std::cout << "[Server] Message relayé : "
              << fromPubkeyHex.substr(0,8) << " → "
              << toPubkeyHex.substr(0,8) << "\n";
    return true;
}

void Server::broadcastUserList() {
    std::vector<std::string> users = connectedUsers();

    Packet pkt;
    pkt.type    = PacketType::USER_LIST;
    pkt.payload = proto::makeUserList(users);

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& [_, session] : m_clients)
        session->send(pkt);
}

} // namespace sovlink
