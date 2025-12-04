#include "mainwindow.h"
#include<QSqlQuery>
#include "ui_mainwindow.h"
#include<QMessageBox>
#include"qreplytimeout.h"
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

#if 0
void MainWindow::postEPFormalVerifyTimeout()//设备参数校验
{
   // postFormalOK=false;
    postEPFormalVerifyOK=false;
    update_mes_link_state(postEPFormalVerifyOK);
    QDateTime PoststartTime=QDateTime::fromString(EqumentVerifyLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    QString msg;
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

         qDebug()<<"设备验证重连不成功";
          msg="设备校验不成功";
         const_msg(msg);
        }
        else
        {
            qDebug()<<"再次发出设备验证请求"<<EPFormalVerifyCnt;
            msg="设备校验重连"+QString::number(EPFormalVerifyCnt);
            const_msg(msg);
/*
            QTimer timer;
            timer.setInterval(50);  // 设置超时时间 50 ms
            timer.setSingleShot(true);  // 单次触发
            connect(&timer, SIGNAL(timeout()), this, SLOT(postEPFormalVerify()));
            //timer.start();
            QEventLoop loop;
            connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            timer.start();
            loop.exec();
*/
            postEPFormalVerify();
        }
        qDebug()<<"设备参数校验超时";

}
void MainWindow::postEPFormalVerify()//设备参数校验
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;
    QJsonArray dataArray;

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
        postEPFormalVerifyOK =false;
        EqumentVerifyLogs.processid=m_IniFile->value("lncProcessNum").toString();
        EqumentVerifyLogs.equmentid=m_IniFile->value("LncDevCode").toString();
        EqumentVerifyLogs.type=3;
        EqumentVerifyLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        EqumentVerifyLogs.postjson = QString(endJsonDoc.toJson());
        EqumentVerifyLogs.retcode="";
        EqumentVerifyLogs.retmsg="";
    }

    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);

   //  connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postEPFormalVerifyFinish(QNetworkReply*)));
    QNetworkRequest request;

    QUrl url(m_IniFile->value("mesDevInfoUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    QNetworkReply *pReply = accessManager->post(request, endJsonDoc.toJson());

    mesTxtMsg("设备验证发出数据：\n"+QString(endJsonDoc.toJson()));

    QReplyTimeout *pTimeout = new QReplyTimeout(pReply, m_IniFile->value("MesTimeOut").toInt());

     connect(pTimeout, SIGNAL(timeout()), this, SLOT(postEPFormalVerifyTimeout()));

     QTimer timer;
     timer.setInterval(m_IniFile->value("MesTimeOut").toInt());  // 设置超时时间 30 秒
     timer.setSingleShot(true);  // 单次触发
     QEventLoop loop;
     connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
     connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
     timer.start();
     loop.exec();  // 启动事件循环

     if (timer.isActive()) {  // 处理响应
         timer.stop();
         if (pReply->error() != QNetworkReply::NoError) {
             // 错误处理
             qDebug() << "Error String : " << pReply->errorString();
         } else {
             postFormalFinish(pReply);
         }
     }
     /*
     else
     {
         disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
         pReply->abort();
         pReply->deleteLater();
         qDebug() << "Timeout";

         postEPFormalVerifyTimeout();
     }
*/


}

void MainWindow::postEPFormalVerifyFinish(QNetworkReply *reply)//设备参数校验
{
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(EqumentVerifyLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    postEPFormalVerifyOK=false;
    /*
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
    */
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
                    FormalVerifyLoop.quit();//退出消息循环
                    postEPFormalVerifyOK=true;
                    update_mes_link_state(postEPFormalVerifyOK);
                    EqumentVerifyLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
                    PostendTime=QDateTime::fromString(EqumentVerifyLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
                    EqumentVerifyLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));
                    add_one_othermeslog(EqumentVerifyLogs);

                    if(object.value("status_code").toString()=="0") {
                       // postEPFormalVerifyOK=true;
                        qDebug()<<"设备验证返回成功";
                        lncCtrlValue=0;
                        qDebug()<<"设备校验申请成功"<<postEPFormalVerifyOK;
                        msg="设备校验成功";
                        const_msg(msg);
                    }
                 else{

                        QString dlgTitle = "设备校验失败消息框";
                        QString strInfo = object.value("message").toString() + "LNC设备将不可工作！";
                        QMessageBox::warning(this, dlgTitle, strInfo
                               , QMessageBox::Ok, QMessageBox::NoButton);

                        msg=object.value("message").toString();
                        const_msg(msg);
                        lncCtrlValue=1;
                    }

                }
           }
     }
}
#endif
