#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<QStandardPaths>
#include<QSqlQuery>

void MainWindow::add_one_user(userdata user)
{
    QSqlQuery query;
    query.prepare("INSERT INTO usertable VALUES (:id,:name,:passwd,:cardid,:priv,:date)");
   // query.bindValue(":id", NULL);
    query.bindValue(":name", user.name);
    query.bindValue(":passwd", user.passwd);
    query.bindValue(":cardid", user.cardid);
    query.bindValue(":priv",user.priv);
    query.bindValue(":date",user.datetime);
    query.exec();
   // qDebug()<<"add user done";
}

void MainWindow::modify_user(userdata user)
{
    QSqlQuery query;
   // QString sqlprepare;
   // sqlprepare="UPDATE usertable SET name='"+user.name+"',passwd='"+user.passwd+"',cardid='"+user.cardid+"',priv='"+user.priv+"' WHERE id=" + QString::number(user.id);
  //  qDebug()<<sqlprepare;
    query.prepare("UPDATE usertable SET name='"+user.name+"',passwd='"+user.passwd+"',cardid='"+user.cardid+"',priv='"+user.priv+"' WHERE id=" + QString::number(user.id));
    query.exec();
    qDebug()<<"modify user done";
}
void MainWindow::delete_one_user(userdata user)
{
    QSqlQuery query;
    query.prepare("DELETE FROM usertable WHERE id='"+QString::number(user.id) + "'");
    query.exec();

}



void MainWindow::add_one_temperature_rec(tempdata temp)
{
    QSqlQuery query;
    query.prepare("INSERT INTO temperaturetable VALUES (:id,:temp,:date)");
   // query.bindValue(":id", NULL);
    query.bindValue(":temp", temp.value);
    query.bindValue(":date",temp.datetime);
    query.exec();
   // qDebug()<<"add user done";
}
void MainWindow::add_one_fanspeed_rec(tempdata temp)
{
    QSqlQuery query;
    query.prepare("INSERT INTO fanspeedtable VALUES (:id,:spd,:date)");
   // query.bindValue(":id", NULL);
    query.bindValue(":spd", temp.value);
    query.bindValue(":date",temp.datetime);
    query.exec();
   // qDebug()<<"add user done";
}

QString MainWindow::querry_temperature_by_time(QString starttime,QString endtime)
{
    QString str="SELECT temp,date FROM temperaturetable where date Between '" + starttime + "' AND '" + endtime + "'";
    return str;
}

void MainWindow::add_one_logmsg(logmsg lmsg)
{
    QSqlQuery query;
    query.prepare("INSERT INTO logtable VALUES (:id,:operator,:msg,:date)");
   // query.bindValue(":id", NULL);
    query.bindValue(":operator", lmsg.user);
    query.bindValue(":msg", lmsg.msg);
    query.bindValue(":date",lmsg.datetime);
    query.exec();
}
QString MainWindow::querry_logmsg_by_time(QString starttime,QString endtime)
{
    QString str="SELECT * FROM logtable where date Between '" + starttime + "' AND '" + endtime + "'";
    return str;
}

