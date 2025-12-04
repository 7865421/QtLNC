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
#include<QDesktopServices>

modbus_t *ctx;
modbus_t *ctxFan;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startInit();
}

MainWindow::~MainWindow()
{
    QFile privfile (ui->lePrivFilename->text());
    bool isOk =privfile.open(QIODevice::ReadOnly | QIODevice::WriteOnly);
    if (isOk)
             {
                 QString privValue="priv="+QString::number(0);
                 //转化为本地平台编码
                 privfile.write(privValue.toLocal8Bit());
             }

             //关闭文件
    privfile.close();

    sc2.Disconnect(ServerIdx);
    sc2.LibraryDestroy();

    delete m_IniFile;
    delete user_model;
    delete temp_model;
    delete fanspd_model;
    delete log_model;
    delete login_model;
    delete logout_model;
    delete craft_model;//工艺参数

    delete meslocalformalmodel;//本地配方

    delete ng_model;//不良品
    delete scan_model;//扫码
    delete finish_model;//加工完成

    delete cellinmodel;//MES进站日志
    delete celloutmodel;//MES出站日志
    delete othersmodel;//其他接口日志

    delete lncftpmodel;
    delete localftpmodel;
    delete analysemodel;

    delete mesmatmodel;//物料
    delete mesmatlistmodel;//物料对列
    delete mesorderlistmodel;//工单队列

    delete mesformaldetailmodel;//配方详细参数



    delete ui;
}

QString MainWindow::const_msg(QString msg)
{
    QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString result;
    result = dtime + "::当前用户'" + logonUser +"'::"+msg;
    ui->statusbar->showMessage(dtime+msg,10000);
    logmsg lgdbmsg;
    lgdbmsg.user=logonUser;
    lgdbmsg.msg=msg;
    lgdbmsg.datetime=dtime;
    add_one_logmsg(lgdbmsg);
    return result;
}

 void MainWindow::ForbidenIDLogin()
 {
     QString msg;
     msg="LNC设置为非刷卡登录，请使用用户方式登录，或修改LNC设置为刷卡登录";
     ui->tbrLogIn->append(const_msg(msg));
 }


void MainWindow::on_btUserManag_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidgetUser->setCurrentIndex(0);
    ui->tabWidgetLNC->setCurrentIndex(0);
    ui->tabWidgetTc->setCurrentIndex(0);
    ui->tabWidgetLncTab->setCurrentIndex(0);
    ui->tabWidgetTcTab->setCurrentIndex(0);

    ui->dteSysLogStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteSysLogEnd->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::on_btLNCSetting_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->tabWidgetUser->setCurrentIndex(0);
    ui->tabWidgetLNC->setCurrentIndex(0);
    ui->tabWidgetTc->setCurrentIndex(0);
    ui->tabWidgetLncTab->setCurrentIndex(0);
    ui->tabWidgetTcTab->setCurrentIndex(0);

}


void MainWindow::on_btTCSetting_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->tabWidgetUser->setCurrentIndex(0);
    ui->tabWidgetLNC->setCurrentIndex(0);
    ui->tabWidgetTc->setCurrentIndex(0);
    ui->tabWidgetLncTab->setCurrentIndex(0);
    ui->tabWidgetTcTab->setCurrentIndex(0);
}

void MainWindow::on_btMesSetting_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->tabWidgetUser->setCurrentIndex(0);
    ui->tabWidgetLNC->setCurrentIndex(0);
    ui->tabWidgetTc->setCurrentIndex(0);
    ui->tabWidgetLncTab->setCurrentIndex(0);
    ui->tabWidgetTcTab->setCurrentIndex(0);
    ui->tabWidgetMes->setCurrentIndex(4);
    qDebug()<<ui->stackedWidget->currentIndex()<<ui->tabWidgetMes->currentIndex();

    ui->dteEnterLogStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteEnterLogEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteExitLogStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteExitLogEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteFinishLogStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteFinishLogEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteNGlogStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteNGlogEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteScanLogStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteScanLogEnd->setDateTime(QDateTime::currentDateTime());

    ui->dteCellInStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteCellInEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteCellOutStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteCellOutEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteInterfaceStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteInterfaceEnd->setDateTime(QDateTime::currentDateTime());

}

void MainWindow::on_btLNCTable_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->tabWidgetUser->setCurrentIndex(0);
    ui->tabWidgetLNC->setCurrentIndex(0);
    ui->tabWidgetTc->setCurrentIndex(0);
    ui->tabWidgetLncTab->setCurrentIndex(0);
    ui->tabWidgetTcTab->setCurrentIndex(0);
    if(lncConOK)
        on_btLncFilesRefresh_clicked();
    on_btPcFilesRefresh_clicked();

    ui->dteCraftEnd->setDateTime(QDateTime::currentDateTime());
}


void MainWindow::on_btTCTable_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    ui->tabWidgetUser->setCurrentIndex(0);
    ui->tabWidgetLNC->setCurrentIndex(0);
    ui->tabWidgetTc->setCurrentIndex(0);
    ui->tabWidgetLncTab->setCurrentIndex(0);
    ui->tabWidgetTcTab->setCurrentIndex(0);
    ui->dteTempStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteTempEnd->setDateTime(QDateTime::currentDateTime());
    ui->dteFanSpdStart->setDateTime(QDateTime::fromString(QDate::currentDate().toString()+" 00:00:00"));
    ui->dteFanSpdEnd->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::wait_to_exit_priv()
{
    QTime startTime = QTime::currentTime();
    QTime stopTime;
    int elapsed;
    while(LncPrivRead!=0){
        QCoreApplication::processEvents();
        stopTime = QTime::currentTime();
        elapsed = startTime.msecsTo(stopTime);
        if(elapsed>8000)
           break;
       }
}

void MainWindow::on_btExit_clicked()
{
    QString msg;
    QMessageBox:: StandardButton result= QMessageBox::information(NULL, "退出程序", "程序正在运行中，是否退出程序？",QMessageBox::Yes|QMessageBox::No);
    switch (result)
    {
    case QMessageBox::Yes:
        on_btLogOut_clicked();
        on_btScanDisConnect_clicked();
        msg="退出程序操作";
        const_msg(msg);
        wait_to_exit_priv();
        qApp->exit(773);
        break;
    case QMessageBox::No:

        break;
    default:
        break;
    }
}

void MainWindow::on_btAddNewUser_clicked()
{
    userdata user;
    user.name=ui->leNewUser->text();
   // user.passwd=ui->leNewUserPaswd->text();
    user.passwd=QCryptographicHash::hash(ui->leNewUserPaswd->text().toLocal8Bit(),QCryptographicHash::Md5).toHex();
   // user.cardid=ui->leNewUserID->text();
    user.cardid=QCryptographicHash::hash(ui->leNewUserID->text().toLocal8Bit(),QCryptographicHash::Md5).toHex();
    user.priv=ui->cbNewUserPriv->itemText(ui->cbNewUserPriv->currentIndex());
    user.datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString msg;
    bool halt = false;

        if(user.name == "")
        {
            msg="未填写用户名，请填写用户名!";
            ui->tbrNewUserLog->append(const_msg(msg));
            halt = true;
        }

        if(ui->leNewUserPaswd->text() == "")
        {
            msg="未填写用户密码，请填写用密码!";
            ui->tbrNewUserLog->append(const_msg(msg));
            halt = true;
        }

        if(user.cardid == "")
        {
            msg="未填写用户卡ID，请填写用卡ID!";
            ui->tbrNewUserLog->append(const_msg(msg));
            halt = true;
        }



        QSqlQuery cQuery;
        cQuery.prepare("SELECT name FROM usertable WHERE name=(:name)");
        cQuery.bindValue(":name", user.name);

        if(cQuery.exec())
        {
            if(cQuery.next())
            {
                msg="用户名已注册，请更换一个新的用户名!";
                ui->tbrNewUserLog->append(const_msg(msg));
                halt = true;
            }
        }

        QSqlQuery cQuery1;
        cQuery1.prepare("SELECT cardid FROM usertable WHERE cardid=(:cardid)");
        cQuery1.bindValue(":cardid", user.cardid);

        if(cQuery1.exec())
        {
            if(cQuery1.next())
            {
                msg="用户卡ID已注册，请更换一个新的用户卡ID!";
                ui->tbrNewUserLog->append(const_msg(msg));
                halt = true;
            }
        }

        if(halt)
        {
            msg="注册失败，请更正错误后重新注册！";
            ui->tbrNewUserLog->append(const_msg(msg));
        }
        else
        {
            qDebug()<<"add user name="<<user.name;
            qDebug()<<"add user passwd="<<user.passwd;
            qDebug()<<"add user cardid="<<user.cardid;
            qDebug()<<"add user priv="<<user.priv;
            qDebug()<<"add user datetime="<<user.datetime;
            add_one_user(user);
            msg="注册成功！可使用用户名+密码登录，或直接刷卡登录!";
            ui->tbrNewUserLog->append(const_msg(msg));
        }

}

void MainWindow::on_btQuerryAllUsers_clicked()
{
    user_model->setQuery(QString("select * from usertable WHERE name!='admin'"));
    user_model->setHeaderData(0,Qt::Horizontal,QObject::tr("用户ID"));
    user_model->setHeaderData(1,Qt::Horizontal,QObject::tr("用户名"));
    user_model->setHeaderData(2,Qt::Horizontal,QObject::tr("密码"));
    user_model->setHeaderData(3,Qt::Horizontal,QObject::tr("卡ID"));
    user_model->setHeaderData(4,Qt::Horizontal,QObject::tr("权限"));
    user_model->setHeaderData(5,Qt::Horizontal,QObject::tr("新增时间"));
    ui->tbvUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvUsers->setModel(user_model);
}

void MainWindow::on_btQuerryByUser_clicked()
{
    user_model->setQuery(QString("select * from usertable where name Like '%") + ui->leQuerryUser->text() +"%'" +" AND name!='admin'");
    user_model->setHeaderData(0,Qt::Horizontal,QObject::tr("用户ID"));
    user_model->setHeaderData(1,Qt::Horizontal,QObject::tr("用户名"));
    user_model->setHeaderData(2,Qt::Horizontal,QObject::tr("密码"));
    user_model->setHeaderData(3,Qt::Horizontal,QObject::tr("卡ID"));
    user_model->setHeaderData(4,Qt::Horizontal,QObject::tr("权限"));
    user_model->setHeaderData(5,Qt::Horizontal,QObject::tr("新增时间"));
    ui->tbvUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvUsers->setModel(user_model);
}

void MainWindow::on_tbvUsers_clicked(const QModelIndex &index)
{
//    qDebug()<<index.row()<<index.column();
//    qDebug()<<user_model->itemData(index);
//    qDebug()<<user_model->data(index);
    ui->leModifyUser->setText(ui->tbvUsers->model()->data(ui->tbvUsers->model()->index(index.row(),1)).toString());
    ModifyUserName = ui->tbvUsers->model()->data(ui->tbvUsers->model()->index(index.row(),1)).toString();
    ModifyUserPasswd = ui->tbvUsers->model()->data(ui->tbvUsers->model()->index(index.row(),2)).toString();
    ModifyUserID = ui->tbvUsers->model()->data(ui->tbvUsers->model()->index(index.row(),3)).toString();
   // ui->leModifyUserPaswd->setPlaceholderText("新密码");
   // ui->leModifyUserID->setPlaceholderText("新卡ID");

    ui->leModifyUserPaswd->clear();
    ui->leModifyUserID->clear();

    int privindex=2;
    QString txt=ui->tbvUsers->model()->data(ui->tbvUsers->model()->index(index.row(),4)).toString();
    if(txt=="无权限")
        privindex=0;
    else if(txt=="操作员")
        privindex=1;
    else if(txt=="技术员")
        privindex=2;
    else if(txt=="工程师")
        privindex=3;
    else if(txt=="管理员")
        privindex=4;
    else
        privindex=0;
    ui->cbModifyUserPriv->setCurrentIndex(privindex);

    select_userid=ui->tbvUsers->model()->data(ui->tbvUsers->model()->index(index.row(),0)).toString();

}

