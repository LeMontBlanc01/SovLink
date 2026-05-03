#ifndef ROUTEURCISCO_H
#define ROUTEURCISCO_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>

class routeurcisco : public QObject
{
    Q_OBJECT
public:
    explicit routeurcisco(QObject *parent = nullptr);

public slots:
    void onNewConnection();
    void lire_message();
    void envoyerMessage(const QString &message);


private:
    QTcpServer *server;
    QMap<QString, QTcpSocket*> tableRoutage;

private slots:
    void auDeconnecter(); // Nouveau slot pour le nettoyage
};

#endif // ROUTEURCISCO_H
