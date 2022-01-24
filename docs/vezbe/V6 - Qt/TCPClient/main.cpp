#include <QCoreApplication>
#include "tcpclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // For input from console
    QTextStream cin(stdin);

    // Instantiate client object
    TCPClient client;

    // Attempt server connection
    client.connectToServer();

    // Get the message to be sent
    qDebug() << "Enter your message:";
    QString msg = cin.readLine(MAX_MESSAGE_LENGTH);

    // Send the message to server
    client.sendMessageToServer(msg);

    return a.exec();
}
