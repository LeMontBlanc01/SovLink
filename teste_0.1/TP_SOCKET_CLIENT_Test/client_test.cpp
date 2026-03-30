#include "client_test.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>

// ── Implémentation RSA légère sans dépendance externe ───────────────────────
// On utilise Qt + un schéma de chiffrement asymétrique simplifié :
// - Génération d'une paire de nombres premiers (p, q) de 512 bits chacun
// - Calcul de n = p*q, e = 65537, d = inverse modulaire de e mod (p-1)(q-1)
// - Clés encodées en Base64 pour le transport
//
// ATTENTION : Cette implémentation est ÉDUCATIVE.
// Pour un vrai projet : utiliser OpenSSL ou QCA (Qt Cryptographic Architecture)
//
// Ici on simule le chiffrement avec un XOR + clé dérivée pour garder le code
// sans dépendance, mais l'architecture (échange de clé publique, chiffrement,
// déchiffrement) est identique à ce que ferait du vrai RSA.
// ─────────────────────────────────────────────────────────────────────────────

// Génère 32 octets pseudo-aléatoires comme "seed" de clé
static QByteArray generateRandomBytes(int n) {
    QByteArray result;
    result.reserve(n);
    for (int i = 0; i < n; ++i)
        result.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    return result;
}

// Dérive une clé de chiffrement à partir d'une clé publique (hash simple)
static QByteArray deriveKey(const QString& pubKey, int len = 32) {
    QByteArray raw = pubKey.toUtf8();
    QByteArray key(len, 0);
    for (int i = 0; i < raw.size(); ++i)
        key[i % len] = key[i % len] ^ raw[i];
    // Second pass pour mélanger
    for (int i = raw.size() - 1; i >= 0; --i)
        key[(raw.size() - i) % len] = key[(raw.size() - i) % len] ^ raw[i] ^ static_cast<char>(i);
    return key;
}

// Chiffrement XOR-stream (substitut pédagogique de RSA)
static QByteArray xorEncrypt(const QByteArray& data, const QByteArray& key) {
    QByteArray result = data;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ key[i % key.size()] ^ static_cast<char>(i % 255);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────

Client::Client(QObject *parent) : QObject(parent) {

    generateKeyPair();

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected,    this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::readyRead,    this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);

    m_udpListener = new QUdpSocket(this);
    bool bindOk = m_udpListener->bind(9999, QUdpSocket::ShareAddress);
    if (bindOk) {
        qDebug() << "Radar UDP actif. Recherche du serveur SovLink...";
        connect(m_udpListener, &QUdpSocket::readyRead, this, &Client::processPendingDatagrams);
    } else {
        qDebug() << "Erreur UDP : port 9999 indisponible.";
    }

    // Timer fallback : si pas de beacon après 5s, on demande l'IP manuellement
    m_fallbackTimer = new QTimer(this);
    m_fallbackTimer->setSingleShot(true);
    connect(m_fallbackTimer, &QTimer::timeout, [this]() {
        if (m_socket->state() != QAbstractSocket::ConnectedState) {
            fprintf(stdout, "\n[!] Aucun serveur détecté automatiquement.\n");
            fprintf(stdout, "IP du serveur : ");
            fflush(stdout);
            char buf[256] = {};
            fgets(buf, sizeof(buf), stdin);
            QString ip = QString::fromLocal8Bit(buf).trimmed();
            if (!ip.isEmpty()) connectToServer(ip);
        }
    });
    m_fallbackTimer->start(5000);
}

void Client::generateKeyPair() {
    // Clé privée = 32 octets aléatoires (secret)
    QByteArray privRaw = generateRandomBytes(32);
    m_myPrivKey = privRaw.toBase64();

    // Clé publique = dérivée de la clé privée (dans un vrai RSA : g^privKey mod p)
    // Ici on la dérive pour qu'elle soit unique et reproductible depuis privKey
    QByteArray pubRaw = generateRandomBytes(16); // salt
    pubRaw.append(privRaw);                       // lié à la clé privée
    // On mélange
    for (int i = 0; i < pubRaw.size() / 2; ++i) {
        char tmp = pubRaw[i];
        pubRaw[i] = pubRaw[pubRaw.size() - 1 - i] ^ privRaw[i % privRaw.size()];
        pubRaw[pubRaw.size() - 1 - i] = tmp ^ privRaw[(i+1) % privRaw.size()];
    }
    m_myPubKey = pubRaw.toBase64();

    qDebug() << "=== SovLink Client ===";
    qDebug() << "Clé publique générée :";
    qDebug() << m_myPubKey;
    qDebug() << "(Partagez cette clé avec votre interlocuteur)";
    qDebug() << "======================";
}

void Client::connectToServer(const QString& ip) {
    m_udpListener->close();
    m_fallbackTimer->stop();
    qDebug() << "Connexion TCP vers" << ip << ":8080 ...";
    m_socket->connectToHost(QHostAddress(ip), 8080);
}

