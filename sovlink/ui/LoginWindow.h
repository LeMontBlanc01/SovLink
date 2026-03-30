#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>

namespace sovlink {

/**
 * LoginWindow — Page d'accueil SovLink
 *
 * Permet à l'utilisateur de :
 *  - Voir sa clé publique générée automatiquement
 *  - Entrer l'adresse du serveur et le port
 *  - Se connecter au serveur
 */
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(const QString& myPubkeyHex, QWidget* parent = nullptr);

signals:
    /** Émis quand l'utilisateur clique "Se connecter" */
    void connectRequested(const QString& host, quint16 port);

public slots:
    void setConnecting(bool connecting);
    void showError(const QString& msg);

private slots:
    void onConnectClicked();

private:
    void setupUi();
    void applyStyle();

    QString      m_myPubkeyHex;

    QLabel*      m_lblMyKey;
    QLabel*      m_lblStatus;
    QLineEdit*   m_editHost;
    QSpinBox*    m_spinPort;
    QLineEdit*   m_editRecipient;  // Clé publique du destinataire (optionnel au login)
    QPushButton* m_btnConnect;
    QPushButton* m_btnCopyKey;
};

} // namespace sovlink
