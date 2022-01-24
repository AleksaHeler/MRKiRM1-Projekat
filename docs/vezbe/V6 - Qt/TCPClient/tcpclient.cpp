#include "tcpclient.h"

TCPClient::TCPClient(QObject *parent) : QObject(parent)
{
    // connectionInit() to be called whenever the connection is estabished
    connect(&tcpSocket,SIGNAL(connected()), this, SLOT(connectionInit()));

    // handleError( ); is called if an error occurs when connecting to the server
    connect(&tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
}

void TCPClient::connectToServer()
{
    // Attempt to connect to server
    tcpSocket.connectToHost(SERVER_IP_ADDRESS, SERVER_PORT, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

    // Notify user
    qDebug() << "Connected to server";
}

void TCPClient::sendMessageToServer(QString msg)
{
    // Check if the connection is open. ie, if we are connected to a server
    if (!tcpSocket.isOpen())
    {
        qCritical() << "Not connected to a server.";
        return;
    }

    // write() returns number of bytes written to device, or -1 if an error occured
    qint64 written = tcpSocket.write(msg.toUtf8());
    if(written == -1)
    {
        qCritical() << "Error sending data to server.";
        return;
    }
}

void TCPClient::endConnection()
{
    // Close the socket.
    tcpSocket.close();
}

void TCPClient::connectionInit()
{
    qDebug() << "Successfully connected";

    // dataArrived(); is called when the data has arrived from the server
    connect(&tcpSocket,SIGNAL(readyRead()),this, SLOT(dataArrived()));

}

void TCPClient::handleError(QAbstractSocket::SocketError err)
{
    // Pop-up to notify an error. tcpSocket.errorString() automatically gets an error message (in english).
    qCritical() << "Error connecting to server: " + tcpSocket.errorString();

    // Formally close the connection
    tcpSocket.close();
}

void TCPClient::dataArrived()
{
    // Read the data that we receive
    // Maximum read MAX_MESSAGE_LENGTH (256) bytes
    QByteArray buf = tcpSocket.read(MAX_MESSAGE_LENGTH);

    // Print the server response to the console
    qDebug() << "Server response: " << QString(buf);

}
