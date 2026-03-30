#include <QCoreApplication>
#include "client_test.h"
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // On demande tout AVANT de démarrer Qt
    fprintf(stdout, "=== SovLink Client ===\n");

    fprintf(stdout, "IP du serveur : ");
    fflush(stdout);
    char ipBuf[256] = {};
    fgets(ipBuf, sizeof(ipBuf), stdin);
    QString serverIp = QString::fromLocal8Bit(ipBuf).trimmed();
    fprintf(stdout, "Cle publique du destinataire : ");
    fflush(stdout);
    char keyBuf[512] = {};
    fgets(keyBuf, sizeof(keyBuf), stdin);
    QString targetKey = QString::fromLocal8Bit(keyBuf).trimmed();

    // On crée le client avec la clé cible déjà connue
    Client monClient;
    monClient.setTargetKey(targetKey);
    monClient.connectToServer(serverIp);
    QTimer inputTimer;
    QObject::connect(&inputTimer, &QTimer::timeout, [&](){
        monClient.messageManualInput();
    });
    inputTimer.start(500);

    return a.exec();
}
