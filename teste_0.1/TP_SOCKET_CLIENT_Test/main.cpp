#include <QCoreApplication>
#include "client_test.h"
#include "inputreader.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // 1. Création du client → génère les clés
    Client monClient;

    // 2. Affiche la clé publique
    fprintf(stdout, "\n=== SovLink ===\n");
    fprintf(stdout, "Votre cle publique :\n%s\n", monClient.publicKey().toUtf8().constData());
    fprintf(stdout, "---\nAppuyez sur ENTREE quand pret...\n");
    fflush(stdout);
    getchar();

    // 3. IP du serveur
    fprintf(stdout, "IP du serveur : ");
    fflush(stdout);
    char ipBuf[256] = {};
    fgets(ipBuf, sizeof(ipBuf), stdin);
    QString serverIp = QString::fromLocal8Bit(ipBuf).trimmed();

    // 4. Clé du destinataire
    fprintf(stdout, "Cle publique du destinataire : ");
    fflush(stdout);
    char keyBuf[512] = {};
    fgets(keyBuf, sizeof(keyBuf), stdin);
    QString targetKey = QString::fromLocal8Bit(keyBuf).trimmed();

    // 5. Connexion
    monClient.setTargetKey(targetKey);
    monClient.connectToServer(serverIp);

    // 6. Thread dédié pour lire stdin sans bloquer Qt
    InputReader reader;

    QObject::connect(&reader, &InputReader::lineRead, &a, [&](const QString& line) {
        int ttl = -1;
        QString message = line;

        if (line.startsWith("[ttl=", Qt::CaseInsensitive)) {
            int end = line.indexOf(']');
            if (end > 0) {
                ttl     = line.mid(5, end - 5).toInt();
                message = line.mid(end + 1).trimmed();
            }
        }

        if (!message.isEmpty())
            monClient.sendMessage(message, ttl);

    }, Qt::QueuedConnection);
    reader.start();

    return a.exec();
}