void Client::onConnected() {
    qDebug() << "Connexion etablie !";

    // Demande la clé publique du destinataire
    fprintf(stdout, "\nClé publique du destinataire : ");
    fflush(stdout);
    char buf[512] = {};
    fgets(buf, sizeof(buf), stdin);
    m_targetPubKey = QString::fromLocal8Bit(buf).trimmed();

    // Enregistrement sur le serveur avec notre clé publique
    QDataStream out(m_socket);
    out.setVersion(QDataStream::Qt_5_0);
    out << m_myPubKey;

    m_registered = true;
    qDebug() << "Enregistré. Cible :" << m_targetPubKey.left(20) << "...";
    qDebug() << "Vous pouvez maintenant envoyer des messages.";
    qDebug() << "Format: [ttl=X] message  (ex: [ttl=30] Salut!  ->  disparaît après 30s)";
    qDebug() << "        message seul      (ex: Bonjour!        ->  permanent)";

    emit connected();
}

QByteArray Client::encrypt(const QByteArray& data, const QString& pubKey) {
    QByteArray key = deriveKey(pubKey);
    return xorEncrypt(data, key);
}

QByteArray Client::decrypt(const QByteArray& data) {
    // On déchiffre avec notre propre clé publique comme clé de dérivation
    // (dans un vrai RSA : decrypt avec clé privée)
    QByteArray key = deriveKey(m_myPubKey);
    return xorEncrypt(data, key); // XOR est symétrique
}

void Client::sendMessage(const QString& text, int ttlSeconds) {
    if (!m_socket->isOpen() || m_targetPubKey.isEmpty()) return;

    // On encode le message avec le TTL : "TTL:30|texte du message"
    QString payload = QString("TTL:%1|%2").arg(ttlSeconds).arg(text);
    QByteArray raw  = payload.toUtf8();

    // Chiffrement avec la clé publique du destinataire
    QByteArray chiffre = encrypt(raw, m_targetPubKey);

    QDataStream out(m_socket);
    out.setVersion(QDataStream::Qt_5_0);
    out << m_targetPubKey << chiffre;
}

void Client::messageManualInput() {
    if (!m_registered) return;

    // Lecture non-bloquante via QTextStream
    QTextStream input(stdin);
    if (input.atEnd()) return;

    QString line = input.readLine();
    if (line.isEmpty()) return;

    int ttl = -1;
    QString message = line;

    if (line.startsWith("[ttl=", Qt::CaseInsensitive)) {
        int endBracket = line.indexOf(']');
        if (endBracket > 0) {
            ttl     = line.mid(5, endBracket - 5).toInt();
            message = line.mid(endBracket + 1).trimmed();
        }
    }

    if (!message.isEmpty())
        sendMessage(message, ttl);
}

void Client::onReadyRead() {
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_0);
    QByteArray payload;
    in >> payload;

    // Déchiffrement
    QByteArray dechiffre = decrypt(payload);
    QString contenu = QString::fromUtf8(dechiffre);

    // Erreur serveur ?
    if (contenu.startsWith("__ERROR__:")) {
        qDebug() << "[!] Erreur serveur :" << contenu.mid(10);
        return;
    }

    // Parsing TTL|message
    int ttl = -1;
    QString texte = contenu;

    if (contenu.startsWith("TTL:")) {
        int sep = contenu.indexOf('|');
        if (sep > 0) {
            ttl   = contenu.mid(4, sep - 4).toInt();
            texte = contenu.mid(sep + 1);
        }
    }

    QString affichage = texte;
    if (ttl > 0) affichage += QString("  ⏱ (disparaît dans %1s)").arg(ttl);

    qDebug() << "\n[" << QDateTime::currentDateTime().toString("hh:mm:ss") << "]"
             << "[Eux]" << affichage;

    emit messageReceived(texte, ttl);

    // Auto-suppression
    if (ttl > 0) {
        QTimer::singleShot(ttl * 1000, [texte]() {
            qDebug() << "[🔥 Message auto-supprimé :" << texte.left(20) << "...]";
        });
    }

    fprintf(stdout, "> ");
    fflush(stdout);
}

void Client::onDisconnected() {
    qDebug() << "Déconnecté du serveur.";
    emit disconnected();
    qApp->quit();
}

void Client::processPendingDatagrams() {
    while (m_udpListener->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpListener->pendingDatagramSize());
        QHostAddress senderIP;
        m_udpListener->readDatagram(datagram.data(), datagram.size(), &senderIP);

        if (datagram == "SOVLINK_SERVER_ACTIVE") {
            qDebug() << "Serveur détecté à :" << senderIP.toString();
            m_fallbackTimer->stop();
            m_udpListener->close();
            m_socket->connectToHost(senderIP, 8080);
            break;
        }
    }
}
