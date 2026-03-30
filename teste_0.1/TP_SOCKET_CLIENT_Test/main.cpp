#include <QCoreApplication>
#include "client_test.h"
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    fprintf(stdout, "=== SovLink Client ===\n");

    // 1. On cree le client → la paire de cles est generee dans le constructeur
    Client monClient;

    // 2. On affiche la cle publique MAINTENANT pour que l'utilisateur puisse la partager
    fprintf(stdout, "\nVotre cle publique (partagez-la avec votre contact) :\n");
    fprintf(stdout, "%s\n", monClient.publicKey().toUtf8().constData());
    fprintf(stdout, "------------------------------------------------------\n");
    fprintf(stdout, "Appuyez sur ENTREE quand l'autre a votre cle...\n");
    fflush(stdout);
    getchar(); // attend que l'utilisateur soit pret

    // 3. Saisie de l'IP et de la cle du destinataire
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

    // 4. On configure et on connecte
    monClient.setTargetKey(targetKey);
    monClient.connectToServer(serverIp);

    QTimer inputTimer;
    QObject::connect(&inputTimer, &QTimer::timeout, [&](){
        monClient.messageManualInput();
    });
    inputTimer.start(500);

    return a.exec();
}
