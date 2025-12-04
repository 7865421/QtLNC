#include "myappnativeevent.h"
#include <windows.h>
#include<QDebug>
MyAppNativeEvent::MyAppNativeEvent()
{

}

bool MyAppNativeEvent::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if ("windows_dispatcher_MSG" == eventType
            || "windows_generic_MSG" == eventType)
        {
            MSG * msg = reinterpret_cast<MSG *>(message);
            if (msg->message == WM_NCMOUSEMOVE)
                qDebug()<<"全局win鼠标移动事件";
            if(msg->message == WM_DEVICECHANGE)
            {
                if( WM_KEYUP)  qDebug()<<"全局键盘按键按下事件";
                if( WM_KEYDOWN) qDebug()<<"全局键盘按键弹起事件";
            }
            //qDebug()<<"全局win事件";
        }
        return false;
}