void MainWindow::on_btDeletUser_clicked()
{
    userdata user;
    QString msg;
    user.id=select_userid.toInt();
    delete_one_user(user);
    on_btQuerryAllUsers_clicked();
    ui->leModifyUser->setText("");
    ui->leModifyUserPaswd->setPlaceholderText("新密码");
    ui->leModifyUserID->setPlaceholderText("新ID");
    ui->cbModifyUserPriv->setCurrentIndex(0);
    msg="删除用户'"+ModifyUserName+"'";

}

void MainWindow::on_btModifyUserInfo_clicked()
{
    bool halt = false;
    userdata user;
    QString msg;

    user.id=select_userid.toInt();
    user.name=ui->leModifyUser->text();
    if(ui->leModifyUserPaswd->text().isEmpty())
        user.passwd=ModifyUserPasswd;
    else
        user.passwd = QCryptographicHash::hash(ui->leModifyUserPaswd->text().toLocal8Bit(),QCryptographicHash::Md5).toHex();
    if(ui->leModifyUserID->text().isEmpty())
        user.cardid=ModifyUserID;
    else
        user.cardid = QCryptographicHash::hash(ui->leModifyUserID->text().toLocal8Bit(),QCryptographicHash::Md5).toHex();

    user.priv = ui->cbModifyUserPriv->itemText(ui->cbModifyUserPriv->currentIndex());
        if(ui->leModifyUser->text() == "")
        {
            msg="修改用户时，未填写用户名，请填写用户名!";
            ui->tbrNewUserLog->append(const_msg(msg));
            halt = true;
        }

        if(ui->leModifyUserPaswd->text() == "")
        {
            msg="修改用户时，密码未做修改!";
            ui->tbrNewUserLog->append(const_msg(msg));
            //halt = true;
        }

        if(ui->leModifyUserID->text() == "")
        {
            msg="修改用户时，用卡ID未做修改!";
            ui->tbrNewUserLog->append(const_msg(msg));
            //halt = true;
        }

        QSqlQuery cQuery;
        cQuery.prepare("SELECT name,id FROM usertable WHERE name=(:name) AND id !=(:id)");
        cQuery.bindValue(":name", user.name);
        cQuery.bindValue(":id", select_userid.toInt());

        if(cQuery.exec())
        {
            if(cQuery.next())
            {
                msg="用户名已注册，请更换一个新的用户名!";
                ui->tbrNewUserLog->append(const_msg(msg));
                halt = true;
                user_model->setQuery(QString("select * from usertable where name = '") + user.name +"'");
                user_model->setHeaderData(0,Qt::Horizontal,QObject::tr("用户ID"));
                user_model->setHeaderData(1,Qt::Horizontal,QObject::tr("用户名"));
                user_model->setHeaderData(2,Qt::Horizontal,QObject::tr("密码"));
                user_model->setHeaderData(3,Qt::Horizontal,QObject::tr("卡ID"));
                user_model->setHeaderData(4,Qt::Horizontal,QObject::tr("权限"));
                user_model->setHeaderData(5,Qt::Horizontal,QObject::tr("新增时间"));
                ui->tbvUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                ui->tbvUsers->setModel(user_model);
            }
        }

        QSqlQuery cQuery1;
        cQuery1.prepare("SELECT cardid,id FROM usertable WHERE cardid=(:cardid) AND id !=(:id)");
        cQuery1.bindValue(":cardid", user.cardid);
        cQuery1.bindValue(":id", select_userid.toInt());

        if(cQuery1.exec())
        {
            if(cQuery1.next())
            {
                msg="修改用户时，用户卡ID已注册，请更换一个新的用户卡ID!";
                ui->tbrNewUserLog->append(const_msg(msg));
                halt = true;
                user_model->setQuery(QString("select * from usertable where cardid = '") + user.cardid +"'");
                user_model->setHeaderData(0,Qt::Horizontal,QObject::tr("用户ID"));
                user_model->setHeaderData(1,Qt::Horizontal,QObject::tr("用户名"));
                user_model->setHeaderData(2,Qt::Horizontal,QObject::tr("密码"));
                user_model->setHeaderData(3,Qt::Horizontal,QObject::tr("卡ID"));
                user_model->setHeaderData(4,Qt::Horizontal,QObject::tr("权限"));
                user_model->setHeaderData(5,Qt::Horizontal,QObject::tr("新增时间"));
                ui->tbvUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                ui->tbvUsers->setModel(user_model);
            }
        }

        if(halt)
        {
            msg="修改用户失败，请更正错误后重新更新信息！";
            ui->tbrNewUserLog->append(const_msg(msg));
        }
        else
        {
            modify_user(user);
            msg="用户信息修改成功！可使用更新后的用户名+密码登录，或直接刷卡登录!";
            ui->tbrNewUserLog->append(const_msg(msg));
            on_btQuerryAllUsers_clicked();
        }
}

void MainWindow::on_btLogIn_clicked()
{
    QString msg;
    logonUser=ui->lbLogInUserName->text();
    qDebug()<<logonUser;
    if(logonUser == "无用户")
    {
       // qDebug()<<"无用户开始切换到有用户";

        if((ui->leLogInUser->text()=="admin") && (RegAlowAdmin==0))
        {
            msg="无法以超级管理员身份登录，请设置LNC允许超级管理员登录选项";
            ui->tbrLogIn->append(const_msg(msg));
            QMessageBox:: StandardButton result= QMessageBox::information(NULL, "提示！", "无法以超级管理员身份登录，请设置LNC允许超级管理员登录选项。",QMessageBox::Yes);
            switch (result)
            {
            case QMessageBox::Yes:
                break;
            case QMessageBox::No:

                break;
            default:
                break;
            }
            return;
        }
        else
        {
            Login(ui->leLogInUser->text(),QCryptographicHash::hash(ui->leLogInPaswd->text().toLocal8Bit(),QCryptographicHash::Md5).toHex());
        }
    }
    else
    {
        msg="当前已有用户登录使用，用户名:"+logonUser+",请退出当前用户后再登录。";
        ui->tbrLogIn->append(const_msg(msg));
    }
}
bool MainWindow::Login(QString u, QString p)
{
    bool exists = false;
    QString msg;
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT name,priv FROM usertable WHERE name = (:name) AND passwd = (:pw)");
    checkQuery.bindValue(":name", u);
    checkQuery.bindValue(":pw", p);

    if (checkQuery.exec())
    {
        if (checkQuery.next())
        {
            exists = true;
            ui->lbLogInUserName->setText(u);
            ui->lbLogInUserPriv->setText(checkQuery.value(1).toString());
            logonUser=ui->lbLogInUserName->text();
            logonPriv=ui->lbLogInUserPriv->text();
            ui->btLogOut->setEnabled(true);
            ui->btLogIn->setEnabled(false);

            if(logonUser != "admin")
            {
            if(logonPriv=="无权限") logonPrivIdx=0;
            if(logonPriv=="操作员") logonPrivIdx=1;
            if(logonPriv=="技术员") logonPrivIdx=2;
            if(logonPriv=="工程师") logonPrivIdx=3;
            if(logonPriv=="管理员") logonPrivIdx=4;
            }
            else
                logonPrivIdx=7;
            //qDebug()<<"权限索引="<<logonPrivIdx;
            msg=logonUser+"登录成功";
            ui->tbrLogIn->append(const_msg(msg));

        }
    }
    if(exists==false)
    {
        msg="账号未注册或密码错误，用户:"+u+",登录失败！";
        ui->tbrLogIn->append(const_msg(msg));
    }
    ui->leLogInUser->clear();
    ui->leLogInPaswd->clear();
    return exists;
}
void MainWindow::LogByID(QString cardid)
{
  bool exists = false;
  QString msg;
  if(logonUser=="无用户")
  {
    if(ui->leLogInUser->text()=="")
    {
        QSqlQuery checkQuery;
        QString idmd5=QCryptographicHash::hash(cardid.toLocal8Bit(),QCryptographicHash::Md5).toHex();
        checkQuery.prepare("SELECT name,priv,cardid FROM usertable WHERE cardid = (:cardid)");
        checkQuery.bindValue(":cardid", idmd5);
    // qDebug()<<"login by cardid="<<QCryptographicHash::hash(logCardID.toLatin1(),QCryptographicHash::Md5).toHex();
        if (checkQuery.exec())
        {

            if (checkQuery.next())
            {
                exists = true;
                ui->lbLogInUserName->setText(checkQuery.value(0).toString());
                ui->lbLogInUserPriv->setText(checkQuery.value(1).toString());
                logonUser=ui->lbLogInUserName->text();
                logonPriv=ui->lbLogInUserPriv->text();
                ui->btLogOut->setEnabled(true);
                ui->btLogIn->setEnabled(false);

                if(logonUser != "admin")
                {
                if(logonPriv=="无权限") logonPrivIdx=0;
                if(logonPriv=="操作员") logonPrivIdx=1;
                if(logonPriv=="技术员") logonPrivIdx=2;
                if(logonPriv=="工程师") logonPrivIdx=3;
                if(logonPriv=="管理员") logonPrivIdx=4;
                }
                else
                    logonPrivIdx=7;

               // qDebug()<<"权限索引="<<logonPrivIdx;
                msg=logonUser +"刷卡登录成功！";
                ui->tbrLogIn->append(const_msg(msg));
            }
        }
        if(exists==false)
        {
            msg="此卡未登记，刷卡登录失败！";
            ui->tbrLogIn->append(const_msg(msg));
        }
    }
    else
        on_btLogIn_clicked();
 }
  else
  {
      msg="当前已有用户登录使用，用户名:"+logonUser+",请退出当前用户后再登录。";
      ui->tbrLogIn->append(const_msg(msg));
  }

    ui->leLogInUser->clear();
    ui->leLogInPaswd->clear();
    logCardID.clear();


}

//温控器手动连接
void MainWindow::on_btConnectTc_clicked()
{
    QString COMport;
    QRegExp rx("(\\d+)");
    int pos=0;
    while ((pos=rx.indexIn(m_IniFile->value("TcComPort").toString(),pos))!=-1){
       if(rx.cap(0).toInt()>9)
           COMport = "\\\\.\\" + m_IniFile->value("TcComPort").toString();
       else
           COMport =m_IniFile->value("TcComPort").toString();
     //  qDebug()<<COMport;
       pos+=rx.matchedLength();
     //  qDebug()<<pos;
      }

    const char* com=COMport.toStdString().c_str();
    int baudrate = m_IniFile->value("TcComBodrate").toInt();
    QString  CRCtext = ui->cbMbCrc->itemText(ui->cbMbCrc->currentIndex());
    char crc=CRCtext.toStdString().at(0);
    int databit=(ui->cbMbDataBits->itemText(ui->cbMbDataBits->currentIndex())).toInt();
    int stopbit=(ui->cbMbStopBits->itemText(ui->cbMbStopBits->currentIndex())).toInt();
    QString msg;

        ctx = modbus_new_rtu(com, baudrate, crc, databit, stopbit);
        msg="Modbus发出重连请求";
        const_msg(msg);
        if(ctx==NULL)
        {
            msg="设备无效，无法创建Modbus运行环境!";
            const_msg(msg);
            qDebug()<<"modbus errno"<<modbus_strerror(errno);
            ui->lbTcComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
            ui->lbTcConnectStatus->setText(QString("%1%2%3").arg("无法打开",ui->leTcComPort->text(),"Modbus初始失败"));
            return;
        }
        struct timeval my_time; //modbus超时时间
        my_time.tv_sec=0;
        my_time.tv_usec=100000;                    //设置modbus超时时间为1000ms
        modbus_set_response_timeout(ctx,uint32_t(my_time.tv_sec),uint32_t(my_time.tv_usec));
        modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_NONE);


    if (modbus_connect(ctx) == -1) {
      // qDebug()<<"modbus errno"<<modbus_strerror(errno);
       modbus_free(ctx);
       ctx=NULL;
       ui->lbTcComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
       ui->lbTcConnectStatus->setText(QString("%1%2%3").arg("温控器链接",ui->leTcComPort->text(),"失败"));
       ui->lbFcComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
       ui->lbFsConnectStatus->setText(QString("%1%2%3").arg("风速仪链接",ui->leTcComPort->text(),"失败"));
       }
     else
       {
        qDebug()<<"打开串口成功";
        ui->btConnectTc->setEnabled(false);
        ui->lbTcConnectStatus->setText(QString("%1%2%3").arg("温控器打开",ui->leTcComPort->text(),"成功"));
        ui->lbFsConnectStatus->setText(QString("%1%2%3").arg("风速仪打开",ui->leTcComPort->text(),"成功"));

       }

}

