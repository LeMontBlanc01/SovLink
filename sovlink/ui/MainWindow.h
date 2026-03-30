#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include "Client.h"
#include "LoginWindow.h"
#include "ChatWindow.h"

namespace sovlink {

/**
 * MainWindow — Fenêtre principale de l'application SovLink
 *
 * Orchestre la navigation entre :
 *   - LoginWindow  : écran de connexion
 *   - ChatWindow   : interface de chat
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onConnectRequested(const QString& host, quint16 port);
    void onConnected();
    void onDisconnected();
    void onConnectionError(const QString& msg);

private:
    Client*        m_client;
    QStackedWidget* m_stack;
    LoginWindow*   m_loginWindow;
    ChatWindow*    m_chatWindow;
};

} // namespace sovlink
