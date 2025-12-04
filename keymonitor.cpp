#include "keycapturer.h"
#include "keymonitor.h"
#include <QDebug>

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

HMODULE WINAPI ModuleFromAddress(PVOID pv);
static HHOOK hHook;

static HHOOK mouseHook = nullptr;


LRESULT KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;
    if (HC_ACTION == nCode)
    {
        if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)  //如果按键为按下状态
        {
           // qDebug()<<Key_Info->vkCode;
            if ((Key_Info->vkCode <= Qt::Key_9 && Key_Info->vkCode >= Qt::Key_0) || (Key_Info->vkCode == 0x0d))
            {
               // qDebug()<<Key_Info->vkCode;
                if (KeyCapturer::instance())
                {
                    KeyCapturer::instance()->setkeyValue(Key_Info->vkCode);
                }
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


LRESULT MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT *mhookstruct = (MSLLHOOKSTRUCT*)lParam; //鼠标HOOK结构体
       POINT pt = mhookstruct->pt;         //将当前鼠标坐标点的x，y坐标作为参数向主程序窗口发送消息
       // 中间滚轮上下滚动
       if (WM_MOUSEWHEEL == wParam || WM_MBUTTONDOWN == wParam || WM_MOUSEMOVE == wParam || WM_LBUTTONDOWN == wParam)
       {
          // qDebug()<<"mouse event"<<pt.x<<pt.y;
           if (KeyCapturer::instance())
           {
               KeyCapturer::instance()->setMouseValue(pt.x,pt.y);
           }
       }


       return CallNextHookEx(mouseHook, nCode, wParam, lParam);     //继续原有的事件队列
}


HMODULE ModuleFromAddress(PVOID pv)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
        return (HMODULE)mbi.AllocationBase;
    } else {
        return NULL;
    }
}

int startHook()
{
    hHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHookProc, ModuleFromAddress((PVOID)KeyboardHookProc), 0);

    mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, ModuleFromAddress((PVOID)MouseHookProc), 0);
    int error = GetLastError();
    return error;
}

bool stopHook()
{

    if(mouseHook != nullptr)// 原作者说是担心一次释放不稳定，才释放二次的。
        {
            UnhookWindowsHookEx(mouseHook);//键盘钩子句不为空时销毁掉
            mouseHook = nullptr;
        }
       if(mouseHook != nullptr)
        {
            UnhookWindowsHookEx(mouseHook);//键盘钩子句不为空时销毁掉
            mouseHook = nullptr;
        }


       UnhookWindowsHookEx(hHook);
       UnhookWindowsHookEx(mouseHook);

      return true;
}
