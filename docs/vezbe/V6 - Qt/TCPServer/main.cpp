#include <QCoreApplication>
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Instantiate server object
    TCPServer server;

    // Start the server
    bool status = server.startServer();

    // Error starting the server
    if(!status)
        exit(EXIT_FAILURE);

    return a.exec();
}
