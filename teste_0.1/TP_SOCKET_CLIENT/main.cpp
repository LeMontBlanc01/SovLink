#include <QCoreApplication>
#include "client.h"
#include <iostream>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Client monClient;

    // On utilise un timer pour vérifier régulièrement si l'utilisateur a tapé un message
    // sans figer la réception des messages réseau
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&](){
        // On vérifie si des données sont prêtes sur l'entrée standard (clavier)
        // Note: Cette partie simplifiée fonctionne mieux sous Linux/macOS.
        // Sous Windows, la lecture console bloquante est plus complexe.
        monClient.messageManualInput();
    });
    timer.start(100);

    return a.exec();
}
