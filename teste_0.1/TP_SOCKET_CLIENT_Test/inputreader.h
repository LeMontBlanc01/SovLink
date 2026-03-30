#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <QThread>
#include <QString>

class InputReader : public QThread {
    Q_OBJECT
public:
    explicit InputReader(QObject *parent = nullptr) : QThread(parent) {}

    void run() override {
        char buf[1024];
        while (true) {
            if (fgets(buf, sizeof(buf), stdin)) {
                QString line = QString::fromLocal8Bit(buf).trimmed();
                if (!line.isEmpty())
                    emit lineRead(line);
            }
        }
    }

signals:
    void lineRead(const QString& line);
};

#endif
