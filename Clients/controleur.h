#ifndef CONTROLEUR_H
#define CONTROLEUR_H

#include <QObject>

class controleur : public QObject
{
    Q_OBJECT
public:
    explicit controleur(QObject *parent = nullptr);

signals:
};

#endif // CONTROLEUR_H
