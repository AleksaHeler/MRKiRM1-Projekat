#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QDebug>

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 27000U
#define MAX_MESSAGE_LENGTH 256

class TCPClient : public QObject
{
    Q_OBJECT
public:
    explicit TCPClient(QObject *parent = nullptr);

    // Initiate connection with server
    void connectToServer();

    // Send a message to server
    void sendMessageToServer(QString msg);

    // End connection
    void endConnection();

private slots:
    // This function is called when the connection has been established successfully with the server stated.
    void connectionInit();

    // This function is called if there is an error (or disruption) in the connection
    void handleError(QAbstractSocket::SocketError err);

    // This function is called when the data is fully arrived from the server to the client
    void dataArrived();

private:
    // Variable (socket) to hold the client connection data
    QTcpSocket tcpSocket;
};

#endif // TCPCLIENT_H
