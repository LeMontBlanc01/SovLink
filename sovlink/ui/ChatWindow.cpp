#include "ChatWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollBar>
#include <QDateTime>
#include <QListWidgetItem>
#include <QApplication>
#include <QClipboard>

namespace sovlink {

ChatWindow::ChatWindow(Client* client, QWidget* parent)
    : QWidget(parent)
    , m_client(client)
    , m_errorTimer(new QTimer(this))
{
    setWindowTitle("SovLink — Secure Channel");
    resize(820, 560);
    setupUi();
    applyStyle();

    m_errorTimer->setSingleShot(true);
    m_errorTimer->setInterval(4000);
    connect(m_errorTimer, &QTimer::timeout, this, [this]() {
        m_lblError->hide();
    });
}

void ChatWindow::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Barre de titre / session ──────────────────────────────────────────
    auto* topBar = new QWidget(this);
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(52);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 0, 16, 0);

    auto* lblTitle = new QLabel("✦  SOVLINK SECURE CHANNEL", topBar);
    lblTitle->setObjectName("channelTitle");
    topLayout->addWidget(lblTitle);

    topLayout->addStretch();

    // Session ID = 8 premiers chars de notre clé
    QString sessionId = m_client->myShortId().left(8).toUpper();
    m_lblSessionId = new QLabel("Session ID: " + sessionId, topBar);
    m_lblSessionId->setObjectName("sessionInfo");
    topLayout->addWidget(m_lblSessionId);

    topLayout->addSpacing(16);

    m_lblEncStatus = new QLabel("🔒 Chiffrement: ACTIF", topBar);
    m_lblEncStatus->setObjectName("encStatus");
    topLayout->addWidget(m_lblEncStatus);

    topLayout->addSpacing(16);

    m_btnDisconnect = new QPushButton("Déconnecter", topBar);
    m_btnDisconnect->setObjectName("btnDisconnect");
    topLayout->addWidget(m_btnDisconnect);

    root->addWidget(topBar);

    // ── Séparateur doré ──
    auto* sep = new QWidget(this);
    sep->setObjectName("goldSep");
    sep->setFixedHeight(1);
    root->addWidget(sep);

    // ── Zone principale (liste users + chat) ─────────────────────────────
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);
    splitter->setObjectName("mainSplitter");

    // Panneau gauche : utilisateurs connectés
    auto* leftPanel = new QWidget(splitter);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setFixedWidth(200);
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    auto* lblUsersTitle = new QLabel("  UTILISATEURS EN LIGNE", leftPanel);
    lblUsersTitle->setObjectName("usersTitle");
    lblUsersTitle->setFixedHeight(36);
    leftLayout->addWidget(lblUsersTitle);

    auto* userSep = new QWidget(leftPanel);
    userSep->setObjectName("goldSep");
    userSep->setFixedHeight(1);
    leftLayout->addWidget(userSep);

    m_userList = new QListWidget(leftPanel);
    m_userList->setObjectName("userList");
    leftLayout->addWidget(m_userList);

    // Info "Mon ID"
    auto* myIdWidget = new QWidget(leftPanel);
    myIdWidget->setObjectName("myIdBox");
    auto* myIdLayout = new QVBoxLayout(myIdWidget);
    myIdLayout->setContentsMargins(8, 6, 8, 6);
    myIdLayout->setSpacing(2);
    auto* lblMyIdTitle = new QLabel("Mon identifiant :", myIdWidget);
    lblMyIdTitle->setObjectName("myIdTitle");
    m_lblMyId = new QLabel(m_client->myShortId() + "...", myIdWidget);
    m_lblMyId->setObjectName("myIdValue");
    m_lblMyId->setToolTip(m_client->myPubkeyHex());
    auto* btnCopyMyId = new QPushButton("Copier ma clé", myIdWidget);
    btnCopyMyId->setObjectName("btnCopyId");
    myIdLayout->addWidget(lblMyIdTitle);
    myIdLayout->addWidget(m_lblMyId);
    myIdLayout->addWidget(btnCopyMyId);
    leftLayout->addWidget(myIdWidget);

    connect(btnCopyMyId, &QPushButton::clicked, this, [this, btnCopyMyId]() {
        QApplication::clipboard()->setText(m_client->myPubkeyHex());
        btnCopyMyId->setText("✓ Copié !");
        QTimer::singleShot(1500, this, [btnCopyMyId]() { btnCopyMyId->setText("Copier ma clé"); });
    });

    splitter->addWidget(leftPanel);

    // Panneau droit : chat
    auto* rightPanel = new QWidget(splitter);
    rightPanel->setObjectName("rightPanel");
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Barre destinataire
    auto* recipientBar = new QWidget(rightPanel);
    recipientBar->setObjectName("recipientBar");
    recipientBar->setFixedHeight(36);
    auto* recLayout = new QHBoxLayout(recipientBar);
    recLayout->setContentsMargins(12, 0, 12, 0);
    m_lblRecipient = new QLabel("Sélectionnez un utilisateur dans la liste", recipientBar);
    m_lblRecipient->setObjectName("recipientLabel");
    recLayout->addWidget(m_lblRecipient);
    rightLayout->addWidget(recipientBar);

    auto* recSep = new QWidget(rightPanel);
    recSep->setObjectName("darkSep");
    recSep->setFixedHeight(1);
    rightLayout->addWidget(recSep);

    // Zone de messages
    m_chatArea = new QTextEdit(rightPanel);
    m_chatArea->setObjectName("chatArea");
    m_chatArea->setReadOnly(true);
    rightLayout->addWidget(m_chatArea, 1);

    // Message d'erreur
    m_lblError = new QLabel(rightPanel);
    m_lblError->setObjectName("errorBar");
    m_lblError->setAlignment(Qt::AlignCenter);
    m_lblError->setFixedHeight(28);
    m_lblError->hide();
    rightLayout->addWidget(m_lblError);

    // Barre de saisie
    auto* inputSep = new QWidget(rightPanel);
    inputSep->setObjectName("goldSep");
    inputSep->setFixedHeight(1);
    rightLayout->addWidget(inputSep);

    auto* inputBar = new QWidget(rightPanel);
    inputBar->setObjectName("inputBar");
    inputBar->setFixedHeight(52);
    auto* inputLayout = new QHBoxLayout(inputBar);
    inputLayout->setContentsMargins(12, 8, 12, 8);
    inputLayout->setSpacing(8);

    m_inputField = new QLineEdit(inputBar);
    m_inputField->setObjectName("inputField");
    m_inputField->setPlaceholderText("Envoyer un message...");
    inputLayout->addWidget(m_inputField, 1);

    m_btnSend = new QPushButton("➤", inputBar);
    m_btnSend->setObjectName("btnSend");
    m_btnSend->setFixedSize(36, 36);
    m_btnSend->setEnabled(false);
    inputLayout->addWidget(m_btnSend);
    rightLayout->addWidget(inputBar);

    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    root->addWidget(splitter, 1);

    // ── Connexions ───────────────────────────────────────────────────────
    connect(m_btnSend,       &QPushButton::clicked,     this, &ChatWindow::onSendClicked);
    connect(m_inputField,    &QLineEdit::returnPressed,  this, &ChatWindow::onEnterPressed);
    connect(m_userList,      &QListWidget::itemClicked,  this, &ChatWindow::onUserSelected);
    connect(m_btnDisconnect, &QPushButton::clicked,      this, &ChatWindow::disconnectRequested);
}

