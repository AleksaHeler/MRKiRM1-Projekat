#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>

#define MAX_MESSAGE_LENGTH 256

#define PORT 27000U

class UDPserver : public QObject
{
    Q_OBJECT

public:
    explicit UDPserver(QObject *parent = nullptr);

public slots:

    void proccessIncData();
    void proccessOutData(QString);

signals:

    void proccessingDone(QString);

private:
    QUdpSocket connectionSoc;
};

#endif // UDPSERVER_H
