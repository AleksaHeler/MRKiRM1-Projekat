#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>

#define PORT 27000U
#define MAX_MESSAGE_LENGTH 275

class UDPclient : public QObject
{
    Q_OBJECT

public:
    explicit UDPclient(QObject *parent = nullptr);

public:
    void processOutData(QString);

public slots:

    void proccessIncData();

private:
    QUdpSocket connectionSoc;
};

#endif // UDPCLIENT_H