void MainWindow::add_one_craftmsg(craftmsg cramsg)
{
    QSqlQuery query;

    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    query.prepare("INSERT INTO craft VALUES (:id,:date,:camx,:camy,:camz,:heightdiff,:probefixz,:probex1,:probey1,:probex2,:probey2,:safetyheight,:digdeep,:digdiffx,:digdiffy,:circleradius,:tripointdist,:cutcomp,:pathcutdeep,:planelevel,:mmspeed,:airpressure,:toolusecnt,:probebasez,:usetime)");

    query.bindValue(":date", cramsg.datetime);
    query.bindValue(":camx", cramsg.camx);
    query.bindValue(":camy",cramsg.camy);
    query.bindValue(":camz",cramsg.camz);
    query.bindValue(":heightdiff",cramsg.heightdiff);
    query.bindValue(":probefixz",cramsg.probefixz);
    query.bindValue(":probex1",cramsg.probex1);
    query.bindValue(":probey1",cramsg.probey1);
    query.bindValue(":probex2",cramsg.probex2);
    query.bindValue(":probey2",cramsg.probey2);
    query.bindValue(":safetyheight",cramsg.safetyheight);
    query.bindValue(":digdeep",cramsg.digdeep);
    query.bindValue(":digdiffx",cramsg.digdiffx);
    query.bindValue(":digdiffy",cramsg.digdiffy);
    query.bindValue(":circleradius",cramsg.circleradius);
    query.bindValue(":tripointdist",cramsg.tripointdist);
    query.bindValue(":cutcomp",cramsg.cutcomp);
    query.bindValue(":pathcutdeep",cramsg.pathcutdeep);
    query.bindValue(":planelevel",cramsg.planelevel);
    query.bindValue(":mmspeed",cramsg.mmspeed);
    query.bindValue(":airpressure",cramsg.aripressure);
    query.bindValue(":toolusecnt",cramsg.toolusecnt);
    query.bindValue(":probebasez",cramsg.probebaseZ);
    query.bindValue(":usetime",cramsg.usetime);

   qDebug()<<"insert craft" <<query.exec();

   craft_model->setQuery(querry_craftmsg_by_time(starttime,endtime));
  // craft_model->setQuery("SELECT date,camx,camy,camz,heightdiff,probefixz,probex1,probey1,probex2,probey2,safetyheight,digdeep,digdiffx,digdiffy,circleradius,tripointdist,cutcomp,pathcutdeep,planelevel,mmspeed,airpressure,toolusecnt,probebasez,usetime FROM craft limit 0");
   craft_model->setHeaderData(0,Qt::Horizontal,QObject::tr("时间戳"));
   craft_model->setHeaderData(1,Qt::Horizontal,QObject::tr("拍照X坐标(mm)"));
   craft_model->setHeaderData(2,Qt::Horizontal,QObject::tr("拍照Y坐标(mm)"));
   craft_model->setHeaderData(3,Qt::Horizontal,QObject::tr("拍照Z坐标(mm)"));
   craft_model->setHeaderData(4,Qt::Horizontal,QObject::tr("测高偏差值(mm)"));
   craft_model->setHeaderData(5,Qt::Horizontal,QObject::tr("测高定位Z坐标(mm)"));
   craft_model->setHeaderData(6,Qt::Horizontal,QObject::tr("测高位置X1(mm)"));
   craft_model->setHeaderData(7,Qt::Horizontal,QObject::tr("测高位置Y1(mm)"));
   craft_model->setHeaderData(8,Qt::Horizontal,QObject::tr("测高位置X2(mm)"));
   craft_model->setHeaderData(9,Qt::Horizontal,QObject::tr("测高位置Y2(mm)"));
   craft_model->setHeaderData(10,Qt::Horizontal,QObject::tr("铣削安全高度(mm)"));
   craft_model->setHeaderData(11,Qt::Horizontal,QObject::tr("铣削深度(mm)"));
   craft_model->setHeaderData(12,Qt::Horizontal,QObject::tr("铣削坐标偏量X(mm)"));
   craft_model->setHeaderData(13,Qt::Horizontal,QObject::tr("铣削坐标偏量Y(mm)"));
   craft_model->setHeaderData(14,Qt::Horizontal,QObject::tr("加工圆半径(mm)"));
   craft_model->setHeaderData(15,Qt::Horizontal,QObject::tr("三点测量间距(mm)"));
   craft_model->setHeaderData(16,Qt::Horizontal,QObject::tr("刀具直径补偿(mm)"));
   craft_model->setHeaderData(17,Qt::Horizontal,QObject::tr("铣削单圈Z轴下刀量(mm)"));
   craft_model->setHeaderData(18,Qt::Horizontal,QObject::tr("表面平整度(mm)"));
   craft_model->setHeaderData(19,Qt::Horizontal,QObject::tr("主轴转速(rpm)"));
   craft_model->setHeaderData(20,Qt::Horizontal,QObject::tr("除尘风速(m3/min)"));
   craft_model->setHeaderData(21,Qt::Horizontal,QObject::tr("刀具使用次数(次)"));
   craft_model->setHeaderData(22,Qt::Horizontal,QObject::tr("铣削Z探测值(mm)"));
   craft_model->setHeaderData(23,Qt::Horizontal,QObject::tr("加工时间(HH:mm:ss)"));

   ui->tbvCraft->setModel(craft_model);
   ui->tbvCraft->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
   ui->tbvCraft->setSelectionBehavior(QAbstractItemView::SelectRows);


}
QString MainWindow::querry_craftmsg_by_time(QString starttime,QString endtime)
{

    QString str="SELECT date,camx,camy,camz,heightdiff,probefixz,probex1,probey1,probex2,probey2,safetyheight,digdeep,digdiffx,digdiffy,circleradius,tripointdist,cutcomp,pathcutdeep,planelevel,mmspeed,airpressure,toolusecnt,probebasez,usetime FROM craft where date Between '" + starttime + "' AND '" + endtime + "'";
    return str;
}

