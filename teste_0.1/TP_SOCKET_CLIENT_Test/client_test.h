#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QTextStream>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QRandomGenerator>
#include <QDateTime>

// ── Chiffrement RSA léger via QByteArray + OpenSSL wrapper ──────────────────
// On utilise la lib standard Qt + un mini RSA maison basé sur QCA ou sur
// une implémentation simple sans dépendance externe (choix ici : sans dépendance)
// On génère une paire RSA-2048 et on stocke les clés en PEM (base64 + header)
// ─────────────────────────────────────────────────────────────────────────────

struct Message {
    QString     text;
    QDateTime   receivedAt;
    int         ttlSeconds;   // durée de vie (-1 = infini)
    QTimer     *timer = nullptr;
};

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    void connectToServer(const QString& ip);
    void sendMessage(const QString& text, int ttlSeconds = -1);
    void messageManualInput();
    void setTargetKey(const QString& key);

    QString publicKey()  const { return m_myPubKey;  }
    QString privateKey() const { return m_myPrivKey; }


signals:
    void messageReceived(const QString& text, int ttlSeconds);
    void connected();
    void disconnected();

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void processPendingDatagrams();

private:
    // Génère une paire RSA simple (XOR + padding maison, sans lib externe)
    void generateKeyPair();

    // Chiffre avec la clé publique du destinataire
    QByteArray encrypt(const QByteArray& data, const QString& pubKey);

    // Déchiffre avec notre clé privée
    QByteArray decrypt(const QByteArray& data);

    QTcpSocket  *m_socket;
    QUdpSocket  *m_udpListener;
    QTimer      *m_fallbackTimer;

    QString      m_myPubKey;
    QString      m_myPrivKey;
    QString      m_targetPubKey;
    bool         m_registered = false;
    quint32  m_blockSize = 0;
};

#endif
