#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <network.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // On crée l'objet réseau
    instanceDeNetwork = new network(this);

    // BRANCHEMENT 1 : Réception (Network -> Interface)
    connect(instanceDeNetwork, &network::messageRecuPourInterface, this, [this](QString msg) {
        ui->Zone_lecture->append(msg);
    });

    // BRANCHEMENT 2 : Connexion (Bouton -> Action locale)
    connect(ui->Bouton_connexion_serveur, &QPushButton::clicked, this, &MainWindow::sonnerie1);
}
void MainWindow::on_Bouton_send_clicked() {
    QString texte = ui->message->text(); // Vérifie si c'est ui->message ou ui->lineEdit_dest
    QString destinataire = ui->lineEdit_dest->text();

    if(texte.isEmpty() || destinataire.isEmpty()) {
        ui->Zone_lecture->append("<i style='color:red;'>Erreur : message ou destinataire vide.</i>");
        return;
    }

    // On prépare le message
    QJsonObject json;
    json["destinataire"] = destinataire;
    json["message"] = texte;
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);

    // ON PASSE PAR L'INSTANCE DE NETWORK !
    // Il faut que tu crées une fonction "envoyerDonnees" dans ta classe network
    instanceDeNetwork->sonnerie2(QString::fromUtf8(data));

    ui->Zone_lecture->append("<b style='color:blue;'>Moi (vers " + destinataire + ") :</b> " + texte);
    ui->message->clear();
}
void MainWindow::sonnerie1() {
    // SURTOUT PAS DE "new network" ICI !
    // On utilise l'objet qui est déjà branché à l'UI
    bool connexionReussie = instanceDeNetwork->connexionAuServeur("127.0.0.1", 12345);

    if(!connexionReussie) {
        // La connexion a échoué (refusée, serveur éteint, etc.)
        ui->Zone_lecture->append("<i style='color:red;'>Erreur : Impossible de se connecter au serveur. </i>");
    } else {
        // Optionnel : un message si ça a marché
        ui->Zone_lecture->append("<i style='color:green;'>Connexion réussie !</i>");
    }

}

MainWindow:: ~MainWindow()
{
    delete ui;


}
