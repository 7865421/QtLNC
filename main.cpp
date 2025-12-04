#include "mainwindow.h"
#include "singleapp.h"
#include <QApplication>
#include<QSplashScreen>
#include <QMessageBox>
#include<QScreen>
#include<QFontDatabase>
int main(int argc, char *argv[])
{
 //   QApplication a(argc, argv);

    SingleApp a(argc, argv);

    //int fontId = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() + "/msyh.ttc");
    int fontId = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() + "/simsun.ttf");
    qDebug() << "fontId = "<< fontId;
    QString msyh = QFontDatabase::applicationFontFamilies(fontId).at(0);//+",9,-1,5,50,0,0,0,0,0";
    qDebug() <<"msyh = " <<msyh;
    QFont font(msyh);
    font.setPointSize(9);
    //QApplication::setFont(font);
    a.setFont(font);

    qDebug()<<a.font();

    QPixmap pix(":/images/splash.jpg");
    QSplashScreen splash(pix);
    splash.show();
    //Sleep(1000);
    a.processEvents();

  //  SingleApp a(argc, argv);
    if(a.isRunning())
    {
        return 0;
    }
   // splash.hide();
    MainWindow w;
   // w.setWindowFlags(w.windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint);
    w.setWindowState(Qt::WindowMaximized);
    w.show();
    w.showMaximized();
     splash.hide();
   //w.setWindowIcon(":/images/LNC.ICO");
  // w.setWindowTitle("LNC上位管理程序");
   // w.showMaximized();
   // a.processEvents();
     /*
     QScreen *screen=a.primaryScreen();
     int width=screen->size().width();
     int hight=screen->size().height();
     w.resize(QSize(800,600));
     w.resize(QSize(width,hight));
     w.setWindowState(Qt::WindowMaximized);
     w.show();
     */
    int ret;
    ret=a.exec();
    if(ret==773)
        a.exit(0);
    return ret;
}
