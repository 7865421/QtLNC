#include "analysethread.h"

#include <QDebug>
#include <QMutex>

AnalyseThread::AnalyseThread(QObject* par) : QThread(par)
{
    isStop = false;
    is_running=false;
}

AnalyseThread::~AnalyseThread()
{
    isStop = false;
    is_running=false;
}

void AnalyseThread::closeThread()
{
    isStop = true;
    is_running=false;
}

int AnalyseThread::AnalyseTxt(QString line)
{
    //首先，查找是否带有_结尾，如果没有，则进入下一级分析，如果有，则截取错误号和错误内容；
    //第二级，查找是否包含:&,如果有，则截取:&以前的内容，并截取最后一个空格后的内容，如果不包含:&则进入下一级
    //第三级,查找;，并将;后的作为内容
    //QString timePatten(".+(?=\&sep;)");//时间截取
    QString timePatten("\\d+/\\d+/\\d+\\s\\d+:\\d+:\\d+(?=\&sep;)");//时间截取

    QString ParamPatten(":\&");//:&截取
    QString EndPatten(";_\\b");//_结尾
    QString Context;
    QString Time;

    QRegExp rxend(EndPatten);
    int endpos=rxend.indexIn(line);
   // qDebug()<<"endpos="<<endpos;
    if(endpos>-1)//第1级，找到下划线结尾
    {
       // qDebug()<<"endposfind";
        QString ErrPatten("Err=.+;_\\b");
        QRegExp rxErr(ErrPatten);
        int errpos=rxErr.indexIn(line);
        Context=rxErr.cap(0);
        //qDebug()<<Context;
    }
    else//第二级查找是否包含:&
    {
        QRegExp rxParam(ParamPatten);
        int parampos=rxParam.indexIn(line);
            if(parampos>-1)//包含:&
            {
                QString msghPatten("\\s[IOACRFS].+\\d+:");
                QRegExp rxmsgh(msghPatten);
                int msghpos=rxmsgh.indexIn(line);
                Context=rxmsgh.cap(0);
              //  qDebug()<<Context;
                QString msgtPatten("\\s\\S+$");
                QRegExp rxmsgt(msgtPatten);
                int msgtpos=rxmsgt.indexIn(line);
              Context+=rxmsgt.cap(0);
               // qDebug()<<Context;

            }
            else//第3级，非包含:&,直接取&sep;后的数据
            {
               // QString msgPatten("(?=\&sep;).+");
                QString msgPatten("\&sep;");
                QRegExp rxmsg(msgPatten);

               // int msgpos=rxmsg.indexIn(line);
               // Context=rxmsg.cap(0).mid(5);
               // qDebug()<<line<<Context;

                int msgpos=0;
                int lastPos=0;
                while ((msgpos=rxmsg.indexIn(line,msgpos))!=-1){
                   msgpos+=rxmsg.matchedLength();
                   lastPos=msgpos;
                  }
               // qDebug()<<"last"<<msgpos;
                Context=line.mid(lastPos);
            }

    }
     QRegExp rxtime(timePatten);
     int timepos=rxtime.indexIn(line);
     Time=rxtime.cap(0);
    // qDebug()<<Time;
     if(Time!="")
   {
        emit SendTableData(Time,Context);
        analyseLineNum=analyseLineNum+1;
   }

}

int AnalyseThread::AnalyseFile(QString fullname,bool stop)
{

    long long LineNum=0;
    qint64 CharNum=0;
    QFile log(fullname);
    static char LastChar=0;
    char CurrenChar=0;
    QString LineContext;

    analyseLineNum=0;//分析行数清零

    if(fullname.isEmpty())
    {
        //qDebug()<<fullname<<"Empty";
        return -1;
    }
    FILE *logfile=NULL;
    char LineStart=0;
    char OneLineChar[500];
    int lineCharNum=0;
  //  qDebug()<<fullname.toStdString().c_str();

    QTextCodec *code = QTextCodec::codecForName("GB2312");

    const char* name = code->fromUnicode(fullname).data();

 //  qDebug()<<name;
   // logfile =fopen(fullname.toStdString().c_str(),"r");
    logfile =fopen(name,"r");
        while(stop==false)
        {
         CurrenChar= fgetc(logfile);
         if(CurrenChar != LastChar)
         {
             if(LastChar==0)
                {
                 LineStart=1;
                }
             if(CurrenChar==0)
                {
                 LineStart=0;
                 LineContext=QString::fromUtf8(OneLineChar,lineCharNum);
                 //ui->tbrFilesLog->append(LineContext);
                 emit SendLineData(LineContext);
                 AnalyseTxt(LineContext);
                 LineNum++;
                 LineContext.clear();
                 lineCharNum=0;
                }
             LastChar=CurrenChar;
         }
         if(LineStart==1)
         {
            OneLineChar[lineCharNum]=CurrenChar;
            lineCharNum++;
         }
        if((CurrenChar==EOF))
            break;
        else
            CharNum++;

        int percent=(int)(((float)CharNum/(float)log.size()) * 100.0);
        emit SendProgress(percent);
        }
        fclose(logfile);
   // ui->tbrFilesLog->append("分析文件完毕，总计"+QString::number(LineNum)+"行日志信息");
        emit SendLineData(fullname+"文件分析完毕，总计"+QString::number(analyseLineNum)+"行日志信息");
        emit AnalyseDone();
        isStop=true;

}

void AnalyseThread::run()
{
    while (1)
    {
        if(isStop)
            return;
        //qDebug()<<tr("mythread QThread::currentThreadId()==")<<QThread::currentThreadId();
        is_running=true;
        AnalyseFile(FileFullName,isStop);
        is_running=false;

      //  sleep(1);
    }
}
