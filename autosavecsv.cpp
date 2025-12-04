#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<QStandardPaths>
#include<QSqlQuery>
#include <QSqlQueryModel>
#include <QCryptographicHash>
#include <QThread>
#include <QFileDialog>
#include <QStringListModel>
#include<QMessageBox>


void MainWindow::AutoSaveCSV()
{
    static int secCnt=0;

    secCnt++;
    if(secCnt==csvSaveInterval)
    {
        if(ui->chbAutoLogs->checkState()==Qt::Checked)
        AutoExport_Formal_CSV();

        if(ui->chbFtpAutoConnect->checkState()==Qt::Checked){
              if(!isAlreadyConnected)
              {
                  connectToFTP();
              }
        }

        QString srcfile=m_IniFile->value("mesNGFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
        QString despath=m_IniFile->value("mesFtpNGHostDir").toString();
        FtpUploadFileToFixPath(srcfile,despath);
        srcfile=m_IniFile->value("mesCellInDirFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
        despath=m_IniFile->value("mesFtpCellInHostDir").toString();
        FtpUploadFileToFixPath(srcfile,despath);
        srcfile=m_IniFile->value("mesCellOutDirFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
        despath=m_IniFile->value("mesFtpCellOutHostDir").toString();
        FtpUploadFileToFixPath(srcfile,despath);
        srcfile=m_IniFile->value("mesWorkDoneFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
        despath=m_IniFile->value("mesFtpWorkDoneHostDir").toString();
        FtpUploadFileToFixPath(srcfile,despath);
    }
    if(secCnt==(csvSaveInterval*2))
    {
        if(ui->chbAutoLogs->checkState()==Qt::Checked)
        AutoExport_FormalVerify_CSV();
        if(ui->chbFtpAutoConnect->checkState()==Qt::Checked){
              if(!isAlreadyConnected)
              {
                  connectToFTP();
              }
        }
        //上传上一个生成的文件
              QString srcfile=m_IniFile->value("mesFormalFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
              QString despath=m_IniFile->value("mesFtpFormalHostDir").toString();
              FtpUploadFileToFixPath(srcfile,despath);
    }
    if(secCnt==(csvSaveInterval*3))
    {
        if(ui->chbAutoLogs->checkState()==Qt::Checked)
        AutoExport_EquVerify_CSV();
        if(ui->chbFtpAutoConnect->checkState()==Qt::Checked){
              if(!isAlreadyConnected)
              {
                  connectToFTP();
              }
        }
              QString srcfile=m_IniFile->value("mesFormalVeryFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
              QString despath=m_IniFile->value("mesFtpFormalVerifyHostDir").toString();
              FtpUploadFileToFixPath(srcfile,despath);

    }
    if(secCnt==(csvSaveInterval*4))
    {
        if(ui->chbAutoLogs->checkState()==Qt::Checked)
        AutoExport_BillInfo_CSV();
        if(ui->chbFtpAutoConnect->checkState()==Qt::Checked){
              if(!isAlreadyConnected)
              {
                  connectToFTP();
              }
        }
              QString srcfile=m_IniFile->value("mesEquVerifyFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
              QString despath=m_IniFile->value("mesFtpEquVerifyHostDir").toString();
              FtpUploadFileToFixPath(srcfile,despath);

    }
    if(secCnt==(csvSaveInterval*5))
    {
        if(ui->chbAutoLogs->checkState()==Qt::Checked)
        AutoExport_BillList_CSV();

        if(ui->chbFtpAutoConnect->checkState()==Qt::Checked){
              if(!isAlreadyConnected)
              {
                  connectToFTP();
              }
        }
              QString srcfile=m_IniFile->value("mesBillInfoFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
              QString despath=m_IniFile->value("mesFtpBillInfoHostDir").toString();
              FtpUploadFileToFixPath(srcfile,despath);

    }
    if(secCnt>=(csvSaveInterval*6))
    {
        secCnt=0;
        if(ui->chbAutoLogs->checkState()==Qt::Checked)
        {
        AutoExport_NG_CSV();
        AutoExport_CellIn_CSV();
        AutoExport_CellOut_CSV();
        AutoExport_Finish_CSV();
        }
        if(ui->chbFtpAutoConnect->checkState()==Qt::Checked){
              if(!isAlreadyConnected)
              {
                  connectToFTP();
              }
        }
              QString srcfile=m_IniFile->value("mesBillListFtpDir").toString()+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
              QString despath=m_IniFile->value("mesFtpBillListHostDir").toString();
              FtpUploadFileToFixPath(srcfile,despath);



    }

}


void MainWindow::AutoExport_Formal_CSV()
{
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

   // qDebug()<<"starttime="<<starttime;
    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type=1 AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");

    othersmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("请求时间"));
    othersmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("耗时ms"));
    othersmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("响应时间"));
    othersmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("工序编码"));
    othersmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("设备编码"));
    othersmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("返回代码"));
    othersmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回信息"));
    othersmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("请求json"));
    othersmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvothers->setModel(othersmodel);
    ui->tbvothers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvothers->setSelectionBehavior(QAbstractItemView::SelectRows);

    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesFormalFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveThread->start();
       }
     }
    //saveThread->deleteLater();
}
void MainWindow::AutoExport_FormalVerify_CSV()
{
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

   // qDebug()<<"starttime="<<starttime;
    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type=2 AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");

    othersmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("请求时间"));
    othersmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("耗时ms"));
    othersmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("响应时间"));
    othersmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("工序编码"));
    othersmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("设备编码"));
    othersmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("返回代码"));
    othersmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回信息"));
    othersmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("请求json"));
    othersmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvothers->setModel(othersmodel);
    ui->tbvothers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvothers->setSelectionBehavior(QAbstractItemView::SelectRows);

    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesFormalVeryFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveThread->start();
       }
     }
    //saveThread->deleteLater();
}
void MainWindow::AutoExport_EquVerify_CSV()
{
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

   // qDebug()<<"starttime="<<starttime;
    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type=3 AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");

    othersmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("请求时间"));
    othersmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("耗时ms"));
    othersmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("响应时间"));
    othersmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("工序编码"));
    othersmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("设备编码"));
    othersmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("返回代码"));
    othersmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回信息"));
    othersmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("请求json"));
    othersmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvothers->setModel(othersmodel);
    ui->tbvothers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvothers->setSelectionBehavior(QAbstractItemView::SelectRows);

    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesEquVerifyFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveThread->start();
       }
     }
   // saveThread->deleteLater();
}
void MainWindow::AutoExport_BillInfo_CSV()
{
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

   // qDebug()<<"starttime="<<starttime;
    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type=4 AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");

    othersmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("请求时间"));
    othersmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("耗时ms"));
    othersmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("响应时间"));
    othersmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("工序编码"));
    othersmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("设备编码"));
    othersmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("返回代码"));
    othersmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回信息"));
    othersmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("请求json"));
    othersmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvothers->setModel(othersmodel);
    ui->tbvothers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvothers->setSelectionBehavior(QAbstractItemView::SelectRows);

    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesBillInfoFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveThread->start();
       }
     }
   // saveThread->deleteLater();
}
void MainWindow::AutoExport_BillList_CSV()
{
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

   // qDebug()<<"starttime="<<starttime;
    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type=5 AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");

    othersmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("请求时间"));
    othersmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("耗时ms"));
    othersmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("响应时间"));
    othersmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("工序编码"));
    othersmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("设备编码"));
    othersmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("返回代码"));
    othersmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回信息"));
    othersmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("请求json"));
    othersmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvothers->setModel(othersmodel);
    ui->tbvothers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvothers->setSelectionBehavior(QAbstractItemView::SelectRows);

    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesBillListFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveThread->start();
       }
     }
   // saveThread->deleteLater();
}
void MainWindow::AutoExport_CellIn_CSV()
{
    /*
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
    cellinmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,billno,billnum,special,postjson,respjson from cellinlogs where retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    cellinmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("电芯条码"));
    cellinmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("请求时间"));
    cellinmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("耗时ms"));
    cellinmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("响应时间"));
    cellinmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("工序编码"));
    cellinmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("设备编码"));
    cellinmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回代码"));
    cellinmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("返回信息"));
    cellinmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("工单号"));
    cellinmodel->setHeaderData(9,Qt::Horizontal,QObject::tr("工单数量"));
    cellinmodel->setHeaderData(10,Qt::Horizontal,QObject::tr("特殊值"));
    cellinmodel->setHeaderData(11,Qt::Horizontal,QObject::tr("请求json"));
    cellinmodel->setHeaderData(12,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvcellin->setModel(cellinmodel);
    ui->tbvcellin->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvcellin->setSelectionBehavior(QAbstractItemView::SelectRows);
    */
    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesCellInDirFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvcellin->model());
        saveThread->start();
       }
     }
   // saveThread->deleteLater();

}
void MainWindow::AutoExport_CellOut_CSV()
{
  /*
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
    celloutmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,billno,opreater,steps,materialdec,productnum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,retcode,retmsg,materialremain,ngcodeone,ngdetailone,ngcodetwo,ngdetailtwo,postjson,respjson from celloutlogs where retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    celloutmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("电芯条码"));
    celloutmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("请求时间"));
    celloutmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("耗时ms"));
    celloutmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("响应时间"));
    celloutmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("工序编码"));
    celloutmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("设备编码"));
    celloutmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("工单号"));
    celloutmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("操作员"));
    celloutmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("工步(可选)"));
    celloutmodel->setHeaderData(9,Qt::Horizontal,QObject::tr("是否扣料(可选)"));
    celloutmodel->setHeaderData(10,Qt::Horizontal,QObject::tr("数量"));
    celloutmodel->setHeaderData(11,Qt::Horizontal,QObject::tr("铣钉深度"));
    celloutmodel->setHeaderData(12,Qt::Horizontal,QObject::tr("铣钉半径"));
    celloutmodel->setHeaderData(13,Qt::Horizontal,QObject::tr("主轴转速"));
    celloutmodel->setHeaderData(14,Qt::Horizontal,QObject::tr("吸尘风速"));
    celloutmodel->setHeaderData(15,Qt::Horizontal,QObject::tr("刀具使用次数"));
    celloutmodel->setHeaderData(16,Qt::Horizontal,QObject::tr("表面平整度"));
    celloutmodel->setHeaderData(17,Qt::Horizontal,QObject::tr("返回代码"));
    celloutmodel->setHeaderData(18,Qt::Horizontal,QObject::tr("返回信息"));
    celloutmodel->setHeaderData(19,Qt::Horizontal,QObject::tr("余料信息(可选)"));
    celloutmodel->setHeaderData(20,Qt::Horizontal,QObject::tr("不良代码1"));
    celloutmodel->setHeaderData(21,Qt::Horizontal,QObject::tr("不良原因1"));
    celloutmodel->setHeaderData(22,Qt::Horizontal,QObject::tr("不良代码1"));
    celloutmodel->setHeaderData(23,Qt::Horizontal,QObject::tr("不良原因1"));
    celloutmodel->setHeaderData(24,Qt::Horizontal,QObject::tr("请求json"));
    celloutmodel->setHeaderData(25,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvcellout->setModel(celloutmodel);
    ui->tbvcellout->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvcellout->setSelectionBehavior(QAbstractItemView::SelectRows);
*/
    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesCellOutDirFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvcellout->model());
        saveThread->start();
       }
     }
  //  saveThread->deleteLater();

}
void MainWindow::AutoExport_NG_CSV()
{
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

   // qDebug()<<"starttime="<<starttime;
    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type=6 AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");

    othersmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("请求时间"));
    othersmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("耗时ms"));
    othersmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("响应时间"));
    othersmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("工序编码"));
    othersmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("设备编码"));
    othersmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("返回代码"));
    othersmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("返回信息"));
    othersmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("请求json"));
    othersmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("返回json"));

    ui->tbvothers->setModel(othersmodel);
    ui->tbvothers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvothers->setSelectionBehavior(QAbstractItemView::SelectRows);

    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesNGFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveThread->start();
       }
     }
   // saveThread->deleteLater();
}
void MainWindow::AutoExport_Finish_CSV()
{
    saveAsCsv *saveThread = new saveAsCsv(this);

    if(saveThread->isRunning()==false)
     {
        saveThread->saveFileName =m_IniFile->value("mesWorkDoneFtpDir").toString()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".csv";
       if(saveThread->saveFileName.isEmpty()==false)
       {
        saveThread->setModel((QSqlQueryModel*)ui->tbvFinishLogs->model());
        saveThread->start();
       }
     }
   // saveThread->deleteLater();
}
