#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include "Crypto.h"
#include "Protocol.h"

namespace sovlink {

class ClientSession;

/**
 * Server — Serveur TCP SovLink
 *
 * Rôle :
 *  - Écoute les connexions entrantes sur le port configuré
 *  - Crée une ClientSession par client connecté
 *  - Maintient la liste des clients actifs (indexed par clé publique hex)
 *  - Route les messages chiffrés vers le destinataire
 *  - Notifie tous les clients lors d'une connexion/déconnexion
 */
class Server {
public:
    explicit Server(boost::asio::io_context& io, uint16_t port);

    /** Démarre l'acceptation des connexions */
    void start();

    /** Enregistre un client après son handshake */
    void registerClient(const std::string& pubkeyHex,
                        std::shared_ptr<ClientSession> session);

    /** Supprime un client déconnecté */
    void unregisterClient(const std::string& pubkeyHex);

    /** Retourne la liste des identifiants (pubkeys hex) des clients connectés */
    std::vector<std::string> connectedUsers() const;

    /**
     * Transmet un message chiffré au destinataire
     * Retourne false si le destinataire est inconnu
     */
    bool deliverMessage(const std::string& toPubkeyHex,
                        const std::string& fromPubkeyHex,
                        const std::vector<uint8_t>& encryptedData);

    /** Diffuse la liste des utilisateurs à tous les clients connectés */
    void broadcastUserList();

private:
    void doAccept();

    boost::asio::io_context&                                    m_io;
    boost::asio::ip::tcp::acceptor                              m_acceptor;
    std::unordered_map<std::string, std::shared_ptr<ClientSession>> m_clients;
    mutable std::mutex                                          m_mutex;

    // Clés du serveur (utilisées pour identifier le serveur dans le handshake)
    Crypto::PublicKey m_serverPub;
    Crypto::SecretKey m_serverSec;
};

} // namespace sovlink
