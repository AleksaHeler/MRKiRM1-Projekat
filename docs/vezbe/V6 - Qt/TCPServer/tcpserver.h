#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

#define MAX_MESSAGE_LENGTH 256
#define PORT 27000U

class TCPServer : public QObject
{
    Q_OBJECT
public:
     explicit TCPServer(QObject *parent = nullptr);
    ~TCPServer();

    bool startServer();

private slots:
    // Called automatically when a client attempts to connect
    void acceptTheConnection();

    // Called automatically when data sent by a client is fully available to the server
    void respondToConnection();

    // Called automatically when client has closed the connection
    void connectionAboutToClose();

private:

    // Variable (socket) to store listening tcpserver
    QTcpServer tcpServer;

    // Variable (socket) to store newly established connection with the client
    QTcpSocket* connectionSoc;
};

#endif // TCPSERVER_H