void ChatWindow::applyStyle() {
    setStyleSheet(R"(
        ChatWindow { background: #1A0E08; }

        QWidget#topBar {
            background: #0D0703;
            border-bottom: 1px solid #3A1A0A;
        }
        QLabel#channelTitle {
            font-family: 'Oswald', 'Impact', sans-serif;
            font-size: 14px;
            font-weight: bold;
            color: #D9C9A8;
            letter-spacing: 4px;
        }
        QLabel#sessionInfo { font-family: 'Courier New', monospace; font-size: 11px; color: #9C8A6C; }
        QLabel#encStatus   { font-size: 11px; color: #4CAF50; }

        QPushButton#btnDisconnect {
            background: transparent;
            color: #9C8A6C;
            border: 1px solid #3A1A0A;
            font-size: 11px;
            padding: 4px 10px;
            border-radius: 2px;
        }
        QPushButton#btnDisconnect:hover { color: #B84332; border-color: #B84332; }

        QWidget#goldSep { background: #C9A84C; }
        QWidget#darkSep { background: #2A1208; }

        /* ─ Panneau gauche ─ */
        QWidget#leftPanel   { background: #0D0703; }
        QLabel#usersTitle   { font-size: 10px; letter-spacing: 2px; color: #9C8A6C; padding-left: 8px; background: #0A0502; }

        QListWidget#userList {
            background: #0D0703;
            border: none;
            color: #D9C9A8;
            font-family: 'Courier New', monospace;
            font-size: 11px;
        }
        QListWidget#userList::item { padding: 8px 12px; border-bottom: 1px solid #1A0E08; }
        QListWidget#userList::item:hover    { background: #1A0A05; }
        QListWidget#userList::item:selected { background: #5C1A0D; color: #E8C96A; }

        QWidget#myIdBox    { background: #0A0502; border-top: 1px solid #2A1208; }
        QLabel#myIdTitle   { font-size: 10px; color: #6C5A3C; }
        QLabel#myIdValue   { font-family: 'Courier New', monospace; font-size: 10px; color: #4CAF50; }
        QPushButton#btnCopyId {
            background: transparent; color: #C9A84C;
            border: 1px solid #3A2210; font-size: 10px;
            padding: 3px; border-radius: 2px;
        }
        QPushButton#btnCopyId:hover { border-color: #C9A84C; }

        /* ─ Panneau droit ─ */
        QWidget#rightPanel  { background: #120805; }
        QWidget#recipientBar { background: #0D0703; }
        QLabel#recipientLabel { font-family: 'Courier New', monospace; font-size: 11px; color: #C9A84C; }

        QTextEdit#chatArea {
            background: #120805;
            border: none;
            color: #D9C9A8;
            font-size: 13px;
            padding: 8px;
        }
        QScrollBar:vertical { background: #0D0703; width: 6px; }
        QScrollBar::handle:vertical { background: #5C1A0D; border-radius: 3px; }

        QLabel#errorBar { background: #3A0A05; color: #FF6B6B; font-size: 11px; }

        QWidget#inputBar { background: #0D0703; }
        QLineEdit#inputField {
            background: #1A0E08;
            border: 1px solid #3A1A0A;
            color: #D9C9A8;
            font-size: 13px;
            padding: 6px 10px;
            border-radius: 2px;
        }
        QLineEdit#inputField:focus { border-color: #C9A84C; }

        QPushButton#btnSend {
            background: #8B2A1A;
            color: #D9C9A8;
            border: 1px solid #C9A84C;
            font-size: 16px;
            border-radius: 2px;
        }
        QPushButton#btnSend:hover   { background: #B84332; }
        QPushButton#btnSend:pressed { background: #5C1A0D; }
        QPushButton#btnSend:disabled { background: #1A0A05; color: #3A2210; border-color: #2A1208; }

        QSplitter#mainSplitter::handle { background: #2A1208; }
    )");
}

