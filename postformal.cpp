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
#if 0
void MainWindow::postFormalTimeout()//配方申请超时
{
    postFormalOK=false;
    QDateTime PoststartTime=QDateTime::fromString(EqumentVerifyLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    QString msg;

    update_mes_link_state(postFormalOK);

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
         msg="配方申请不成功";
         const_msg(msg);

        }
        else
        {
            qDebug()<<"再次发出配方申请请求"<<FormalCnt;
            msg="配方申请重连"+QString::number(FormalCnt);
            const_msg(msg);
            QTimer timer;
            timer.setInterval(50);  // 设置超时时间 30 秒
            timer.setSingleShot(true);  // 单次触发
            connect(&timer, SIGNAL(timeout()), this, SLOT(postFormal()));
           // timer.start();
            QEventLoop loop;
            connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            timer.start();
            loop.exec();
           // postFormal();
        }

}

void MainWindow::postFormal()//工艺申请上传,获取配方
{
    QJsonObject mainjson;//要上传的json
    QJsonDocument endJsonDoc;

    mainjson.insert("process_id",m_IniFile->value("lncProcessNum").toString());
    mainjson.insert("equipment_id",m_IniFile->value("LncDevCode").toString());
    endJsonDoc.setObject(mainjson);


    LocalFormalLogs.processid=m_IniFile->value("lncProcessNum").toString();
    LocalFormalLogs.equmentid=m_IniFile->value("LncDevCode").toString();
    LocalFormalLogs.type=1;
    LocalFormalLogs.reqtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    LocalFormalLogs.postjson = QString(endJsonDoc.toJson());
    LocalFormalLogs.retcode="";
    LocalFormalLogs.retmsg="";


    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    //QUrl url("http://127.0.0.1:8000/Formal");
    QUrl url(m_IniFile->value("mesCraftUrl").toString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);

    QNetworkReply *pReply = accessManager->post(request, endJsonDoc.toJson());

  //  mesTxtMsg("配方申请发出数据：\n"+QString(endJsonDoc.toJson()));

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
    } else {  // 处理超时
        disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        qDebug() << "Timeout";

        postFormalTimeout();
    }

}


//配方申请完成响应
void MainWindow::postFormalFinish(QNetworkReply *reply)//工艺申请上传,获取配方
{
    QString msg;
    QDateTime PoststartTime=QDateTime::fromString(EqumentVerifyLogs.reqtime,"yyyy-MM-dd hh:mm:ss.zzz");
    QDateTime PostendTime;
    /*
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
  */
    //3、获取响应信息
    QByteArray bytes = reply->readAll();
   // qDebug() <<"获取响应信息"<< bytes ;
   // ui->textBrowser->append(bytes);
    QJsonParseError jsonError;
    //转换为Json文档
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
   // qDebug()<<"申请配方"<<doucment;
  //  mesTxtMsg("配方申请返回数据：\n"+QString(bytes));
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
               LocalFormalLogs.resptime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
               PostendTime=QDateTime::fromString(LocalFormalLogs.resptime,"yyyy-MM-dd hh:mm:ss.zzz");
               LocalFormalLogs.usedtime = QString::number(PoststartTime.msecsTo(PostendTime));

               qDebug()<<"配方申请成功"<<postFormalOK;
               msg="配方申请成功";
               const_msg(msg);
               add_one_othermeslog(LocalFormalLogs);//添加一条日志


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
#endif
