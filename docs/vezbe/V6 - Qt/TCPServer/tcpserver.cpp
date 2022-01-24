#include "tcpserver.h"

TCPServer::TCPServer(QObject *parent) : QObject(parent)
{
    // set : acceptTheConnection() will be called whenever there is a new connection
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptTheConnection()));
}

TCPServer::~TCPServer()
{
    // Stop the server from listening for incoming connections
    tcpServer.close();
}

bool TCPServer::startServer()
{
    // Instruction to start listening for incoming connection.
    bool status = tcpServer.listen(QHostAddress::Any, PORT);

    // Check if the server did start correctly or not
    if (status == true)
    {
        qDebug() << "Server started. Server now listening on port #" + QString::number(PORT);
    }
    else
    {
        qDebug() << "Server Start failure:" + tcpServer.errorString();
    }

    return status;
}

void TCPServer::acceptTheConnection()
{
    // Accept and establish the connection. Note that, the data-transfer happens with `connectionSoc` and not with `tcpServer`
    // `tcpServer` only waits and listens to new connections
    connectionSoc = tcpServer.nextPendingConnection();

    // Notify the user about incoming connection
    qDebug() << "Client from address" + connectionSoc->peerAddress().toString() + " connected";

    // Set : respondToConnection() will be called whenever the data (coming from client) is available
    connect(connectionSoc, SIGNAL(readyRead()), this, SLOT(respondToConnection()));

    // Set : connectionAboutToClose() will be called whenever the connection is close by the client
    connect(connectionSoc, SIGNAL(disconnected()), this, SLOT(connectionAboutToClose()));
}

void TCPServer::respondToConnection()
{
    // Declare a byte array (this is how it is done with Qt)
    QByteArray buf;

    // Read the incoming data. Here MAX_MESSAGE_LENGTH (256) is the maximum length
    buf = connectionSoc->read(MAX_MESSAGE_LENGTH);

    // Convert this data to QString. Note that QString is the class which holds strings in Qt
    QString bufString = QString(buf);

    // Write the message to the console
    qDebug() << "Message from client:";
    qDebug() << bufString;

    // Convert this string to upper case
    bufString = bufString.toUpper();

    // Respond to the client. Note that string.toUtf8() is used to convert string to QByteArray
    connectionSoc->write(bufString.toUtf8());
}

void TCPServer::connectionAboutToClose()
{
    // Write the message about disconnection to the console
    qDebug() << "Client from address" + connectionSoc->peerAddress().toString() + " disconnected";

    // Disconnect the signals
    disconnect(connectionSoc, nullptr, this, nullptr);

    // Close the connection (Say bye)
    connectionSoc->close();
}