void MainWindow::on_btFindLNC_clicked()
{
       unsigned short Num, i;
       QString str;
       LOCAL_CONTROLLER_INFO Info;
       //QStringListModel *Model;
       QStandardItemModel *ItemModel;
       QStringList strList;
       ItemModel = new QStandardItemModel(this);

       Num=sc2.LocalDetectControllers();
       //qDebug()<<Num;

       for (i=0; i<Num; i++)
       {
           sc2.LocalReadController(i, &Info);
           qDebug()<<Info.IP;
           //strList.append(QString(Info.IP));
           QStandardItem *item = new QStandardItem(QString(Info.IP));
           ItemModel->appendRow(item);
       }

       //Model = new QStringListModel(strList);

       ui->lvLncIP->setModel(ItemModel);

}

void MainWindow::on_btConnectLncByIP_clicked()
{
    QString msg;
    //============連線時，設定要持續讀回的資料=========
    int val = sc2.GetConnectionMsg(ServerIdx, SCIF_CONNECT_STATE);
    if (val == SC_CONN_STATE_DISCONNECT)
    {
        char *c;
        //c=ui->leLncIP->text().toLatin1().data();
        c=m_IniFile->value("LncIP").toString().toLatin1().data();
        int rt=0;
        if(ui->chbNoLncPaswd->checkState()==Qt::Checked)
            rt=sc2.ConnectLocalIP(ServerIdx, c);
        else
         {
            rt=sc2.ConnectLocalIP(ServerIdx, c);
            rt=sc2.ConnectSetPwd(ServerIdx,ui->leLNCPaswd->text().toLatin1().data());
         }
        if(rt)
        {
            ui->lbLncConnectStatus->setText("正确发出连接请求！");
            msg="向LNC发出链接请求";

   //循环读
          /*
            if(pointReadData !=NULL)
            {
                free(pointReadData);
                pointReadData=NULL;
                qDebug()<<"free  dp";
            }
            int pTran;
            sc2.LClearQueue(ServerIdx);      //清除先前設定的內容
            sc2.LReadBegin(ServerIdx);                   //設定自行組合內容，以減少封包數
            sc2.LReadNR(ServerIdx,m_IniFile->value("LncPrivAddrR").toInt(),1);//R5000001
            sc2.LReadNR(ServerIdx,m_IniFile->value("LncTempAddrR").toInt(),1);//R5000002
            sc2.LReadNR(ServerIdx,m_IniFile->value("LncAlowAdminAddrR").toInt(),1);//R5000003
            sc2.LReadNR(ServerIdx,m_IniFile->value("LncHeartCtrlAddrR").toInt(),1);//R5000050
            sc2.LReadNR(ServerIdx, m_IniFile->value("LncLogOutAddrR").toInt(), 1);//R5009993
            sc2.LReadNR(ServerIdx, m_IniFile->value("LncIDorNameAddrR").toInt(), 1);//R5922701
            pTran=sc2.LReadEnd(ServerIdx);
            pointReadData=sc2.GetDataPointer(pTran);
            qDebug()<<pointReadData;
         */
        }
        else
        {
            ui->lbLncConnectStatus->setText("发出连接请求错误！");
            msg="向LNC发出链接请求错误！";
        }
        const_msg(msg);
      }
    ui->lbLncIP->setText(ui->leLncIP->text());

}
//LNC断开连接
void MainWindow::on_btDisconnectLnc_clicked()
{
    QString msg;
    if(sc2.Disconnect(ServerIdx))
    {
        ui->lbLncConnectStatus->setText("正确发出断开请求！");
        msg="向LNC发出断开请求";
    }
    else
    {
        ui->lbLncConnectStatus->setText("发出断开请求错误！");
        msg="向LNC发出断开请求错误";
    }
    const_msg(msg);
}

void MainWindow::show_lnc_filelist()
{
    unsigned short n;
    int i=0;
    QString str1;
    n = sc2.FtpReadFileCount();
    //msg=const_msg("刷新LNC目录文件，获取文件数:"+QString::number(n));
    QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString msg=dtime+"::刷新LNC目录文件，获取文件数:"+QString::number(n);
    ui->tbrFilesLog->append(msg);
    ui->statusbar->showMessage(msg);
    lncftpmodel->clear();
    QStringList titles;
    titles.push_back("文件名");
    titles.push_back("文件大小");
    titles.push_back("时间");
    lncftpmodel->setHorizontalHeaderLabels(titles);
    if (n>0)
    {
        FTP_FILE fff;
        for (i=0; i<n; i++)
        {
            QList<QStandardItem*> rowData;
            sc2.FtpReadFile(i, &fff);
            rowData<<new QStandardItem(QString(QLatin1String(fff.filename)));
            rowData<<new QStandardItem(QString::number(fff.filesize));
            rowData<<new QStandardItem(str1.sprintf("%d/%d/%d %d:%d:%d", fff.year, fff.month, fff.day, fff.hour, fff.minute, fff.second));
            lncftpmodel->appendRow( rowData );
          //  qDebug()<<"文件--"<<rowData;
        }
    }
    ui->btLncFileDownload->setEnabled(true);
}
//LNC发现对应目录文件
void MainWindow::on_btLncFilesRefresh_clicked()
{

  int lncfolder,rt;
  QString msg;
  if(LncConnStatus==SC_CONN_STATE_OK) {

  rt = sc2.FtpSetConnection(ServerIdx);
  switch(ui->cbLncFtpFolder->currentIndex())
  {
    case 0:
      lncfolder=FTP_FOLDER_NCFILES;break;
    case 1:
      lncfolder=FTP_FOLDER_DXF;break;
    case 2:
      lncfolder=FTP_FOLDER_DATA;break;
    case 3:
      lncfolder=FTP_FOLDER_LOG;break;
    default :
      lncfolder=FTP_FOLDER_LOG;
  }
 if(rt==0)
     rt=sc2.FtpSetConnection(ServerIdx);

  rt = sc2.FtpGetFileList(lncfolder, "", "", "");

  if(rt)
  {
      sc_FtpAction=FTP_Actoin_FIND_FILES;
    //  ui->btLncFileDownload->setEnabled(false);

      unsigned short n;
      int i=0;
      QString str1;
      n = sc2.FtpReadFileCount();
      //msg=const_msg("刷新LNC目录文件，获取文件数:"+QString::number(n));
      ui->tbrFilesLog->append("刷新LNC目录文件，获取文件数:"+QString::number(n));
      ui->statusbar->showMessage("刷新LNC目录文件，获取文件数:"+QString::number(n));
      lncftpmodel->clear();
      QStringList titles;
      titles.push_back("文件名");
      titles.push_back("文件大小");
      titles.push_back("时间");
      lncftpmodel->setHorizontalHeaderLabels(titles);
      if (n>0)
      {
          FTP_FILE fff;
          for (i=0; i<n; i++)
          {
              QList<QStandardItem*> rowData;
              sc2.FtpReadFile(i, &fff);
              rowData<<new QStandardItem(QString(QLatin1String(fff.filename)));
              rowData<<new QStandardItem(QString::number(fff.filesize));
              rowData<<new QStandardItem(str1.sprintf("%d/%d/%d %d:%d:%d", fff.year, fff.month, fff.day, fff.hour, fff.minute, fff.second));
              lncftpmodel->appendRow( rowData );
            //  qDebug()<<"文件--"<<rowData;
          }
      }

  }
  else
  {
      msg=const_msg("刷新LNC目录文件，无获取到文件或刷新无效");
  }
  }//conOK
  else {
    msg=const_msg("LNC控制器未连接，无法刷新LNC目录文件");
  }
    ui->tbrFilesLog->append(msg);
   // ui->statusbar->showMessage(msg);
}

void MainWindow::on_btSysLogExport_clicked()
{
    ui->prgExportSyslog->setValue(0);
    if(sysExcelSaveProgress->isRunning()==false)
     {
        sysExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "系统日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(sysExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        sysExcelSaveProgress->type=0;
        sysExcelSaveProgress->sheetTabName="系统日志记录";
        sysExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvSysLog->model());
        sysExcelSaveProgress->start();
       }
      }
}
//IP地址列表被点中
void MainWindow::on_lvLncIP_clicked(const QModelIndex &index)
{
    select_ip=index.data().toString();
    //qDebug()<<"选中IP"<<select_ip;
    ui->leLncIP->setText(select_ip);
}

void MainWindow::on_btConnectLncByFind_clicked()
{
    on_btConnectLncByIP_clicked();
}

void MainWindow::on_btBrowsPcDownloadFolder_clicked()
{
    QString localpath;
    localpath =QFileDialog::getExistingDirectory(this, tr("打开文件夹"));
    if(!localpath.isEmpty())
    {
        ui->lePcDownloadFolder->setText(localpath);
        m_IniFile->setValue("LocalDownloadFolder",ui->lePcDownloadFolder->text());

    }
}

void MainWindow::on_btDisconnectTc_clicked()
{
    ui->btConnectTc->setEnabled(true);
    ui->chbTcConectAuto->setCheckState(Qt::Unchecked);
   // temptimer->stop();
    //tempAdaptTimer->stop();
    if(ctx!=NULL)
    {
        modbus_close(ctx);
        modbus_free(ctx);
        ctx=NULL;
    }
}

void MainWindow::on_btLncFileDownload_clicked()
{
    QString sub="";
    QString fln,sfln;
    char *subpath,*filename;
    char *savefilename;
    int lncfolder;
   // qDebug()<<"ui->tbvLncFiles->currentIndex()="<<ui->tbvLncFiles->currentIndex();
    if((ui->tbvLncFiles->currentIndex().row() < 0) || (ui->tbvLncFiles->currentIndex().column() < 0) )
     {
      QMessageBox fileSlectMsg;
      fileSlectMsg.setText("未选择下载文件，请选择文件后再下载！");
      fileSlectMsg.exec();
      return;
     }
    fln=lncftpmodel->item(ui->tbvLncFiles->currentIndex().row(),0)->text();
    //qDebug()<<"fln="<<fln;
    if(fln.isEmpty())
     {
      QMessageBox fileSlectMsg;
      fileSlectMsg.setText("选择的文件名无效，请重新选择有效文件！");
      fileSlectMsg.exec();
      return;
     }
   // qDebug()<<"getfilename"<<fln;
    sfln=ui->lePcDownloadFolder->text()+"/"+fln;
    subpath="";
    subpath =sub.toLatin1().data();
    filename=fln.toLatin1().data();
   // savefilename=sfln.toLatin1().data();
    QTextCodec *code = QTextCodec::codecForName("GB2312");

    std::string name = code->fromUnicode(sfln).data();

    savefilename=const_cast<char *>(name.c_str());
  //  qDebug()<<"savefilename"<<sfln;
    switch(ui->cbLncFtpFolder->currentIndex())
    {
        case 0:
        lncfolder=FTP_FOLDER_NCFILES;break;
        case 1:
        lncfolder=FTP_FOLDER_DXF;break;
        case 2:
        lncfolder=FTP_FOLDER_DATA;break;
        case 3:
        lncfolder=FTP_FOLDER_LOG;break;
        default :
        lncfolder=FTP_FOLDER_LOG;
    }
    sc2.FtpDownloadFile(lncfolder,subpath,filename,savefilename);
    sc_FtpAction=FTP_Action_DOWNLOAD_FILE;
   // ui->btLncFilesRefresh->setEnabled(false);
    QString msg="开始从LNC下载文件"+fln+"到管理机，存放路径为"+sfln;
    ui->tbrFilesLog->append(const_msg(msg));

}

