#include "MainWindow.h"
#include <QApplication>

namespace sovlink {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Créer le client (génère les clés dès maintenant)
    m_client = new Client(this);

    // Créer les écrans
    m_loginWindow = new LoginWindow(m_client->myPubkeyHex(), this);
    m_chatWindow  = new ChatWindow(m_client, this);

    // Stack
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_loginWindow);  // index 0
    m_stack->addWidget(m_chatWindow);   // index 1
    m_stack->setCurrentIndex(0);

    setCentralWidget(m_stack);
    setWindowTitle("SovLink — Secure Messaging");

    // ── Connexions Client ↔ UI ──────────────────────────────────────────
    connect(m_loginWindow, &LoginWindow::connectRequested,
            this,          &MainWindow::onConnectRequested);

    connect(m_client, &Client::connected,
            this,     &MainWindow::onConnected);

    connect(m_client, &Client::disconnected,
            this,     &MainWindow::onDisconnected);

    connect(m_client, &Client::connectionError,
            this,     &MainWindow::onConnectionError);

    connect(m_client, &Client::messageReceived,
            m_chatWindow, &ChatWindow::appendMessage);

    connect(m_client, &Client::userListUpdated,
            m_chatWindow, &ChatWindow::updateUserList);

    connect(m_client, &Client::deliveryError,
            m_chatWindow, &ChatWindow::showDeliveryError);

    connect(m_chatWindow, &ChatWindow::disconnectRequested,
            m_client,     &Client::disconnectFromServer);
}

// ─── Slots ───────────────────────────────────────────────────────────────────

void MainWindow::onConnectRequested(const QString& host, quint16 port) {
    m_loginWindow->setConnecting(true);
    m_client->connectToServer(host, port);
}

void MainWindow::onConnected() {
    m_stack->setCurrentIndex(1);  // Afficher le chat
    resize(820, 560);
}

void MainWindow::onDisconnected() {
    m_stack->setCurrentIndex(0);  // Revenir au login
    resize(520, 420);
    m_loginWindow->setConnecting(false);
}

void MainWindow::onConnectionError(const QString& msg) {
    m_loginWindow->showError("Erreur de connexion : " + msg);
}

} // namespace sovlink
