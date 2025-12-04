#include "mainwindow.h"
#include<QSqlQuery>
#include "ui_mainwindow.h"
#include<QMessageBox>
//json数据交换
/*
 请求：
{
"process_id":"JRD",
"equipment_id":"M1CL01JRJI0001"
}
*/

void MainWindow::postFormalTimeout()//配方申请超时
{
    postFormalOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
   //     QEventLoop::exec();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postFormalOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(LocalFormalLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;

    update_mes_link_state(postFormalOK);
    if(postFormalOK){
        LocalFormalLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(LocalFormalLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        LocalFormalLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        qDebug()<<"配方申请成功"<<postFormalOK;
        msg="配方申请链接成功，有数据返回";
        const_msg(msg);
        add_one_othermeslog(LocalFormalLogs);
       // postEPFormalVerify();//设备校验
    }
    else{
        LocalFormalLogs.retcode="";
        LocalFormalLogs.retmsg="超时";
        LocalFormalLogs.respjson="";
        LocalFormalLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(LocalFormalLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        LocalFormalLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(LocalFormalLogs);
        qDebug()<<"配方申请失败或网络不可达"<<postFormalOK;
        FormalCnt=FormalCnt-1;
        if(FormalCnt<=0)
        {
         qDebug()<<"配方申请重连不成功";
         msg="配方申请失败，超时！";
         const_msg(msg);
         QMessageBox::warning(this, "接口超时提示", "工艺参数申请失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);

        }
        else
        {
            qDebug()<<"再次发出配方申请请求"<<FormalCnt;
            msg="配方申请重连"+QString::number(FormalCnt);
            const_msg(msg);
            postFormal();
        }
    }
}

void MainWindow::postFormal()//工艺申请上传,获取配方
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postFormalFinish(QNetworkReply*)));
    mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
    mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/Formal");
    QUrl url(m_IniFile->value("mesCraftUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());

    LocalFormalLogs.processid=m_IniFile->value("lncProcessNum").toString();
    LocalFormalLogs.equmentid=m_IniFile->value("LncDevCode").toString();
    LocalFormalLogs.type=1;
    LocalFormalLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    LocalFormalLogs.postjson = QString(endJsonDoc.toJson());
    LocalFormalLogs.retcode="";
    LocalFormalLogs.retmsg="";
    mesTxtMsg("配方申请发出数据：\n"+QString(endJsonDoc.toJson()));
    postFormalTimeout();

}


/*
请求：
{
 "process_id":"JRD",
 "equipment_id":"M1CL01JRJI0001",
 "formula_no":"9137032275449967XN",
 "product_no":"CCCAB71L7H328001",
 "version":"1",
 "data": [
    {
     "param_code":"HCD001",
     "param_name":"Step#1 参数名称1",
     "param_unit":"",
     "param_upper":"",
     "param_value":"60",
     "param_lower":""
    },
    {
     "param_code":"HCD003",
     "param_name":"Step#2参数名称2",
     "param_unit":"",
     "param_upper":"",
     "param_value":"60",
     "param_lower":""
    }
  ]
}
*/

void MainWindow::postFormalVerifyTimeout(QString id)//配方校验超时
{
    //等待验证成功，如果验证通过，则更新，验证不过，则不更新
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
     //   QEventLoop::exec();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postFormalVerifyOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(FormalVerifyLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    update_mes_link_state(postFormalVerifyOK);
    if(postFormalVerifyOK)//传输成功
    {
        FormalVerifyLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(FormalVerifyLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        FormalVerifyLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(FormalVerifyLogs);
    if(postFormalVerifySucceed){//校验成功
    LocalFormal formal;
    qDebug()<<"执行操作";
    msg="执行操作，配方验证接口链接成功，有数据返回，"+(postFormalVerifySucceed==true)?"配方校验成功！":"配方校验失败！";
    const_msg(msg);
    formal.usestatus="";
    update_usestatus_formal_before(formal);//将当前使用中的数据标记为不再使用状态
    formal.usestatus="使用中";
    QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    formal.daterun=dtime;
    formal.id=id.toInt();
    qDebug()<<formal.id;
    update_one_formal(formal);
    //从数据库中选择在使用中的配方，并将参数送到详细列表
     QSqlQuery query;
     QString str="SELECT dataparm from localformaltable WHERE id="+id;
     query.prepare(str);
     query.exec();
     while(query.next())
     {
         QString string = query.value(0).toString();
         QJsonDocument jadoc = QJsonDocument::fromJson(string.toUtf8());
         QJsonArray paramArray = jadoc.array();
         int pcnt=0;
         QJsonObject Param;
         FormalParam Fparam;
         mesformaldetailmodel->removeRows(0,mesformaldetailmodel->rowCount()); //删除所有行
         for(pcnt=0;pcnt<paramArray.count();pcnt++)//添加使用中的配方参数
         {
            Param=paramArray.at(pcnt).toObject();
            Fparam.param_code=Param.value("param_code").toString();
            Fparam.param_name=Param.value("param_name").toString();;
            Fparam.param_unit=Param.value("param_unit").toString();;
            Fparam.param_upper=Param.value("param_upper").toString();
            Fparam.param_value=Param.value("param_value").toString();
            Fparam.param_lower=Param.value("param_lower").toString();
            add_one_record_formal(Fparam);

            //添加数据更新到lnc
            if(Fparam.param_code=="SC-XDJ006")
                lncWorkDeepValue=(int)(Fparam.param_value.toFloat()*1000.0);
            if(Fparam.param_code=="SC-XDJ003")
                lncCircleRidus=(int)(Fparam.param_value.toFloat()*1000.0);
            if(Fparam.param_code=="SC-XDJ002")
                lncMMSpdValue=(int)(Fparam.param_value.toFloat()*1.0);

            msg="执行操作，配方验证成功，更新参数到配方明细表中";
            const_msg(msg);

         }
        qDebug()<<"配方配方参数成功，更新到参数表";
     //   lncCtrlValue=0;
      //  lncCtrlValue2=0;
     }
    }
  }//update
  else{
        FormalVerifyLogs.retcode="";
        FormalVerifyLogs.retmsg="超时";
        FormalVerifyLogs.respjson="";
        FormalVerifyLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(FormalVerifyLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        FormalVerifyLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(FormalVerifyLogs);

        qDebug()<<"配方验证失败或网络不可达";
        FormalVerifyCnt=FormalVerifyCnt-1;
        if(FormalVerifyCnt<=0)
        {
         qDebug()<<"配方验证重连不成功";
         msg="执行操作，配方验证接口链接失败，超时！";
         const_msg(msg);
         /*
         if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
             lncCtrlValue=123456;
             lncCtrlValue2=654321;
         }
         else
         {
             lncCtrlValue=0;
             lncCtrlValue2=0;
         }
         */
         if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
         QMessageBox::warning(this, "接口超时提示", "配方验证失败，超时！LNC将不可加工！"
                , QMessageBox::Ok, QMessageBox::NoButton);
         }
         else
         {
             QMessageBox::warning(this, "接口超时提示", "配方验证失败，超时！"
                    , QMessageBox::Ok, QMessageBox::NoButton);
         }
        }
        else
        {
            qDebug()<<"再次发出配方验证请求"<<FormalVerifyCnt;
            msg="配方验证重连"+QString::number(FormalVerifyCnt);
            const_msg(msg);
            postFormalVerify(id);
        }
    }
}

void MainWindow::postFormalVerify(QString index)//配方校验
{


    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QJsonArray dataArray;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postFormalVerifyFinish(QNetworkReply*)));

    postFormalVerifySucceed=false;//校验不成功
    postFormalVerifyOK=false;//传输不成功
    //查询正在使用中的配方，并上发数据

    //先上传选中的配方数据，校验，然后再更加校验结果进行更新数据。
    QSqlQuery query;
    QString str="SELECT formulano,version,productno,productname,dataparm from localformaltable WHERE id=" + index;
    query.prepare(str);
    query.exec();
    while(query.next())//将读取到的配方数据结构
    {
        LocalFormal formal;
        formal.formulano=query.value(0).toString();
        formal.version=query.value(1).toString();
        formal.productno=query.value(2).toString();
        formal.productname=query.value(3).toString();
        formal.dataparm=query.value(4).toString();
        mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
        mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
        mainjson.insert("formula_no",formal.formulano);
        mainjson.insert("product_no",formal.productno);
        mainjson.insert("version",formal.version);//
        //获取参数数据，并组装成数组
        QJsonDocument jadoc = QJsonDocument::fromJson(formal.dataparm.toUtf8());
        dataArray = jadoc.array();
        mainjson.insert("data",dataArray);
        endJsonDoc.setObject(mainjson);
        //qDebug()<<"mainjson:"<<endJsonDoc.toJson();
        QNetworkRequest request;
        //QUrl url("http://127.0.0.1:8000/FormalVerify");
        QUrl url(m_IniFile->value("mesFormulaUrl").toString());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setUrl(url);
        accessManager->post(request, endJsonDoc.toJson());
        FormalVerifyLogs.processid=m_IniFile->value("lncProcessNum").toString();
        FormalVerifyLogs.equmentid=m_IniFile->value("LncDevCode").toString();
        FormalVerifyLogs.type=2;
        FormalVerifyLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        FormalVerifyLogs.postjson = QString(endJsonDoc.toJson());
        FormalVerifyLogs.retcode="";
        FormalVerifyLogs.retmsg="";
        mesTxtMsg("配方验证发出数据：\n"+QString(endJsonDoc.toJson()));
       postFormalVerifyTimeout(index);
    }

}

/*
请求：
{
 "process_id":"JRD",
 "equipment_id":"M1CL01JRJI0001",
 "bill_no":"CC3-21080001",
 "data": [
    {
     "param_code":"HCD001",
     "param_name":"Step#1 参数名称1",
     "param_unit":"",
     "param_upper":"",
     "param_value":"60",
     "param_lower":""
    },
    {
     "param_code":"HCD003",
     "param_name":"Step#2参数名称2",
     "param_unit":"",
     "param_upper":"",
     "param_value":"60",
     "param_lower":""
    }
  ]
}

 */

void MainWindow::postEPFormalVerifyTimeout()//设备参数校验
{

    postEPFormalVerifyOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;

    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postEPFormalVerifyOK);


    update_mes_link_state(postEPFormalVerifyOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(EqumentVerifyLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    if(postEPFormalVerifyOK)
    {
        EqumentVerifyLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(EqumentVerifyLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        EqumentVerifyLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(EqumentVerifyLogs);

        qDebug()<<"设备校验申请接口链接成功"<<postEPFormalVerifyOK;
        msg="设备参数校验接口链接成功，有数据返回！"+(postEPFormalVerifySucceed==true)?"设备校验成功！":"设备校验失败！";
        const_msg(msg);
        /*
        if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
            if(postEPFormalVerifySucceed==false)
            {
                lncCtrlValue=123456;
                lncCtrlValue2=654321;
            }
            else
            {
                lncCtrlValue=0;
                lncCtrlValue2=0;
            }
        }
        else
        {
            lncCtrlValue=0;
            lncCtrlValue2=0;
        }
        */
    }
    else
    {
        EqumentVerifyLogs.retcode="";
        EqumentVerifyLogs.retmsg="超时";
        EqumentVerifyLogs.respjson="";
        EqumentVerifyLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(EqumentVerifyLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        EqumentVerifyLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(EqumentVerifyLogs);

        qDebug()<<"设备校验申请失败或网络不可达"<<postEPFormalVerifyOK;
        EPFormalVerifyCnt=EPFormalVerifyCnt-1;
        if(EPFormalVerifyCnt<=0)
        {
         qDebug()<<"设备验证重连失败，超时！";
         msg="设备校验超时！";
         const_msg(msg);
         /*
         if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
             lncCtrlValue=123456;
             lncCtrlValue2=654321;
         }
         else
         {
             lncCtrlValue=0;
             lncCtrlValue2=0;
         }
         */
          if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
         QMessageBox::warning(this, "接口超时提示", "设备参数校验失败，超时！MES在线模式,LNC将不可加工！"
                , QMessageBox::Ok, QMessageBox::NoButton);
          }
          else
          {
              QMessageBox::warning(this, "接口超时提示", "设备参数校验失败，超时！"
                     , QMessageBox::Ok, QMessageBox::NoButton);
          }
        }
        else
        {
            qDebug()<<"再次发出设备验证请求"<<EPFormalVerifyCnt;
            msg="设备校验重连"+QString::number(EPFormalVerifyCnt);
            const_msg(msg);
            postEPFormalVerify();
        }
    }
}
void MainWindow::postEPFormalVerify()//设备参数校验
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QJsonArray dataArray;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postEPFormalVerifyFinish(QNetworkReply*)));

    //查询正在使用中的参数，并上发数据

    //先上传选中的配方数据，校验，然后再更加校验结果进行更新数据。
    QSqlQuery query;
    QString str="SELECT formulano,version,productno,productname,dataparm from localformaltable WHERE usestatus='使用中'";
    query.prepare(str);
    query.exec();
    while(query.next())//将读取到的配方数据结构
    {
        LocalFormal formal;
        formal.formulano=query.value(0).toString();
        formal.version=query.value(1).toString();
        formal.productno=query.value(2).toString();
        formal.productname=query.value(3).toString();
        formal.dataparm=query.value(4).toString();
        mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
        mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
        mainjson.insert("bill_no",m_IniFile->value("mesBillCode").toString());
        //获取参数数据，并组装成数组
        QJsonDocument jadoc = QJsonDocument::fromJson(formal.dataparm.toUtf8());
        dataArray = jadoc.array();
        mainjson.insert("data",dataArray);
        endJsonDoc.setObject(mainjson);
        //qDebug()<<"mainjson:"<<endJsonDoc.toJson();
        QNetworkRequest request;
        //QUrl url("http://127.0.0.1:8000/DevInfo");
        QUrl url(m_IniFile->value("mesDevInfoUrl").toString());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setUrl(url);
        accessManager->post(request, endJsonDoc.toJson());
        postEPFormalVerifyOK =false;
        EqumentVerifyLogs.processid=m_IniFile->value("lncProcessNum").toString();
        EqumentVerifyLogs.equmentid=m_IniFile->value("LncDevCode").toString();
        EqumentVerifyLogs.type=3;
        EqumentVerifyLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        EqumentVerifyLogs.postjson = QString(endJsonDoc.toJson());
        EqumentVerifyLogs.retcode="";
        EqumentVerifyLogs.retmsg="";

        mesTxtMsg("设备验证发出数据：\n"+QString(endJsonDoc.toJson()));

        postEPFormalVerifyTimeout();
    }
}

