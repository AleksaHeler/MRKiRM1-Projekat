#include <QCoreApplication>
#include <QDebug>
#include "counter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Make counter object instances
    Counter a, b;

    qDebug() << "--- Initial values ---";
    qDebug() << "a: " << a.counter();
    qDebug() << "b: " << b.counter() << "\n";


    // Let's connect a and b via signal-slot connection.
    // Whenever we change the value of a, we want to change
    // the value of b as well, to the same value.

    // Do this by connecting a and b via signal-slot connection
    // This is done by the QObject::connect() function:

    // bool QObject::connect(const QObject * sender, const char * signal,
    //                      const QObject * receiver, const char * slot,
    //                      Qt::ConnectionType type = Qt::AutoConnection)

    QObject::connect(&a, SIGNAL(valueChanged(int)),
                     &b, SLOT(setCounter(int)));
    // The connection type is optional. We'll leave it at default.
    // For more information, read the docs about this.

    // Change the value of a
    a.setCounter(5);
    qDebug() << "--- Value of a changed ---";
    qDebug() << "a: " << a.counter();
    qDebug() << "b: " << b.counter() << "\n";
    // Both a and b should be changed, since we said above
    // that when a emits a signal that its value is changed,
    // the setter for the b should be called too.


    // Change the value of b
    b.setCounter(12);
    qDebug() << "--- Value of b changed ---";
    qDebug() << "a: " << a.counter();
    qDebug() << "b: " << b.counter() << "\n";
    // Here, only the value of b should be changed, since we
    // didn't tell b to emit any signals when its value is changed.
    // The b will, in fact emit a signal, but it will be ignored since
    // b wasn't specified anywhere as the sender of the signal.

    return app.exec();
}