// ─── Slots ───────────────────────────────────────────────────────────────────

void ChatWindow::updateUserList(const QStringList& users) {
    m_userList->clear();
    for (const QString& hex : users) {
        auto* item = new QListWidgetItem(
            "● " + hex.left(16) + "...",
            m_userList
        );
        item->setData(Qt::UserRole, hex);  // Stocker la clé complète
        item->setToolTip(hex);
    }
    if (users.isEmpty()) {
        auto* item = new QListWidgetItem("(aucun utilisateur)", m_userList);
        item->setFlags(Qt::NoItemFlags);
    }
}

void ChatWindow::onUserSelected(QListWidgetItem* item) {
    if (!item || !(item->flags() & Qt::ItemIsEnabled)) return;
    QString pubkey = item->data(Qt::UserRole).toString();
    selectUser(pubkey);
}

void ChatWindow::selectUser(const QString& pubkeyHex) {
    m_selectedPubkeyHex = pubkeyHex;
    m_lblRecipient->setText("Conversation avec : " + pubkeyHex.left(16) + "...");
    m_btnSend->setEnabled(true);
    m_inputField->setFocus();
}

void ChatWindow::appendMessage(const sovlink::Message& msg) {
    QString time = QDateTime::currentDateTime().toString("hh:mm");

    QString html;
    if (msg.isOwn) {
        html = QString(
            "<div style='text-align:right; margin: 6px 0;'>"
            "<span style='color:#9C8A6C; font-size:10px; font-family:monospace;'>Vous — %1</span><br>"
            "<span style='background:#8B2A1A; color:#D9C9A8; padding:4px 10px;"
            " border-radius:3px; display:inline-block;'>%2</span>"
            "</div>"
        ).arg(time).arg(msg.text.toHtmlEscaped());
    } else {
        html = QString(
            "<div style='text-align:left; margin: 6px 0;'>"
            "<span style='color:#4CAF50; font-size:10px; font-family:monospace;'>%1 — %2</span><br>"
            "<span style='background:#1A0E08; color:#D9C9A8; padding:4px 10px;"
            " border:1px solid #3A1A0A; border-radius:3px; display:inline-block;'>%3</span>"
            "</div>"
        ).arg(msg.fromId).arg(time).arg(msg.text.toHtmlEscaped());
    }

    m_chatArea->append(html);

    // Scroller automatiquement vers le bas
    QScrollBar* sb = m_chatArea->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void ChatWindow::onSendClicked() {
    QString text = m_inputField->text().trimmed();
    if (text.isEmpty() || m_selectedPubkeyHex.isEmpty()) return;

    bool ok = m_client->sendMessage(m_selectedPubkeyHex, text);
    if (ok) m_inputField->clear();
}

void ChatWindow::onEnterPressed() {
    onSendClicked();
}

void ChatWindow::showDeliveryError(const QString& msg) {
    m_lblError->setText("⚠ " + msg);
    m_lblError->show();
    m_errorTimer->start();
}

} // namespace sovlink