/*
请求：
{
"process_id":"JRD",
"equipment_id":"M1CL01JRJI0001"
}
返回：
{
"status_code":"0",
"message":"请求成功",
"bill_no":"CC3-21080001",
"bill_num":"1000"
}
*/
void MainWindow::postGetBillInfoTimeout()//获取工单
{
    postGetBillInfoOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postGetBillInfoOK);
     update_mes_link_state(postGetBillInfoOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(BillInfoLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    if(postGetBillInfoOK)
    {
        BillInfoLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(BillInfoLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        BillInfoLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(BillInfoLogs);

        qDebug()<<"获取工单申请接口链接成功"<<postGetBillInfoOK;
        msg="获取工单信息接口正常，有数据返回";
        const_msg(msg);

    }
    else
    {
        BillInfoLogs.retcode="";
        BillInfoLogs.retmsg="超时";
        BillInfoLogs.respjson="";
        BillInfoLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(BillInfoLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        BillInfoLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(BillInfoLogs);

        qDebug()<<"获取工单申请失败或网络不可达"<<postGetBillInfoOK;
        GetBillInfoCnt=GetBillInfoCnt-1;
        if(GetBillInfoCnt<=0)
        {
         qDebug()<<"获取工单重连不成功";
         msg="获取工单信息失败，超时！";
         const_msg(msg);
         QMessageBox::warning(this, "接口超时提示", "获取工单信息失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        }
        else
        {
            qDebug()<<"再次发出获取工单请求"<<GetBillInfoCnt;
            msg="获取工单申请重连"+QString::number(GetBillInfoCnt);
            const_msg(msg);
            postGetBillInfo();
        }
    }
}
void MainWindow::postGetBillInfo()//获取工单
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postGetBillInfoFinish(QNetworkReply*)));
    mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
    mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/BillInfo");
    QUrl url(m_IniFile->value("mesOrderUrl").toString());
    qDebug()<<m_IniFile->value("mesOrderUrl").toString();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
     accessManager->post(request, endJsonDoc.toJson());

     BillInfoLogs.processid=m_IniFile->value("lncProcessNum").toString();
     BillInfoLogs.equmentid=m_IniFile->value("LncDevCode").toString();
     BillInfoLogs.type=4;
     BillInfoLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
     BillInfoLogs.postjson = QString(endJsonDoc.toJson());
     BillInfoLogs.retcode="";
     BillInfoLogs.retmsg="";

    mesTxtMsg("获取工单请求发出数据：\n"+QString(endJsonDoc.toJson()));
    postGetBillInfoTimeout();

}


void MainWindow::postGetBillInfoListTimeout()//获取工单队列超时
{
    postGetBillInfoListOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postGetBillInfoListOK);
    update_mes_link_state(postGetBillInfoListOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(BillListLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    if(postGetBillInfoListOK)
    {
        BillListLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(BillListLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        BillListLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(BillListLogs);

        qDebug()<<"获取工单队列申请成功"<<postGetBillInfoListOK;
        msg="获取工单队列接口链接成功，有数据返回";
        const_msg(msg);
    }
    else
    {
        BillListLogs.retcode="";
        BillListLogs.retmsg="超时";
        BillListLogs.respjson="";
        BillListLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(BillListLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        BillListLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(BillListLogs);

        qDebug()<<"获取工单队列申请失败或网络不可达"<<postGetBillInfoListOK;
        GetBillInfoListCnt=GetBillInfoListCnt-1;
        if(GetBillInfoListCnt<=0)
        {
         qDebug()<<"获取工单队列重连不成功";
         msg="获取工单队列失败，超时！";
         const_msg(msg);
         QMessageBox::warning(this, "接口超时提示", "获取工单队列失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        // update_mes_link_state(postGetBillInfoListOK);
        }
        else
        {
            qDebug()<<"再次发出获取工单队列请求"<<GetBillInfoListCnt;
            msg="获取工单队列申请重连"+QString::number(GetBillInfoListCnt);
            const_msg(msg);
            postGetBillInfoList();
        }

    }
}
void MainWindow::postGetBillInfoList()//获取工单队列
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postGetBillInfoListFinish(QNetworkReply*)));
    mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
    mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/BillInfoList");
    QUrl url(m_IniFile->value("mesOrderListUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());
    BillListLogs.processid=m_IniFile->value("lncProcessNum").toString();
    BillListLogs.equmentid=m_IniFile->value("LncDevCode").toString();
    BillListLogs.type=5;
    BillListLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    BillListLogs.postjson = QString(endJsonDoc.toJson());
    BillListLogs.retcode="";
    BillListLogs.retmsg="";
    mesTxtMsg("获取工单队列请求发出数据：\n"+QString(endJsonDoc.toJson()));
    postGetBillInfoListTimeout();

}

/*
{
"process_id":"JRD",
"equipment_id":"M1CL01JRJI0001",
"material_code":"RMRMMZ05G012B004-001",
"number":"100"
}

*/

void MainWindow::postMaterialFeedingVerifityTimeout(LoadMaterial mat)//上料申请超时
{
    postMaterialFeedingVerifityOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postMaterialFeedingVerifityOK);
     update_mes_link_state(postMaterialFeedingVerifityOK);
    QString msg;
    if(postMaterialFeedingVerifityOK)
    {
        qDebug()<<"上料验证申请成功"<<postMaterialFeedingVerifityOK;
        msg="上料验证接口链接成功，有数据返回";
        const_msg(msg);
    }
    else
    {
        qDebug()<<"上料验证申请失败或网络不可达"<<postMaterialFeedingVerifityOK;
        MaterialFeedingCnt=MaterialFeedingCnt-1;
        if(MaterialFeedingCnt<=0)
        {
         qDebug()<<"上料申请重连不成功";
         msg="上料验证失败，超时！";
         const_msg(msg);
        // update_mes_link_state(postMaterialFeedingVerifityOK);
         QMessageBox::warning(this, "接口超时提示", "上料验证失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        }
        else
        {
            qDebug()<<"再次发上料验证请求"<<MaterialFeedingCnt;
            msg="上料验证申请重连"+QString::number(MaterialFeedingCnt);
            const_msg(msg);
            postMaterialFeedingVerifity(mat);
        }
    }
}
void MainWindow::postMaterialFeedingVerifity(LoadMaterial mat)//上料
{
  //查询对应物料编码的数据上发
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postMaterialFeedingVerifityFinish(QNetworkReply*)));
    mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
    mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
    mainjson.insert("material_code",mat.material_code);
    mainjson.insert("number",mat.number);
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/LoadMaterial");
    QUrl url(m_IniFile->value("mesLoadUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());
    mesTxtMsg("上料请求发出数据：\n"+QString(endJsonDoc.toJson()));
    postMaterialFeedingVerifityTimeout(mat);

}
/*
{
"process_id":"JRD",
"equipment_id":"M1CL01JRJI0001",
"material_code":"RMRMMZ05G012B004-001",
"id":"NO001"
}
*/
void MainWindow::postMaterialUnloadingVerifityTimeout(LoadMaterial mat)//卸料申请超时
{
    postMaterialUnloadingVerifityOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postMaterialUnloadingVerifityOK);
    update_mes_link_state(postMaterialUnloadingVerifityOK);
    QString msg;
    if(postMaterialUnloadingVerifityOK)
    {
        qDebug()<<"卸料验证申请成功"<<postMaterialUnloadingVerifityOK;
        msg="卸料验证申请接口链接成功，有数据返回";
        const_msg(msg);
        QString dlgTitle = "卸料验证成功消息框";
        QString strInfo = "卸料验证成功！";
        QMessageBox::information(this, dlgTitle, strInfo
               , QMessageBox::Ok, QMessageBox::NoButton);
    }
    else
    {
        qDebug()<<"卸料验证申请失败或网络不可达"<<postMaterialUnloadingVerifityOK;
        MaterialUnloadCnt=MaterialUnloadCnt-1;
        if(MaterialUnloadCnt<=0)
        {
        // update_mes_link_state(postMaterialUnloadingVerifityOK);
         qDebug()<<"卸料验证重连不成功";
         msg="卸料验证失败，超时！";
         const_msg(msg);
         QMessageBox::warning(this, "接口超时提示", "此次卸料验证失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        }
        else
        {
            qDebug()<<"再次卸料验证请求"<<MaterialUnloadCnt;
            msg="卸料验证申请重连"+QString::number(MaterialUnloadCnt);
            const_msg(msg);
            postMaterialUnloadingVerifity(mat);
        }
    }
}

void MainWindow::postMaterialUnloadingVerifity(LoadMaterial mat)//卸料
{
    //查询对应物料编码的数据上发
      QJsonObject mainjson;//要上传的json
      QJsonDocument endJsonDoc;
      QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
      connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postMaterialUnloadingVerifityFinish(QNetworkReply*)));
      mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
      mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
      mainjson.insert("material_code",mat.material_code);
      mainjson.insert("id",mat.id);
      endJsonDoc.setObject(mainjson);
      QNetworkRequest request;
      //QUrl url("http://127.0.0.1:8000/unLoadMaterial");
      QUrl url(m_IniFile->value("mesUnLoadUrl").toString());
      request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
      request.setUrl(url);
      accessManager->post(request, endJsonDoc.toJson());
      mesTxtMsg("卸料请求发出数据：\n"+QString(endJsonDoc.toJson()));
      postMaterialUnloadingVerifityTimeout(mat);
}
void MainWindow::postGetEPMaterialFeedingTimeout()//物料申请超时
{
    postGetEPMaterialFeedingOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    postGetEPMaterialFeedingOK=false;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postGetEPMaterialFeedingOK);
    update_mes_link_state(postGetEPMaterialFeedingOK);
    QString msg;
    if(postGetEPMaterialFeedingOK)
    {
        qDebug()<<"更新物料申请成功"<<postGetEPMaterialFeedingOK;
        msg="更新物料申请接口链接成功，有数据返回";
        const_msg(msg);
    }
    else
    {
        qDebug()<<"更新物料申请失败或网络不可达"<<postGetEPMaterialFeedingOK;
        GetEPMaterialFeedCnt=GetEPMaterialFeedCnt-1;
        if(GetEPMaterialFeedCnt<=0)
        {
         qDebug()<<"更新物料申请重连不成功";
         msg="更新物料申请失败，超时！";
         const_msg(msg);
        // update_mes_link_state(postGetEPMaterialFeedingOK);
         QMessageBox::warning(this, "接口超时提示", "更新物料申请失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        }
        else
        {
            qDebug()<<"再次更新物料请求"<<GetEPMaterialFeedCnt;
            msg="更新物料申请重连"+QString::number(GetEPMaterialFeedCnt);
            const_msg(msg);
            postGetEPMaterialFeeding();
        }
    }
}
void MainWindow::postGetEPMaterialFeeding()//获取物料
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postGetEPMaterialFeedingFinish(QNetworkReply*)));
    mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
    mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/Material");
    QUrl url(m_IniFile->value("mesMaterialUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());
    mesTxtMsg("获取物料列请求发出数据：\n"+QString(endJsonDoc.toJson()));
    postGetEPMaterialFeedingTimeout();
}


/*
{
"process_id":"ZHD",
"equipment_id":"M1CL01QHJJ0001",
"bar_code":"E200526174927391"
}
 */
void MainWindow::postCellIntoVerifityTimeout(ReportCellIn CellIn)//电芯进站超时
{
    //qDebug()<<"电芯进站上报超时";
    postCellIntoVerifityOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postCellIntoVerifityOK);
    update_mes_link_state(postCellIntoVerifityOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(CellInLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    if(postCellIntoVerifityOK)
    {
        CellInLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(CellInLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        CellInLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_cellinmeslog(CellInLogs);

        update_CellIn_Log(CellIn);
        qDebug()<<"进站日志申请成功"<<postCellIntoVerifityOK;
        msg="进站日志验证接口链接成功，有数据返回，"+(CellIntoVerifySucceed==true)?"进站校验成功！":"进站校验失败！";
        const_msg(msg);
        /*
        if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
            if(CellIntoVerifySucceed==false)
            {
                lncCtrlValue=123456;
                lncCtrlValue2=654321;
            }
            else
            {
                lncCtrlValue=0;
                lncCtrlValue2=0;
            }
        }
        else
        {
            lncCtrlValue=0;
            lncCtrlValue2=0;
        }
        */
    }
    else
    {
        CellInLogs.retcode="";
        CellInLogs.retmsg="超时";
        CellInLogs.respjson="";
        CellInLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(CellInLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        CellInLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_cellinmeslog(CellInLogs);

        qDebug()<<"进站日志申请失败或网络不可达"<<postCellIntoVerifityOK;
        CellIntolCnt=CellIntolCnt-1;
        if(CellIntolCnt<=0)
        {
         qDebug()<<"进站日志申请重连不成功";
         msg="进站日志验证失败，超时！";
         const_msg(msg);
        // update_mes_link_state(postCellIntoVerifityOK);
         /*
         if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
             lncCtrlValue=123456;
             lncCtrlValue2=654321;
         }
         else
         {
             lncCtrlValue=0;
             lncCtrlValue2=0;
         }
         */
         if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){
         QMessageBox::warning(this, "接口超时提示", "进站日志验证失败，超时！MES在线模式，LNC将处于不可工作模式！"
                , QMessageBox::Ok, QMessageBox::NoButton);
         }
         else
         {
             QMessageBox::warning(this, "接口超时提示", "进站日志验证失败，超时！"
                    , QMessageBox::Ok, QMessageBox::NoButton);
         }
        }
        else
        {
            qDebug()<<"进站日志验证请求"<<CellIntolCnt;
            msg="进站日志验证申请重连"+QString::number(CellIntolCnt);
            const_msg(msg);
            postCellIntoVerifity(CellIn);
        }
    }
}
void MainWindow::postCellIntoVerifity(ReportCellIn CellIn)//电芯进站
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postCellIntoVerifityFinish(QNetworkReply*)));
    mainjson.insert("process_id",CellIn.processid);
    mainjson.insert("equipment_id",CellIn.equmentid);
    mainjson.insert("bar_code",CellIn.barcode);
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/CellInto");
    QUrl url(m_IniFile->value("mesEntertUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());

    CellInLogs.barcode=CellIn.barcode;
    CellInLogs.equmentid=CellIn.equmentid;
    CellInLogs.processid=CellIn.processid;
    CellInLogs.postjson=QString(endJsonDoc.toJson());
    CellInLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    CellInLogs.retcode="";
    CellInLogs.retmsg="";
    CellInLogs.billno="";
    CellInLogs.billnum="";
    CellInLogs.special="";

    mesTxtMsg("电芯进站发出数据：\n"+QString(endJsonDoc.toJson()));
    postCellIntoVerifityTimeout(CellIn);
}
/*
{
"process_id":"JRD",
"equipment_id":"M1CL01JRJI0001",
"data": [{
  "bar_code":"E200526174927391",
  "bad_code":"NC-001-01",
  "number": 1
  },
  {
  "bar_code":"E200526174927391",
  "bad_code":"NC-001-02",
  "number": 1
  }
 ]
}

 */

void MainWindow::postSaveRejectRecordListTimeout(NGmsg nglog)//不良品超时
{
    qDebug()<<"不良履历上报超时";
    postSaveRejectRecordListOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postSaveRejectRecordListOK);
    update_mes_link_state(postSaveRejectRecordListOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(NGPLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    if(postSaveRejectRecordListOK)
    {
        NGPLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(NGPLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        NGPLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(NGPLogs);
        update_NG_Log(nglog);
        qDebug()<<"不良品上传日志申请成功"<<postSaveRejectRecordListOK;
        msg=nglog.barcode+"不良品日志验证接口链接成功，有数据返回";
        const_msg(msg);

    }
    else
    {
        NGPLogs.retcode="";
        NGPLogs.retmsg="超时";
        NGPLogs.respjson="";
        NGPLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(NGPLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        NGPLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_othermeslog(NGPLogs);

        qDebug()<<"不良品日志申请失败或网络不可达"<<postSaveRejectRecordListOK;
        RejectRecordCnt=RejectRecordCnt-1;
        if(RejectRecordCnt<=0)
        {
         qDebug()<<"不良品日志申请重连不成功";
         msg=nglog.barcode+"不良品日志验证失败，超时！";
         const_msg(msg);
        // update_mes_link_state(postCellIntoVerifityOK);

         QMessageBox::warning(this, "接口超时提示", "不良品日志验证失败，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        }
        else
        {
            qDebug()<<"进站日志验证请求"<<RejectRecordCnt;
            msg=nglog.barcode+"不良品日志验证申请重连"+QString::number(RejectRecordCnt);
            const_msg(msg);
            postSaveRejectRecordList(nglog);
        }
    }
}
void MainWindow::postSaveRejectRecordList(NGmsg nglog)//不良履历
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postSaveRejectRecordListFinish(QNetworkReply*)));
    mainjson.insert("process_id",nglog.processid);
    mainjson.insert("equipment_id",nglog.equmentid);
    QJsonArray dataArray;
    QJsonObject subobj;
    subobj.insert("bar_code",nglog.barcode);
    subobj.insert("bad_code",nglog.badcode);
    subobj.insert("number",nglog.badnum);
    dataArray.append(subobj);
    mainjson.insert("data",dataArray);
    endJsonDoc.setObject(mainjson);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/CellInto");
    QUrl url(m_IniFile->value("mesNotGoodUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());
            NGPLogs.processid=m_IniFile->value("lncProcessNum").toString();
            NGPLogs.equmentid=m_IniFile->value("LncDevCode").toString();
            NGPLogs.type=6;
            NGPLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
            NGPLogs.postjson = QString(endJsonDoc.toJson());
            NGPLogs.retcode="";
            NGPLogs.retmsg="";
    mesTxtMsg("电芯不良品发出数据：\n"+QString(endJsonDoc.toJson()));
    postSaveRejectRecordListTimeout(nglog);
}

/*
{
 "process_id":"RYD",
 "equipment_id":"M1CL01RYJI0001",
 "bill_no":"CC3-21080001",
 "creator":"system",
 "entityList": [
    {
     "bar_code":"E200526193516377",
     "number": 1,
     "techList":[
        {
         "param_code":"SC-DMH001",
         "act_value":"700"
        },
        {
         "param_code":"SC-DMH002",
         "act_value":"800"
        }
      ]
    },
    {
    }
  ]
}
*/

void MainWindow::postProductRecordListTimeout(ReportCellOut CellOut)//出站超时
{
    //qDebug()<<"出站生产履历上报超时";
    postProductRecordListOK=false;
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    do {
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>m_IniFile->value("MesTimeOut").toInt())
            break;
       }
    while(!postProductRecordListOK);
    update_mes_link_state(postProductRecordListOK);
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(CellOutLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    if(postProductRecordListOK)
    {
        CellOutLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(CellOutLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        CellOutLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_celloutmeslog(CellOutLogs);

        update_CellOut_Log(CellOut);
        qDebug()<<"出站日志申请成功"<<postProductRecordListOK;
        msg="出站履历日志验证接口链接成功，有数据返回";
        const_msg(msg);

    }
    else
    {
        CellOutLogs.retcode="";
        CellOutLogs.retmsg="超时";
        CellOutLogs.respjson="";
        CellOutLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        PostendTime=QDateTime::fromString(CellOutLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
        CellOutLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
        add_one_celloutmeslog(CellOutLogs);

        qDebug()<<"出站日志申请失败或网络不可达"<<postProductRecordListOK;
        ProductRecordCnt=ProductRecordCnt-1;
        if(ProductRecordCnt<=0)
        {
         qDebug()<<"出站日志申请重连不成功";
         msg="出站履历日志验证失败，超时！";
         const_msg(msg);
        // update_mes_link_state(postCellIntoVerifityOK);
         QMessageBox::warning(this, "接口超时提示", "出站履历日志验证，超时！"
                , QMessageBox::Ok, QMessageBox::NoButton);
        }
        else
        {
            qDebug()<<"出站日志验证请求"<<ProductRecordCnt;
            msg="出站履历日志验证申请重连"+QString::number(ProductRecordCnt);
            const_msg(msg);
            postProductRecordList(CellOut);
        }
    }
}
void MainWindow::postProductRecordList(ReportCellOut CellOut)//出站，生产履历
{

    ProductMsg ProdMsg;
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postProductRecordListFinish(QNetworkReply*)));
    ProdMsg.processid =CellOut.processid;
    ProdMsg.equmentid=CellOut.equmentid;
    ProdMsg.billno=CellOut.billno;
    ProdMsg.barcode=CellOut.barcode;
    ProdMsg.creator=logonUser;
    ProdMsg.num="1";
    ProdMsg.param1="SC-XDJ006";//铣钉深度
    ProdMsg.value1=CellOut.param1;
    ProdMsg.param2="SC-XDJ003";//铣孔半径
    ProdMsg.value2=CellOut.param2;
    ProdMsg.param3="SC-XDJ002";//主轴转速
    ProdMsg.value3=CellOut.param3;
    ProdMsg.param4="SC-XDJ007";//吸尘风速
    ProdMsg.value4=CellOut.param4;
    ProdMsg.param5="SC-XDJ004";//刀具使用次数
    ProdMsg.value5=CellOut.param5;
    ProdMsg.param6="SC-XDJ001";//表面平整度
    ProdMsg.value6=CellOut.param6;
    ProdMsg.param7="SC-XDJ005";//吸尘风压
    ProdMsg.value7=CellOut.param7;
    ProdMsg.param8="SC-XDJ008";//探测Z值
    ProdMsg.value8=CellOut.param8;

     mainjson.insert("process_id",ProdMsg.processid);
     mainjson.insert("equipment_id",ProdMsg.equmentid);
     mainjson.insert("bill_no",ProdMsg.billno);
     mainjson.insert("creator",ProdMsg.creator);

    QJsonArray subParam;//参数数组
    QJsonObject subsubjson;//参数

    subsubjson.insert("param_code",ProdMsg.param1);
    subsubjson.insert("act_value",ProdMsg.value1);
    subParam.append(subsubjson);
    subsubjson.empty();
    subsubjson.insert("param_code",ProdMsg.param2);
    subsubjson.insert("act_value",ProdMsg.value2);
    subParam.append(subsubjson);
    subsubjson.empty();
    subsubjson.insert("param_code",ProdMsg.param3);
    subsubjson.insert("act_value",ProdMsg.value3);
    subParam.append(subsubjson);
    subsubjson.empty();
    subsubjson.insert("param_code",ProdMsg.param4);
    subsubjson.insert("act_value",ProdMsg.value4);
    subParam.append(subsubjson);
    subsubjson.empty();
    if(ui->chbToolUsed->checkState()==Qt::Checked)
    {
    subsubjson.insert("param_code",ProdMsg.param5);
    subsubjson.insert("act_value",ProdMsg.value5);
    subParam.append(subsubjson);
    subsubjson.empty();
    }
    if(ui->chbPlane->checkState()==Qt::Checked)
    {
    subsubjson.insert("param_code",ProdMsg.param6);
    subsubjson.insert("act_value",ProdMsg.value6);
    subParam.append(subsubjson);
    subsubjson.empty();
    }
    if(ui->chbAirPressure->checkState()==Qt::Checked)
    {
    subsubjson.insert("param_code",ProdMsg.param7);
    subsubjson.insert("act_value",ProdMsg.value7);
    subParam.append(subsubjson);
    subsubjson.empty();
    }
    if(ui->chbProbeBazeZ->checkState()==Qt::Checked)
    {
    subsubjson.insert("param_code",ProdMsg.param8);
    subsubjson.insert("act_value",ProdMsg.value8);
    subParam.append(subsubjson);
    subsubjson.empty();
    }
    QJsonObject subjson;//参数
    subjson.insert("bar_code",ProdMsg.barcode);
    subjson.insert("number",ProdMsg.num);
    subjson.insert("techList",subParam);

   // QJsonObject emptyjson;//空参数
    QJsonArray dataParam;//参数数组
    dataParam.append(subjson);
   // dataParam.append(emptyjson);
    mainjson.insert("entityList",dataParam);

    endJsonDoc.setObject(mainjson);

    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/CellInto");
    QUrl url(m_IniFile->value("mesExitUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    accessManager->post(request, endJsonDoc.toJson());
    mesTxtMsg("电芯出站发出数据：\n"+QString(endJsonDoc.toJson()));

    CellOutLogs.barcode=ProdMsg.barcode;
    CellOutLogs.equmentid=ProdMsg.equmentid;
    CellOutLogs.processid=ProdMsg.processid;
    CellOutLogs.barcode=ProdMsg.barcode;
    CellOutLogs.billno=ProdMsg.billno;
    CellOutLogs.opreater=ProdMsg.creator;
    CellOutLogs.productnum=m_IniFile->value("mesBillNum").toString();
    CellOutLogs.materialdec="0";
    CellOutLogs.paramone=ProdMsg.value1;
    CellOutLogs.paramtwo=ProdMsg.value2;
    CellOutLogs.paramthree=ProdMsg.value3;
    CellOutLogs.paramfour=ProdMsg.value4;
    CellOutLogs.paramfive=ProdMsg.value5;
    CellOutLogs.paramsix=ProdMsg.value6;
    CellOutLogs.reqtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    CellOutLogs.postjson=QString(endJsonDoc.toJson());
    CellOutLogs.respjson="";
    CellOutLogs.resptime="";
    CellOutLogs.retmsg="";
    CellOutLogs.retcode="";
    CellOutLogs.materialremain="";
    CellOutLogs.steps="";
    CellOutLogs.usedtime="";

    postProductRecordListTimeout(CellOut);


}




//配方申请完成响应
void MainWindow::postFormalFinish(QNetworkReply *reply)//工艺申请上传,获取配方
{
    QString msg;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
        msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
      //  qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"申请配方"<<doucment;
    mesTxtMsg("配方申请返回数据：\n"+QString(bytes));
    LocalFormalLogs.respjson=QString(bytes);//返回信息
   LocalFormal formal;

   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {
       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
           QJsonObject object = doucment.object();
           QJsonArray data;//配方数组
           QJsonArray dataParam;//对应配方的参数数组
           QJsonObject dataobj;
           QJsonValue jsonvalue;
           QJsonDocument JsonDoc;
           int datacnt=0;
           if (object.contains("status_code")) {
               postFormalOK=true;
               update_mes_link_state(postFormalOK);
               LocalFormalLogs.retcode=object.value("status_code").toString();
               LocalFormalLogs.retmsg=object.value("message").toString();

               if(object.value("status_code").toString()=="0") {//配方返回，且验证通过
                   //postFormalOK=true;
                   qDebug()<<"配方申请返回成功";
                   // 包含指定的 key
                   if (object.contains("data")) {
                       // 获取指定 key 对应的 value
                       jsonvalue = object.value("data");
                       data=jsonvalue.toArray();
                       if(data.count()>0)//有效数据
                      {
                       for(datacnt=0;datacnt<data.count();datacnt++)
                       {
                         //   qDebug() << "data: " << data;
                            dataobj=data.at(datacnt).toObject();
                            jsonvalue = dataobj.value("formula_no");
                            formal.formulano=jsonvalue.toString();
                            jsonvalue = dataobj.value("product_no");
                            formal.productno=jsonvalue.toString();
                            jsonvalue = dataobj.value("product_name");
                            formal.productname=jsonvalue.toString();
                            jsonvalue = dataobj.value("version");
                            formal.version=jsonvalue.toString();
                            jsonvalue = dataobj.value("dataParm");
                            dataParam =jsonvalue.toArray();
                            JsonDoc.setArray(dataParam);
                            formal.dataparm=QString::fromUtf8(JsonDoc.toJson().data());
                            QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                            formal.dateget=dtime;
                            formal.opreate="执行";
                            add_one_formal(formal);//新增一条本地配方
                       }
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
                    }//有效数据
                    else
                       {
                           QString dlgTitle = "配方申请失败消息框";
                           QString strInfo = "配方申请返回成功，但配方中无配方数据";
                           QMessageBox::warning(this, dlgTitle, strInfo
                                  , QMessageBox::Ok, QMessageBox::NoButton);
                       }
            }
          }//==0
               else//配方申请失败
               {
                   QString dlgTitle = "配方申请失败消息框";
                   QString strInfo = object.value("message").toString();
                   QMessageBox::warning(this, dlgTitle, strInfo
                          , QMessageBox::Ok, QMessageBox::NoButton);
               }
         }
        }//is an object
   }//json good

}

void MainWindow::postFormalVerifyFinish(QNetworkReply *reply)//配方校验
{
    QString msg;
    postFormalVerifySucceed=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);

    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    mesTxtMsg("配方验证返回数据：\n"+QString(bytes));
    FormalVerifyLogs.respjson=QString(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"配方验证"<<doucment;

    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject()) // JSON 文档为对象
          {
                // 转化为对象
                QJsonObject object = doucment.object();

                FormalVerifyLogs.retcode=object.value("status_code").toString();
                FormalVerifyLogs.retmsg=object.value("message").toString();

                if (object.contains("status_code")) {
                    postFormalVerifyOK=true;
                    update_mes_link_state(postFormalVerifyOK);
                    if(object.value("status_code").toString()=="0") {
                        //postFormalVerifyOK=true;
                        qDebug()<<"配方验证返回成功";
                        postFormalVerifySucceed=true;
                       // lncCtrlValue=0;
                      //  lncCtrlValue2=0;

                    }

                 else
                    {
                        postFormalVerifySucceed=false;

                        QString dlgTitle = "配方验证失败消息框";
                        QString strInfo = object.value("message").toString()+"！参数不被更新！不可进行加工！";
                        QMessageBox::warning(this, dlgTitle, strInfo
                               , QMessageBox::Ok, QMessageBox::NoButton);
                    }
                }
           }
     }
}

void MainWindow::postEPFormalVerifyFinish(QNetworkReply *reply)//设备参数校验
{
    QString msg;
    postEPFormalVerifyOK=false;
    postEPFormalVerifySucceed=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);

    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    mesTxtMsg("设备验证返回数据：\n"+QString(bytes));
    EqumentVerifyLogs.respjson=QString(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"配方验证"<<doucment;
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject()) // JSON 文档为对象
          {
                // 转化为对象
                QJsonObject object = doucment.object();
                        EqumentVerifyLogs.retcode=object.value("status_code").toString();
                        EqumentVerifyLogs.retmsg=object.value("message").toString();
                if (object.contains("status_code")) {

                    postEPFormalVerifyOK=true;
                    update_mes_link_state(postEPFormalVerifyOK);
                    if(object.value("status_code").toString()=="0") {
                       // postEPFormalVerifyOK=true;
                        postEPFormalVerifySucceed=true;
                        qDebug()<<"设备验证返回成功";
                       // lncCtrlValue=0;
                    }
                 else{
                        postEPFormalVerifySucceed=false;
                        QString dlgTitle = "设备校验失败消息框";
                        QString strInfo = object.value("message").toString() + "！LNC设备将不可工作！";
                        QMessageBox::warning(this, dlgTitle, strInfo
                               , QMessageBox::Ok, QMessageBox::NoButton);

                        msg=object.value("message").toString();
                        const_msg(msg);
                      //  lncCtrlValue=1;
                    }
                }
           }
     }
}

void MainWindow::postGetBillInfoFinish(QNetworkReply *reply)//获取工单
{
    QString msg;
    postGetBillInfoOK=false;
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
    mesTxtMsg("工单返回数据：\n"+QString(bytes));
    BillInfoLogs.respjson=QString(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject()) // JSON 文档为对象
          {
                // 转化为对象
                QJsonObject object = doucment.object();

                BillInfoLogs.retcode=object.value("status_code").toString();
                BillInfoLogs.retmsg=object.value("message").toString();

                if (object.contains("status_code")) {
                    postGetBillInfoOK=true;
                    update_mes_link_state(postGetBillInfoOK);
                   // qDebug()<<"工单返回成功"<<object.value("status_code")<<object.value("status_code").toInt();
                    if(object.value("status_code").toString()=="0") {
                      //  postGetBillInfoOK=true;
                        qDebug()<<"工单返回成功";
                        m_IniFile->setValue("MesBillInfo",object.value("bill_no").toString());
                        m_IniFile->setValue("MesBillNum",object.value("bill_num").toString());
                        update_billInfo();
                    }
                    else
                     {
                        QString dlgTitle = "获取工单失败消息框";
                        QString strInfo = object.value("message").toString();;
                        QMessageBox::warning(this, dlgTitle, strInfo
                               , QMessageBox::Ok, QMessageBox::NoButton);
                    }
                }
           }
     }
}
void MainWindow::postGetBillInfoListFinish(QNetworkReply *reply)//获取工单队列
{

    QString msg;
    postGetBillInfoListOK=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"申请配方"<<doucment;
    mesTxtMsg("工单队列申请返回数据：\n"+QString(bytes));
    BillListLogs.respjson=QString(bytes);
   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {

       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();
            QJsonArray data;//配方数组
            QJsonObject dataobj;
            QJsonValue jsonvalue;
            int datacnt=0;
            BillListLogs.retcode=object.value("status_code").toString();
            BillListLogs.retmsg=object.value("message").toString();
           if (object.contains("status_code")) {
               postGetBillInfoListOK=true;
               update_mes_link_state(postGetBillInfoListOK);
               if(object.value("status_code").toString()=="0") {
                 //  postGetBillInfoListOK=true;
                   qDebug()<<"工单队列申请返回成功";
                   QJsonDocument JsonDoc;
                   JsonDoc.setObject(object);
                   db_update_oderlist(QString::fromUtf8(JsonDoc.toJson().data()));

                // 包含指定的 key
                if (object.contains("data")) {
                    update_record_orderlist();
           }
        }//==0
               else
               {
                   QString dlgTitle = "获取工单队列失败消息框";
                   QString strInfo = object.value("message").toString();;
                   QMessageBox::warning(this, dlgTitle, strInfo
                          , QMessageBox::Ok, QMessageBox::NoButton);
               }
      }
     }//is an object
   }//json good
}

void MainWindow::postMaterialFeedingVerifityFinish(QNetworkReply *reply)//上料
{
    QString msg;
    postMaterialFeedingVerifityOK=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"申请配方"<<doucment;
    mesTxtMsg("上料申请返回数据：\n"+QString(bytes));
   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {

       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();
            Material mat;
           if (object.contains("status_code")) {
               postMaterialFeedingVerifityOK=true;
               update_mes_link_state(postMaterialFeedingVerifityOK);
               if(object.value("status_code").toString()=="0") {

                   qDebug()<<"上料申请返回成功";
                   mat.id=object.value("id").toString();
                   mat.material_code=object.value("material_code").toString();
                   mat.material_PN=object.value("material_PN").toString();
                   mat.material_name=object.value("material_name").toString();
                   mat.unit=object.value("unit").toString();
                   mat.number=object.value("number").toString();
                   mat.surplus_num=object.value("surplus_num").toString();
                   mat.feeding_time=object.value("feeding_time").toString();
                   mat.bill_no=object.value("bill_no").toString();
                   mat.bill_num=object.value("bill_num").toString();
                   update_upload_material(mat);
        }//==0
               else
               {
                   QString dlgTitle = "上料验证失败消息框";
                   QString strInfo = object.value("message").toString();;
                   QMessageBox::warning(this, dlgTitle, strInfo
                          , QMessageBox::Ok, QMessageBox::NoButton);
               }
      }
     }//is an object
   }//json good
}
void MainWindow::postMaterialUnloadingVerifityFinish(QNetworkReply *reply)//卸料
{
    QString msg;
    postMaterialUnloadingVerifityOK=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"申请配方"<<doucment;
    mesTxtMsg("卸料申请返回数据：\n"+QString(bytes));
   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {

       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();
            Material mat;
           if (object.contains("status_code")) {
               postMaterialUnloadingVerifityOK=true;
               update_mes_link_state(postMaterialUnloadingVerifityOK);
               if(object.value("status_code").toString()=="0") {
                   //postMaterialUnloadingVerifityOK=true;
                   qDebug()<<"卸料申请返回成功";
                   mat.surplus_num=object.value("surplus_num").toString();
                   mat.unit=object.value("unit").toString();
                   update_unload_material(mat);
        }//==0
               else
               {

                    QString dlgTitle = "卸料验证失败消息框";
                    QString strInfo = object.value("message").toString();;
                    QMessageBox::warning(this, dlgTitle, strInfo
                              , QMessageBox::Ok, QMessageBox::NoButton);

               }
      }
     }//is an object
   }//json good
}
void MainWindow::postGetEPMaterialFeedingFinish(QNetworkReply *reply)//获取物料
{
    QString msg;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"申请配方"<<doucment;
    mesTxtMsg("获取物料申请返回数据：\n"+QString(bytes));
   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {
       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();
            QJsonArray data;//配方数组
            QJsonObject dataobj;
            QJsonValue jsonvalue;
            int datacnt=0;
           if (object.contains("status_code")) {
               postGetEPMaterialFeedingOK=true;
               update_mes_link_state(postGetEPMaterialFeedingOK);
               if(object.value("status_code").toString()=="0") {
                 //  postGetEPMaterialFeedingOK=true;
                   qDebug()<<"获取物料申请返回成功";
                   QJsonDocument JsonDoc;
                   JsonDoc.setObject(object);
                    db_update_matirial(QString::fromUtf8(JsonDoc.toJson().data()));
            // 包含指定的 key
             //       qDebug()<<"物料申请返回data个数"<<object.value("data").toArray().count();
            if ((object.contains("data")) && (object.value("data").toArray().count()>0)) {//有物料数据将数据送到表中
                   //先将数据送入队列表格
                QSqlQuery query;
                QString prepare;
                prepare="SELECT matirial FROM localmatirialorderlist WHERE id=1";
                qDebug()<<prepare;
                query.prepare(prepare);
                query.exec();
                if(query.next())
                {
                    QString str=query.value(0).toString();
                    QJsonDocument jadoc = QJsonDocument::fromJson(str.toUtf8());
                    QJsonObject object=jadoc.object();
                    QJsonArray dataArray = object.value("data").toArray();
                    int pcnt=0;
                    QJsonObject Param;
                    Material dbmat;
                    //删除材料表
                    QSqlQuery sec_query;
                    sec_query.prepare("delete from materiallist");
                    sec_query.exec();
                    qDebug()<<"删除物料列表"<<dataArray.count();
                    mesmatlistmodel->removeRows(0,mesmatlistmodel->rowCount()); //删除所有行
                    for(pcnt=0;pcnt<dataArray.count();pcnt++)//添加物料列表
                    {
                        Param=dataArray.at(pcnt).toObject();
                        dbmat.id=Param.value("id").toString();
                        dbmat.material_code=Param.value("material_code").toString();
                        dbmat.material_PN=Param.value("material_PN").toString();
                        dbmat.material_name=Param.value("material_name").toString();
                        dbmat.number=Param.value("number").toString();
                        dbmat.surplus_num=Param.value("surplus_num").toString();
                        dbmat.unit=Param.value("unit").toString();
                        dbmat.feeding_time=Param.value("feeding_time").toString();
                        dbmat.loadopreat="上料";
                        dbmat.opreat="卸料";
                        add_one_record_material(dbmat);
                    }
                }
               }
            else//无物料数据
             {
                 QString dlgTitle = "获取物料失败消息框";
                 QString strInfo = "物料申请返回成功，但无物料数据";
                 QMessageBox::warning(this, dlgTitle, strInfo
                        , QMessageBox::Ok, QMessageBox::NoButton);
             }
            }//==0
               else
               {

                       QString dlgTitle = "获取物料失败消息框";
                       QString strInfo = object.value("message").toString();;
                       QMessageBox::warning(this, dlgTitle, strInfo
                              , QMessageBox::Ok, QMessageBox::NoButton);

               }
          }
        }//is an object
   }//json good
}


void MainWindow::postCellIntoVerifityFinish(QNetworkReply *reply)//电芯进站
{
    QString msg;
    postCellIntoVerifityOK=false;
    CellIntoVerifySucceed=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);

    mesTxtMsg("进站日志申请返回数据：\n"+QString(bytes));
    CellInLogs.respjson=QString(bytes);
   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {

       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();

           if (object.contains("status_code")) {
                postCellIntoVerifityOK=true;
                update_mes_link_state(postCellIntoVerifityOK);
                qDebug()<<"收到电芯进站返回数据"<<object.value("status_code")<<object.value("status_code").toInt()<<object.value("status_code").toString();

                CellInLogs.billno=object.value("bill_no").toString();
                CellInLogs.billnum=object.value("bill_num").toString();
                CellInLogs.special=object.value("spec_value").toString();
                CellInLogs.retcode=object.value("status_code").toString();
                CellInLogs.retmsg=object.value("message").toString();

               if(object.value("status_code").toString()=="0") {
                   qDebug()<<"进站日志申请返回成功";
                   CellIntoVerifySucceed=true;
                  // lncCtrlValue=0;
                  // lncCtrlValue2=0;
                   //插入进站日志一条信息
                   Entermsg cellIn;
                  // QRegExp det(":\\S*");
                  // QRegExp num(".*:");
                  // det.indexIn(ui->lbScanCode->text());
                  // num.indexIn(ui->lbScanCode->text());
                  // cellIn.barcode= num.cap(0).left(num.cap(0).size()-1);
                   cellIn.barcode = CellInUsed.barcode;
                   cellIn.ordernum = object.value("bill_no").toString();
                   cellIn.result = object.value("message").toString();
                  // cellIn.detail=det.cap(0).right(det.cap(0).size()-1);
                   cellIn.detail=CellInUsed.result;
                   qDebug()<<cellIn.barcode<<cellIn.detail;
                  // QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                   cellIn.datetime=CellInUsed.date;
                   add_one_cellin(cellIn);
                   //更新显示
               /*    QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
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
                   */
        }//==0
        else
           {
                   qDebug()<<"进站日志返回错误信息";
                   CellIntoVerifySucceed=false;
                  // lncCtrlValue=123456;
                  // lncCtrlValue2=654321;
                   Entermsg cellIn;
                   cellIn.barcode = CellInUsed.barcode;
                   cellIn.ordernum =CellInUsed.ordernum;
                   cellIn.result = object.value("message").toString();
                   cellIn.detail=CellInUsed.result;
                   qDebug()<<cellIn.barcode<<cellIn.detail;

                   cellIn.datetime=CellInUsed.date;
                   add_one_cellin(cellIn);
                   //更新显示
                 /*  QString starttime=QDate::currentDate().toString("yyyy-MM-dd")+" 00:00:00";
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
                   */

                       QString dlgTitle = "电芯进站认证失败消息框";
                       QString strInfo = object.value("message").toString()+"！进站验证不通过，不可进站加工！";
                       QMessageBox::warning(this, dlgTitle, strInfo
                              , QMessageBox::Ok, QMessageBox::NoButton);

           }
      }
     }//is an object
   }//json good
}
void MainWindow::postSaveRejectRecordListFinish(QNetworkReply *reply)//不良履历
{
    QString msg;
    postSaveRejectRecordListOK=false;
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);

    mesTxtMsg("不良品日志申请返回数据：\n"+QString(bytes));
    NGPLogs.respjson=QString(bytes);
   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {

       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();

           if (object.contains("status_code")) {
                postSaveRejectRecordListOK=true;
                update_mes_link_state(postSaveRejectRecordListOK);
                NGPLogs.retcode=object.value("status_code").toString();
                NGPLogs.retmsg=object.value("message").toString();
                qDebug()<<"收到不良品上报返回数据"<<object.value("status_code")<<object.value("status_code").toInt()<<object.value("status_code").toString();
               if(object.value("status_code").toString()=="0") {
                   qDebug()<<"不良品日志申请返回成功";
                   //插入进站日志一条信息
                   msg=nglogUsed.barcode+"不良品上传,"+object.value("message").toString();
                   const_msg(msg);

        }//==0
        else
           {

                   qDebug()<<"上传不良品日志返回错误信息";
                   msg=nglogUsed.barcode+"不良品上传,"+object.value("message").toString();
                   const_msg(msg);

           }
      }
     }//is an object
   }//json good
}