void MainWindow::on_btPcFilesRefresh_clicked()
{

    QDir *dir=new QDir(ui->lePcDownloadFolder->text());
    QString filter;
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Time)); //只获取该目录下的文件，按最迟修改时间排序);
    int n=fileInfo->count();
    //qDebug()<<"本地文件夹读到文件个数："<<n;
    QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString msg=dtime+"::查询管理机下载目录文件，获得文件数为"+QString::number(n);
    //ui->tbrFilesLog->append(const_msg(msg));
    ui->tbrFilesLog->append(msg);
    ui->statusbar->showMessage(msg);
    localftpmodel->clear();
    QStringList titles;
    titles.push_back("文件名");
    titles.push_back("文件大小");
    titles.push_back("时间");
    localftpmodel->setHorizontalHeaderLabels(titles);
    if (n > 0)
    {
        int i=0;
        for (i=0; i<n; i++)
        {
            QList<QStandardItem*> rowData;
            fileInfo->at(i).fileName();
            rowData<<new QStandardItem(fileInfo->at(i).fileName());
            rowData<<new QStandardItem(QString::number(fileInfo->at(i).size()));
            rowData<<new QStandardItem(fileInfo->at(i).lastModified().toString("yyyy/MM/dd hh:mm:ss"));
            localftpmodel->appendRow( rowData );
        }
    }



}
//退出登录，注销
void MainWindow::on_btLogOut_clicked()
{
    QString msg;
    if(logonUser != "无用户")
    {
        msg=logonUser+"注销，退出登录状态。";
        ui->tbrLogIn->append(const_msg(msg));
        logonPriv="无权限";
        logonUser="无用户";
        logonPrivIdx=0;
        ui->lbLogInUserName->setText(logonUser);
        ui->lbLogInUserPriv->setText(logonPriv);
        ui->btLogIn->setEnabled(true);
        ui->btLogOut->setEnabled(false);
    }

}

void MainWindow::on_btSysLogQuerry_clicked()
{
    QString startT,stopT;
    startT=ui->dteSysLogStart->text();
    stopT=ui->dteSysLogEnd->text();
   // qDebug()<<querry_logmsg_by_time(startT,stopT);

    log_model->setQuery(querry_logmsg_by_time(startT,stopT));
    log_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
    log_model->setHeaderData(1,Qt::Horizontal,QObject::tr("用户"));
    log_model->setHeaderData(2,Qt::Horizontal,QObject::tr("事件信息"));
    log_model->setHeaderData(3,Qt::Horizontal,QObject::tr("时间戳"));
    ui->tbvSysLog->setModel(log_model);
    ui->tbvSysLog->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvSysLog->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::on_btClearLogInLog_clicked()
{
    ui->tbrLogIn->clear();
    QString msg="清除登录日志文本";
    const_msg(msg);
}


void MainWindow::on_btSaveLogInLog_clicked()
{
 QString logFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "./登录日志"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".txt", tr("txt (*.txt)"));
 QFile file(logFile);
 if(file.fileName().isEmpty())
     return;
 if(!file.open(QFile::WriteOnly | QIODevice::Text))
 {
     QMessageBox::warning(this,tr("保存文件"),tr("打开文件 %1 失败，无法保存\n%2").arg(logFile).arg(file.errorString()),QMessageBox::Ok);
    return;
 }
 QTextStream out(&file);
 out<<ui->tbrLogIn->toPlainText();
 file.close();
}

void MainWindow::on_btClearNewUserLog_clicked()
{
    ui->tbrNewUserLog->clear();
    QString msg="清除登录用户管理日志文本";
    const_msg(msg);
}

void MainWindow::on_btSaveNewUserLog_clicked()
{
    QString logFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "./新增用户日志"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".txt", tr("txt (*.txt)"));
    QFile file(logFile);
    if(file.fileName().isEmpty())
        return;
    if(!file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("保存文件"),tr("打开文件 %1 失败，无法保存\n%2").arg(logFile).arg(file.errorString()),QMessageBox::Ok);
       return;
    }
    QTextStream out(&file);
    out<<ui->tbrNewUserLog->toPlainText();
    file.close();
}

void MainWindow::on_btClearFilesLog_clicked()
{
    ui->tbrFilesLog->clear();
    QString msg="清除文件下载日志文本";
    const_msg(msg);
}

void MainWindow::on_btSaveFilesLog_clicked()
{
    QString logFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "./文件操作日志"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".txt", tr("txt (*.txt)"));
    QFile file(logFile);
    if(file.fileName().isEmpty())
        return;
    if(!file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("保存文件"),tr("打开文件 %1 失败，无法保存\n%2").arg(logFile).arg(file.errorString()),QMessageBox::Ok);
       return;
    }
    QTextStream out(&file);
    out<<ui->tbrFilesLog->toPlainText();
    file.close();
}

void MainWindow::on_btTempQuerry_clicked()
{
    QString startT,stopT;
    startT=ui->dteTempStart->text();
    stopT=ui->dteTempEnd->text();
    temp_model->setQuery(querry_temperature_by_time(startT,stopT));
   // temp_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
    temp_model->setHeaderData(0,Qt::Horizontal,QObject::tr("温度值"));
    temp_model->setHeaderData(1,Qt::Horizontal,QObject::tr("时间戳"));
    ui->tbvTempDatas->setModel(temp_model);
    ui->tbvTempDatas->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvTempDatas->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::on_btTempExport_clicked()
{
    ui->prgExportTmpData->setValue(0);
    if(tempExcelSaveProgress->isRunning()==false)
     {
        tempExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "温度数据表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(tempExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        tempExcelSaveProgress->type=0;
        tempExcelSaveProgress->sheetTabName="温度记录表";
        tempExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvTempDatas->model());
        tempExcelSaveProgress->start();
       }
      }
}

void MainWindow::on_btClearTemp_clicked()
{
            QSqlQuery query;
            QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                        tr("清除操作将清除数据库中的内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                        QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                        QMessageBox::No);
            if (clearBtn == QMessageBox::Yes) {
                query.prepare("delete from temperaturetable");
                query.exec();
                on_btTempQuerry_clicked();
                QString msg="清除温度表在数据库中的数据";
                const_msg(msg);
            }
            else
            {
                temp_model->setQuery("select temp,date from temperaturetable limit 0");
                temp_model->setHeaderData(0,Qt::Horizontal,QObject::tr("温度值"));
                temp_model->setHeaderData(1,Qt::Horizontal,QObject::tr("时间戳"));
                ui->tbvTempDatas->setModel(temp_model);
                ui->tbvTempDatas->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                ui->tbvTempDatas->setSelectionBehavior(QAbstractItemView::SelectRows);
            }

/*
            query.prepare("INSERT INTO temperaturetable VALUES (:id,:temp,:date)");
            query.bindValue(":id", 0);
            query.bindValue(":temp", CurrentTempValue);
            query.bindValue(":date",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            query.exec();
*/
}
 /*
void MainWindow::on_btLimitTempMaxItems_clicked()
{

    QSqlQuery query;
    query.prepare("DROP TRIGGER keep_rows");
    query.exec();

    QString str = QString("CREATE TRIGGER keep_rows AFTER INSERT ON temperaturetable \
                              WHEN (SELECT COUNT(*) FROM temperaturetable) > ")+ ui->leTcTempMaxItems->text();
            str = str + QString("  BEGIN \
                                   DELETE FROM temperaturetable \
                                   WHERE id=(SELECT id FROM temperaturetable ORDER BY date,id LIMIT 1); \
                                   END; \
                                 END; ");
    qDebug()<<str;
    query.prepare(str);
    qDebug()<<query.exec();


}
*/



void MainWindow::on_btAnanlyseFile_clicked()
{
    if((ui->tbvPcFiles->currentIndex().row() < 0) || (ui->tbvPcFiles->currentIndex().column() < 0) )
     {
      QMessageBox fileSlectMsg;
      fileSlectMsg.setText("未选择需要分析的文件，请选择文件后再分析！");
      fileSlectMsg.exec();
      return;
     }
    QString fln=localftpmodel->item(ui->tbvPcFiles->currentIndex().row(),0)->text();
  //  qDebug()<<"fln="<<fln;
    if(fln.isEmpty())
     {
      QMessageBox fileSlectMsg;
      fileSlectMsg.setText("选择的文件名无效，请重新选择有效文件！");
      fileSlectMsg.exec();
      return;
     }
    QString anfln=ui->lePcDownloadFolder->text()+"/"+fln;
    qDebug()<<"选择分析文件"<<anfln;
    if(analyProgress->is_running==true)
        return;
    analyProgress->FileFullName=anfln;
    analysemodel->clear();
    QStringList titles;
    titles.push_back("事件发生时间");
    titles.push_back("事件内容");
    analysemodel->setHorizontalHeaderLabels(titles);
    analyProgress->isStop=false;
    analyProgress->start();
   // analysefile(anfln);
    /*
    QMessageBox msgbx;
    msgbx.setText("报警分析功能未完善，请联系供应商");
    msgbx.exec();
    */
    QString msg="开始分析日志报告，分析文件名为'"+fln+"'";
    const_msg(msg);
}
//删除所选文件
void MainWindow::on_btGenTableVeiw_clicked()
{
    if((ui->tbvPcFiles->currentIndex().row() < 0) || (ui->tbvPcFiles->currentIndex().column() < 0) )
     {
      QMessageBox fileSlectMsg;
      fileSlectMsg.setText("未选择需要删除的文件，请选择文件后再删除！");
      fileSlectMsg.exec();
      return;
     }
    QString fln=localftpmodel->item(ui->tbvPcFiles->currentIndex().row(),0)->text();
    //qDebug()<<"fln="<<fln;
    if(fln.isEmpty())
     {
      QMessageBox fileSlectMsg;
      fileSlectMsg.setText("选择的文件名无效，请重新选择有效文件！");
      fileSlectMsg.exec();
      return;
     }
    QString deletfln=ui->lePcDownloadFolder->text()+"/"+fln;
    QFile removeFile(deletfln);
    removeFile.remove();
    on_btPcFilesRefresh_clicked();
    QString msg="删除了文件‘"+deletfln+"'";
    const_msg(msg);
}

void MainWindow::on_btClearSysLog_clicked()
{
    QSqlQuery query;

    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除数据库中的内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        query.prepare("delete from logtable");
        query.exec();
        on_btSysLogQuerry_clicked();
        QString msg="清除系统日志数据库中的数据";
        const_msg(msg);
    }
    else
    {
        log_model->setQuery("select * from logtable limit 0");
        log_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
        log_model->setHeaderData(1,Qt::Horizontal,QObject::tr("用户"));
        log_model->setHeaderData(2,Qt::Horizontal,QObject::tr("事件信息"));
        log_model->setHeaderData(3,Qt::Horizontal,QObject::tr("时间戳"));
        ui->tbvSysLog->setModel(log_model);
        ui->tbvSysLog->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tbvSysLog->setSelectionBehavior(QAbstractItemView::SelectRows);
    }

}



