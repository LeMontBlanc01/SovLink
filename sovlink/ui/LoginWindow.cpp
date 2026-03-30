#include "LoginWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

namespace sovlink {

LoginWindow::LoginWindow(const QString& myPubkeyHex, QWidget* parent)
    : QWidget(parent)
    , m_myPubkeyHex(myPubkeyHex)
{
    setWindowTitle("SovLink — Secure Messaging Client");
    setFixedSize(520, 420);
    setupUi();
    applyStyle();
}

void LoginWindow::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(40, 32, 40, 32);
    root->setSpacing(0);

    // ── Logo ──
    auto* lblLogo = new QLabel("★  SOVLINK", this);
    lblLogo->setObjectName("logo");
    lblLogo->setAlignment(Qt::AlignCenter);
    root->addWidget(lblLogo);

    auto* lblSub = new QLabel("Secure Messaging Client", this);
    lblSub->setObjectName("subtitle");
    lblSub->setAlignment(Qt::AlignCenter);
    root->addWidget(lblSub);

    root->addSpacing(24);

    // ── Ma clé publique ──
    auto* lblKeyTitle = new QLabel("— Votre identifiant de session —", this);
    lblKeyTitle->setObjectName("sectionTitle");
    lblKeyTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(lblKeyTitle);

    root->addSpacing(8);

    auto* keyRow = new QHBoxLayout();
    keyRow->setSpacing(8);

    m_lblMyKey = new QLabel(m_myPubkeyHex.left(32) + "...", this);
    m_lblMyKey->setObjectName("pubkeyLabel");
    m_lblMyKey->setToolTip(m_myPubkeyHex);
    keyRow->addWidget(m_lblMyKey, 1);

    m_btnCopyKey = new QPushButton("Copier", this);
    m_btnCopyKey->setObjectName("btnSecondary");
    m_btnCopyKey->setFixedWidth(70);
    keyRow->addWidget(m_btnCopyKey);
    root->addLayout(keyRow);

    root->addSpacing(24);

    // ── Connexion serveur ──
    auto* lblConnTitle = new QLabel("— Connexion au serveur —", this);
    lblConnTitle->setObjectName("sectionTitle");
    lblConnTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(lblConnTitle);

    root->addSpacing(12);

    auto* grid = new QGridLayout();
    grid->setSpacing(8);

    auto* lblHost = new QLabel("Hôte :", this);
    lblHost->setObjectName("formLabel");
    m_editHost = new QLineEdit("127.0.0.1", this);
    m_editHost->setObjectName("formInput");
    m_editHost->setPlaceholderText("Adresse IP ou nom d'hôte");

    auto* lblPort = new QLabel("Port :", this);
    lblPort->setObjectName("formLabel");
    m_spinPort = new QSpinBox(this);
    m_spinPort->setObjectName("formInput");
    m_spinPort->setRange(1, 65535);
    m_spinPort->setValue(5555);

    grid->addWidget(lblHost,    0, 0);
    grid->addWidget(m_editHost, 0, 1);
    grid->addWidget(lblPort,    1, 0);
    grid->addWidget(m_spinPort, 1, 1);
    root->addLayout(grid);

    root->addSpacing(20);

    // ── Bouton connexion ──
    m_btnConnect = new QPushButton("SE CONNECTER", this);
    m_btnConnect->setObjectName("btnPrimary");
    m_btnConnect->setFixedHeight(42);
    root->addWidget(m_btnConnect);

    root->addSpacing(12);

    // ── Status ──
    m_lblStatus = new QLabel("", this);
    m_lblStatus->setObjectName("statusLabel");
    m_lblStatus->setAlignment(Qt::AlignCenter);
    m_lblStatus->setWordWrap(true);
    root->addWidget(m_lblStatus);

    root->addStretch();

    // ── Connections ──
    connect(m_btnConnect,  &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(m_btnCopyKey,  &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_myPubkeyHex);
        m_btnCopyKey->setText("✓");
        QTimer::singleShot(1500, this, [this]() { m_btnCopyKey->setText("Copier"); });
    });
}

void LoginWindow::applyStyle() {
    setStyleSheet(R"(
        LoginWindow {
            background-color: #1A0E08;
        }

        QLabel#logo {
            font-family: 'Oswald', 'Impact', sans-serif;
            font-size: 28px;
            font-weight: bold;
            color: #D9C9A8;
            letter-spacing: 6px;
        }

        QLabel#subtitle {
            font-size: 11px;
            color: #9C8A6C;
            letter-spacing: 3px;
        }

        QLabel#sectionTitle {
            font-size: 11px;
            color: #C9A84C;
            letter-spacing: 2px;
        }

        QLabel#pubkeyLabel {
            font-family: 'Courier New', monospace;
            font-size: 11px;
            color: #4CAF50;
            background: #0D0703;
            border: 1px solid #2E7D32;
            padding: 6px 10px;
            border-radius: 2px;
        }

        QLabel#formLabel {
            color: #9C8A6C;
            font-size: 12px;
            font-weight: bold;
        }

        QLabel#statusLabel {
            font-size: 11px;
            color: #B84332;
        }

        QLineEdit#formInput, QSpinBox#formInput {
            background: #0D0703;
            border: 1px solid #5C1A0D;
            color: #D9C9A8;
            padding: 6px 10px;
            border-radius: 2px;
            font-size: 12px;
        }
        QLineEdit#formInput:focus, QSpinBox#formInput:focus {
            border-color: #C9A84C;
        }

        QPushButton#btnPrimary {
            background: #8B2A1A;
            color: #D9C9A8;
            border: 1px solid #C9A84C;
            font-size: 13px;
            font-weight: bold;
            letter-spacing: 3px;
            border-radius: 2px;
        }
        QPushButton#btnPrimary:hover { background: #B84332; }
        QPushButton#btnPrimary:pressed { background: #5C1A0D; }
        QPushButton#btnPrimary:disabled { background: #2A1208; color: #5C4030; }

        QPushButton#btnSecondary {
            background: transparent;
            color: #C9A84C;
            border: 1px solid #5C3A1A;
            font-size: 11px;
            border-radius: 2px;
            padding: 4px;
        }
        QPushButton#btnSecondary:hover { border-color: #C9A84C; }
    )");
}

// ─── Slots ───────────────────────────────────────────────────────────────────

void LoginWindow::onConnectClicked() {
    QString host = m_editHost->text().trimmed();
    if (host.isEmpty()) {
        showError("Veuillez entrer une adresse hôte");
        return;
    }
    m_lblStatus->setText("");
    emit connectRequested(host, static_cast<quint16>(m_spinPort->value()));
}

void LoginWindow::setConnecting(bool connecting) {
    m_btnConnect->setEnabled(!connecting);
    m_btnConnect->setText(connecting ? "CONNEXION..." : "SE CONNECTER");
    if (connecting)
        m_lblStatus->setStyleSheet("color: #C9A84C;");
    m_lblStatus->setText(connecting ? "Connexion en cours..." : "");
}

void LoginWindow::showError(const QString& msg) {
    m_lblStatus->setStyleSheet("color: #B84332;");
    m_lblStatus->setText(msg);
    m_btnConnect->setEnabled(true);
    m_btnConnect->setText("SE CONNECTER");
}

} // namespace sovlink
