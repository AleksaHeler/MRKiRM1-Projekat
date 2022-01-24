#include <QCoreApplication>

#include "udpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UDPserver server;

    return a.exec();
}
