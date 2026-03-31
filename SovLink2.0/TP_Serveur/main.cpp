#include <iostream>
#include <boost/asio.hpp>
#include "Server.h"
#include "Crypto.h"

int main(int argc, char* argv[]) {
    uint16_t port = 5555;
    if (argc >= 2) {
        try { port = static_cast<uint16_t>(std::stoi(argv[1])); }
        catch (...) { std::cerr << "Port invalide, utilisation du port par défaut 5555\n"; }
    }

    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║    SovLink Serveur TCP  v1.0         ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "Port : " << port << "\n\n";

    // Initialiser libsodium
    if (!sovlink::Crypto::init()) {
        std::cerr << "[ERREUR] Impossible d'initialiser libsodium !\n";
        return 1;
    }

    try {
        boost::asio::io_context io;

        // Gestion du signal SIGINT (Ctrl+C)
        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            std::cout << "\n[Server] Arrêt...\n";
            io.stop();
        });

        sovlink::Server server(io, port);
        server.start();

        // Lancer l'io_context dans plusieurs threads pour gérer plusieurs clients
        unsigned int nThreads = std::max(2u, std::thread::hardware_concurrency());
        std::cout << "[Server] Threads I/O : " << nThreads << "\n";

        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < nThreads - 1; ++i)
            threads.emplace_back([&io]() { io.run(); });

        io.run();
        for (auto& t : threads) t.join();

    } catch (const std::exception& e) {
        std::cerr << "[ERREUR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
