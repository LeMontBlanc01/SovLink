#include "Protocol.h"
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <map>

namespace sovlink {

// ─── Packet ──────────────────────────────────────────────────────────────────

std::vector<uint8_t> Packet::serialize() const {
    std::vector<uint8_t> buf;
    buf.reserve(8 + payload.size());

    // Type (4 bytes big-endian)
    uint32_t t = static_cast<uint32_t>(type);
    buf.push_back((t >> 24) & 0xFF);
    buf.push_back((t >> 16) & 0xFF);
    buf.push_back((t >>  8) & 0xFF);
    buf.push_back( t        & 0xFF);

    // Longueur payload (4 bytes big-endian)
    uint32_t len = static_cast<uint32_t>(payload.size());
    buf.push_back((len >> 24) & 0xFF);
    buf.push_back((len >> 16) & 0xFF);
    buf.push_back((len >>  8) & 0xFF);
    buf.push_back( len        & 0xFF);

    // Payload
    buf.insert(buf.end(), payload.begin(), payload.end());
    return buf;
}

bool Packet::deserialize(const std::vector<uint8_t>& buffer,
                         Packet& out,
                         size_t& consumed)
{
    consumed = 0;
    if (buffer.size() < 8) return false;  // En-tête incomplet

    uint32_t t = ((uint32_t)buffer[0] << 24)
                 | ((uint32_t)buffer[1] << 16)
                 | ((uint32_t)buffer[2] <<  8)
                 |  (uint32_t)buffer[3];

    uint32_t len = ((uint32_t)buffer[4] << 24)
                   | ((uint32_t)buffer[5] << 16)
                   | ((uint32_t)buffer[6] <<  8)
                   |  (uint32_t)buffer[7];

    if (buffer.size() < 8 + len) return false;  // Payload incomplet

    out.type    = static_cast<PacketType>(t);
    out.payload = std::vector<uint8_t>(buffer.begin() + 8,
                                       buffer.begin() + 8 + len);
    consumed = 8 + len;
    return true;
}

// ─── Helpers JSON ultra-simple ────────────────────────────────────────────────
// On évite une dépendance externe en implémentant un mini-JSON pour les besoins du projet

static std::string escapeJson(const std::string& s) {
    std::string r;
    for (char c : s) {
        if      (c == '"')  r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else                r += c;
    }
    return r;
}

static std::vector<uint8_t> toBytes(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}

// ─── proto:: ─────────────────────────────────────────────────────────────────

namespace proto {

std::vector<uint8_t> makeHandshake(const std::string& pubkeyHex) {
    std::string json = "{\"pubkey\":\"" + escapeJson(pubkeyHex) + "\"}";
    return toBytes(json);
}

std::vector<uint8_t> makeHandshakeOk(const std::string& serverPubHex,
                                     const std::vector<std::string>& userList)
{
    std::string json = "{\"pubkey\":\"" + escapeJson(serverPubHex) + "\","
                                                                     "\"users\":[";
    for (size_t i = 0; i < userList.size(); ++i) {
        if (i) json += ',';
        json += '"' + escapeJson(userList[i]) + '"';
    }
    json += "]}";
    return toBytes(json);
}

std::vector<uint8_t> makeUserList(const std::vector<std::string>& userList) {
    std::string json = "{\"users\":[";
    for (size_t i = 0; i < userList.size(); ++i) {
        if (i) json += ',';
        json += '"' + escapeJson(userList[i]) + '"';
    }
    json += "]}";
    return toBytes(json);
}

std::vector<uint8_t> makeMessage(const std::string& toHex,
                                 const std::string& fromHex,
                                 const std::vector<uint8_t>& encryptedData)
{
    // Encode les données chiffrées en hex pour le transport JSON
    std::ostringstream hexStream;
    for (uint8_t b : encryptedData)
        hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)b;

    std::string json = "{\"to\":\""   + escapeJson(toHex)   + "\","
                                                          "\"from\":\"" + escapeJson(fromHex) + "\","
                                               "\"data\":\"" + hexStream.str()     + "\"}";
    return toBytes(json);
}

std::vector<uint8_t> makeDeliver(const std::string& fromHex,
                                 const std::vector<uint8_t>& encryptedData)
{
    std::ostringstream hexStream;
    for (uint8_t b : encryptedData)
        hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)b;

    std::string json = "{\"from\":\"" + escapeJson(fromHex) + "\","
                                                              "\"data\":\"" + hexStream.str()     + "\"}";
    return toBytes(json);
}

// ─── Parseur JSON minimaliste ────────────────────────────────────────────────

std::map<std::string, std::string> parseSimpleJson(const std::vector<uint8_t>& payload) {
    std::string s(payload.begin(), payload.end());
    std::map<std::string, std::string> result;

    size_t i = 0;
    auto skipWs = [&]() { while (i < s.size() && (s[i]==' '||s[i]=='\n'||s[i]=='\r'||s[i]=='\t')) ++i; };
    auto readString = [&]() -> std::string {
        if (i >= s.size() || s[i] != '"') return "";
        ++i;
        std::string out;
        while (i < s.size() && s[i] != '"') {
            if (s[i] == '\\' && i+1 < s.size()) { ++i; out += s[i]; }
            else out += s[i];
            ++i;
        }
        if (i < s.size()) ++i; // consume closing "
        return out;
    };

    skipWs();
    if (i >= s.size() || s[i] != '{') return result;
    ++i;

    while (i < s.size()) {
        skipWs();
        if (s[i] == '}') break;
        if (s[i] == ',') { ++i; continue; }

        std::string key = readString();
        skipWs();
        if (i < s.size() && s[i] == ':') ++i;
        skipWs();

        // Valeur : string ou tableau (on stocke le tableau brut)
        if (i < s.size() && s[i] == '"') {
            result[key] = readString();
        } else if (i < s.size() && s[i] == '[') {
            size_t start = i;
            int depth = 0;
            while (i < s.size()) {
                if (s[i] == '[') ++depth;
                else if (s[i] == ']') { --depth; if (depth == 0) { ++i; break; } }
                ++i;
            }
            result[key] = s.substr(start, i - start);
        }
    }
    return result;
}

std::vector<std::string> parseUserList(const std::string& jsonArray) {
    std::vector<std::string> users;
    size_t i = 0;
    auto skipWs = [&]() { while (i < jsonArray.size() && (jsonArray[i]==' '||jsonArray[i]=='\n'||jsonArray[i]=='\r'||jsonArray[i]=='\t')) ++i; };

    skipWs();
    if (i >= jsonArray.size() || jsonArray[i] != '[') return users;
    ++i;

    while (i < jsonArray.size()) {
        skipWs();
        if (jsonArray[i] == ']') break;
        if (jsonArray[i] == ',') { ++i; continue; }
        if (jsonArray[i] == '"') {
            ++i;
            std::string u;
            while (i < jsonArray.size() && jsonArray[i] != '"') u += jsonArray[i++];
            if (i < jsonArray.size()) ++i;
            users.push_back(u);
        } else ++i;
    }
    return users;
}

} // namespace proto
} // namespace sovlink

// Fix manquant pour <iomanip>
#include <iomanip>
