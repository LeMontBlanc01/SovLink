<div align="center">

<img src="media/sovlink_spin.gif" width="160" alt="SovLink"/>

<img src="https://readme-typing-svg.demolab.com?font=Share+Tech+Mono&size=38&duration=2800&pause=1200&color=CC0000&background=00000000&center=true&vCenter=true&width=620&height=65&lines=SOVLINK;CHIFFRÉ.+ÉPHÉMÈRE.+ANONYME;TU+PARLES.+ON+N'ÉCOUTE+PAS;0+COMPTE.+0+HISTORIQUE.+0+TRACE" alt="SovLink typing"/>

<br/>

![Version](https://img.shields.io/badge/VERSION-0.1--ALPHA-cc0000?style=for-the-badge&logoColor=white)
![Langage](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Chiffrement](https://img.shields.io/badge/LIBSODIUM-CHIFFRÉ-cc0000?style=for-the-badge&logo=letsencrypt&logoColor=white)

<br/><br/>

```
╔══════════════════════════════════════════════════════╗
║  Vos messages ne regardent que vous.                 ║
║  Pas de serveur qui stocke. Pas d'œil qui surveille. ║
║  Juste deux personnes. Juste des mots. Puis rien.    ║
╚══════════════════════════════════════════════════════╝
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

| ✦ | Objectif |
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
SovLink/
│
├── 🖥️  server.cpp          ← cœur TCP, gestion des connexions
├── 📡  client.cpp          ← interface réseau côté utilisateur
├── 🔐  crypto.cpp          ← chiffrement Libsodium
│
├── 📁 include/
│   ├── sovlink.hpp
│   └── crypto.hpp
│
├── 📁 ui/                  ← interfaces Qt Designer
│   ├── mainwindow.ui
│   └── chat.ui
│
├── 📁 Image/
│   └── sovlink_spin.gif
│
└── 📖 README.md
```

---

## ★ Stack

<div align="center">

![C++](https://img.shields.io/badge/C++17-Langage_principal-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Libsodium](https://img.shields.io/badge/Libsodium-Chiffrement_E2E-cc0000?style=for-the-badge&logo=letsencrypt&logoColor=white)
![Boost Asio](https://img.shields.io/badge/Boost_Asio-Réseau_TCP-FF6600?style=for-the-badge&logoColor=white)
![Qt](https://img.shields.io/badge/Qt_Creator-Interface-41CD52?style=for-the-badge&logo=qt&logoColor=white)

</div>

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

## ★ Lancer le projet

```bash
# pas encore prêt — repassez plus tard ★
```

---

## ★ Contribuer

Fork, propose, améliore.
Toute contribution qui renforce la **confidentialité** ou la **sobriété du code** est bienvenue.

**Ce projet est sérieux. Les issues aussi. Les trolls beaucoup moins.**

---

## ★ Stats

<div align="center">

![Stars](https://img.shields.io/github/stars/ton-pseudo/SovLink?style=for-the-badge&color=cc0000&logo=github)
![Forks](https://img.shields.io/github/forks/ton-pseudo/SovLink?style=for-the-badge&color=cc0000&logo=github)
![Issues](https://img.shields.io/github/issues/ton-pseudo/SovLink?style=for-the-badge&color=cc0000&logo=github)

</div>

---

<div align="center">

```
★  ★  ★
```

*SovLink — Groupe 3*
**Ce qui a été dit ici ne sera plus demain.**

</div>
