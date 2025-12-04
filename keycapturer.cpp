#include "keycapturer.h"

KeyCapturer::KeyCapturer()
{

}

KeyCapturer::~KeyCapturer()
{

}


void KeyCapturer::setkeyValue(int key)
{
    emit getKey(key);
}
void KeyCapturer::setMouseValue(int x, int y)
{
    emit getPoint(x,y);
}
