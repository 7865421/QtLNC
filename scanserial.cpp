#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
//得到扫描码的分析程序
//进站日志：电芯条码，工单号，进站结果，结果说明，进站时间
 void MainWindow::analysisScanCode(QString& sc)
{
     QString msg;
     QString result;
     qDebug()<<"收到扫码数据"<<sc;
     QString ScanCode;
     bool rulesSucc=false;
     if(m_IniFile->value("ScanActiveRules").toInt()==1)//开启规则匹配
     {
         QString rex=m_IniFile->value("ScanRules").toString();
         QString rex1="\\w{"+m_IniFile->value("ScanCodeLength").toString()+"}$";
         QRegularExpression rExp(rex);
         QRegularExpression rExp1(rex1);
         qDebug()<<rex<<rex1<<rExp.match(sc).capturedStart()<<rExp1.match(sc).capturedStart();
         if(rExp.match(sc).capturedStart()==0)
         {  qDebug()<<"扫码头字符串匹配成功";
             msg="扫码字符串匹配成功，";
             if(rExp1.match(sc).capturedStart()==0)
             {
                 qDebug()<<"扫码检查规则长度匹配成功";
                 //进行进站录入
                 rulesSucc =true;
                 msg +="长度规则匹配成功";

             }
             else
             {
                 qDebug()<<"扫码码长度匹配不成功";
                 msg +="扫码长度不匹配";
             }
         }
         else
         {
             qDebug()<<"头字符串匹配不成功！";
             msg ="扫码字符串不匹配";
         }
         const_msg(msg);

     }
     else
         rulesSucc=true;

     if(rulesSucc)
     {
         ScanCode = sc;
         ScanCodeGlb=sc;
         if(m_IniFile->value("ScanActiveRules").toInt()==1){
             result="扫码成功";
            ui->lbScanCode->setText(ScanCode+":"+result);
            }
         else{
             result="无规则扫码完成";
            ui->lbScanCode->setText(ScanCode+":"+result);
         }
     }
     else
     {
         ScanCode = sc;
         ScanCodeGlb="无效条码";
         result="扫码失败";
         ui->lbScanCode->setText(ScanCode+":"+result+msg);
        // return;
     }
    if(ui->stackedWidget->currentIndex()==3 && ui->tabWidgetMes->currentIndex()==7)//录入不良品页面
    {
        ui->leNGbarCode->setText(ScanCode);
    }
    else//扫码进站
    {
        //调用扫码进站,上传数据在定时器中自动完成
       // postCellIntoVerifity(ScanCode);
        //将扫码结果存入数据库

        QSqlQuery query;
        query.prepare("INSERT INTO scantable VALUES (:id,:processid,:equmentid,:barcode,:ordernum,:result,:detail,:date)");
       // query.bindValue(":id", NULL);
        query.bindValue(":processid",ui->leProcessNumR->text());
        query.bindValue(":equmentid",ui->leDevCodeR->text());
        query.bindValue(":barcode",ScanCodeGlb);
        query.bindValue(":ordernum", ui->leOrderNumR->text());
        query.bindValue(":result", result+","+msg);
        query.bindValue(":detail","未上传");
        query.bindValue(":date",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.exec();

        QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
        scan_model->setQuery("select id,processid,equmentid,barcode,ordernum,result,detail,date from scantable where date between '"+starttime+"' and '"+endtime +"'");
        scan_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
        scan_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工序号"));
        scan_model->setHeaderData(2,Qt::Horizontal,QObject::tr("设备ID"));
        scan_model->setHeaderData(3,Qt::Horizontal,QObject::tr("扫码值"));
        scan_model->setHeaderData(4,Qt::Horizontal,QObject::tr("工单号"));
        scan_model->setHeaderData(5,Qt::Horizontal,QObject::tr("结果"));
        scan_model->setHeaderData(6,Qt::Horizontal,QObject::tr("详情"));
        scan_model->setHeaderData(7,Qt::Horizontal,QObject::tr("日期时间"));

        ui->tbvScanLogs->setModel(scan_model);
        ui->tbvScanLogs->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tbvScanLogs->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->lbScanNum->setText(QString::number(scan_model->rowCount()));

    }

}



 //读取扫码枪数值，并按规则校验，如果扫码错误，则应触发扫码错误日志，扫码成功则进行进站校验
void MainWindow::ScanSerialRead()//扫码枪串口接收函数
{
    QByteArray CodeTmp = ScanSerial->readAll();
    QStringList list;
    QString BarCodes;
    if(!CodeTmp.isEmpty())
    {
        ScanCodeBytes.append(CodeTmp);
      //  qDebug()<<"receive data from uart"<<ScanCodeBytes;
        if(ScanCodeBytes.contains("\n") || ScanCodeBytes.contains("\r") || ScanCodeBytes.contains("\t"))//接收信号完成
        {

            QString OneBarCode;
             OneBarCode =  ScanCodeBytes;
            if(ScanCodeBytes.contains("\n"))
                list=OneBarCode.split("\n");
            if(ScanCodeBytes.contains("\r"))
                list=OneBarCode.split("\r");
            if(ScanCodeBytes.contains("\t"))
                list=OneBarCode.split("\t");
            BarCodes=list[0];
            ScanCodeBytes.clear();
       //   qDebug()<<"receive data from uart contains"<<BarCodes;
            emit ScanCodeReadDone(BarCodes);
        }
    }

}
