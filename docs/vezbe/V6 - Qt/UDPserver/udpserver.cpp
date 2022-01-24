#include "udpserver.h"

UDPserver::UDPserver(QObject *parent) : QObject(parent)
{
    //example of how to check if a socket is succesfully bound
    if(connectionSoc.bind(QHostAddress(QHostAddress::AnyIPv4), PORT))
    {
        qDebug() << "Socket bound.";
    }

    //whenever a new datagram arrives, processIncData is called
    connect(&connectionSoc, SIGNAL(readyRead()), this, SLOT(proccessIncData()));

    //when processingDone is emited, processOutData is called
    connect(this, SIGNAL(proccessingDone(QString)), this, SLOT(proccessOutData(QString)));
}

void UDPserver::proccessIncData()
{
    //Data array for incoming datagrams
    QByteArray buf(512, 0);

    //Reading incoming datagrams and placing them into the array
    connectionSoc.readDatagram(buf.data(), buf.size());

    //Converting binary data into a string
    QString bufString = QString(buf);

    qDebug() << "Message from client:";
    qDebug() << bufString;

    //Changing the entire string into uppercase
    bufString = bufString.toUpper();

    //calling processOutData and sending it bufString variable
    emit proccessingDone(bufString);
}

void UDPserver::proccessOutData(QString bufString)
{
    qDebug() << "Sending message...";
    qDebug() << bufString;
    //Setting the out address for the datagram
    QHostAddress address = QHostAddress("127.0.0.1");
    //Sending the datagram
    connectionSoc.writeDatagram(bufString.toUtf8(), address, PORT+1);
}
