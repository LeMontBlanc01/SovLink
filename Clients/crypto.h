#ifndef CRYPTO_H
#define CRYPTO_H

#include <QObject>

class crypto : public QObject
{
    Q_OBJECT
public:
    explicit crypto(QObject *parent = nullptr);

signals:
};

#endif // CRYPTO_H