void MainWindow::add_one_formal(LocalFormal formal)//新增本地配方库
{
    QSqlQuery query;
    QString prepare;
    QString msg;
    prepare="SELECT formulano,version FROM localformaltable WHERE formulano='" + formal.formulano + "' AND version='"+formal.version+"'";
    qDebug()<<prepare;
    query.prepare(prepare);
    query.exec();
    if(query.next())
    {
        prepare="UPDATE localformaltable SET productno='"+formal.productno+"',productname='"+formal.productname+"',dataparm='"+formal.dataparm+"',dateget='"+formal.dateget + "' WHERE formulano='" + formal.formulano + "' and version='"+formal.version + "'";
        query.prepare(prepare);
        query.exec();
        msg="存在相同版本号和配方号的配方，更新配方数值";
        const_msg(msg);
        while (query.next()) {
            query.prepare(prepare);
            query.exec();
        }
    }
    else
    {
    msg="无相同的配方号和版本号，进行插入新配方";
    const_msg(msg);
    query.prepare("INSERT INTO localformaltable VALUES (:id,:formulano,:version,:productno,:productname,:dataparm,:dateget,:daterun,:usestatus,:opreate)");
    //query.bindValue(":id", NULL);
    query.bindValue(":formulano", formal.formulano);
    query.bindValue(":version", formal.version);
    query.bindValue(":productno", formal.productno);
    query.bindValue(":productname",formal.productname);
    query.bindValue(":dataparm",formal.dataparm);
    query.bindValue(":dateget",formal.dateget);
    query.bindValue(":daterun",formal.daterun);
    query.bindValue(":usestatus",formal.usestatus);
    query.bindValue(":opreate",formal.opreate);
    query.exec();
    }
}
void MainWindow::update_one_formal(LocalFormal formal)
{
    QSqlQuery query;
    QString sqlprepare;
    sqlprepare="UPDATE localformaltable SET daterun='"+formal.daterun+"',usestatus='"+formal.usestatus+"' WHERE id=" + QString::number(formal.id);
    query.prepare(sqlprepare);
    query.exec();
    qDebug()<<"update formal done";

    meslocalformalmodel->setQuery("select id,formulano,version,productno,productname,dateget,daterun,usestatus,opreate from localformaltable");
    meslocalformalmodel->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
    meslocalformalmodel->setHeaderData(1,Qt::Horizontal,QObject::tr("配方号"));
    meslocalformalmodel->setHeaderData(2,Qt::Horizontal,QObject::tr("版本"));
    meslocalformalmodel->setHeaderData(3,Qt::Horizontal,QObject::tr("产品编码"));
    meslocalformalmodel->setHeaderData(4,Qt::Horizontal,QObject::tr("产品名称"));
    meslocalformalmodel->setHeaderData(5,Qt::Horizontal,QObject::tr("下发时间"));
    meslocalformalmodel->setHeaderData(6,Qt::Horizontal,QObject::tr("执行时间"));
    meslocalformalmodel->setHeaderData(7,Qt::Horizontal,QObject::tr("状态"));
    meslocalformalmodel->setHeaderData(8,Qt::Horizontal,QObject::tr("执行操作"));
    ui->tbvFormal->setModel(meslocalformalmodel);
    ui->tbvFormal->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvFormal->setSelectionBehavior(QAbstractItemView::SelectItems);
}

