# SovLink — Messagerie Sécurisée

**Projet BTSCIEL1 — Elric Evrard, Jean Chevrier, Abdel Camara**  
Date : 09/03/2026

---

## Architecture du projet

```
sovlink/
├── CMakeLists.txt
├── common/
│   ├── Crypto.h / Crypto.cpp       ← Chiffrement (libsodium)
│   └── Protocol.h / Protocol.cpp   ← Protocole TCP (sérialisation)
├── server/
│   ├── main_server.cpp             ← Point d'entrée serveur
│   ├── Server.h / Server.cpp       ← Serveur TCP multi-clients
│   └── ClientSession.h / .cpp      ← Session par client
├── client/
│   ├── main_client.cpp             ← Point d'entrée client Qt
│   └── Client.h / Client.cpp       ← Gestion connexion TCP + crypto
└── ui/
    ├── MainWindow.h / .cpp         ← Contrôleur principal
    ├── LoginWindow.h / .cpp        ← Écran de connexion
    └── ChatWindow.h / .cpp         ← Interface de chat
```

---

## Dépendances

| Bibliothèque   | Rôle                                 | Installation (Ubuntu/Debian)              |
|----------------|--------------------------------------|-------------------------------------------|
| **C++17**      | Langage principal                    | `sudo apt install build-essential`        |
| **Boost.Asio** | Serveur TCP asynchrone multi-clients | `sudo apt install libboost-all-dev`       |
| **libsodium**  | Chiffrement clé publique/privée      | `sudo apt install libsodium-dev`          |
| **Qt6**        | Interface graphique cliente          | `sudo apt install qt6-base-dev`           |
| **CMake**      | Système de build                     | `sudo apt install cmake`                  |

---

## Compilation

### 1. Installer les dépendances

```bash
# Ubuntu / Debian
sudo apt update
sudo apt install build-essential cmake libboost-all-dev libsodium-dev qt6-base-dev
```

### 2. Compiler le projet

```bash
cd sovlink/
mkdir build && cd build
cmake ..
make -j$(nproc)
```

Deux exécutables sont générés :
- `sovlink_server` — le serveur TCP
- `sovlink_client` — le client avec interface Qt

---

## Utilisation

### Lancer le serveur

```bash
./sovlink_server [port]
# Exemple :
./sovlink_server 5555
```

Le serveur affiche sa clé publique et attend les connexions.

### Lancer le client

```bash
./sovlink_client
```

L'interface graphique s'ouvre. Sur l'écran de connexion :
1. Votre **clé publique** (identifiant de session) est générée automatiquement
2. Copiez-la pour la partager avec vos contacts
3. Entrez l'adresse du serveur et le port
4. Cliquez **SE CONNECTER**

### Envoyer un message

1. Dans la liste des utilisateurs connectés (panneau gauche), cliquez sur un utilisateur
2. Tapez votre message dans le champ de saisie
3. Appuyez sur **Entrée** ou cliquez **➤**

Le message est **chiffré automatiquement** avec la clé publique du destinataire avant envoi.

---

## Fonctionnement technique

### Chiffrement (libsodium)

- Algorithme : **crypto_box** (Curve25519 + XSalsa20 + Poly1305)
- À chaque connexion, une **nouvelle paire de clés** est générée
- La clé publique sert d'**identifiant temporaire** de session
- Les messages sont chiffrés **de bout en bout** : le serveur ne peut pas les lire
- **Aucun historique** n'est conservé : les messages disparaissent à la déconnexion

### Protocole réseau (TCP / Boost.Asio)

Format d'un paquet :
```
[4 bytes: type] [4 bytes: longueur payload] [payload JSON]
```

Types de paquets :
| Type          | Direction         | Description                        |
|---------------|-------------------|------------------------------------|
| `HANDSHAKE`   | Client → Serveur  | Envoi de la clé publique           |
| `HANDSHAKE_OK`| Serveur → Client  | Confirmation + liste utilisateurs  |
| `USER_LIST`   | Serveur → Client  | Mise à jour liste connectés        |
| `MESSAGE`     | Client → Serveur  | Message chiffré à relayer          |
| `DELIVER`     | Serveur → Client  | Livraison du message chiffré       |
| `DISCONNECT`  | Client → Serveur  | Déconnexion propre                 |
| `ERROR_MSG`   | Serveur → Client  | Erreur (ex: destinataire inconnu)  |

### Diagramme de flux

```
Client A                    Serveur                    Client B
   |                           |                           |
   |── HANDSHAKE (pubkeyA) ──► |                           |
   |◄─ HANDSHAKE_OK ──────────|                           |
   |                           |◄─ HANDSHAKE (pubkeyB) ── |
   |                           |── USER_LIST ────────────► |
   |◄─ USER_LIST ─────────────|                           |
   |                           |                           |
   |── MESSAGE (to=B, chiffré) ► |                        |
   |                           |── DELIVER (from=A) ────► |
   |                           |                           |
   | [B déchiffre avec sa clé privée]                     |
```

---

## Sécurité

- **Pas de compte** : aucune donnée personnelle n'est enregistrée
- **Identifiant temporaire** : la clé publique change à chaque connexion (anonymat)
- **Chiffrement E2E** : seul le destinataire peut déchiffrer le message
- **Sans historique** : aucun message n'est stocké côté serveur
- **Authentification implicite** : la signature Poly1305 garantit l'intégrité

---

## Développé avec

- **C++17**
- **Libsodium** — chiffrement
- **Boost.Asio** — réseau TCP asynchrone
- **Qt6** (Widgets) — interface graphique
- **Qt Creator** — environnement de développement
