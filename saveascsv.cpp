#include "saveascsv.h"
#include<QFile>
#include<QTextStream>
saveAsCsv::saveAsCsv(QObject* par) : QThread(par)
{
is_running=false;
}
saveAsCsv::~saveAsCsv()
{
is_running=false;
}

void saveAsCsv::setModel(void *model)
{
        sqlmodel =(QSqlQueryModel*) model;
}

void saveAsCsv::ReadDataFromSqlWriteToCSV()//将表导出为csv格式表格
{
    if (saveFileName.isEmpty())
     return;

     QFile file(saveFileName);

     if (file.open(QIODevice::WriteOnly))
     {
     QTextStream stream(&file);
     int cc=sqlmodel->columnCount();

     QStringList list;
     for (int i=0;i<cc;i++)
     {
     list<< sqlmodel->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
     }
     stream<< list.join(",")<<"\r\n";

     for (int i=0;i<sqlmodel->rowCount();i++)
     {
     list.clear();
     for (int j=0;j<cc;j++)
     {
     QString csvdata;
     csvdata=sqlmodel->index(i,j).data().toString();
     QString search1=",";
     QString replace1="，";
     QString search2="\n";
     QString replace2=" ";
    // QString search3="-";
    // QString replace3="/";
     csvdata.replace(QRegExp(search1),replace1);
     csvdata.replace(QRegExp(search2),replace2);
    // csvdata.replace(QRegExp(search3),replace3);
     list<<csvdata;
     }

     stream<< list.join(",")<<"\r\n";
     }
     file.close();
     }
     emit SaveDone(saveFileName);
}

void saveAsCsv::run()
{
    is_running=true;
    ReadDataFromSqlWriteToCSV();
    is_running=false;
}
