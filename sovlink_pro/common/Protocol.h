#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace sovlink {

/**
 * Protocol — sérialisation / désérialisation des paquets TCP
 *
 * Format d'un paquet :
 *   [4 bytes: type (uint32)] [4 bytes: longueur payload (uint32)] [payload]
 *
 * Types de messages :
 *   HANDSHAKE    Client → Serveur : envoie sa clé publique
 *   HANDSHAKE_OK Serveur → Client : confirme la connexion + liste des clients
 *   USER_LIST    Serveur → Client : mise à jour de la liste des utilisateurs connectés
 *   MESSAGE      Client → Serveur : message chiffré destiné à un autre client
 *   DELIVER      Serveur → Client : livraison d'un message chiffré
 *   DISCONNECT   Client → Serveur : déconnexion propre
 */

enum class PacketType : uint32_t {
    HANDSHAKE    = 1,
    HANDSHAKE_OK = 2,
    USER_LIST    = 3,
    MESSAGE      = 4,
    DELIVER      = 5,
    DISCONNECT   = 6,
    ERROR_MSG    = 7
};

struct Packet {
    PacketType           type;
    std::vector<uint8_t> payload;

    /** Sérialise le paquet en bytes prêts à être envoyés */
    std::vector<uint8_t> serialize() const;

    /**
     * Tente de désérialiser un paquet depuis un buffer.
     * Retourne true si un paquet complet a été lu.
     * 'consumed' indique le nombre d'octets consommés.
     */
    static bool deserialize(const std::vector<uint8_t>& buffer,
                            Packet& out,
                            size_t& consumed);
};

/**
 * Helpers pour construire les payloads des différents types de paquets.
 * Format JSON simple encodé en UTF-8 dans le payload.
 */
namespace proto {

// CLIENT → SERVEUR : {"pubkey": "<hex>"}
std::vector<uint8_t> makeHandshake(const std::string& pubkeyHex);

// SERVEUR → CLIENT : {"pubkey": "<hex>", "users": ["<hex>", ...]}
std::vector<uint8_t> makeHandshakeOk(const std::string& serverPubHex,
                                      const std::vector<std::string>& userList);

// SERVEUR → CLIENT : {"users": ["<hex>", ...]}
std::vector<uint8_t> makeUserList(const std::vector<std::string>& userList);

// CLIENT → SERVEUR : {"to": "<hex>", "from": "<hex>", "data": "<hex_encrypted>"}
std::vector<uint8_t> makeMessage(const std::string& toHex,
                                  const std::string& fromHex,
                                  const std::vector<uint8_t>& encryptedData);

// SERVEUR → CLIENT : {"from": "<hex>", "data": "<hex_encrypted>"}
std::vector<uint8_t> makeDeliver(const std::string& fromHex,
                                  const std::vector<uint8_t>& encryptedData);

// Parse un payload JSON simple (clé/valeur strings uniquement)
// Retourne une map string→string
#include <map>
std::map<std::string, std::string> parseSimpleJson(const std::vector<uint8_t>& payload);

// Parse le champ "users" (tableau JSON de strings)
std::vector<std::string> parseUserList(const std::string& jsonUsers);

} // namespace proto
} // namespace sovlink