void MainWindow::on_btExportAnanlyse_clicked()
{
    //线程产生表格测试
    //    saveExcelThead *ExcelSaveThread = new saveExcelThead(this);
   //     connect(ExcelSaveThread,&saveExcelThead::SendProgress,this,&MainWindow::export_analyse_progress);
    ui->prgExportAnalylog->setValue(0);
    if(analyseExcelSaveProgress->isRunning()==false)
     {
        analyseExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "事件日志分析表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(analyseExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        analyseExcelSaveProgress->type=1;
        analyseExcelSaveProgress->sheetTabName="事件日志报表";
        analyseExcelSaveProgress->setModel((QStandardItemModel*)ui->tbvFilesAnanlyse->model());
        analyseExcelSaveProgress->start();
       }
      }
   QString msg="开始导出日志报表";
    ui->tbrFilesLog->append(const_msg(msg));
}


void MainWindow::Open_Excel(QString filename)
{
    if(filename.isEmpty()==false)
  {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                filename+"导出成功，是否打开表格？\n",
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    }
  }
}


//获取文件名
void MainWindow::on_tbvPcFiles_clicked(const QModelIndex &index)
{

}

void MainWindow::on_tbvPcFiles_doubleClicked(const QModelIndex &index)
{
    if((ui->tbvPcFiles->currentIndex().row() < 0) || (ui->tbvPcFiles->currentIndex().column() < 0) )
     {
      return;
     }
    QString fln=localftpmodel->item(ui->tbvPcFiles->currentIndex().row(),0)->text();
    //qDebug()<<"fln="<<fln;
    if(fln.isEmpty())
     {
      return;
     }
    QString openfln=ui->lePcDownloadFolder->text()+"/"+fln;

    QDesktopServices::openUrl(QUrl::fromLocalFile(openfln));
}

void MainWindow::on_btCraftQuerry_clicked()
{
    QString startT,stopT;
    startT=ui->dteCraftStart->text();
    stopT=ui->dteCraftEnd->text();
   // qDebug()<<startT<<stopT;
   // craft_model->setQuery(querry_craftmsg_by_time(startT,stopT));
   // craft_model->setQuery("SELECT date,camx,camy,camz,heightdiff,probefixz,probex1,probey1,probex2,probey2,safetyheight,digdeep,digdiffx,digdiffy,circleradius,tripointdist,cutcomp,pathcutdeep,planelevel,mmspeed,airpressure,usetime,toolusecnt FROM craft WHERE date Between '" + startT + "' AND '" + stopT + "'");
    craft_model->setQuery("SELECT date,camx,camy,camz,heightdiff,probefixz,probex1,probey1,probex2,probey2,safetyheight,digdeep,digdiffx,digdiffy,circleradius,tripointdist,cutcomp,pathcutdeep,planelevel,mmspeed,airpressure,toolusecnt,probebasez,usetime FROM craft WHERE date Between '" + startT + "' AND '" + stopT + "'");
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

void MainWindow::on_btCraftExport_clicked()
{
    ui->prgExportCraft->setValue(0);
    if(craftExcelSaveProgress->isRunning()==false)
     {
        craftExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "加工工艺数据表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(craftExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        craftExcelSaveProgress->type=0;
        craftExcelSaveProgress->sheetTabName="加工工艺记录表";
        craftExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvCraft->model());
        craftExcelSaveProgress->start();
       }
      }
}

void MainWindow::on_btClearCraft_clicked()
{

    QSqlQuery query;

    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("该操作将清除数据库中的数据，请先做好数据导出，避免数据丢失！\n确定要删除数据库中的数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        query.prepare("delete from craft");
        query.exec();
        on_btCraftQuerry_clicked();
        QString msg="清除加工工艺数据库中的数据";
        const_msg(msg);
    }
    else
    {
        QString startT,stopT;
        startT=ui->dteCraftStart->text();
        stopT=ui->dteCraftEnd->text();
       // qDebug()<<startT<<stopT;
       // craft_model->setQuery(querry_craftmsg_by_time(startT,stopT));
       // craft_model->setQuery("SELECT date,camx,camy,camz,heightdiff,probefixz,probex1,probey1,probex2,probey2,safetyheight,digdeep,digdiffx,digdiffy,circleradius,tripointdist,cutcomp,pathcutdeep,planelevel,mmspeed,airpressure,usetime,toolusecnt FROM craft WHERE date Between '" + startT + "' AND '" + stopT + "'");
        craft_model->setQuery("SELECT date,camx,camy,camz,heightdiff,probefixz,probex1,probey1,probex2,probey2,safetyheight,digdeep,digdiffx,digdiffy,circleradius,tripointdist,cutcomp,pathcutdeep,planelevel,mmspeed,airpressure,usetime,toolusecnt,probebasez FROM craft WHERE date Between '" + startT + "' AND '" + stopT + "'");
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


}


void MainWindow::add_one_record_formal(FormalParam param)//配方明细新增一行
{
    QList<QStandardItem*> rowData;
    rowData<<new QStandardItem(param.param_code);
    rowData<<new QStandardItem(param.param_name);
    rowData<<new QStandardItem(param.param_unit);
    rowData<<new QStandardItem(param.param_upper);
    rowData<<new QStandardItem(param.param_value);
    rowData<<new QStandardItem(param.param_lower);
    mesformaldetailmodel->appendRow( rowData );
    ui->tbvFormalDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
void MainWindow::add_one_record_material(Material mat)//物料新增一行
{
    QSqlQuery query;
    query.prepare("INSERT INTO materiallist VALUES (:id,:vid,:pn,:code,:name,:num,:remain,:unit,:feedtime)");
    query.bindValue(":vid", mat.id);
    query.bindValue(":pn", mat.material_PN);
    query.bindValue(":code",mat.material_code);
    query.bindValue(":name",mat.material_name);
    query.bindValue(":num",mat.number);
    query.bindValue(":remain",mat.surplus_num);
    query.bindValue(":unit",mat.unit);
    query.bindValue(":feedtime",mat.feeding_time);
    qDebug()<<"插入物料列表"<<query.exec();

    QList<QStandardItem*> rowData;
    rowData<<new QStandardItem(mat.id);//虚拟ID
    rowData<<new QStandardItem(mat.material_code);//批次
    rowData<<new QStandardItem(mat.material_PN);//编码
    rowData<<new QStandardItem(mat.material_name);//名称
    rowData<<new QStandardItem(mat.number);//上料数量
    rowData<<new QStandardItem(mat.surplus_num);//剩余数量
    rowData<<new QStandardItem(mat.unit);//单位
    rowData<<new QStandardItem(mat.feeding_time);//上料时间
    rowData<<new QStandardItem(mat.loadopreat);//上料操作
    rowData<<new QStandardItem(mat.opreat);//卸料操作
    mesmatlistmodel->appendRow( rowData );
}

void MainWindow::update_record_material()//从数据库中更新物料表格
{
    mesmatlistmodel->removeRows(0,mesmatlistmodel->rowCount()); //删除所有行
    QSqlQuery query;
    QString prepare;
    prepare="SELECT * FROM materiallist";
    qDebug()<<prepare;
    query.prepare(prepare);
    query.exec();
    while(query.next())
    {
         Material dbmat;
         dbmat.id=query.value(1).toString();
         dbmat.material_PN=query.value(2).toString();
         dbmat.material_code=query.value(3).toString();
         dbmat.material_name=query.value(4).toString();
         dbmat.number=query.value(5).toString();
         dbmat.surplus_num=query.value(6).toString();
         dbmat.unit=query.value(7).toString();
         dbmat.feeding_time=query.value(8).toString();
         dbmat.loadopreat="上料";
         dbmat.opreat="卸料";
         QList<QStandardItem*> rowData;
         rowData<<new QStandardItem(dbmat.id);//虚拟ID
         rowData<<new QStandardItem(dbmat.material_code);//批次
         rowData<<new QStandardItem(dbmat.material_PN);//编码
         rowData<<new QStandardItem(dbmat.material_name);//名称
         rowData<<new QStandardItem(dbmat.number);//上料数量
         rowData<<new QStandardItem(dbmat.surplus_num);//剩余数量
         rowData<<new QStandardItem(dbmat.unit);//单位
         rowData<<new QStandardItem(dbmat.feeding_time);//上料时间
         rowData<<new QStandardItem(dbmat.loadopreat);//上料操作
         rowData<<new QStandardItem(dbmat.opreat);//卸料操作
         mesmatlistmodel->appendRow( rowData );
    }
}
/*
void MainWindow::update_record_material()//从数据库中更新物料表格
{
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
        mesmatmodel->removeRows(0,mesmatmodel->rowCount()); //删除所有行
        //

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
*/


void MainWindow::add_one_record_orderlist(OrderList olist)//工单队列新增一行
{
    QList<QStandardItem*> rowData;
    rowData<<new QStandardItem(olist.bill_no);//工单号
    rowData<<new QStandardItem(olist.bill_num);//工单数量
    rowData<<new QStandardItem(olist.unit);//单位
    rowData<<new QStandardItem(olist.bill_state);//工单状态
    mesorderlistmodel->appendRow( rowData );
}

void MainWindow::update_record_orderlist()//从数据库中更新工单表格
{
    QSqlQuery query;
    QString prepare;
    prepare="SELECT orderlist FROM localmatirialorderlist WHERE id=1";
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
        OrderList dbol;
        mesorderlistmodel->removeRows(0,mesorderlistmodel->rowCount()); //删除所有行
        for(pcnt=0;pcnt<dataArray.count();pcnt++)//添加工单列表
        {
            Param=dataArray.at(pcnt).toObject();
            dbol.bill_no=Param.value("bill_no").toString();
            dbol.bill_num=Param.value("bill_num").toString();
            dbol.unit=Param.value("unit").toString();
            dbol.bill_state=Param.value("bill_state").toString();
            add_one_record_orderlist(dbol);
        }
    }
}


//扫码枪链接按钮
void MainWindow::on_btScanConnect_clicked()
{
    ScanSerial->setPortName(m_IniFile->value("ScanComPort").toString());
    qDebug() << "串口名称"<<ScanSerial->portName();
    int scbdrate = m_IniFile->value("ScanComBodrate").toInt();
    qDebug() << "波特率"<<scbdrate;
    switch (scbdrate) {
    case 1200:
        ScanSerial->setBaudRate(QSerialPort::Baud1200); break;
    case 2400:
        ScanSerial->setBaudRate(QSerialPort::Baud2400); break;
    case 4800:
        ScanSerial->setBaudRate(QSerialPort::Baud4800); break;
    case 9600:
        ScanSerial->setBaudRate(QSerialPort::Baud9600); break;
    case 19200:
        ScanSerial->setBaudRate(QSerialPort::Baud19200); break;
    case 38400:
        ScanSerial->setBaudRate(QSerialPort::Baud38400); break;
    case 57600:
        ScanSerial->setBaudRate(QSerialPort::Baud57600); break;
    case 115200:
        ScanSerial->setBaudRate(QSerialPort::Baud115200); break;
    default :
        ScanSerial->setBaudRate(QSerialPort::Baud9600); break;
    }

    int databits=(ui->cbScDataBits->itemText(ui->cbScDataBits->currentIndex())).toInt();
    qDebug()<<"数据位"<<databits;
    switch (databits) {
    case 7:
        ScanSerial->setDataBits(QSerialPort::Data7);break;
    case 8:
        ScanSerial->setDataBits(QSerialPort::Data8);break;
    default :
        ScanSerial->setDataBits(QSerialPort::Data8);break;
    }

    int stopbits=(ui->cbScStopBits->itemText(ui->cbScStopBits->currentIndex())).toInt();
    qDebug()<<"停止位"<<stopbits;
    switch (stopbits) {
    case 1:
        ScanSerial->setStopBits(QSerialPort::OneStop);break;
    case 2:
        ScanSerial->setStopBits(QSerialPort::TwoStop);break;
    default :
        ScanSerial->setStopBits(QSerialPort::OneStop);break;
    }

    int crc=ui->cbScCrc->currentIndex();
    qDebug()<<"校验Index"<<crc;
    switch (crc) {
    case 0:
        ScanSerial->setParity(QSerialPort::OddParity);break;
    case 1:
        ScanSerial->setParity(QSerialPort::EvenParity);break;
    case 2:
        ScanSerial->setParity(QSerialPort::NoParity);break;

    default :
        ScanSerial->setParity(QSerialPort::NoParity);break;
    }

    if(!ScanSerial->open(QIODevice::ReadOnly))
        {
            qDebug() << "扫码枪端口打开失败！";
            scConOK=false;
            ui->lbScComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
        }
        else
        {
        qDebug() << "扫码枪端口打开成功！";
        scConOK=true;
        ui->lbScComStatus->setStyleSheet("QLabel{background-color: rgb(85, 255, 0);}");
        }

}

void MainWindow::on_btScanDisConnect_clicked()
{
    ScanSerial->close();
    scConOK=false;
    ui->chbScAutoConnect->setChecked(false);
    ui->lbScComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
   // ReportCellOut cellout;
   // postProductRecordList(cellout);
}


void MainWindow::update_mes_link_state(bool state)//更新mes链接图标指示
{
    if(state){
        ui->lbMesComStatus->setStyleSheet("QLabel{background-color: rgb(85, 255, 0);}");
        mesConOK=true;
    }
    else
    {
        ui->lbMesComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
        mesConOK=false;
    }

}

void MainWindow::on_btFormalReq_clicked()
{
    postFormalOK=false;
    //FormalCnt=m_IniFile->value("MesMaxCnt").toInt();
    FormalCnt=0;
    postFormal();
}

//点击执行后执行的动作
void MainWindow::on_tbvFormal_clicked(const QModelIndex &index)
{
    QAbstractItemModel *Imodel=ui->tbvFormal->model();
    QModelIndex Iindex = Imodel->index(index.row(),index.column());  //index.row()为算选择的行号。1为所选中行的第一列。。
    QString name=Imodel->data(Iindex).toString();        //name即为所选择行的第一列的值
    QString id = Imodel->data(Imodel->index(index.row(),0)).toString();
    qDebug()<<name<<id;
    if((logonPrivIdx>=4) && (lncAlarmValueR==1))
    {
        if(name=="执行")
        {
            QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                    tr("该操作将选中配方数据更新到LNC中！\n确定要更新LNC的工艺数据吗？\nYes-更新\nNo-取消\n"),
                                                                    QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::No);
            if (resBtn == QMessageBox::Yes) {
            postFormalVerifyOK=false;
           // FormalCnt=m_IniFile->value("MesMaxCnt").toInt();
            FormalCnt=0;//手动触发
            postFormalVerify(id);
            }

        }//执行
    }
    else
    {
        if(name=="执行"){
        QMessageBox::warning(this, "LNC权限管理及报表", "该操作需要管理员权限或LNC不在空闲状态！"
               , QMessageBox::Ok, QMessageBox::NoButton);}
    }

}