void MainWindow::postProductRecordListFinish(QNetworkReply *reply)//出站，生产履历
{
    QString msg;
    postProductRecordListOK=false;
    qDebug()<<"******进入出站响应钩子函数****";
    //1、获取响应状态码
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (variant.isValid()){
      // qDebug() <<"获取响应状态码"<< variant.toInt();
      //  msg="获取响应状态码:"+QString::number(variant.toInt(),10)+"......";
    }
    //2、获取响应头
    variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
       {
       // qDebug() <<"获取响应头"<< variant.toString();
     //   msg="获取响应头:"+ variant.toString() +"......";
       }
  //  Show_Message(msg);
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);

    mesTxtMsg("出站履历日志申请返回数据：\n"+QString(bytes));

    CellOutLogs.respjson=QString(bytes);

   if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
   {

       if (doucment.isObject()) // JSON 文档为对象
       {
           // 转化为对象
            QJsonObject object = doucment.object();

           if (object.contains("status_code")) {
                postProductRecordListOK=true;
                update_mes_link_state(postProductRecordListOK);
                CellOutLogs.retcode=object.value("status_code").toString();
                CellOutLogs.retmsg=object.value("message").toString();

                qDebug()<<"收到电芯出站返回数据"<<object.value("status_code")<<object.value("status_code").toInt()<<object.value("status_code").toString();
               if(object.value("status_code").toString()=="0") {
                   qDebug()<<"出站日志申请返回成功";
                   //插入出站日志一条信息
                   QJsonArray dataArray = object.value("resultList").toArray();
                   int pcnt=0;
                    QJsonObject Param;
                    for(pcnt=0;pcnt<dataArray.count();pcnt++)//添加物料列表
                    {
                        Param=dataArray.at(pcnt).toObject();
                        Exitmsg cellOut;
                        cellOut.barcode=CellOutUsed.barcode;
                        cellOut.ordernum=CellOutUsed.billno;
                        cellOut.result=(Param.value("listStatus_code").toString()=="0")?"OK":"NG";
                        cellOut.ngreason=Param.value("listMessage").toString();
                        cellOut.detail=Param.value("ngList").toString();

                        QJsonArray nglistArray =Param.value("ngList").toArray();
                        int ngcnt=0;
                        QJsonObject  ngobj;
                        for(ngcnt=0;ngcnt<nglistArray.count();ngcnt++)
                        {
                            ngobj=nglistArray.at(ngcnt).toObject();
                            if(ngcnt==0){
                                CellOutLogs.ngcodeone=ngobj.value("bad_code").toString();
                                CellOutLogs.ngdetailone=ngobj.value("bad_name").toString();
                                }
                            if(ngcnt==1){
                                CellOutLogs.ngcodetwo=ngobj.value("bad_code").toString();
                                CellOutLogs.ngdetailtwo=ngobj.value("bad_name").toString();
                                }
                        }

                        cellOut.datetime=CellOutUsed.date;
                        add_one_cellout(cellOut);
                    }



        }//==0
        else
           {

                   QJsonArray dataArray = object.value("resultList").toArray();
                   int pcnt=0;
                    QJsonObject Param;
                    for(pcnt=0;pcnt<dataArray.count();pcnt++)//
                    {
                        Param=dataArray.at(pcnt).toObject();
                        QJsonArray nglistArray =Param.value("ngList").toArray();
                        int ngcnt=0;
                        QJsonObject  ngobj;
                        for(ngcnt=0;ngcnt<nglistArray.count();ngcnt++)
                        {
                            ngobj=nglistArray.at(ngcnt).toObject();
                            if(ngcnt==0){
                                CellOutLogs.ngcodeone=ngobj.value("bad_code").toString();
                                CellOutLogs.ngdetailone=ngobj.value("bad_name").toString();
                                }
                            if(ngcnt==1){
                                CellOutLogs.ngcodetwo=ngobj.value("bad_code").toString();
                                CellOutLogs.ngdetailtwo=ngobj.value("bad_name").toString();
                                }
                        }
                    }




                   qDebug()<<"出站日志返回错误信息";
                   Exitmsg cellOut;
                   cellOut.barcode=CellOutUsed.barcode;
                   cellOut.ordernum=CellOutUsed.billno;
                   cellOut.result=object.value("message").toString();
                   cellOut.ngreason=object.value("message").toString();
                   cellOut.detail=object.value("message").toString();
                   cellOut.datetime=CellOutUsed.date;
                   add_one_cellout(cellOut);

                       QString dlgTitle = "电芯出站履历认证失败消息框";
                       QString strInfo = object.value("message").toString();;
                       QMessageBox::warning(this, dlgTitle, strInfo
                              , QMessageBox::Ok, QMessageBox::NoButton);

           }
      }
     }//is an object
   }//json good
}
