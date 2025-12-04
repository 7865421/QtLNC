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

int MainWindow::analysefile(QString filename)
{
    long long LineNum=0;
    static char LastChar=0;
    char CurrenChar=0;
    QString LineContext;
    if(filename.isEmpty())
        return -1;
    FILE *logfile=NULL;
    char LineStart=0;
    char OneLineChar[500];
    int lineCharNum=0;
    logfile =fopen(filename.toLatin1().data(),"r");
        while(1)
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
                 ui->tbrFilesLog->append(LineContext);
                 //qDebug()<<"添加一行数据"<<LineContext;
                 LineNum++;
                 LineContext.clear();
                 lineCharNum=0;
                }
             LastChar=CurrenChar;
         }
         if(LineStart==1)
         {
            //LineContext += QString::fromUtf8(QString(CurrenChar));
            OneLineChar[lineCharNum]=CurrenChar;
            lineCharNum++;
         }
        // qDebug()<<""<<readChar;
        if((CurrenChar==EOF))
            break;
        }
        fclose(logfile);
    ui->tbrFilesLog->append("分析文件完毕，总计"+QString::number(LineNum)+"行日志信息");

    return 1;
}



void MainWindow::append_one_Line(const QString& line)
{
    ui->tbrFilesLog->append(line);
}

void  MainWindow::update_analyse(const int& percent)
{
    ui->prbAnalyse->setMaximum(100);
    ui->prbAnalyse->setValue((int)percent);
}
void  MainWindow::analyse_progress_done()
{
   // qDebug()<<"分析完成，可进行表格预览";
    QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->tbrFilesLog->append(dtime+"::分析完成，可进行表格预览");
    ui->statusbar->showMessage(dtime+"::分析完成，可进行表格预览");
}

void MainWindow::add_one_record(const QString& time,const QString& msg)//分析表心中一行
{
   // qDebug()<<"添加一条信息"<<time<<msg;
    QList<QStandardItem*> rowData;
    rowData<<new QStandardItem(time);
    rowData<<new QStandardItem(msg);
    analysemodel->appendRow( rowData );
}


void MainWindow::export_syslog_progress(const int& percent)
{
    ui->prgExportSyslog->setMaximum(100);
    ui->prgExportSyslog->setValue((int)percent);
}

void MainWindow::export_temp_progress(const int& percent)
{
    ui->prgExportTmpData->setMaximum(100);
    ui->prgExportTmpData->setValue((int)percent);
}

void MainWindow::export_spd_progress(const int& percent)
{
    ui->prgExportFanSpdData->setMaximum(100);
    ui->prgExportFanSpdData->setValue((int)percent);
}

void MainWindow::export_analyse_progress(const int& percent)
{
    ui->prgExportAnalylog->setMaximum(100);
    ui->prgExportAnalylog->setValue((int)percent);

}
void MainWindow::export_craft_progress(const int& percent)
{
    ui->prgExportCraft->setMaximum(100);
    ui->prgExportCraft->setValue((int)percent);

}

void MainWindow::export_cellIn_progress(const int& percent)
{
    ui->prgExportEnterLog->setMaximum(100);
    ui->prgExportEnterLog->setValue((int)percent);
}
void MainWindow::export_cellOut_progress(const int& percent)
{
    ui->prgExportExitlog->setMaximum(100);
    ui->prgExportExitlog->setValue((int)percent);
}
void MainWindow::export_NG_progress(const int& percent)
{
    ui->prgExportNGlog->setMaximum(100);
    ui->prgExportNGlog->setValue((int)percent);
}
void MainWindow::export_Scan_progress(const int& percent)
{
    ui->prgExportScanLog->setMaximum(100);
    ui->prgExportScanLog->setValue((int)percent);
}
void MainWindow::export_Finish_progress(const int& percent)
{
    ui->prgExportFinishLog->setMaximum(100);
    ui->prgExportFinishLog->setValue((int)percent);
}
