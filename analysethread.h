#ifndef ANALYSETHREAD_H
#define ANALYSETHREAD_H
#include<QtCore>
#include <QThread>

class AnalyseThread : public QThread
{
    Q_OBJECT
signals:
    void SendLineData(const QString& lineData);
    void SendTableData(const QString& time,const QString& msgData);
    void SendProgress(const int& percent);
    void AnalyseDone();
public:
    AnalyseThread(QObject* par);
    ~AnalyseThread();
    void closeThread();
    void run();
    int AnalyseFile(QString fullname,bool stop);
    int AnalyseTxt(QString line);

public:
    QString FileFullName;
     bool isStop;       //isStop是易失性变量，需要用volatile进行申明
     bool isPause;//暂停
     bool is_running=false;
     long long analyseLineNum=0;

};

#endif // ANALYSETHREAD_H