void MainWindow::slotContextMenu(QPoint pos)//配方表右键删除槽
{
    auto index = ui->tbvFormal->indexAt(pos);
        if (index.isValid())
        {
            popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
        }
}

void MainWindow::on_btClearFormal_clicked()//清空配方库
{
    QSqlQuery query;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("该操作将清除本地数据库中的配方数据数据！\n确定要删除数据库中的数据吗？\nYes-清空\nNo-取消\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        query.prepare("delete from localformaltable");
        query.exec();
        query.prepare("DROP TABLE  localformaltable");
        query.exec();
        //构建创建本地配方表的sql语句字符串
        QString str = QString("CREATE TABLE localformaltable (\
                              id INTEGER PRIMARY KEY AUTOINCREMENT,\
                              formulano TEXT NOT NULL,\
                              version TEXT NOT NULL,\
                              productno TEXT NOT NULL,\
                              productname TEXT NOT NULL,\
                              dataparm TEXT,\
                              dateget TEXT,\
                              daterun TEXT,\
                              usestatus TEXT,\
                              opreate TEXT)");
        //执行sql语句
        query.exec(str);
        QString msg="清除本地配方库在数据库中的数据";
        const_msg(msg);
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
    ui->tbvFormal->setModel(meslocalformalmodel);
    /*
    int idx=0;
    int row =ui->tbvFormal->model()->rowCount();
    int colum=ui->tbvFormal->model()->columnCount();
    QStandardItemModel *Imodel=ui->tbvFormal->model();
    for(idx=0;idx<row;idx++)
        Imodel->item(idx,colum)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
*/
}


void MainWindow::mesTxtMsg(QString msg)//MES日志明细
{
    QString dtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString result;
    result ="******" + dtime + "::当前用户::'" + logonUser +"'::"+msg;
    ui->tbrMeslogs->append(result);
}
void MainWindow::update_billInfo()//更新工单信息
{
    ui->leOrderNumR->setText( m_IniFile->value("MesBillInfo").toString());
    ui->leOrderCntR->setText( m_IniFile->value("MesBillNum").toString());
}


void MainWindow::on_btManualGetInfo_clicked()
{
    postGetBillInfoOK=false;
    //GetBillInfoCnt=m_IniFile->value("MesMaxCnt").toInt();
    GetBillInfoCnt=0;
    postGetBillInfo();

}

void MainWindow::on_btOrderList_clicked()
{
    postGetBillInfoListOK=false;
   // GetBillInfoListCnt=m_IniFile->value("MesMaxCnt").toInt();
    GetBillInfoListCnt=0;
    postGetBillInfoList();

}

void MainWindow::on_btUpateMat_clicked()//更新物料
{
    postGetEPMaterialFeedingOK=false;
   // GetEPMaterialFeedCnt=m_IniFile->value("MesMaxCnt").toInt();
    GetEPMaterialFeedCnt=0;//手动触发1
    postGetEPMaterialFeeding();

}

void MainWindow::update_upload_material(Material mat)//上料成功后，更新物料
{

    db_update_matirial_num_ft(mat);
    update_record_material();
    ui->leMatBatNumR->setText(mat.material_code);//物料编码
    ui->leLoadCntR->setText(mat.number);//上料数量
    ui->leOrderNumR->setText(mat.bill_no);
    ui->leOrderCntR->setText(mat.bill_num);
}
void MainWindow::update_unload_material(Material mat)//卸料成功后，更新物料
{
    Material mattmp=mat;
    mattmp.material_code=ui->leMatBatNumR->text();
    db_update_matirial_remain(mattmp);
    update_record_material();
}

void MainWindow::on_tbvMaterials_clicked(const QModelIndex &index)//物料表格被点击
{
    QAbstractItemModel *Imodel=ui->tbvMaterials->model();
    QModelIndex Iindex = Imodel->index(index.row(),index.column());  //index.row()为算选择的行号。1为所选中行的第一列。。
    QString name=Imodel->data(Iindex).toString();        //name即为所选择行的第一列的值
    QString id = Imodel->data(Imodel->index(index.row(),0)).toString();
    qDebug()<<name<<id;
      ui->leMatBatNumR->setText(Imodel->data(Imodel->index(index.row(),1)).toString());//物料编码
      ui->leLoadCntR->setText(Imodel->data(Imodel->index(index.row(),4)).toString());//上料数量
    if(logonPrivIdx>=4)  {
    if(name=="上料")//卸料操作
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                    tr("该操作将物料数据上传MES进行上料校验！\n确定要上料吗？\nYes-上料\nNo-取消\n"),
                                                                    QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::No);
        if (resBtn == QMessageBox::Yes) {

            LoadMaterial mat;
            mat.material_code =  Imodel->data(Imodel->index(index.row(),2)).toString();
            mat.number =  Imodel->data(Imodel->index(index.row(),4)).toString();
            postMaterialFeedingVerifityOK=false;
           // MaterialFeedingCnt=m_IniFile->value("MesMaxCnt").toInt();
            MaterialFeedingCnt=0;//手动触发
            postMaterialFeedingVerifity(mat);
        }
    }
    if(name=="卸料")//卸料操作
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                    tr("该操作将物料数据上传MES进行卸料校验！\n确定要卸料吗？\nYes-卸料\nNo-取消\n"),
                                                                    QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::No);
        if (resBtn == QMessageBox::Yes) {
            LoadMaterial mat;
            mat.material_code =  Imodel->data(Imodel->index(index.row(),2)).toString();
            mat.id =  Imodel->data(Imodel->index(index.row(),0)).toString();

            postMaterialUnloadingVerifityOK=false;
            //MaterialUnloadCnt=m_IniFile->value("MesMaxCnt").toInt();
            MaterialUnloadCnt=0;
            postMaterialUnloadingVerifity(mat);
        }
    }
   }//priv
    else
    {
        if(name=="卸料" || name=="上料"){
        QMessageBox::warning(this, "LNC权限管理及报表", "该操作需要管理员权限！"
               , QMessageBox::Ok, QMessageBox::NoButton);}
    }

}







void MainWindow::on_btFanSpdQuerry_clicked()
{
//查询风速
    QString startT,stopT;
    startT=ui->dteFanSpdStart->text();
    stopT=ui->dteFanSpdEnd->text();
   // QString str="SELECT spd,date FROM fanspeedtable where date Between '" + startT + "' AND '" + stopT + "'";
    QString str="SELECT spd,date FROM fanspeedtable where date Between '" + startT + "' AND '" + stopT + "'";
    fanspd_model->setQuery(str);
   // temp_model->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
    fanspd_model->setHeaderData(0,Qt::Horizontal,QObject::tr("风速值"));
    fanspd_model->setHeaderData(1,Qt::Horizontal,QObject::tr("时间戳"));
    ui->tbvFanSpdDatas->setModel(fanspd_model);
    ui->tbvFanSpdDatas->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tbvFanSpdDatas->setSelectionBehavior(QAbstractItemView::SelectRows);
   // ui->lbCellIntoNum=fanspd_model->rowCount();
}

void MainWindow::on_btFanSpdExport_clicked()
{
    ui->prgExportFanSpdData->setValue(0);
    if(spdExcelSaveProgress->isRunning()==false)
     {
        spdExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "吸尘器风速表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(spdExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        spdExcelSaveProgress->type=0;
        spdExcelSaveProgress->sheetTabName="吸尘器风速记录表";
        spdExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvFanSpdDatas->model());
        spdExcelSaveProgress->start();
       }
      }
}

void MainWindow::on_btEnterLogExport_clicked()
{
    ui->prgExportEnterLog->setValue(0);
    if(cellInExcelSaveProgress->isRunning()==false)
     {
        cellInExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "进站日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(cellInExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        cellInExcelSaveProgress->type=0;
        cellInExcelSaveProgress->sheetTabName="进站日志表";
        cellInExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvEnterLogs->model());
        cellInExcelSaveProgress->start();
       }
      }
}



void MainWindow::on_btExitLogExport_clicked()
{
    ui->prgExportExitlog->setValue(0);
    if(cellOutExcelSaveProgress->isRunning()==false)
     {
        cellOutExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "出站日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(cellOutExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        cellOutExcelSaveProgress->type=0;
        cellOutExcelSaveProgress->sheetTabName="出站日志表";
        cellOutExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvExitLogs->model());
        cellOutExcelSaveProgress->start();
       }
      }
}

