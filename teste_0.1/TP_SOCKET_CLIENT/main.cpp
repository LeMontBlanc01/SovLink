#include <QCoreApplication>
#include "client.h"
#include <iostream>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Client monClient;

    if (argc > 1) {
        monClient.connectToServer(QString::fromLocal8Bit(argv[1]));
    } else {
        // Forcer l'affichage avant la saisie
        fprintf(stdout, "IP du serveur (laisser vide pour decouverte auto) : ");
        fflush(stdout);

        char buf[256] = {};
        fgets(buf, sizeof(buf), stdin);
        QString ip = QString::fromLocal8Bit(buf).trimmed();

        if (!ip.isEmpty()) {
            monClient.connectToServer(ip);
        }
    }

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&](){
        monClient.messageManualInput();
    });
    timer.start(100);

    return a.exec();
}
