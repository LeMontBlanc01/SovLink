#pragma once

#include <string>
#include <vector>
#include <sodium.h>

namespace sovlink {

/**
 * Crypto — gestion des clés publiques/privées et du chiffrement
 * Utilise libsodium : crypto_box (Curve25519 + XSalsa20 + Poly1305)
 */
class Crypto {
public:
    static constexpr size_t PUBLIC_KEY_SIZE  = crypto_box_PUBLICKEYBYTES;  // 32
    static constexpr size_t SECRET_KEY_SIZE  = crypto_box_SECRETKEYBYTES;  // 32
    static constexpr size_t NONCE_SIZE       = crypto_box_NONCEBYTES;      // 24
    static constexpr size_t MAC_SIZE         = crypto_box_MACBYTES;        // 16

    using PublicKey = std::vector<unsigned char>;
    using SecretKey = std::vector<unsigned char>;
    using Bytes     = std::vector<unsigned char>;

    /**
     * Initialise libsodium (à appeler une seule fois au démarrage)
     * Retourne false si l'initialisation échoue
     */
    static bool init();

    /**
     * Génère une paire de clés (clé publique + clé privée)
     * La clé publique sert d'identifiant temporaire pour la session
     */
    static void generateKeyPair(PublicKey& pub, SecretKey& sec);

    /**
     * Chiffre un message pour un destinataire donné (clé publique destinataire)
     * Utilise notre clé privée + clé publique destinataire
     * Retourne le message chiffré (nonce || ciphertext)
     */
    static Bytes encrypt(const std::string&  plaintext,
                         const PublicKey&    recipientPub,
                         const SecretKey&    senderSec);

    /**
     * Déchiffre un message reçu
     * Utilise notre clé privée + clé publique expéditeur
     * Retourne le texte clair, ou "" en cas d'échec
     */
    static std::string decrypt(const Bytes&      cipherData,
                               const PublicKey&  senderPub,
                               const SecretKey&  recipientSec);

    /**
     * Encode une clé en hexadécimal (pour l'affichage / l'identifiant)
     */
    static std::string toHex(const Bytes& data);

    /**
     * Décode une chaîne hexadécimale en bytes
     */
    static Bytes fromHex(const std::string& hex);

    /**
     * Retourne une représentation courte (16 chars) d'une clé publique
     * utilisée comme identifiant visible dans l'UI
     */
    static std::string shortId(const PublicKey& pub);
};

} // namespace sovlink