long MainWindow::update_usestatus_formal_before(LocalFormal formal)//把其他未选中的在使用中的配方标记为未使用
{
    QSqlQuery query;
    QString sqlprepare;
    sqlprepare="UPDATE localformaltable SET usestatus='"+formal.usestatus+"' WHERE usestatus='使用中'";
    query.prepare(sqlprepare);
    query.exec();

}

void MainWindow::db_update_matirial(QString Mat)
{
    QSqlQuery query;
    QString sqlprepare;
    sqlprepare="UPDATE localmatirialorderlist SET matirial='"+ Mat +"' WHERE id=1";
    query.prepare(sqlprepare);
    query.exec();
    qDebug()<<"update matirial done";
}

void MainWindow::db_update_oderlist(QString oderlist)
{
    QSqlQuery query;
    QString sqlprepare;
    sqlprepare="UPDATE localmatirialorderlist SET orderlist='"+ oderlist +"' WHERE id=1";
    query.prepare(sqlprepare);
    query.exec();
    qDebug()<<"update oderlist done";
}


void MainWindow::db_update_matirial_num_ft(Material mat)
{

    QSqlQuery query;
    QString sqlprepare;
    sqlprepare="UPDATE materiallist SET num='"+mat.number+"',feedtime='"+mat.feeding_time +"' WHERE vid='"+mat.id+"'";
    query.prepare(sqlprepare);
    query.exec();
    qDebug()<<"update matirial feed number and feeding time done";
}

 void MainWindow::db_update_matirial_remain(Material mat)
 {

     QSqlQuery query;
     QString sqlprepare;
     sqlprepare="UPDATE materiallist SET remain='"+mat.surplus_num +"',unit='"+mat.unit +"' WHERE code='"+mat.material_code+"'";
     query.prepare(sqlprepare);
     query.exec();
     qDebug()<<"update matirial remain done";
 }




 void MainWindow::add_one_cellin(Entermsg msg)
 {
     QSqlQuery query;
     query.prepare("INSERT INTO logintable VALUES (:id,:barcode,:ordernum,:result,:detail,:date)");
    // query.bindValue(":id", NULL);
     query.bindValue(":barcode",msg.barcode);
     query.bindValue(":ordernum", msg.ordernum);
     query.bindValue(":result", msg.result);
     query.bindValue(":detail",msg.detail);
     query.bindValue(":date",msg.datetime);
     query.exec();

     QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
     QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
     login_model->setQuery("select barcode,ordernum,result,detail,date from logintable where date between '"+starttime+"' and '"+endtime +"'");
     //temp_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
     login_model->setHeaderData(0,Qt::Horizontal,QObject::tr("电芯条形码"));
     login_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工单号"));
     login_model->setHeaderData(2,Qt::Horizontal,QObject::tr("结果"));
     login_model->setHeaderData(3,Qt::Horizontal,QObject::tr("结果说明"));
     login_model->setHeaderData(4,Qt::Horizontal,QObject::tr("进站时间"));
     ui->tbvEnterLogs->setModel(login_model);
     ui->lbCellIntoNum->setText(QString::number(login_model->rowCount()));



 }

 void MainWindow::add_one_cellout(Exitmsg msg)
 {
     QSqlQuery query;
     query.prepare("INSERT INTO logouttable VALUES (:id,:barcode,:ordernum,:result,:ngreason,:detail,:date)");
    // query.bindValue(":id", NULL);
     query.bindValue(":barcode",msg.barcode);
     query.bindValue(":ordernum", msg.ordernum);
     query.bindValue(":result", msg.result);
     query.bindValue(":ngreason", msg.ngreason);
     query.bindValue(":detail",msg.detail);
     query.bindValue(":date",msg.datetime);
     query.exec();

     QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
     QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
     logout_model->setQuery("select barcode,ordernum,result,ngreason,detail,date from logouttable where date between '"+starttime+"' and '"+endtime +"'");
     //temp_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
     logout_model->setHeaderData(0,Qt::Horizontal,QObject::tr("电芯条形码"));
     logout_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工单号"));
     logout_model->setHeaderData(2,Qt::Horizontal,QObject::tr("结果"));
     logout_model->setHeaderData(3,Qt::Horizontal,QObject::tr("不良品名称"));
     logout_model->setHeaderData(4,Qt::Horizontal,QObject::tr("结果说明"));
     logout_model->setHeaderData(5,Qt::Horizontal,QObject::tr("出站时间"));
     ui->tbvExitLogs->setModel(logout_model);
     ui->tbvExitLogs->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     ui->tbvExitLogs->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->lbCellOutNum->setText(QString::number(logout_model->rowCount()));
 }

 void MainWindow::add_one_finish_msg()
 {

     QSqlQuery query;
     qDebug()<<"insert one finish";
     query.prepare("INSERT INTO finishtable VALUES (:id,:processid,:equmentid,:operater,:barcode,:ordernum,:paramone,:paramtwo,:paramthree,:paramfour,:paramfive,:paramsix,:paramseven,:detail,:datein,:date)");
    // query.bindValue(":id", NULL);
     query.bindValue(":processid",ui->leProcessNumR->text());
     query.bindValue(":equmentid",ui->leDevCodeR->text());
     query.bindValue(":operater",logonUser);
     query.bindValue(":barcode",ScanCodeGlb);
     query.bindValue(":ordernum", ui->leOrderNumR->text());
     query.bindValue(":paramone", QString::number(DigDeep));//深度
     query.bindValue(":paramtwo",QString::number(CircleRadius));//半径
     query.bindValue(":paramthree",QString::number(MMspeed));//转速
     query.bindValue(":paramfour",QString::number(FanSpeedR));//风速
     query.bindValue(":paramfive",QString::number(toolUseCnt+1));//刀具使用次数
     query.bindValue(":paramsix",QString::number(PlaneBlance));//表面平整度
     query.bindValue(":paramseven",QString::number(lncProbeBaseZValue));//测量Z值
     query.bindValue(":detail","未上传");//未上传
     //从扫码日志表中查询对应条码的进站时间
     QSqlQuery querycellintime;
     querycellintime.prepare("select date from scantable where barcode='"+ScanCodeGlb+"'");
     querycellintime.exec();
     if(querycellintime.next())
         query.bindValue(":datein", querycellintime.value(0).toString());
     else
     {
         query.bindValue(":datein", "未知时间");
     }

     while(querycellintime.next())
     {
        query.bindValue(":datein", querycellintime.value(0).toString());
     }

     query.bindValue(":date",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

     query.exec();

     QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
     QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

     finish_model->setQuery("select id,processid,equmentid,operater,barcode,ordernum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,paramseven,detail,datein,date from finishtable where date between '"+starttime+"' and '"+endtime +"'");

     finish_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
     finish_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工序号"));
     finish_model->setHeaderData(2,Qt::Horizontal,QObject::tr("设备ID"));
     finish_model->setHeaderData(3,Qt::Horizontal,QObject::tr("操作员"));
     finish_model->setHeaderData(4,Qt::Horizontal,QObject::tr("扫码值"));
     finish_model->setHeaderData(5,Qt::Horizontal,QObject::tr("工单号"));
     finish_model->setHeaderData(6,Qt::Horizontal,QObject::tr("铣钉深度(mm)"));
     finish_model->setHeaderData(7,Qt::Horizontal,QObject::tr("加工半径(mm)"));
     finish_model->setHeaderData(8,Qt::Horizontal,QObject::tr("主轴转速(rpm)"));
     finish_model->setHeaderData(9,Qt::Horizontal,QObject::tr("风速(L/min)"));
     finish_model->setHeaderData(10,Qt::Horizontal,QObject::tr("刀具使用次数"));
     finish_model->setHeaderData(11,Qt::Horizontal,QObject::tr("表面平整度(mm)"));
     finish_model->setHeaderData(12,Qt::Horizontal,QObject::tr("铣削Z测量值(mm)"));
     finish_model->setHeaderData(13,Qt::Horizontal,QObject::tr("详情"));
     finish_model->setHeaderData(14,Qt::Horizontal,QObject::tr("进站扫码时间"));
     finish_model->setHeaderData(15,Qt::Horizontal,QObject::tr("加工完成时间"));


     ui->tbvFinishLogs->setModel(finish_model);
     ui->tbvFinishLogs->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     ui->tbvFinishLogs->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->lbFinishNum->setText(QString::number(finish_model->rowCount()));

 }

  void MainWindow::update_CellIn_Log(ReportCellIn CellIn)
  {
      QSqlQuery query;
      QString sqlprepare;
      sqlprepare="UPDATE scantable SET detail='已上传' WHERE id="+QString::number(CellIn.id);
      query.prepare(sqlprepare);
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

  void MainWindow::update_CellOut_Log(ReportCellOut CellOut)
  {
      QSqlQuery query;
      QString sqlprepare;
      sqlprepare="UPDATE finishtable SET detail='已上传' WHERE id="+QString::number(CellOut.id);
      query.prepare(sqlprepare);
      query.exec();

      QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
      QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
      finish_model->setQuery("select id,processid,equmentid,barcode,ordernum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,detail,date from finishtable where date between '"+starttime+"' and '"+endtime +"'");

      finish_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
      finish_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工序号"));
      finish_model->setHeaderData(2,Qt::Horizontal,QObject::tr("设备ID"));
      finish_model->setHeaderData(3,Qt::Horizontal,QObject::tr("扫码值"));
      finish_model->setHeaderData(4,Qt::Horizontal,QObject::tr("工单号"));
      finish_model->setHeaderData(5,Qt::Horizontal,QObject::tr("铣钉深度"));
      finish_model->setHeaderData(6,Qt::Horizontal,QObject::tr("加工半径"));
      finish_model->setHeaderData(7,Qt::Horizontal,QObject::tr("主轴转速"));
      finish_model->setHeaderData(8,Qt::Horizontal,QObject::tr("风速"));
      finish_model->setHeaderData(9,Qt::Horizontal,QObject::tr("刀具使用次数"));
      finish_model->setHeaderData(10,Qt::Horizontal,QObject::tr("表面平整度"));
      finish_model->setHeaderData(11,Qt::Horizontal,QObject::tr("详情"));
      finish_model->setHeaderData(12,Qt::Horizontal,QObject::tr("日期时间"));

      ui->tbvFinishLogs->setModel(finish_model);
      ui->tbvFinishLogs->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
      ui->tbvFinishLogs->setSelectionBehavior(QAbstractItemView::SelectRows);
      ui->lbFinishNum->setText(QString::number(finish_model->rowCount()));
  }

void MainWindow::update_NG_Log(NGmsg nglog)
{
    QSqlQuery query;
    QString sqlprepare;
    sqlprepare="UPDATE ngtable SET upload='已上传' WHERE id="+QString::number(nglog.id);
    query.prepare(sqlprepare);
    query.exec();
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

    ng_model->setQuery("select id,processid,equmentid,barcode,badcode,badnum,detail,upload,date from ngtable where date between '"+starttime+"' and '"+endtime +"'");

    ng_model->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
    ng_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工序号"));
    ng_model->setHeaderData(2,Qt::Horizontal,QObject::tr("设备ID"));
    ng_model->setHeaderData(3,Qt::Horizontal,QObject::tr("扫码值"));
    ng_model->setHeaderData(4,Qt::Horizontal,QObject::tr("不良代号"));
    ng_model->setHeaderData(5,Qt::Horizontal,QObject::tr("个数"));
    ng_model->setHeaderData(6,Qt::Horizontal,QObject::tr("不良原因"));
    ng_model->setHeaderData(7,Qt::Horizontal,QObject::tr("上传状态"));
    ng_model->setHeaderData(8,Qt::Horizontal,QObject::tr("出站时间"));
    ui->tbvNotGood->setModel(ng_model);
    ui->tbvNotGood->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvNotGood->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->lbNoGoodNum->setText(QString::number(ng_model->rowCount()));
}

void MainWindow::add_one_ng_record(NGmsg nglog)//不良记录
{
    QSqlQuery query;
    query.prepare("INSERT INTO ngtable VALUES (:id,:processid,:equmentid,:barcode,:badcode,:badnum,:detail,:upload,:date)");
   // query.bindValue(":id", NULL);
    query.bindValue(":processid",nglog.processid);
    query.bindValue(":equmentid",nglog.equmentid);
    query.bindValue(":barcode",nglog.barcode);
    query.bindValue(":badcode", nglog.badcode);
    query.bindValue(":badnum", nglog.badnum);
    query.bindValue(":detail",nglog.detail);
    query.bindValue(":upload",nglog.upload);
    query.bindValue(":date",nglog.datetime);
    query.exec();

    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

    ng_model->setQuery("select id,processid,equmentid,barcode,badcode,badnum,detail,upload,date from ngtable where date between '"+starttime+"' and '"+endtime +"'");

    ng_model->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
    ng_model->setHeaderData(1,Qt::Horizontal,QObject::tr("工序号"));
    ng_model->setHeaderData(2,Qt::Horizontal,QObject::tr("设备ID"));
    ng_model->setHeaderData(3,Qt::Horizontal,QObject::tr("扫码值"));
    ng_model->setHeaderData(4,Qt::Horizontal,QObject::tr("不良代号"));
    ng_model->setHeaderData(5,Qt::Horizontal,QObject::tr("个数"));
    ng_model->setHeaderData(6,Qt::Horizontal,QObject::tr("不良原因"));
    ng_model->setHeaderData(7,Qt::Horizontal,QObject::tr("上传状态"));
    ng_model->setHeaderData(8,Qt::Horizontal,QObject::tr("出站时间"));
    ui->tbvNotGood->setModel(ng_model);
    ui->tbvNotGood->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvNotGood->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->lbNoGoodNum->setText(QString::number(ng_model->rowCount()));
}



//新增进站mes接口日志

void MainWindow::add_one_cellinmeslog(MesCellInLogs cellinlog)//新增一条进站mes日志
{
    QSqlQuery query;
    query.prepare("INSERT INTO cellinlogs VALUES (:id,:barcode,:reqtime,:usedtime,:resptime,:processid,:equmentid,:retcode,:retmsg,:billno,:billnum,:special,:postjson,:respjson)");
   // query.bindValue(":id", NULL);
    query.bindValue(":barcode",cellinlog.barcode);
    query.bindValue(":reqtime",cellinlog.reqtime);
    query.bindValue(":usedtime",cellinlog.usedtime);
    query.bindValue(":resptime", cellinlog.resptime);
    query.bindValue(":processid", cellinlog.processid);
    query.bindValue(":equmentid",cellinlog.equmentid);
    query.bindValue(":retcode",cellinlog.retcode);
    query.bindValue(":retmsg",cellinlog.retmsg);
    query.bindValue(":billno",cellinlog.billno);
    query.bindValue(":billnum",cellinlog.billnum);
    query.bindValue(":postjson",cellinlog.postjson);
    query.bindValue(":respjson",cellinlog.respjson);
    query.exec();
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

    cellinmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,billno,billnum,special,postjson,respjson from cellinlogs where reqtime between '"+starttime+".000' and '"+endtime +".999'");
//序号	电芯条码	请求时间	耗时ms	响应时间	工序编码	设备编码	返回代码	返回信息	工单号	工单数量	特殊值（一注注液前重量）	请求json	返回json
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

}

//序号	电芯条码	请求时间	耗时ms	响应时间	工序编码	设备编码	工单号	操作员	工步(可选)	是否扣料(可选)	数量	参数1	参数2	…	参数N	返回代码	返回信息	余料信息(可选)	不良代码1	不良原因1	不良代码2	不良原因2	请求json	返回json

void MainWindow::add_one_celloutmeslog(MesCellOutLogs celloutlog)//新增一条出站mes日志
{
           QSqlQuery query;
           query.prepare("INSERT INTO celloutlogs VALUES (:id,:barcode,:reqtime,:usedtime,:resptime,:processid,:equmentid,:billno,:opreater,:steps,:materialdec,:productnum,:paramone,:paramtwo,:paramthree,:paramfour,:paramfive,:paramsix,:retcode,:retmsg,:materialremain,:ngcodeone,:ngdetailone,:ngcodetwo,:ngdetailtwo,:postjson,:respjson)");
          // query.bindValue(":id", NULL);
           query.bindValue(":barcode",celloutlog.barcode);
           query.bindValue(":reqtime",celloutlog.reqtime);
           query.bindValue(":usedtime",celloutlog.usedtime);
           query.bindValue(":resptime", celloutlog.resptime);
           query.bindValue(":processid", celloutlog.processid);
           query.bindValue(":equmentid",celloutlog.equmentid);
           query.bindValue(":billno",celloutlog.billno);
           query.bindValue(":opreater",celloutlog.opreater);
           query.bindValue(":steps",celloutlog.steps);
           query.bindValue(":materialdec",celloutlog.materialdec);
           query.bindValue(":productnum",celloutlog.productnum);
           query.bindValue(":paramone",celloutlog.paramone);
           query.bindValue(":paramtwo",celloutlog.paramtwo);
           query.bindValue(":paramthree",celloutlog.paramthree);
           query.bindValue(":paramfour",celloutlog.paramfour);
           query.bindValue(":paramfive",celloutlog.paramfive);
           query.bindValue(":paramsix",celloutlog.paramsix);
           query.bindValue(":retcode",celloutlog.retcode);
           query.bindValue(":retmsg",celloutlog.retmsg);
           query.bindValue(":materialremain",celloutlog.materialremain);
           query.bindValue(":ngcodeone",celloutlog.ngcodeone);
           query.bindValue(":ngdetailone",celloutlog.ngdetailone);
           query.bindValue(":ngcodetwo",celloutlog.ngcodetwo);
           query.bindValue(":ngdetailtwo",celloutlog.ngdetailtwo);
           query.bindValue(":postjson",celloutlog.postjson);
           query.bindValue(":respjson",celloutlog.respjson);
           query.exec();
           QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
           QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

           celloutmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,billno,opreater,steps,materialdec,productnum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,retcode,retmsg,materialremain,ngcodeone,ngdetailone,ngcodetwo,ngdetailtwo,postjson,respjson from celloutlogs where reqtime between '"+starttime+".000' and '"+endtime +".999'");
       //序号	电芯条码	请求时间	耗时ms	响应时间	工序编码	设备编码	工单号	操作员	工步(可选)	是否扣料(可选)	数量	参数1	参数2	…	参数N	返回代码	返回信息	余料信息(可选)	不良代码1	不良原因1	不良代码2	不良原因2	请求json	返回json
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
}


void MainWindow::add_one_othermeslog(MesOtherLogs otherlog)//新增一条其他接口mes日志
{
    QSqlQuery query;
    query.prepare("INSERT INTO otherlogs VALUES (:id,:type,:reqtime,:usedtime,:resptime,:processid,:equmentid,:retcode,:retmsg,:postjson,:respjson)");
   // query.bindValue(":id", NULL);
    query.bindValue(":type",otherlog.type);
    query.bindValue(":reqtime",otherlog.reqtime);
    query.bindValue(":usedtime",otherlog.usedtime);
    query.bindValue(":resptime", otherlog.resptime);
    query.bindValue(":processid", otherlog.processid);
    query.bindValue(":equmentid",otherlog.equmentid);
    query.bindValue(":retcode",otherlog.retcode);
    query.bindValue(":retmsg",otherlog.retmsg);
    query.bindValue(":postjson",otherlog.postjson);
    query.bindValue(":respjson",otherlog.respjson);
    query.exec();
    QString endtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";

    othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where reqtime between '"+starttime+".000' and '"+endtime +".999'");

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

}