void MainWindow::on_btNGlogsExport_clicked()
{
    ui->prgExportNGlog->setValue(0);
    if(NGExcelSaveProgress->isRunning()==false)
     {
        NGExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "不良品日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(NGExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        NGExcelSaveProgress->type=0;
        NGExcelSaveProgress->sheetTabName="不良品日志表";
        NGExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvNotGood->model());
        NGExcelSaveProgress->start();
       }
      }
}

void MainWindow::on_btClearFanSpd_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除数据库中风速数据内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        query.prepare("delete from fanspeedtable");
        query.exec();
        on_btFanSpdQuerry_clicked();
        QString msg="清除风速表在数据库中的数据";
        const_msg(msg);
    }
    else
    {
        fanspd_model->setQuery("select spd,date from fanspeedtable limit 0");
        fanspd_model->setHeaderData(0,Qt::Horizontal,QObject::tr("风速值"));
        fanspd_model->setHeaderData(1,Qt::Horizontal,QObject::tr("时间戳"));
        ui->tbvFanSpdDatas->setModel(fanspd_model);
        ui->tbvFanSpdDatas->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tbvFanSpdDatas->setSelectionBehavior(QAbstractItemView::SelectRows);
    }
}








void MainWindow::on_btScanLogExport_clicked()
{
    ui->prgExportScanLog->setValue(0);
    if(ScanExcelSaveProgress->isRunning()==false)
     {
        ScanExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "扫码日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(ScanExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        ScanExcelSaveProgress->type=0;
        ScanExcelSaveProgress->sheetTabName="扫码日志表";
        ScanExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvScanLogs->model());
        ScanExcelSaveProgress->start();
       }
      }
}


void MainWindow::on_btFinishLogExport_clicked()
{
    ui->prgExportFinishLog->setValue(0);
    if(FinishExcelSaveProgress->isRunning()==false)
     {
        FinishExcelSaveProgress->xlsFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "加工完成日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("excel (*.xlsx)"));
       if(FinishExcelSaveProgress->xlsFile.isEmpty()==false)
       {
        FinishExcelSaveProgress->type=0;
        FinishExcelSaveProgress->sheetTabName="加工完成日志表";
        FinishExcelSaveProgress->setModel((QSqlQueryModel*)ui->tbvFinishLogs->model());
        FinishExcelSaveProgress->start();
       }
      }
}

void MainWindow::on_btFinishLogQuerry_clicked()
{
    //加工完成查询
        QString startT,stopT;
        startT=ui->dteFinishLogStart->text();
        stopT=ui->dteFinishLogEnd->text();

        finish_model->setQuery("select id,processid,equmentid,operater,barcode,ordernum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,paramseven,detail,datein,date from finishtable where date between '"+startT+"' and '"+stopT +"'");

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

void MainWindow::on_btClearScanLog_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除数据库中扫码数据内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        query.prepare("delete from scantable");
        query.exec();
        on_btScanLogQuerry_clicked();
        QString msg="清除扫码表在数据库中的数据";
        const_msg(msg);
    }
    else
    {
        on_btScanLogQuerry_clicked();
    }
}

void MainWindow::on_btClearFinishLog_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除数据库中加工完成数据内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        query.prepare("delete from finishtable");
        query.exec();
        on_btFinishLogQuerry_clicked();
        QString msg="清除加工完成表在数据库中的数据";
        const_msg(msg);
    }
    else
    {
        on_btFinishLogQuerry_clicked();
    }
}

void MainWindow::on_btScanLogQuerry_clicked()
{
    //扫码查询
        QString startT,stopT;
        startT=ui->dteScanLogStart->text();
        stopT=ui->dteScanLogEnd->text();

        scan_model->setQuery("select id,processid,equmentid,barcode,ordernum,result,detail,date from scantable where date between '"+startT+"' and '"+stopT +"'");
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

       // report_cellIn_Log();
}

void MainWindow::report_cellIn_Log()//上传进站数据，扫码验证成功，且未上传的数据
{
    ReportCellIn CellIn;
    QSqlQuery query;
    QString str="select id,processid,equmentid,barcode,ordernum,result,detail,date from scantable where detail='未上传' and result!='扫码失败' limit 1";//获取一条未上传数据，且扫码验证成功的数据
   // QString str="select id,processid,equmentid,barcode,ordernum,result,detail,date from scantable where detail='未上传'  limit 1";//获取一条未上传数据，且扫码验证成功的数据
    query.prepare(str);
    query.exec();
    while(query.next())//将读取到的配方数据结构
    {
        CellIn.id=query.value(0).toInt();
        CellIn.processid=query.value(1).toString();
        CellIn.equmentid=query.value(2).toString();
        CellIn.barcode=query.value(3).toString();
        CellIn.ordernum=query.value(4).toString();
        CellIn.result=query.value(5).toString();
        CellIn.date=query.value(7).toString();
        //qDebug()<<CellIn.id<<CellIn.processid<<CellIn.equmentid<<CellIn.barcode;
       // update_CellIn_Log(CellIn);
        CellInUsed=CellIn;
        postCellIntoVerifityOK=false;
        CellIntolCnt=m_IniFile->value("MesMaxCnt").toInt();
        postCellIntoVerifity(CellIn);
    }
}

void MainWindow::report_cellOut_Log()//上传出站数据,条码有效的才上传
{
    ReportCellOut CellOut;
    QSqlQuery query;
    QString str="select id,processid,equmentid,barcode,ordernum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,paramseven,detail,date from finishtable where detail='未上传' and barcode!='无效条码' limit 1";//获取一条未上传数据
    query.prepare(str);
    query.exec();
    while(query.next())//将读取到的配方数据结构
    {
        CellOut.id=query.value(0).toInt();
        CellOut.processid=query.value(1).toString();
        CellOut.equmentid=query.value(2).toString();
        CellOut.barcode=query.value(3).toString();
        CellOut.billno=query.value(4).toString();
        CellOut.param1=query.value(5).toString();
        CellOut.param2=query.value(6).toString();
        CellOut.param3=query.value(7).toString();
        CellOut.param4=query.value(8).toString();
        CellOut.param5=query.value(9).toString();
        CellOut.param6=query.value(10).toString();
        CellOut.param7=query.value(11).toString();
        CellOut.date=query.value(13).toString();
        CellOutUsed =CellOut;
        //qDebug()<<CellIn.id<<CellIn.processid<<CellIn.equmentid<<CellIn.barcode;
       // update_CellIn_Log(CellIn);
        postProductRecordListOK=false;
        ProductRecordCnt=m_IniFile->value("MesMaxCnt").toInt();
        postProductRecordList(CellOut);
    }
}
void MainWindow::report_NG_Log()//上传不良品
{
    NGmsg nglog;
    QSqlQuery query;
    QString str="select id,processid,equmentid,barcode,badcode,badnum,detail,upload,date from ngtable where upload='未上传' limit 1";//获取一条未上传数据
    query.prepare(str);
    query.exec();
    while(query.next())//将读取到的配方数据结构
    {
        nglog.id=query.value(0).toInt();
        nglog.processid=query.value(1).toString();
        nglog.equmentid=query.value(2).toString();
        nglog.barcode=query.value(3).toString();
        nglog.badcode=query.value(4).toString();
        nglog.badnum=query.value(5).toString();
        nglog.detail=query.value(6).toString();
        nglog.upload=query.value(7).toString();
        nglog.datetime=query.value(8).toString();
        nglogUsed =nglog;
        //qDebug()<<CellIn.id<<CellIn.processid<<CellIn.equmentid<<CellIn.barcode;
       // update_CellIn_Log(CellIn);
        postSaveRejectRecordListOK=false;
        RejectRecordCnt=m_IniFile->value("MesMaxCnt").toInt();
        postSaveRejectRecordList(nglog);
    }
}
//断网自动上传日志
void MainWindow::autopostprg()
{
    if(mesConOK)
    {
        report_cellIn_Log();
        report_cellOut_Log();
        report_NG_Log();
    }
}


void MainWindow::on_btExitLogQuerry_clicked()
{
    //出站查询
        QString endtime=ui->dteExitLogEnd->text();
        QString starttime=ui->dteExitLogStart->text();
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
        finish_model->setHeaderData(9,Qt::Horizontal,QObject::tr("风压"));
        finish_model->setHeaderData(10,Qt::Horizontal,QObject::tr("表面平整度"));
        finish_model->setHeaderData(11,Qt::Horizontal,QObject::tr("详情"));
        finish_model->setHeaderData(12,Qt::Horizontal,QObject::tr("日期时间"));

        ui->tbvFinishLogs->setModel(finish_model);
        ui->tbvFinishLogs->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tbvFinishLogs->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->lbFinishNum->setText(QString::number(finish_model->rowCount()));

}

void MainWindow::on_btEnterLogQuerry_clicked()
{
    //进站站查询
        QString endtime=ui->dteEnterLogEnd->text();
        QString starttime=ui->dteEnterLogStart->text();
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

void MainWindow::on_btClearEnterLog_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除数据库中进站数据内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        query.prepare("delete from logintable");
        query.exec();
        on_btEnterLogQuerry_clicked();
        QString msg="清除进站日志表在数据库中的数据";
        const_msg(msg);
    }
    else
    {
        on_btEnterLogQuerry_clicked();
    }
}

void MainWindow::on_btClearExitLog_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除数据库中出站数据内容！\n清除前请做好数据导出，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        query.prepare("delete from logouttable");
        query.exec();
        on_btExitLogQuerry_clicked();
        QString msg="清除出站日志表在数据库中的数据";
        const_msg(msg);
    }
    else
    {
        on_btExitLogQuerry_clicked();
    }
}

void MainWindow::on_btAddNGrecord_clicked()
{
    //从加工完成库中，查询扫码，获取对应信息，并录入
    NGmsg nglog;
    QSqlQuery query;
    query.prepare("select processid,equmentid,barcode,date from finishtable where barcode='"+ui->leNGbarCode->text()+"'");
    query.exec();
    if(query.next())
    {
        nglog.processid=query.value(0).toString();
        nglog.equmentid=query.value(1).toString();
        nglog.barcode=query.value(2).toString();
        nglog.datetime=query.value(3).toString();
        nglog.badcode="NC-001-0"+QString::number(ui->cmbNGreason->currentIndex()+1);
        nglog.badnum="1";
        nglog.detail=ui->cmbNGreason->currentText();
        nglog.upload="未上传";
        add_one_ng_record(nglog);
        QString msg;
        msg="录入不良品成功，条码值为："+nglog.badcode;
        const_msg(msg);

        while(query.next())//将读取到的配方数据结构
        {
            nglog.processid=query.value(0).toString();
            nglog.equmentid=query.value(1).toString();
            nglog.barcode=query.value(2).toString();
            nglog.datetime=query.value(3).toString();
            nglog.badcode=m_IniFile->value("mesNGLeadCode").toString()+QString::number(ui->cmbNGreason->currentIndex()+1);
            nglog.badnum="1";
            nglog.detail=ui->cmbNGreason->currentText();
            nglog.upload="未上传";
            add_one_ng_record(nglog);
            QString msg;
            msg="录入不良品成功，条码值为："+nglog.badcode;
            const_msg(msg);
        }
    }
    else
    {
        QString dlgTitle = "不良品录入失败消息框";
        QString strInfo = "加工完成库中无对应条码！不可录入！";
        QMessageBox::warning(this, dlgTitle, strInfo
               , QMessageBox::Ok, QMessageBox::NoButton);

    }

}

