#include <QApplication>
#include <QFont>
#include "MainWindow.h"
#include "Crypto.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialiser libsodium
    if (!sovlink::Crypto::init()) {
        qCritical("Impossible d'initialiser libsodium !");
        return 1;
    }

    // Police par défaut
    QFont font("Rajdhani", 11);
    app.setFont(font);

    // Métadonnées
    app.setApplicationName("SovLink");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("BTSCIEL1");

    sovlink::MainWindow window;
    window.show();

    return app.exec();
}
