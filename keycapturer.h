#ifndef CAPTURER_H
#define CAPTURER_H

#include <QObject>

class KeyCapturer:public QObject
{
    Q_OBJECT
    public:
        virtual ~KeyCapturer();
        static KeyCapturer*& instance()
        {
            static KeyCapturer *s=nullptr;
            if (s==nullptr)
            {
                s=new KeyCapturer();
            }
            return s;
        }

    public:
        void setkeyValue(int key);
        void setMouseValue(int x,int y);

    protected:
        KeyCapturer();

    signals:
        void getKey(int);
        void getPoint(int x,int y);

    private:
};

#endif // CAPTURER_H
