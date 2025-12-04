#ifndef MYAPPNATIVEEVENT_H
#define MYAPPNATIVEEVENT_H
#include<QAbstractNativeEventFilter>
#include <QApplication>
class MyAppNativeEvent:public QAbstractNativeEventFilter
{
public:
    MyAppNativeEvent();
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result)override;

};

#endif // MYAPPNATIVEEVENT_H
