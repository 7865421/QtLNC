#include "saveexcelthead.h"
#include <QDebug>
//#include<QDateTime>
saveExcelThead::saveExcelThead(QObject* par) : QThread(par)
{
is_running=false;
}

saveExcelThead::~saveExcelThead()
{
is_running=false;
}

void saveExcelThead::setModel(void *model)
{
    if(type==0)
    {
        sqlmodel =(QSqlQueryModel*) model;
        stdmodel=NULL;
    }
    if(type==1)
    {
        sqlmodel=NULL;
        stdmodel = (QStandardItemModel*) model;
    }
}

void saveExcelThead::saveExcelSqlmodel()
{
    int rowCount=0,columnCount=0;
  //  QString xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), saveFileName+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
   // qDebug()<<xlsFile;
    if(xlsFile.isEmpty())
        return;
    if(m_xls.isNull())
        m_xls.reset(new ExcelBase);
        m_xls->create(xlsFile);
        QList< QList<QVariant> > m_datas;

        rowCount = sqlmodel->rowCount();
        columnCount = sqlmodel->columnCount();

        //qDebug()<<rowCount<<columnCount;

        QList<QVariant> hd;
          for(int i = 0; i < columnCount; ++i)
         {
              QString hdd=sqlmodel->headerData(i,Qt::Horizontal).toString();
            //  qDebug()<<hdd;
               hd.append(hdd);
         }
        m_datas.append(hd);
        int prgPercent=0;
         for(int i = 0; i < rowCount; ++i){
             QList<QVariant> rows;
             for(int j = 0; j < columnCount; ++j){
              QString cell_dat= sqlmodel->data(sqlmodel->index(i,j)).toString();
              rows.append(cell_dat);
             // qDebug()<<cell_dat;
             }
              m_datas.append(rows);
            //  qDebug()<<rows;
            //  int prgPercent=0;
              if(rowCount>2)
              {
                prgPercent=(int)(((float)i/(float)(rowCount-1))*100.0);
              }
              else
                prgPercent=100;
              emit SendProgress(prgPercent);

              }
              prgPercent=100;
              emit SendProgress(prgPercent);

    //m_xls->setCurrentSheet(1);
    //QString sheetname=sheetTabName;
    m_xls->setCurrentSheetName(1,sheetTabName);
    m_xls->writeCurrentSheet(m_datas);
    m_xls->Autofit();
    m_xls->save();
    m_xls->close();
    emit SaveDone(xlsFile);
}


void saveExcelThead::saveExcelStdmodel()
{
    int rowCount=0,columnCount=0;
//    QString xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), saveFileName+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
   // qDebug()<<xlsFile;
    if(xlsFile.isEmpty())
        return;
    if(m_xls.isNull())
        m_xls.reset(new ExcelBase);
        m_xls->create(xlsFile);
        QList< QList<QVariant> > m_datas;

        rowCount = stdmodel->rowCount();
        columnCount = stdmodel->columnCount();

        // qDebug()<<rowCount<<columnCount;
        QList<QVariant> hd;
          for(int i = 0; i < columnCount; ++i)
         {
              QString hdd=stdmodel->headerData(i,Qt::Horizontal).toString();
            //  qDebug()<<hdd;
               hd.append(hdd);
         }
        m_datas.append(hd);
        int prgPercent=0;
         for(int i = 0; i < rowCount; ++i){
             QList<QVariant> rows;
             for(int j = 0; j < columnCount; ++j){
              QString cell_dat= stdmodel->data(stdmodel->index(i,j)).toString();
              rows.append(cell_dat);
             // qDebug()<<cell_dat;
             }
              m_datas.append(rows);
            //  qDebug()<<rows;
             // int prgPercent=0;
              if(rowCount>2)
              {
                prgPercent=(int)(((float)i/(float)(rowCount-1))*100.0);
              }
              else
                prgPercent=100;
              emit SendProgress(prgPercent);
              }
         prgPercent=100;
         emit SendProgress(prgPercent);

    //m_xls->setCurrentSheet(1);
    //QString sheetname=sheetTabName;
    m_xls->setCurrentSheetName(1,sheetTabName);
    m_xls->writeCurrentSheet(m_datas);
    m_xls->Autofit();
    m_xls->save();
    m_xls->close();
    emit SaveDone(xlsFile);
}

void saveExcelThead::run()
{
    if(type==0)
      {
        is_running=true;
        saveExcelSqlmodel();
      }
    else
     {
        is_running=true;
        saveExcelStdmodel();
      }

        is_running=false;
}
