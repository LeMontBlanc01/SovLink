<div align="center">

<img src="media/Logo_sovlink.png" width="800" alt="SovLink"/>

<img src="https://readme-typing-svg.demolab.com?font=Share+Tech+Mono&size=42&duration=3000&pause=1500&color=CC0000&background=00000000&center=true&vCenter=true&width=600&height=60&lines=0+historique;Tout+anonyme;" alt="SovLink"/>


<br/>

![Version](https://img.shields.io/badge/VERSION-0.1--ALPHA-cc0000?style=for-the-badge&logoColor=white)
![Langage](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Chiffrement](https://img.shields.io/badge/LIBSODIUM-CHIFFRÉ-cc0000?style=for-the-badge&logo=letsencrypt&logoColor=white)

<br/>

<div align="center">

```
  Big Brother vous regarde.
  SovLink, non.
```

</div>

---

## ★ C'est quoi SovLink ?

**SovLink** est une messagerie TCP chiffrée, conçue autour d'un principe simple :
**ce qui a été dit ne doit laisser aucune trace.**

Pas de compte à créer. Pas d'email à donner. À chaque connexion, une nouvelle paire de clés cryptographiques est générée — ta clé publique devient ton identité le temps d'une session, puis disparaît. Le serveur transmet, jamais il ne retient.

> *Dans un monde où chaque message est archivé, analysé, monétisé —*
> *SovLink choisit l'oubli comme fonctionnalité.*

---

## ★ Pourquoi ça existe

| ✦ | Fonctionnalitées |
|:---:|:---|
| 🔐 | Chiffrement asymétrique de bout en bout — seul le destinataire lit |
| 👤 | Anonymat structurel : nouvelle identité à chaque session |
| ⚡ | Temps réel, multi-clients, sans latence perceptible |
| 🗑️ | Zéro stockage — les messages n'existent que le temps du transit |
| 🔑 | Aucun compte, aucun mot de passe, aucune traçabilité |

---

## ★ Ce qu'on construit

```
  ╭─────────────────────────────────────────╮
  │         SOVLINK — ROADMAP v0.1          │
  ├─────────────────────────────────────────┤
  │  [x]  Serveur TCP multi-clients         │
  │  [x]  Connexion / identification        │
  │  [ ]  Chiffrement Libsodium E2E         │
  │  [ ]  Génération de clés par session    │
  │  [ ]  Messages éphémères (0 stockage)   │
  │  [ ]  Partage de clé publique externe   │
  │  [ ]  Interface Qt (fenêtre de chat)    │
  │  [ ]  Suppression auto après lecture    │
  ╰─────────────────────────────────────────╯
```

---

## ★ Arborescence

```
/PROJET_SOCKET_CHIFRE

│

├── /TP_SOCKET_SERVER

│   ├── TP_SOCKET_SERVER.pro    # Ton fichier projet serveur 

│   ├── main.cpp                # Instancie le serveur 

│   ├── server.h                # Déclaration de la classe MyServer 

│   ├── server.cpp              # Logique de routage par clé publique 

│   └── protocol.h              # Structure commune des paquets (Shared)

│

├── /TP_SOCKET_CLIENT

│   ├── TP_SOCKET_CLIENT.pro    # Ton fichier projet client 

│   ├── main.cpp                # Boucle d'interaction utilisateur 

│   ├── client.h                # Déclaration de la classe MyClient 

│   ├── client.cpp              # Gestion des clés et du chiffrement 

│   └── protocol.h              # Copie du fichier de structure (doit être identique)

│

└── /libs                       # (Optionnel) Si tu inclus libsodium en local
 
```

---

## ★ Comment ça marche

```
  [Alice]                              [Bob]
     │                                   │
     │── génère clé privée + publique    │── génère clé privée + publique
     │                                   │
     │── partage clé publique ──────────►│
     │                                   │
     │── chiffre avec clé pub de Bob ───►│── déchiffre avec sa clé privée
     │                                   │
     │         [ serveur TCP ]           │
     │    transmet. ne lit pas. oublie.  │
```
---

## ★ Contribuer

Fork, propose, améliore.
Toute contribution qui renforce la **confidentialité** ou la **sobriété du code** est bienvenue.

**Ce projet est sérieux. Les issues aussi. Les trolls beaucoup moins.**

---

<div align="center">

★  ★  ★

*SovLink — Groupe 3*
**Ce qui a été dit ici ne sera plus demain.**

</div>
