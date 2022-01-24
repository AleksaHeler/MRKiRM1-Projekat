#include <QCoreApplication>
#include "udpclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // For input from console
    QTextStream cin(stdin);

    // Get the message to be sent
    qDebug() << "Enter your message:";
    QString msg = cin.readLine(MAX_MESSAGE_LENGTH);

    UDPclient client;
    client.processOutData(msg);

    return a.exec();
}
