#pragma once

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "Client.h"

namespace sovlink {

/**
 * ChatWindow — Interface de messagerie principale
 *
 * Affiche :
 *  - La liste des utilisateurs connectés (à gauche)
 *  - L'historique des messages (zone principale)
 *  - Le champ de saisie + bouton d'envoi (bas)
 *  - Les infos de session (haut)
 */
class ChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatWindow(Client* client, QWidget* parent = nullptr);

public slots:
    void appendMessage(const sovlink::Message& msg);
    void updateUserList(const QStringList& users);
    void showDeliveryError(const QString& msg);

signals:
    void disconnectRequested();

private slots:
    void onSendClicked();
    void onUserSelected(QListWidgetItem* item);
    void onEnterPressed();

private:
    void setupUi();
    void applyStyle();
    void selectUser(const QString& pubkeyHex);

    Client*      m_client;

    // Destinataire actuellement sélectionné
    QString      m_selectedPubkeyHex;

    // Widgets
    QLabel*      m_lblSessionId;
    QLabel*      m_lblEncStatus;
    QLabel*      m_lblMyId;
    QListWidget* m_userList;
    QTextEdit*   m_chatArea;
    QLineEdit*   m_inputField;
    QPushButton* m_btnSend;
    QPushButton* m_btnDisconnect;
    QLabel*      m_lblRecipient;
    QLabel*      m_lblError;
    QTimer*      m_errorTimer;
};

} // namespace sovlink
