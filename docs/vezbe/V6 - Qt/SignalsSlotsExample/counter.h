#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>

class Counter : public QObject
{
    Q_OBJECT

public:
    explicit Counter(QObject *parent = nullptr);

    // m_counter getter
    int counter() const;

// The "public" access modifier only tells the slot how to behave
// when it's used as an ordinary member function. Slots can be called
// from wherever via signal-slot connection, even if declared private.
public slots:

    // The setter can be called as a response to a signal.
    // Note that slots can be used as ordinary member functions.
    void setCounter(int counter);


signals:
    // We want to emit a signal when the value of the counter is changed.
    void valueChanged(int newValue);

private:
    int m_counter;
};

#endif // COUNTER_H