void MainWindow::on_btDeletNGrecord_clicked()
{
    if(ngDelet.id<1)
    {
        QString dlgTitle = "删除所选不良品记录";
        QString strInfo = "未选择要删除的条目！";
        QMessageBox::warning(this, dlgTitle, strInfo
               , QMessageBox::Ok, QMessageBox::NoButton);
    }
    else
    {
        QSqlQuery query;
        QString sqlprepare;
        sqlprepare="DELETE FROM ngtable WHERE id="+QString::number(ngDelet.id);
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
}

void MainWindow::on_btNGLogQuerry_clicked()
{
    QString endtime=ui->dteEnterLogEnd->text();
    QString starttime=ui->dteEnterLogStart->text();

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

void MainWindow::on_btClearNGdata_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("该操作将清除本地数据库中的不良品数据数据！\n确定要删除数据库中的数据吗？\nYes-清空\nNo-取消\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        query.prepare("delete from ngtable");
        query.exec();
        query.prepare("DROP TABLE  ngtable");
        query.exec();

     //构建创建不良品日志表的sql语句字符串
          QString   str = QString("CREATE TABLE ngtable (\
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,\
                                      processid TEXT ,\
                                      equmentid TEXT ,\
                                      barcode TEXT,\
                                      badcode TEXT,\
                                      badnum TEXT,\
                                      detail TEXT,\
                                      upload TEXT,\
                                      date TEXT NOT NULL)");
                //执行sql语句
                query.exec(str);
        //执行sql语句
        query.exec(str);
        QString msg="清除不良品数据库中的数据";
        const_msg(msg);
    }
    on_btNGLogQuerry_clicked();
}

void MainWindow::on_tbvNotGood_clicked(const QModelIndex &index)//查找ID
{
    QAbstractItemModel *Imodel=ui->tbvNotGood->model();
    QModelIndex Iindex = Imodel->index(index.row(),index.column());  //index.row()为算选择的行号。1为所选中行的第一列。。
    QString name=Imodel->data(Iindex).toString();        //name即为所选择行的第一列的值
    QString id = Imodel->data(Imodel->index(index.row(),0)).toString();
    qDebug()<<name<<id;
    ngDelet.id=id.toInt();
}



void MainWindow::on_btClearMesLogs_clicked()
{
    QMessageBox::StandardButton clearBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("清除操作将清除MES收发日志数据内容！\n清除前请做好数据保存，避免数据丢失!\n确定要清除数据吗？\nYes-清除数据库数据\nNo-清空表格内容\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (clearBtn == QMessageBox::Yes) {
        ui->tbrMeslogs->clear();
        QString msg="清除加工完成表在数据库中的数据";
        const_msg(msg);
    }

}

void MainWindow::on_btSaveMesLogs_clicked()
{
    QString logFile = QFileDialog::getSaveFileName(this, tr("保存文件"), "./MES接口数据日志"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".txt", tr("txt (*.txt)"));
    QFile file(logFile);
    if(file.fileName().isEmpty())
        return;
    if(!file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("保存文件"),tr("打开文件 %1 失败，无法保存\n%2").arg(logFile).arg(file.errorString()),QMessageBox::Ok);
       return;
    }
    QTextStream out(&file);
    out<<ui->tbrMeslogs->toPlainText();
    file.close();
}






//导出进站日志CSV文件

void MainWindow::on_btCellInExport_clicked()
{
    if(saveCellInLogsThread->isRunning()==false)
     {
        saveCellInLogsThread->saveFileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "进站验证接口日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("CSV (*.csv)"));
       if(saveCellInLogsThread->saveFileName.isEmpty()==false)
       {
        saveCellInLogsThread->setModel((QSqlQueryModel*)ui->tbvcellin->model());
        saveCellInLogsThread->start();
       }
      }

}
//导出出站日志表
void MainWindow::on_btCellOutExport_clicked()
{
    if(saveCellOutLogsThread->isRunning()==false)
     {
        saveCellOutLogsThread->saveFileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "出站接口日志表"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("CSV (*.csv)"));
       if(saveCellOutLogsThread->saveFileName.isEmpty()==false)
       {
        saveCellOutLogsThread->setModel((QSqlQueryModel*)ui->tbvcellout->model());
        saveCellOutLogsThread->start();
       }
      }
}
//导出其他接口日志csv
void MainWindow::on_btInterfaceExport_clicked()
{
    if(saveOthersLogsThread->isRunning()==false)
     {
        saveOthersLogsThread->saveFileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "其他接口日志表-"+ ui->cmbInterface->currentText() +QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"), tr("CSV (*.csv)"));
       if(saveOthersLogsThread->saveFileName.isEmpty()==false)
       {
        saveOthersLogsThread->setModel((QSqlQueryModel*)ui->tbvothers->model());
        saveOthersLogsThread->start();
       }
      }
}

//查询其他接口日志
void MainWindow::on_btInterfaceQuerry_clicked()
{

    QString endtime=ui->dteInterfaceEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=ui->dteInterfaceStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");



    if(ui->cmbInterface->currentIndex()==0 && ui->cmbInterfaceFilter->currentIndex()==0)
        othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbInterface->currentIndex()!=0 && ui->cmbInterfaceFilter->currentIndex()==0)
        othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type="+QString::number(ui->cmbInterface->currentIndex()) +" AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbInterface->currentIndex()==0 && ui->cmbInterfaceFilter->currentIndex()==1)
        othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbInterface->currentIndex()==0 && ui->cmbInterfaceFilter->currentIndex()==2)
        othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where retmsg='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbInterface->currentIndex()!=0 && ui->cmbInterfaceFilter->currentIndex()==1)
        othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type="+QString::number(ui->cmbInterface->currentIndex()) +" AND retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbInterface->currentIndex()!=0 && ui->cmbInterfaceFilter->currentIndex()==2)
        othersmodel->setQuery("select reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,postjson,respjson from otherlogs where type="+QString::number(ui->cmbInterface->currentIndex()) +" AND retmsg='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");



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

//查询进站接口日志表
void MainWindow::on_btCellInQuerry_clicked()
{
    QString endtime=ui->dteCellInEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=ui->dteCellInStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(ui->cmbCellInFilter->currentIndex()==0)
        cellinmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,billno,billnum,special,postjson,respjson from cellinlogs where reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbCellInFilter->currentIndex()==1)
        cellinmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,billno,billnum,special,postjson,respjson from cellinlogs where retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbCellInFilter->currentIndex()==2)
        cellinmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,retcode,retmsg,billno,billnum,special,postjson,respjson from cellinlogs where retmsg='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
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

//查询出站接口日志表
void MainWindow::on_btCellOutQuerry_clicked()
{
    QString endtime=ui->dteCellOutEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=ui->dteCellOutStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    if(ui->cmbCellOutFilter->currentIndex()==0)
        celloutmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,billno,opreater,steps,materialdec,productnum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,retcode,retmsg,materialremain,ngcodeone,ngdetailone,ngcodetwo,ngdetailtwo,postjson,respjson from celloutlogs where reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbCellOutFilter->currentIndex()==1)
        celloutmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,billno,opreater,steps,materialdec,productnum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,retcode,retmsg,materialremain,ngcodeone,ngdetailone,ngcodetwo,ngdetailtwo,postjson,respjson from celloutlogs where retmsg!='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
    if(ui->cmbCellOutFilter->currentIndex()==2)
        celloutmodel->setQuery("select barcode,reqtime,usedtime,resptime,processid,equmentid,billno,opreater,steps,materialdec,productnum,paramone,paramtwo,paramthree,paramfour,paramfive,paramsix,retcode,retmsg,materialremain,ngcodeone,ngdetailone,ngcodetwo,ngdetailtwo,postjson,respjson from celloutlogs where retmsg='超时' AND reqtime between '"+starttime+".000' and '"+endtime +".999'");
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



void MainWindow::on_btCellInClear_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("该操作将清除本地数据库中的进站验证接口日志数据！\n确定要删除数据库中的数据吗？\nYes-清空\nNo-取消\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        query.prepare("delete from cellinlogs");
        query.exec();
        query.prepare("DROP TABLE  cellinlogs");
        query.exec();
        //构建创建本地配方表的sql语句字符串
         //进站接口日志表,条码 请求时间，花费时间，响应时间，工序号，设备号，返回码，返回信息，工单号，工单数量，特殊值，发送代码，返回代码
        QString  str = QString("CREATE TABLE cellinlogs (\
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,\
                                      barcode TEXT,\
                                      reqtime TEXT,\
                                      usedtime TEXT,\
                                      resptime TEXT,\
                                      processid TEXT,\
                                      equmentid TEXT,\
                                      retcode TEXT,\
                                      retmsg TEXT,\
                                      billno TEXT,\
                                      billnum TEXT,\
                                      special TEXT,\
                                      postjson TEXT,\
                                      respjson TEXT)");
        //执行sql语句
        query.exec(str);
        QString msg="清除本地进站接口日志在数据库中的数据";
        const_msg(msg);
    }
    QString endtime=ui->dteCellInEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=ui->dteCellInStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");

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


void MainWindow::on_btCellOutClear_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("该操作将清除本地数据库中的出站生产履历接口日志数据！\n确定要删除数据库中的数据吗？\nYes-清空\nNo-取消\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        query.prepare("delete from celloutlogs");
        query.exec();
        query.prepare("DROP TABLE  celloutlogs");
        query.exec();
        //构建创建本地配方表的sql语句字符串
         //进站接口日志表,条码 请求时间，花费时间，响应时间，工序号，设备号，返回码，返回信息，工单号，工单数量，特殊值，发送代码，返回代码
        QString  str = QString("CREATE TABLE celloutlogs (\
                               id INTEGER PRIMARY KEY AUTOINCREMENT,\
                               barcode TEXT,\
                               reqtime TEXT,\
                               usedtime TEXT,\
                               resptime TEXT,\
                               processid TEXT,\
                               equmentid TEXT,\
                               billno TEXT,\
                               opreater TEXT,\
                               steps TEXT,\
                               materialdec TEXT,\
                               productnum TEXT,\
                               paramone TEXT,\
                               paramtwo TEXT,\
                               paramthree TEXT,\
                               paramfour TEXT,\
                               paramfive TEXT,\
                               paramsix TEXT,\
                               retcode TEXT,\
                               retmsg TEXT,\
                               materialremain TEXT,\
                               ngcodeone TEXT,\
                               ngdetailone TEXT,\
                               ngcodetwo TEXT,\
                               ngdetailtwo TEXT,\
                               postjson TEXT,\
                               respjson TEXT)");
        //执行sql语句
        query.exec(str);
        QString msg="清除本地出站生产履历接口日志在数据库中的数据";
        const_msg(msg);
    }
    QString endtime=ui->dteCellOutEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=ui->dteCellOutStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");
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

void MainWindow::on_btInterfaceClear_clicked()
{
    QSqlQuery query;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "LNC权限管理及报表",
                                                                tr("该操作将清除本地数据库中的其他接口日志数据！\n确定要删除数据库中的数据吗？\nYes-清空\nNo-取消\n"),
                                                                QMessageBox::QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        query.prepare("delete from otherlogs");
        query.exec();
        query.prepare("DROP TABLE  otherlogs");
        query.exec();

        QString  str = QString("CREATE TABLE otherlogs (\
                               id INTEGER PRIMARY KEY AUTOINCREMENT,\
                               type INTERGER,\
                               reqtime TEXT,\
                               usedtime TEXT,\
                               resptime TEXT,\
                               processid TEXT,\
                               equmentid TEXT,\
                               retcode TEXT,\
                               retmsg TEXT,\
                               postjson TEXT,\
                               respjson TEXT)");
        //执行sql语句
        query.exec(str);
        QString msg="清除本地其他接口日志在数据库中的数据";
        const_msg(msg);
    }
    QString endtime=ui->dteInterfaceEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString starttime=ui->dteInterfaceStart->dateTime().toString("yyyy-MM-dd hh:mm:ss");

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










