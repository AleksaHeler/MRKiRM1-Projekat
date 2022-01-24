#include "counter.h"

Counter::Counter(QObject *parent) : QObject(parent)
{
    m_counter = 0;
}

int Counter::counter() const
{
    return m_counter;
}

void Counter::setCounter(int counter)
{
    // To prevent entering an infinite loop when the slot is called
    // after the signal is emmitted
    if (m_counter != counter)
    {
        // Set the new value of the counter
        m_counter = counter;

        // This signal will always be emmitted.
        // If emmitted from an object that isn't bound to a slot,
        // the signal will be ignored.
        emit valueChanged(counter);
    }
}
