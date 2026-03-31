#include <QCoreApplication>
#include <QDebug>
#include "server_test.h" // Assure-toi que le fichier server.h est bien dans le même dossier
#include <QProcess>


int main(int argc, char *argv[])
{

    // QCoreApplication gère la boucle d'événements pour un mode console
    QCoreApplication a(argc, argv);

    qDebug() << "--- Initialisation du Serveur de Relais Chiffre ---";

    // Instanciation de la classe Server définie dans server.h
    Server monServeur;

    // Lance la boucle infinie pour écouter les sockets
    return a.exec();
}
