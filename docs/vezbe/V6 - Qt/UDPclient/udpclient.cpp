#include "udpclient.h"

UDPclient::UDPclient(QObject *parent) : QObject(parent)
{
    //Binding the socket
    connectionSoc.bind(QHostAddress(QHostAddress::AnyIPv4), PORT+1);

    //whenever a new datagram arrives, processIncData is called
    connect(&connectionSoc, SIGNAL(readyRead()), this, SLOT(proccessIncData()));
}

void UDPclient::processOutData(QString msg)//this function is called manualy from main()
{
    qDebug() << "Sending data...";
    //Setting the out address for the datagram
    QHostAddress address = QHostAddress("127.0.0.1");
    //Sending the datagram
    connectionSoc.writeDatagram(msg.toUtf8(), address, PORT);
}

void UDPclient::proccessIncData()
{
    //Data array for incoming datagrams
    QByteArray buf(512, 0);
    //Reading incoming datagrams and placing them into the array
    connectionSoc.readDatagram(buf.data(), buf.size());
    //Converting binary data into a string
    QString bufString = QString(buf);

    qDebug() << "Message from server:";
    qDebug() << bufString;

    connectionSoc.close();
}

