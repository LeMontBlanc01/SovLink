#include "Crypto.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace sovlink {

bool Crypto::init() {
    return sodium_init() >= 0;
}

void Crypto::generateKeyPair(PublicKey& pub, SecretKey& sec) {
    pub.resize(PUBLIC_KEY_SIZE);
    sec.resize(SECRET_KEY_SIZE);
    crypto_box_keypair(pub.data(), sec.data());
}

Crypto::Bytes Crypto::encrypt(const std::string&  plaintext,
                               const PublicKey&    recipientPub,
                               const SecretKey&    senderSec)
{
    if (recipientPub.size() != PUBLIC_KEY_SIZE || senderSec.size() != SECRET_KEY_SIZE)
        throw std::invalid_argument("Crypto::encrypt : tailles de clés invalides");

    // Générer un nonce aléatoire
    Bytes nonce(NONCE_SIZE);
    randombytes_buf(nonce.data(), NONCE_SIZE);

    // Buffer de sortie : MAC + ciphertext
    Bytes ciphertext(MAC_SIZE + plaintext.size());

    int ret = crypto_box_easy(
        ciphertext.data(),
        reinterpret_cast<const unsigned char*>(plaintext.data()),
        plaintext.size(),
        nonce.data(),
        recipientPub.data(),
        senderSec.data()
    );

    if (ret != 0)
        throw std::runtime_error("Crypto::encrypt : échec du chiffrement");

    // Retourner nonce || ciphertext
    Bytes result;
    result.insert(result.end(), nonce.begin(), nonce.end());
    result.insert(result.end(), ciphertext.begin(), ciphertext.end());
    return result;
}

std::string Crypto::decrypt(const Bytes&      cipherData,
                             const PublicKey&  senderPub,
                             const SecretKey&  recipientSec)
{
    if (cipherData.size() < NONCE_SIZE + MAC_SIZE)
        return ""; // données trop courtes

    if (senderPub.size() != PUBLIC_KEY_SIZE || recipientSec.size() != SECRET_KEY_SIZE)
        return "";

    // Extraire nonce et ciphertext
    Bytes nonce(cipherData.begin(), cipherData.begin() + NONCE_SIZE);
    Bytes ciphertext(cipherData.begin() + NONCE_SIZE, cipherData.end());

    // Buffer pour le texte clair
    std::vector<unsigned char> plaintext(ciphertext.size() - MAC_SIZE);

    int ret = crypto_box_open_easy(
        plaintext.data(),
        ciphertext.data(),
        ciphertext.size(),
        nonce.data(),
        senderPub.data(),
        recipientSec.data()
    );

    if (ret != 0)
        return ""; // Déchiffrement échoué (message falsifié ou mauvaise clé)

    return std::string(plaintext.begin(), plaintext.end());
}

std::string Crypto::toHex(const Bytes& data) {
    std::ostringstream oss;
    for (unsigned char b : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return oss.str();
}

Crypto::Bytes Crypto::fromHex(const std::string& hex) {
    if (hex.size() % 2 != 0)
        throw std::invalid_argument("Crypto::fromHex : longueur impaire");
    Bytes result;
    result.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        unsigned char byte = (unsigned char)std::stoi(hex.substr(i, 2), nullptr, 16);
        result.push_back(byte);
    }
    return result;
}

std::string Crypto::shortId(const PublicKey& pub) {
    return toHex(pub).substr(0, 16);
}

} // namespace sovlink
