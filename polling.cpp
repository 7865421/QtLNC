#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<QStandardPaths>
#include<QSqlQuery>
#include "keymonitor.h"

void MainWindow::lncpolling()
{
    //-----呼叫函式庫主程序
    sc2.MainProcess();

    //检查连线状态
       LncConnStatus = sc2.GetConnectionMsg(ServerIdx, SCIF_CONNECT_STATE);
       switch (LncConnStatus)
       {
       case SC_CONN_STATE_DISCONNECT:
             ui->lbLncConnectStatus->setText("断开连接！");
             ui->lbLncComStatus->setStyleSheet("QLabel{background-color: rgb(170, 170, 255);}");
             break;
       case SC_CONN_STATE_CONNECTING:
             ui->lbLncConnectStatus->setText("连接中...！");
             ui->lbLncComStatus->setStyleSheet("QLabel{background-color: rgb(0, 170, 127);}"); break;
       case SC_CONN_STATE_FAIL:
             ui->lbLncConnectStatus->setText("连接失败！");
             ui->lbLncComStatus->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
              break;
       case SC_CONN_STATE_OK:
             ui->lbLncConnectStatus->setText("连接成功！");
             ui->lbLncComStatus->setStyleSheet("QLabel{background-color: rgb(85, 255, 0);}"); break;
       case SC_CONN_STATE_NORESPONSE:
             ui->lbLncConnectStatus->setText("连接无反应！");
             ui->lbLncComStatus->setStyleSheet("QLabel{background-color: rgb(255, 255, 0);}"); break;
       }

        if(LncConnStatus==SC_CONN_STATE_OK)
            lncConOK=true;
        else
            lncConOK=false;

      if(lncConOK)
     {
       //----检查档案传输
       //----FTP 状态显示
       unsigned int i=0;
       int rt = sc2.FtpCheckDone();

      if(sc_FtpAction==FTP_Action_DOWNLOAD_FILE)
      {
       ui->progressBar->setMaximum(sc2.GetLibraryMsg(SCIF_FTP_TOTAL_PACKAGE));
       ui->progressBar->setValue(sc2.GetLibraryMsg(SCIF_FTP_CURRENT_PACKAGE));
      }

       int ftpResult = sc2.GetLibraryMsg (SCIF_FTP_RESULT);
       int ftpStatus = sc2.GetLibraryMsg (SCIF_FTP_STATE);

            switch (ftpResult)
             {
               case FTP_RESULT_IDLE:
                    ui->lbFtpStatus->setText("空闲");break;
               case FTP_RESULT_PROCESSING:
                    ui->lbFtpStatus->setText("进行中");break;
               case FTP_RESULT_SUCCESS:
                    ui->lbFtpStatus->setText("FTP操作完成");break;
               case FTP_RESULT_FAIL_TO_READ_LOCAL_FILE:
                    ui->lbFtpStatus->setText("读本地文件失败");break;
               case FTP_RESULT_FAIL_TO_WRITE_LOCAL_FILE:
                    ui->lbFtpStatus->setText("写本地文件失败");break;
               case FTP_RESULT_FAIL_TO_READ_REMOTE_FILE:
                    ui->lbFtpStatus->setText("读远端文件失败");break;
               case FTP_RESULT_FAIL_TO_WRITE_REMOTE_FILE:
                    ui->lbFtpStatus->setText("写远端文件失败");break;
               case FTP_RESULT_FAIL_TO_SET_COMMAND:
                    ui->lbFtpStatus->setText("FTP命令失败");break;
               case FTP_RESULT_FAIL_TO_COMMUNICATION:
                    ui->lbFtpStatus->setText("FTP通信错误");break;
               case FTP_RESULT_FILE_MISMATCH:
                    ui->lbFtpStatus->setText("文件校验错误");break;
               default :
                    ui->lbFtpStatus->setText("未知");break;
                    ui->progressBar->setMaximum(100);
                    ui->progressBar->setValue(0);
             }
            if (ftpResult==FTP_RESULT_SUCCESS)
            {
                if(ftpStatus==FTP_STATE_LIST && sc_FtpAction==FTP_Actoin_FIND_FILES)
                {
                    show_lnc_filelist();
                   // sc_FtpAction=FTP_Action_IDEL;
                }

                if(ftpStatus==FTP_STATE_DOWNLOAD  && sc_FtpAction==FTP_Action_DOWNLOAD_FILE)
                {
                    //sc2.FtpReadFileCount();
                   // ui->btLncFilesRefresh->setEnabled(true);
                    on_btPcFilesRefresh_clicked();
                   // sc_FtpAction=FTP_Action_IDEL;
                    sc2.FtpTransferFileReset();
                }
                sc_FtpAction=FTP_Action_IDEL;
            }


  }//conOK
}

extern modbus_t *ctx;

void MainWindow::temppolling()
{
  static int TimeOutCnt=0;
  static int TcTimerCnt=0;
  static int FcTimerCnt=0;
  static int ScanCodeCnt=0;
  static float minitues=0.0;

  static bool OnRunFirst=true;

    if(ctx!=NULL)
   {
    uint16_t temp[2];
    float tv=0.0;
    int slaveID;
    int rc,ret;
    TcTimerCnt = TcTimerCnt + 100;
    FcTimerCnt = FcTimerCnt +100;
    if(TcTimerCnt >= m_IniFile->value("TcAdaptTime").toInt()) {
    //温控器
        modbus_close(ctx);
       slaveID =m_IniFile->value("TcMbSlave").toInt();
    //   qDebug()<<"tcmbslave="<<slaveID;
        ret=modbus_set_slave(ctx,slaveID);
      ret=modbus_connect(ctx);
    if(ret != -1)
     {
        rc=modbus_read_registers(ctx,ui->leTempMbAddr->text().toInt(),2,temp);
        if(rc!=-1)
          {
            tcConOK=true;
            tv=temp[0];
     //       qDebug()<<"rc1="<<rc<<"temp[0]="<<temp[0]<<"temp[1]="<<temp[1];
            CurrentTempValue = tv*ui->leTempFactor->text().toFloat();
            ui->lbCurrentTemp->setText(QString::number(CurrentTempValue)+"°C");
            ui->lbTcComStatus->setStyleSheet("QLabel{background-color: rgb(85, 255, 0);}");
           // LncDevAlarm=LncDevAlarm & 0xfffe;
          }
        else
        {   
            tcConOK=false;
            tv=temp[0];
            CurrentTempValue = 0.0;
            ui->lbCurrentTemp->setText("无采集值");
            ui->lbTcComStatus->setStyleSheet("QLabel{background-color: rgb(255, 255, 0);}");
            //LncDevAlarm = LncDevAlarm | 0x0001;
        }
      }
    TcTimerCnt =0;
    }
    //风速仪
    modbus_close(ctx);
    if(FcTimerCnt >= m_IniFile->value("FanAdaptTime").toInt()) {
    slaveID =m_IniFile->value("FanMbSlave").toInt();
    //qDebug()<<"FanMbSlave="<<slaveID;
    ret=modbus_set_slave(ctx,slaveID);
    ret=modbus_connect(ctx);
    if(ret != -1)
     {
         rc=modbus_read_registers(ctx,ui->leFsMbSpdAddr->text().toInt(),1,temp);
        if(rc!=-1)
        {
         fanConOK=true;
         tv= temp[0];
        // qDebug()<<"rc2="<<rc<<"temp[0]="<<temp[0]<<"temp[1]="<<temp[1];
         CurrentFanSpdValue = tv*ui->leFsFactor->text().toFloat();
         ui->lbCurrentFanSpd->setText(QString::number(CurrentFanSpdValue)+"米/秒");
         ui->lbFcComStatus->setStyleSheet("QLabel{background-color: rgb(85, 255, 0);}");
         //LncDevAlarm=LncDevAlarm & 0xffd;
        }
        else
        {
         fanConOK=false;
         CurrentFanSpdValue = 0.0;
         ui->lbCurrentFanSpd->setText("无采集值");
         ui->lbFcComStatus->setStyleSheet("QLabel{background-color: rgb(255, 255, 0);}");
         LncDevAlarm=LncDevAlarm | 0x0002;
        }
     }
    FcTimerCnt =0;
    }
   }
   if(ui->chbTcConectAuto->checkState() == Qt::Checked)
   {

       if(tcConOK==false && fanConOK==false)
        {
           TimeOutCnt = TimeOutCnt + 100;
          //  qDebug()<<"TimeOutCnt="<<TimeOutCnt<<"m_IniFile->value(TcAdaptTime).toInt()="<<m_IniFile->value("TcAdaptTime").toInt();
           if(TimeOutCnt >= m_IniFile->value("TcComTimeOut").toInt())
           {
               TimeOutCnt =0;
               modbus_close(ctx);
               modbus_free(ctx);
              // qDebug()<<"mobus 进行一次重新链接";
               on_btConnectTc_clicked();
           }
       }
   }
   else
       TimeOutCnt =0;
//扫码枪重连
    if(ui->chbScAutoConnect->checkState()==Qt::Checked)
    {
        if(scConOK==false)
      {
            ScanCodeCnt=ScanCodeCnt+100;
          //  qDebug()<<"sccnt"<<ScanCodeCnt;
        if(ScanCodeCnt>=m_IniFile->value("ScanComTimeOut").toInt())
        {
            ScanCodeCnt=0;
            ScanSerial->close();
            scConOK=false;
            on_btScanConnect_clicked();
        }
      }
    }
    else
        ScanCodeCnt=0;
 //定时设备校验
    if(ui->chbMesOnlineMode->checkState()==Qt::Checked)
    {
        minitues=minitues+0.0016667;
        if((minitues>=m_IniFile->value("MesCraftTimeOut").toFloat()) && (m_IniFile->value("MesCraftTimeOut").toFloat()>0.0))
        {
            minitues=0.0;
            if(mesConOK)
                postEPFormalVerifyOK=false;
                EPFormalVerifyCnt=m_IniFile->value("MesMaxCnt").toInt();
                postEPFormalVerify();
        }
    }
//在线模式，开机自动链接一次MES，进行设备校验

    if((ui->chbMesOnlineMode->checkState()==Qt::Checked) && OnRunFirst && (m_IniFile->value("MesCraftTimeOut").toFloat()>0.0))//开机自动发生设备校验
    {
        OnRunFirst=false;
        postEPFormalVerifyOK=false;
        EPFormalVerifyCnt=0;//一次申请
        postEPFormalVerify();
    }

    if(tcConOK)
        LncDevAlarm=LncDevAlarm & 0xfffe;
    else
        LncDevAlarm = LncDevAlarm | 0x0001;
    if(fanConOK)
        LncDevAlarm=LncDevAlarm & 0xfffd;
    else
        LncDevAlarm = LncDevAlarm | 0x0002;
    if(mesConOK)
        LncDevAlarm=LncDevAlarm & 0xfffb;
    else
        LncDevAlarm = LncDevAlarm | 0x0004;
    if(scConOK)
        LncDevAlarm=LncDevAlarm & 0xffef;
    else
        LncDevAlarm = LncDevAlarm | 0x0010;

}
//100毫秒定时器，用来插入温度和风速
void MainWindow::tempInsertPolling()
{
    static int tcCnt=0;
    static int fcCnt=0;

    if(ctx!=NULL && tcConOK)
    {
        tcCnt=tcCnt+100;
        if(tcCnt>=m_IniFile->value("TcSaveTime").toInt()){
        tcCnt=0;
        tempdata tval;
        tval.value=CurrentTempValue;
        tval.datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        add_one_temperature_rec(tval);
        }
    }
    if(ctx!=NULL && fanConOK)
    {
        fcCnt=fcCnt+100;
        if(fcCnt>=m_IniFile->value("FanSaveTime").toInt()){
        tempdata tval;
        tval.value=CurrentFanSpdValue;
        tval.datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        add_one_fanspeed_rec(tval);
        }
    }
   // qDebug()<<"insert";

}
//LNC读写
void MainWindow::lncRWpolling()
{
    int Rlogout=0;//LNC登出寄存器值
    static int lastRlogout=0;
    static int TimeOutCnt=0;
    static int MESTimeOutCnt=0;
    int LNCTimeOutLogOut=0;
    static int lastTimeOutStatus=0;
    long long NoOpKeepMin;

    LncPrivAddrR=m_IniFile->value("LncPrivAddrR").toInt();
    AutoLogOutTime=m_IniFile->value("AutoLogOutTime").toLongLong();
    MesTimeOut=m_IniFile->value("MesTimeOut").toFloat();
    MesMaxCnt=m_IniFile->value("MesMaxCnt").toFloat();
    LncTempAddrR=m_IniFile->value("LncTempAddrR").toInt();
    LncFanSpdWAddrR=m_IniFile->value("LncFanSpdWAddrR").toInt();
    LncHearCtrlInterval=m_IniFile->value("LncHearCtrlInterval").toFloat();
    DevAlarmAddrR=m_IniFile->value("DevAlarmAddrR").toInt();
    LncNoOperateAddrR=m_IniFile->value("LncNoOperateAddrR").toInt();
    LncHeartCtrlAddrR=m_IniFile->value("LncHeartCtrlAddrR").toInt();
    LncUseParamWAddrR=m_IniFile->value("LncUseParamWAddrR").toInt();
    LncWorkDeepAddrR=m_IniFile->value("LncWorkDeepAddrR").toInt();
    LncWorkRidusAddrR=m_IniFile->value("LncWorkRidusAddrR").toInt();
    LncMMSpdWAddrR=m_IniFile->value("LncMMSpdWAddrR").toInt();
    LncStopCtrlWAddrR=m_IniFile->value("LncStopCtrlWAddrR").toInt();
    LncStopCtrlWAddrR2=m_IniFile->value("LncStopCtrlWAddrR2").toInt();
    LncWorkStatusAddrR=m_IniFile->value("LncWorkStatusAddrR").toInt();
    LncAlowAdminAddrR=m_IniFile->value("LncAlowAdminAddrR").toInt();
    LncLogOutAddrR=m_IniFile->value("LncLogOutAddrR").toInt();
    LncIDorNameAddrR=m_IniFile->value("LncIDorNameAddrR").toInt();
    LncCamXAddrR=m_IniFile->value("LncCamXAddrR").toInt();
    LncCamYAddrR=m_IniFile->value("LncCamYAddrR").toInt();
    LncCamZAddrR=m_IniFile->value("LncCamZAddrR").toInt();
    LncProbeFixZAddrR=m_IniFile->value("LncProbeFixZAddrR").toInt();
    LncProbeX1AddrR=m_IniFile->value("LncProbeX1AddrR").toInt();
    LncProbeY1AddrR=m_IniFile->value("LncProbeY1AddrR").toInt();
    LncProbeX2AddrR=m_IniFile->value("LncProbeX2AddrR").toInt();
    LncProbeY2AddrR=m_IniFile->value("LncProbeY2AddrR").toInt();
    LncSafetyHeithtAddrR=m_IniFile->value("LncSafetyHeithtAddrR").toInt();
    LncHightDiffAddrR=m_IniFile->value("LncHightDiffAddrR").toInt();
    LncDigDeepAddrR=m_IniFile->value("LncDigDeepAddrR").toInt();
    LncDigDiffXAddrR=m_IniFile->value("LncDigDiffXAddrR").toInt();
    LncDigDiffYAddrR=m_IniFile->value("LncDigDiffYAddrR").toInt();
    LncCircleRadiusAddrR=m_IniFile->value("LncCircleRadiusAddrR").toInt();
    LncTriPointDistAddrR=m_IniFile->value("LncTriPointDistAddrR").toInt();
    LncCutCompAddrR=m_IniFile->value("LncCutCompAddrR").toInt();
    LncPathCutDeepAddrR=m_IniFile->value("LncPathCutDeepAddrR").toInt();
    LncMarkTrigerAddrR=m_IniFile->value("LncMarkTrigerAddrR").toInt();
    LncUseHourAddrR=m_IniFile->value("LncUseHourAddrR").toInt();
    LncUseMinitesAddrR=m_IniFile->value("LncUseMinitesAddrR").toInt();
    LncUseSecondsAddrR=m_IniFile->value("LncUseSecondsAddrR").toInt();
    LncFanSpeedR=m_IniFile->value("LncFanSpeedR").toInt();
    LncMMSpeedAddrR=m_IniFile->value("LncMMSpeedAddrR").toInt();
    LncAirPresureAddrR=m_IniFile->value("LncAirPresureAddrR").toInt();
    LncPlaneLevelAddrR=m_IniFile->value("LncPlaneLevelAddrR").toInt();
    LncAlarmCodeAddrR=m_IniFile->value("LncAlarmCodeAddrR").toInt();
    LncToolUseCntR = m_IniFile->value("LncToolUseCntAddrR").toInt();
    LncProbeBaseZR = m_IniFile->value("LncProbeBaseZAddrR").toInt();


    static int lastMarkstatus=0;

    if((ui->chbLogOutAuto->checkState()==Qt::Checked) && (logonPrivIdx != 0))
     {
        LogInKeepTime = LogInKeepTime + 200;//监控无操作时间
        NoOpKeepMin=(LogInKeepTime/1000)/60;
       // if(NoOpKeepMin >= ui->leLogoutTime->text().toLongLong())
        if(NoOpKeepMin >= AutoLogOutTime)
            LNCTimeOutLogOut =1;
        else
            LNCTimeOutLogOut=0;
        if(LNCTimeOutLogOut != lastTimeOutStatus)//无操作超时
        {
            lastTimeOutStatus = LNCTimeOutLogOut;
            if(lastTimeOutStatus != 0)
            {
                on_btLogOut_clicked();
                QString msg="界面程序无操作时长打到设定值，登录用户退出登录！";
                ui->tbrLogIn->append(const_msg(msg));
            }
        }
     }
    else
    {
           LogInKeepTime = 0;
           lastTimeOutStatus=0;
    }

    if(lncConOK)//链接成功，进行读取和逻辑操作
    {
        LncHeartTime=LncHeartTime+200;
        int tempIntValue=0;//送往R地址的值为1个R,为实际温度1000倍整形值
         int fanspdValue=0;
        tempIntValue = (int)(CurrentTempValue*1000.0);
        fanspdValue = (int)(CurrentFanSpdValue*1000.0);
//MES超时，或者配方校验不通过
        if(ui->chbMesOnlineMode->checkState()==Qt::Checked && ui->chbMesDelayStop->checkState()==Qt::Checked){

            MESTimeOutCnt=MESTimeOutCnt+200;
           // qDebug()<<(int)((m_IniFile->value("MesTimeOut").toFloat())*(m_IniFile->value("MesMaxCnt").toFloat()))<<MESTimeOutCnt;
            if(((mesConOK==false) && (MESTimeOutCnt >= (int)(MesTimeOut*MesMaxCnt))) || (postFormalVerifySucceed==false) || (CellIntoVerifySucceed==false))
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
          MESTimeOutCnt=0;
          lncCtrlValue=0;
          lncCtrlValue2=0;
        }


        int pwTran;
        sc2.DWriteBegin (ServerIdx);
        if(LncPrivAddrR>=0)
        sc2.DWrite1R(ServerIdx, LncPrivAddrR, logonPrivIdx);
        if(LncTempAddrR>=0)
        sc2.DWrite1R(ServerIdx, LncTempAddrR, tempIntValue);
        if(LncFanSpdWAddrR>=0)
        sc2.DWrite1R(ServerIdx, LncFanSpdWAddrR, fanspdValue);
        if(LncHeartTime >= (int)(LncHearCtrlInterval*1000.0))//到达心跳时间，往某地址写0
         {
            LncHeartTime=0;
           if(LncHeartCtrlAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncHeartCtrlAddrR,0);
         }

        if(ui->chbUseParam->checkState()==Qt::Checked){
            if(LncUseParamWAddrR >=0)
                sc2.DWrite1R(ServerIdx,LncUseParamWAddrR,1);//启用配方
            if(LncWorkDeepAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncWorkDeepAddrR,lncWorkDeepValue);//写入加工深度
            if(LncWorkRidusAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncWorkRidusAddrR,lncCircleRidus);//写入加工半径
            if(LncMMSpdWAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncMMSpdWAddrR,lncMMSpdValue);//写入主轴转速
         }
        else
          {
            if(LncUseParamWAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncUseParamWAddrR,0);//不启用配方
            if(LncWorkDeepAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncWorkDeepAddrR,0);//写入加工深度
            if(LncWorkRidusAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncWorkRidusAddrR,0);//写入加工半径
            if(LncMMSpdWAddrR>=0)
                sc2.DWrite1R(ServerIdx,LncMMSpdWAddrR,0);//写入主轴转速
          }
        if(LncStopCtrlWAddrR>=0)
            sc2.DWrite1R(ServerIdx,LncStopCtrlWAddrR,lncCtrlValue);//写入停机控制
        if(LncStopCtrlWAddrR2>=0)
            sc2.DWrite1R(ServerIdx,LncStopCtrlWAddrR2,lncCtrlValue2);//写入停机控制
        if(DevAlarmAddrR>=0)
            sc2.DWrite1R(ServerIdx,DevAlarmAddrR,LncDevAlarm);//写入外设异常值
        if(LncNoOperateAddrR>=0)
            sc2.DWrite1R(ServerIdx,LncNoOperateAddrR,LogInKeepTime/1000);
      //加工完成，80000，如果等于80000，写入90000，表示当前已经加工一件完成
      if(lncStatusValueR==80000 &&  productDone==false)
       {
         if(LncWorkStatusAddrR>=0)
            sc2.DWrite1R(ServerIdx,LncWorkStatusAddrR,90000);//写入状态寄存器90000
            productDone=true;
             emit workfinish();
       }
      if(lncStatusValueR!=80000)
          productDone=false;
        pwTran = sc2.DWriteEnd (ServerIdx);


     /*

        if(logonPrivIdx != LncPrivRead){
            //if(m_IniFile->value("LncPrivAddrR").toInt()>=0)
            sc2.DWrite1R(ServerIdx,m_IniFile->value("LncPrivAddrR").toUInt(),logonPrivIdx);
        }
        //if(m_IniFile->value("LncTempAddrR").toInt()>=0)
         sc2.DWrite1R(ServerIdx,m_IniFile->value("LncTempAddrR").toUInt(),tempIntValue);
        //if(m_IniFile->value("LncFanSpdWAddrR").toInt()>=0)
        // sc2.DWrite1R(ServerIdx,m_IniFile->value("LncFanSpdWAddrR").toUInt(),fanspdValue);

         if(LncHeartTime >= (int)(m_IniFile->value("LncHearCtrlInterval").toFloat()*1000.0))//到达心跳时间，往某地址写0
          {
             LncHeartTime=0;
            //if(m_IniFile->value("LncHeartCtrlAddrR").toInt()>=0)
             sc2.DWrite1R(ServerIdx,m_IniFile->value("LncHeartCtrlAddrR").toUInt(),0);
          }

         //写入其他几个控制寄存器，比如停机控制
          if(ui->chbUseParam->checkState()==Qt::Checked){
              if(m_IniFile->value("LncUseParamWAddrR").toInt()>=0)
              sc2.DWrite1R(ServerIdx,m_IniFile->value("LncUseParamWAddrR").toInt(),1);//启用配方
              if(m_IniFile->value("LncWorkDeepAddrR").toInt()>=0)
              sc2.DWrite1R(ServerIdx,m_IniFile->value("LncWorkDeepAddrR").toInt(),lncWorkDeepValue);//写入加工深度
              if(m_IniFile->value("LncWorkRidusAddrR").toInt()>=0)
              sc2.DWrite1R(ServerIdx,m_IniFile->value("LncWorkRidusAddrR").toInt(),lncCircleRidus);//写入加工半径
              if(m_IniFile->value("LncMMSpdWAddrR").toInt()>=0)
              sc2.DWrite1R(ServerIdx,m_IniFile->value("LncMMSpdWAddrR").toInt(),lncMMSpdValue);//写入主轴转速
           }
          else
            {
              if(m_IniFile->value("LncUseParamWAddrR").toInt()>=0)
              sc2.DWrite1R(ServerIdx,m_IniFile->value("LncUseParamWAddrR").toInt(),0);//不启用配方
            }

          if(ui->chbMesOnlineMode->checkState()==Qt::Checked && mesConOK==false && ui->chbMesDelayStop->checkState()==Qt::Checked){
                     lncCtrlValue=123456;
                     lncCtrlValue2=654321;
          }
          else
           {
            lncCtrlValue=0;
            lncCtrlValue2=0;
            }
            if(m_IniFile->value("LncStopCtrlWAddrR").toInt()>=0)
            sc2.DWrite1R(ServerIdx,m_IniFile->value("LncStopCtrlWAddrR").toInt(),lncCtrlValue);//写入停机控制
            if(m_IniFile->value("LncStopCtrlWAddrR2").toInt()>=0)
            sc2.DWrite1R(ServerIdx,m_IniFile->value("LncStopCtrlWAddrR2").toInt(),lncCtrlValue2);//写入停机控制

          //加工完成，80000，如果等于80000，写入90000，表示当前已经加工一件完成
          if(lncStatusValueR==80000)
           {
             if(m_IniFile->value("LncWorkStatusAddrR").toInt()>=0)
             sc2.DWrite1R(ServerIdx,m_IniFile->value("LncWorkStatusAddrR").toInt(),90000);//写入状态寄存器90000
             productDone=true;
           }
*/
//直连读取
         int pTran;
         sc2.DReadBegin (ServerIdx);
         if(LncPrivAddrR>=0)
         sc2.DReadNR(ServerIdx, LncPrivAddrR, 1);
         if(LncTempAddrR>=0)
         sc2.DReadNR(ServerIdx, LncTempAddrR, 1);
         if(LncFanSpdWAddrR>=0)
         sc2.DReadNR(ServerIdx, LncFanSpdWAddrR, 1);//风速写入值
         if(LncAlowAdminAddrR>=0)
         sc2.DReadNR(ServerIdx, LncAlowAdminAddrR, 1);
         if(LncHeartCtrlAddrR>=0)
         sc2.DReadNR(ServerIdx, LncHeartCtrlAddrR, 1);
         if(LncNoOperateAddrR>=0)
         sc2.DReadNR(ServerIdx, LncNoOperateAddrR, 1);//无 操作时间
         if(DevAlarmAddrR>=0)
         sc2.DReadNR(ServerIdx, DevAlarmAddrR, 1);//外设异常值读取
         if(LncLogOutAddrR>=0)
         sc2.DReadNR(ServerIdx, LncLogOutAddrR, 1);
         if(LncIDorNameAddrR>=0)
         sc2.DReadNR(ServerIdx, LncIDorNameAddrR, 1);
         if(LncAlarmCodeAddrR>=0)
         sc2.DReadNR(ServerIdx, LncAlarmCodeAddrR, 1);//状态
         if(LncWorkStatusAddrR>=0)
         sc2.DReadNR(ServerIdx, LncWorkStatusAddrR, 1);//加工完成
         if(LncUseParamWAddrR>=0)
         sc2.DReadNR(ServerIdx, LncUseParamWAddrR, 1);//启用参数配方
         if(LncWorkDeepAddrR>=0)
         sc2.DReadNR(ServerIdx, LncWorkDeepAddrR, 1);//配方加工深度
         if(LncWorkRidusAddrR>=0)
         sc2.DReadNR(ServerIdx, LncWorkRidusAddrR, 1);//配方加工半径
         if(LncMMSpdWAddrR>=0)
         sc2.DReadNR(ServerIdx, LncMMSpdWAddrR, 1);//配方主轴转速
         if(LncStopCtrlWAddrR>=0)
         sc2.DReadNR(ServerIdx, LncStopCtrlWAddrR, 1);//停机控制1
         if(LncStopCtrlWAddrR2>=0)
         sc2.DReadNR(ServerIdx, LncStopCtrlWAddrR2, 1);//停机控制2


         //18个工艺值
         if(LncCamXAddrR>=0)
         sc2.DReadNR(ServerIdx, LncCamXAddrR, 1);
         if(LncCamYAddrR>=0)
         sc2.DReadNR(ServerIdx, LncCamYAddrR, 1);
         if(LncCamZAddrR>=0)
         sc2.DReadNR(ServerIdx, LncCamZAddrR, 1);
         if(LncHightDiffAddrR>=0)
         sc2.DReadNR(ServerIdx, LncHightDiffAddrR, 1);
         if(LncProbeFixZAddrR>=0)
         sc2.DReadNR(ServerIdx, LncProbeFixZAddrR, 1);
         if(LncProbeX1AddrR>=0)
         sc2.DReadNR(ServerIdx, LncProbeX1AddrR, 1);
         if(LncProbeY1AddrR>=0)
         sc2.DReadNR(ServerIdx, LncProbeY1AddrR, 1);
         if(LncProbeX2AddrR>=0)
         sc2.DReadNR(ServerIdx, LncProbeX2AddrR, 1);
         if(LncProbeY2AddrR>=0)
         sc2.DReadNR(ServerIdx, LncProbeY2AddrR, 1);
         if(LncSafetyHeithtAddrR>=0)
         sc2.DReadNR(ServerIdx, LncSafetyHeithtAddrR, 1);
         if(LncDigDeepAddrR>=0)
         sc2.DReadNR(ServerIdx, LncDigDeepAddrR, 1);
         if(LncDigDiffXAddrR>=0)
         sc2.DReadNR(ServerIdx, LncDigDiffXAddrR, 1);
         if(LncDigDiffYAddrR>=0)
         sc2.DReadNR(ServerIdx, LncDigDiffYAddrR, 1);
         if(LncCircleRadiusAddrR>=0)
         sc2.DReadNR(ServerIdx, LncCircleRadiusAddrR, 1);
         if(LncTriPointDistAddrR>=0)
         sc2.DReadNR(ServerIdx, LncTriPointDistAddrR, 1);
         if(LncCutCompAddrR>=0)
         sc2.DReadNR(ServerIdx, LncCutCompAddrR, 1);
         if(LncPathCutDeepAddrR>=0)
         sc2.DReadNR(ServerIdx, LncPathCutDeepAddrR, 1);
         if(LncMarkTrigerAddrR>=0)
         sc2.DReadNR(ServerIdx, LncMarkTrigerAddrR, 1);
         if(LncUseHourAddrR>=0)
         sc2.DReadNR(ServerIdx, LncUseHourAddrR, 1);
         if(LncUseMinitesAddrR>=0)
         sc2.DReadNR(ServerIdx, LncUseMinitesAddrR, 1);
         if(LncUseSecondsAddrR>=0)
         sc2.DReadNR(ServerIdx, LncUseSecondsAddrR, 1);
         if(LncFanSpeedR>=0)
         sc2.DReadNR(ServerIdx, LncFanSpeedR, 1);//风速
         if(LncPlaneLevelAddrR>=0)
         sc2.DReadNR(ServerIdx, LncPlaneLevelAddrR, 1);//平整度
         if(LncMMSpeedAddrR>=0)
         sc2.DReadNR(ServerIdx, LncMMSpeedAddrR, 1);//主轴转速
         if(LncAirPresureAddrR>=0)
         sc2.DReadNR(ServerIdx, LncAirPresureAddrR, 1);//风刀气压
         if(LncToolUseCntR>=0)
         sc2.DReadNR(ServerIdx, LncToolUseCntR, 1);//刀具使用次数
         if(LncProbeBaseZR>=0)
         sc2.DReadNR(ServerIdx, LncProbeBaseZR, 1);//探测Z值

         pTran = sc2.DReadEnd (ServerIdx);


         LncPrivRead=sc2.memR(ServerIdx,LncPrivAddrR);
         ui->lbPrivValue->setText(QString::number(LncPrivRead));//R5000001的值
         ui->lbTmpValue->setText(QString::number(sc2.memR(ServerIdx,LncTempAddrR)));//R5000002的值
         ui->lbFsValue->setText(QString::number(sc2.memR(ServerIdx,LncFanSpdWAddrR)));//R5000006的值


         RegAlowAdmin=sc2.memR(ServerIdx,LncAlowAdminAddrR);
         ui->lbAlowAdminValue->setText(QString::number(RegAlowAdmin));//R5000003的值
         ui->lbHeartRegValue->setText(QString::number(sc2.memR(ServerIdx,LncHeartCtrlAddrR)));//R5000050的值
         Rlogout=sc2.memR(ServerIdx,LncLogOutAddrR);//R5009993的值
         ui->lbLogoutValue->setText(QString::number(Rlogout));
         RegNameOrID=sc2.memR(ServerIdx,LncIDorNameAddrR);
         ui->lbIDorNameValue->setText(QString::number(RegNameOrID));

         ui->lbUseParamRegWValue->setText(QString::number(sc2.memR(ServerIdx,LncUseParamWAddrR)));

         lncAlarmValueR=  sc2.memR(ServerIdx,LncAlarmCodeAddrR);//加工状态
         ui->lbAlarmRegValue->setText(QString::number(lncAlarmValueR));
         if(lncAlarmValueR==0)
             ui->lbLNCStatus->setText("未准备完成");
         else if(lncAlarmValueR==1)
             ui->lbLNCStatus->setText("空闲状态");
         else if(lncAlarmValueR==2)
             ui->lbLNCStatus->setText("加工状态");
         else if(lncAlarmValueR==3)
             ui->lbLNCStatus->setText("暂停状态");

         lncStatusValueR=sc2.memR(ServerIdx,LncWorkStatusAddrR);
         ui->lbWorkStatusRegWValue->setText(QString::number(lncStatusValueR));
         ui->lbNoOperate->setText(QString::number(sc2.memR(ServerIdx,LncNoOperateAddrR)));//无操作时间
         ui->lbDevAlarm->setText(QString::number(sc2.memR(ServerIdx,DevAlarmAddrR)));//外设异常
         ui->lbDeepRegWValue->setText(QString::number(sc2.memR(ServerIdx,LncWorkDeepAddrR)));
         ui->lbRidusRegWValue->setText(QString::number(sc2.memR(ServerIdx,LncWorkRidusAddrR)));
         ui->lbMMSpdRegWValue->setText(QString::number(sc2.memR(ServerIdx,LncMMSpdWAddrR)));
         ui->lbStopCtrlRegWValue->setText(QString::number(sc2.memR(ServerIdx,LncStopCtrlWAddrR)));
         ui->lbStopCtrlRegWValue2->setText(QString::number(sc2.memR(ServerIdx,LncStopCtrlWAddrR2)));







         //检测LNC的退出寄存器从0-1时，退出登录
         if(ui->chbLogOutByLnc->checkState()==Qt::Checked)
         {
             if(Rlogout != lastRlogout)
             {
                 lastRlogout=Rlogout;
                 if(Rlogout == 0)
                     {
                         on_btLogOut_clicked();
                         QString msg="检测到LNC登出值信号，登录用户退出登录！";
                         ui->tbrLogIn->append(const_msg(msg));
                     }
             }
         }

       //      if(logonPrivIdx != LncPrivRead)
       //          sc2.DWrite1R(ServerIdx,m_IniFile->value("LncPrivAddrR").toUInt(),logonPrivIdx);

         //18个工艺值显示
         CamX=((float)(sc2.memR(ServerIdx,LncCamXAddrR)))/1000.0;
         CamY=((float)(sc2.memR(ServerIdx,LncCamYAddrR)))/1000.0;
         CamZ=((float)(sc2.memR(ServerIdx,LncCamZAddrR)))/1000.0;
         HightDiff=((float)(sc2.memR(ServerIdx,LncHightDiffAddrR)))/1000.0;
         ProbeFixZ=((float)(sc2.memR(ServerIdx,LncProbeFixZAddrR)))/1000.0;
         ProbeX1=((float)(sc2.memR(ServerIdx,LncProbeX1AddrR)))/1000.0;
         ProbeY1=((float)(sc2.memR(ServerIdx,LncProbeY1AddrR)))/1000.0;
         ProbeX2=((float)(sc2.memR(ServerIdx,LncProbeX2AddrR)))/1000.0;
         ProbeY2=((float)(sc2.memR(ServerIdx,LncProbeY2AddrR)))/1000.0;
         SafetyHeitht=((float)(sc2.memR(ServerIdx,LncSafetyHeithtAddrR)))/1000.0;
         DigDeep=((float)(sc2.memR(ServerIdx,LncDigDeepAddrR)))/1000.0;
         DigDiffX=((float)(sc2.memR(ServerIdx,LncDigDiffXAddrR)))/1000.0;
         DigDiffY=((float)(sc2.memR(ServerIdx,LncDigDiffYAddrR)))/1000.0;
         CircleRadius=((float)(sc2.memR(ServerIdx,LncCircleRadiusAddrR)))/1000.0;
         TriPointDist=((float)(sc2.memR(ServerIdx,LncTriPointDistAddrR)))/1000.0;
         CutComp=((float)(sc2.memR(ServerIdx,LncCutCompAddrR)))/1000.0;
         PathCutDeep=((float)(sc2.memR(ServerIdx,LncPathCutDeepAddrR)))/1000.0;
         MarkTriger=sc2.memR(ServerIdx,LncMarkTrigerAddrR);
         LncUseHour=sc2.memR(ServerIdx,LncUseHourAddrR);
         toolUseCnt=((int)(sc2.memR(ServerIdx,LncToolUseCntR)));//刀具使用次数
         lncProbeBaseZValue=((float)sc2.memR(ServerIdx,LncProbeBaseZR))/1000.0;//探测Z


         int MicroSeconds=sc2.memR(ServerIdx,LncUseMinitesAddrR);

         FanSpeedR=((float)(sc2.memR(ServerIdx,LncFanSpeedR)))/1000.0;//风速
         PlaneBlance=((float)(sc2.memR(ServerIdx,LncPlaneLevelAddrR)))/1000.0;//平整度
         MMspeed=((float)(sc2.memR(ServerIdx,LncMMSpeedAddrR)))/1000.0;//主轴转速
         AirPressure=((float)(sc2.memR(ServerIdx,LncAirPresureAddrR)))/1000.0;//风刀气压



         LncUseMinites=MicroSeconds / 60000;
        // LncUseSeconds=sc2.memR(ServerIdx,m_IniFile->value("LncUseSecondsAddrR").toInt());
         LncUseSeconds=(MicroSeconds-LncUseMinites*60000)/1000;



        ui->lbCamX->setText(QString("%1mm").arg(CamX));
        ui->lbCamY->setText(QString("%1mm").arg(CamY));
        ui->lbCamZ->setText(QString("%1mm").arg(CamZ));
        ui->lbHightDiff->setText(QString("%1mm").arg(HightDiff));
        ui->lbProbeFixZ->setText(QString("%1mm").arg(ProbeFixZ));
        ui->lbProbeX1->setText(QString("%1mm").arg(ProbeX1));
        ui->lbProbeY1->setText(QString("%1mm").arg(ProbeY1));
        ui->lbProbeX2->setText(QString("%1mm").arg(ProbeX2));
        ui->lbProbeY2->setText(QString("%1mm").arg(ProbeY2));
        ui->lbSafetyHeitht->setText(QString("%1mm").arg(SafetyHeitht));
        ui->lbDigDeep->setText(QString("%1mm").arg(DigDeep));
        ui->lbDigDiffX->setText(QString("%1mm").arg(DigDiffX));
        ui->lbDigDiffY->setText(QString("%1mm").arg(DigDiffY));
        ui->lbCircleRadius->setText(QString("%1mm").arg(CircleRadius));
        ui->lbTriPointDist->setText(QString("%1mm").arg(TriPointDist));
        ui->lbCutComp->setText(QString("%1mm").arg(CutComp));
        ui->lbPathCutDeep->setText(QString("%1mm").arg(PathCutDeep));
        ui->lbMarkTriger->setText(QString("%1").arg(MarkTriger));
        ui->lbUseHour->setText(QString("%1小时").arg(LncUseHour));
        ui->lbUseMinites->setText(QString("%1毫秒").arg(MicroSeconds));//毫秒值

        ui->lbFanSpeed->setText(QString("%1米/秒").arg(FanSpeedR));
        ui->lbPlaneLevel->setText(QString("%1mm").arg(PlaneBlance));
        ui->lbMMSpeed->setText(QString("%1转/分钟").arg(MMspeed));
        ui->lbAirPresure->setText(QString("%1KPa").arg(AirPressure));
        ui->lbToolUseCnt->setText(QString::number(toolUseCnt));
        ui->lbProbeBaseZ->setText(QString::number(lncProbeBaseZValue));

        if(MarkTriger != lastMarkstatus)//工艺值变更，记录数据
        {
          int insert_flag = lastMarkstatus;
          lastMarkstatus= MarkTriger;
          craftmsg  craft;
          craft.datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
          craft.camx = CamX;
          craft.camy = CamY;
          craft.camz = CamZ;
          craft.heightdiff = HightDiff;
          craft.probefixz = ProbeFixZ;
          craft.probex1 = ProbeX1;
          craft.probey1 = ProbeY1;
          craft.probex2 = ProbeX2;
          craft.probey2 = ProbeY2;
          craft.safetyheight = SafetyHeitht;
          craft.digdeep = DigDeep;
          craft.digdiffx = DigDiffX;
          craft.digdiffy = DigDiffY;
          craft.circleradius = CircleRadius;
          craft.tripointdist = TriPointDist;
          craft.cutcomp = CutComp;
          craft.pathcutdeep = PathCutDeep;
          craft.planelevel = PlaneBlance;
          craft.mmspeed = MMspeed;
          craft.aripressure=FanSpeedR;//风速值
          craft.toolusecnt=toolUseCnt;
          craft.probebaseZ=lncProbeBaseZValue;
         // craft.usetime=QString("%1:%2:%3").arg(LncUseHour).arg(LncUseMinites).arg(LncUseSeconds);
          craft.usetime.sprintf("%02d:%02d:%02d",LncUseHour,LncUseMinites,LncUseSeconds);
          if(insert_flag !=0)
            emit InsertCraftRecord(craft);

        }

    }
    else//链接失败，自动重连接
        {
            if(ui->chbConnectLncAuto->checkState()==Qt::Checked)
            {
                if(LncConnStatus != SC_CONN_STATE_OK)
                    TimeOutCnt = TimeOutCnt + 200;
                //if(TimeOutCnt >= ui->leLncTimeOut->text().toInt())
                if(TimeOutCnt >= m_IniFile->value("LncTimeOut").toInt())
                {
                    TimeOutCnt =0;
                    if(LncConnStatus==SC_CONN_STATE_DISCONNECT)
                     {
                        on_btConnectLncByIP_clicked();
                       // qDebug()<<"重连";
                      }
                    else
                      {
                      if(LncConnStatus==SC_CONN_STATE_NORESPONSE)
                        {
                          sc2.Disconnect(ServerIdx);
                         // qDebug()<<"断开";
                        }
                     }
                }
            }

        }
/*
    //测试
    static uint tsetcnt=0;
    tsetcnt +=200;
    if(tsetcnt>=2000)
    {
        tsetcnt=0;
        craftmsg  craft;
        craft.datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        craft.camx = CamX;
        craft.camy = CamY;
        craft.camz = CamZ;
        craft.heightdiff = HightDiff;
        craft.probefixz = ProbeFixZ;
        craft.probex1 = ProbeX1;
        craft.probey1 = ProbeY1;
        craft.probex2 = ProbeX2;
        craft.probey2 = ProbeY2;
        craft.safetyheight = SafetyHeitht;
        craft.digdeep = DigDeep;
        craft.digdiffx = DigDiffX;
        craft.digdiffy = DigDiffY;
        craft.circleradius = CircleRadius;
        craft.tripointdist = TriPointDist;
        craft.cutcomp = CutComp;
        craft.pathcutdeep = PathCutDeep;
        craft.planelevel = PlaneBlance;
        craft.mmspeed = MMspeed;
        craft.aripressure=FanSpeedR;//风速值
        craft.toolusecnt=toolUseCnt;
       // craft.usetime=QString("%1:%2:%3").arg(LncUseHour).arg(LncUseMinites).arg(LncUseSeconds);
        craft.usetime.sprintf("%02d:%02d:%02d",LncUseHour,LncUseMinites,LncUseSeconds);
      //  qDebug()<<"instert one craft";
          emit InsertCraftRecord(craft);

    }
    //测试结束
*/
}


void MainWindow::privpolling()
{
  static int lastpriv=1000;
  static int lastIDstatus=1000;
 // static int lastMarkstatus=0;

//  if(lncConOK)
//  {
//      if(logonPrivIdx != LncPrivRead)
//          sc2.DWrite1R(ServerIdx,m_IniFile->value("LncPrivAddrR").toUInt(),logonPrivIdx);
//  }
//logonPrivIdx=7;
  if(logonPrivIdx != lastpriv)//权限变更
   {
        lastpriv=logonPrivIdx;
       // if(lncConOK)
        //    sc2.DWrite1R(ServerIdx,m_IniFile->value("LncPrivAddrR").toUInt(),logonPrivIdx);

    if(logonPrivIdx<=5)
    {
        if(logonPrivIdx<=3)
        {
        ui->tab_userreg->setEnabled(false);
        ui->tab_usermanag->setEnabled(false);
        //ui->tab_log->setEnabled(false);
        ui->tab_lnc_file->setEnabled(false);
        ui->tab_lnc_article->setEnabled(false);
        ui->leLNCPaswd->setEnabled(false);
        ui->leLncTimeOut->setEnabled(false);
        ui->leLncIP->setEnabled(false);
        ui->chbNoLncPaswd->setEnabled(false);
        ui->chbDispLncPaswd->setEnabled(false);
        ui->btConnectLncByFind->setEnabled(false);
        ui->btConnectLncByIP->setEnabled(false);
        ui->btDisconnectLnc->setEnabled(false);
        ui->cbLncFtpFolder->setEditable(false);
        ui->lePcDownloadFolder->setEnabled(false);
        ui->btBrowsPcDownloadFolder->setEnabled(false);
        ui->lePrivFilename->setEnabled(false);
        ui->btGetFileName->setEnabled(false);
       // ui->btFormalReq->setEnabled(false);
        ui->chbUseParam->setEnabled(false);

        ui->leProbeBaseZ->setEnabled(false);
        ui->chbProbeBazeZ->setEnabled(false);
        ui->chbPlane->setEnabled(false);
        ui->chbToolUsed->setEnabled(false);
        ui->chbAirPressure->setEnabled(false);
        }
        else
        {
            ui->tab_userreg->setEnabled(true);
            ui->tab_usermanag->setEnabled(true);
            //ui->tab_log->setEnabled(false);
            ui->tab_lnc_file->setEnabled(true);
            ui->tab_lnc_article->setEnabled(true);
            ui->leLNCPaswd->setEnabled(true);
            ui->leLncTimeOut->setEnabled(true);
            ui->leLncIP->setEnabled(true);
            ui->chbNoLncPaswd->setEnabled(true);
            ui->chbDispLncPaswd->setEnabled(true);
            ui->btConnectLncByFind->setEnabled(true);
            ui->btConnectLncByIP->setEnabled(true);
            ui->btDisconnectLnc->setEnabled(true);
            ui->cbLncFtpFolder->setEditable(true);
            ui->lePcDownloadFolder->setEnabled(true);
            ui->btBrowsPcDownloadFolder->setEnabled(true);
            ui->lePrivFilename->setEnabled(true);
            ui->btGetFileName->setEnabled(true);
           // ui->btFormalReq->setEnabled(true);
            ui->chbUseParam->setEnabled(true);
            ui->chbProbeBazeZ->setEnabled(true);
            ui->chbPlane->setEnabled(true);
            ui->chbToolUsed->setEnabled(true);
            ui->chbAirPressure->setEnabled(true);
        }

        //lnc
         ui->lePrivAddr->setEnabled(false);
         ui->leTempAddr->setEnabled(false);
         ui->leFsAddr->setEnabled(false);
         ui->leLncLogoutAddr->setEnabled(false);
         ui->leLncIDorNameAddr->setEnabled(false);
         ui->leLncAlowAdminAddr->setEnabled(false);
         ui->leLncHeartRegAddr->setEnabled(false);
         ui->leHeartInterval->setEnabled(false);
         ui->leLncAlarmRegAddr->setEnabled(false);
         ui->leLncWorkStatusRegAddr->setEnabled(false);
         ui->leLncDeepRegWAddr->setEnabled(false);
         ui->leLncRidusRegWAddr->setEnabled(false);
         ui->leLncMMSpdWRegAddr->setEnabled(false);
         ui->leLncStopCtrlRegAddr->setEnabled(false);
         ui->leLncStopCtrlRegAddr2->setEnabled(false);
         ui->leLncUseParamRegAddr->setEnabled(false);

        // ui->chbUseParam->setEnabled(false);
         ui->leLncNoOperateAddr->setEnabled(false);
         ui->leDevAlarmAddr->setEnabled(false);
         ui->leToolUseCnt->setEnabled(false);

        // ui->chbPlane->setEnabled(false);
        // ui->chbToolUsed->setEnabled(false);
        //Craft
        ui->leCamX->setEnabled(false);
        ui->leCamY->setEnabled(false);
        ui->leCamZ->setEnabled(false);
        ui->leHightDiff->setEnabled(false);
        ui->leProbeFixZ->setEnabled(false);
        ui->leProbeX1->setEnabled(false);
        ui->leProbeX2->setEnabled(false);
        ui->leProbeY1->setEnabled(false);
        ui->leProbeY2->setEnabled(false);
        ui->leSafetyHeitht->setEnabled(false);
        ui->leDigDeep->setEnabled(false);
        ui->leDigDiffX->setEnabled(false);
        ui->leDigDiffY->setEnabled(false);
        ui->leCircleRadius->setEnabled(false);
        ui->leTriPointDist->setEnabled(false);
        ui->lePathCutDeep->setEnabled(false);
        ui->leCutComp->setEnabled(false);
        ui->leUseHour->setEnabled(false);
        ui->leUseMinites->setEnabled(false);
        ui->leUseSeconds->setEnabled(false);
        ui->leFanSpeedR->setEnabled(false);
        ui->leMMSpeed->setEnabled(false);
        ui->lePlaneLevel->setEnabled(false);
        ui->leAirPresure->setEnabled(false);
        ui->leMarkTriger->setEnabled(false);
       // ui->chbAirPressure->setEnabled(false);

        //TC
        ui->leTempAddr->setEnabled(false);
        ui->leTcComPort->setEnabled(false);
        ui->leTcBdrate->setEnabled(false);
        ui->cbMbDataBits->setEnabled(false);
        ui->cbMbCrc->setEnabled(false);
        ui->cbMbStopBits->setEnabled(false);
        ui->btConnectTc->setEnabled(false);
        ui->btDisconnectTc->setEnabled(false);
        ui->leTcTimeOut->setEnabled(false);
        ui->leTcAdaptInterval->setEnabled(false);
        ui->leTcSaveTempInterval->setEnabled(false);
        ui->leTcMbAddr->setEnabled(false);
        ui->leTempMbAddr->setEnabled(false);
        ui->leTempFactor->setEnabled(false); 
        ui->chbTcConectAuto->setEnabled(false);
      //FC
        ui->leFsAddr->setEnabled(false);
        ui->leFsAdaptInterval->setEnabled(false);
        ui->leFsSaveTempInterval->setEnabled(false);
        ui->leFsMbAddr->setEnabled(false);
        ui->leFsMbSpdAddr->setEnabled(false);
        ui->leFsFactor->setEnabled(false);

        if(logonPrivIdx<=3)
        {
       //扫码枪
        ui->leScanComPort->setEnabled(false);
        ui->leScanBdrate->setEnabled(false);
        ui->leScanLength->setEnabled(false);
        ui->leScanRules->setEnabled(false);
        ui->leScanTimeOut->setEnabled(false);
        ui->chbScAutoConnect->setEnabled(false);
        ui->chbScRuleCtrl->setEnabled(false);
        ui->cbScDataBits->setEnabled(false);
        ui->cbScStopBits->setEnabled(false);
        ui->cbScCrc->setEnabled(false);
        //MES设置
        ui->leMesMaxDelay->setEnabled(false);
        ui->leMesMaxCnt->setEnabled(false);
        ui->leMesCraftDelay->setEnabled(false);
        ui->chbMesDelayStop->setEnabled(false);
        ui->chbMesOnlineMode->setEnabled(false);
        ui->leScanLength->setEnabled(false);
        ui->leScanRules->setEnabled(false);
        ui->chbScRuleCtrl->setEnabled(false);

        ui->btManualGetInfo->setEnabled(false);
        ui->btOrderList->setEnabled(false);
        ui->btUpateMat->setEnabled(false);
       // ui->btFormalReq->setEnabled(false);
        ui->btFtpConnect->setEnabled(false);
       }
        else
        {

            ui->leScanComPort->setEnabled(true);
            ui->leScanBdrate->setEnabled(true);
            ui->leScanLength->setEnabled(true);
            ui->leScanRules->setEnabled(true);
            ui->leScanTimeOut->setEnabled(true);
            ui->chbScAutoConnect->setEnabled(true);
            ui->chbScRuleCtrl->setEnabled(true);
            ui->cbScDataBits->setEnabled(true);
            ui->cbScStopBits->setEnabled(true);
            ui->cbScCrc->setEnabled(true);
            //MES设置
            ui->leMesMaxDelay->setEnabled(true);
            ui->leMesMaxCnt->setEnabled(true);
            ui->leMesCraftDelay->setEnabled(true);
            ui->chbMesDelayStop->setEnabled(true);
            ui->chbMesOnlineMode->setEnabled(true);
            ui->leScanLength->setEnabled(true);
            ui->leScanRules->setEnabled(true);
            ui->chbScRuleCtrl->setEnabled(true);

            ui->btManualGetInfo->setEnabled(true);
            ui->btOrderList->setEnabled(true);
            ui->btUpateMat->setEnabled(true);
          //  ui->btFormalReq->setEnabled(true);
            ui->btFtpConnect->setEnabled(true);
        }
        if(logonPrivIdx<3)//工艺申请
            ui->btFormalReq->setEnabled(false);
         else
            ui->btFormalReq->setEnabled(true);

        ui->btClearLogInLog->setEnabled(false);
        ui->btClearNewUserLog->setEnabled(false);
        ui->btClearFilesLog->setEnabled(false);
        ui->btSaveNewUserLog->setEnabled(false);
        ui->btSaveLogInLog->setEnabled(false);
        ui->btSaveFilesLog->setEnabled(false);

        ui->btClearTemp->setVisible(false);
        ui->chbConnectLncAuto->setEnabled(false);
        ui->chbTcConectAuto->setEnabled(false);
        ui->btClearSysLog->setVisible(false);
        ui->btClearCraft->setVisible(false);
        ui->btGenTableVeiw->setEnabled(false);//删除文件



        if(ui->tab_file_ananlys->isEnabled())
        {

            ui->tab_file_ananlys->setEnabled(false);
           // ui->tab_file_ananlys->setVisible(false);
            ui->tabWidgetLncTab->removeTab(0);
            showMaximized();
        }
        if(ui->tab_lnc_logtab->isEnabled())
        {
            ui->tab_lnc_logtab->setEnabled(false);
           // ui->tab_lnc_logtab->setVisible(false);
            ui->tabWidgetLncTab->removeTab(0);
            showMaximized();
        }
        if(logonPrivIdx<=3)
        {
        //MES
        ui->leDevCodeW->setEnabled(false);
        ui->leProcessNumW->setEnabled(false);
        ui->leCraftAdr->setEnabled(false);
        ui->leFormulaAdr->setEnabled(false);
        ui->leDevInfoAdr->setEnabled(false);
        ui->leOrderAdr->setEnabled(false);
        ui->leOrderListAdr->setEnabled(false);
        ui->leLoadAdr->setEnabled(false);
        ui->leUnloadAdr->setEnabled(false);
        ui->leMaterialAdr->setEnabled(false);
        ui->leEnterAdr->setEnabled(false);
        ui->leNGAdr->setEnabled(false);
        ui->leExitAdr->setEnabled(false);
        ui->leFtpPath->setEnabled(false);
        ui->chbAutoLogs->setEnabled(false);
        ui->chbFtpAutoConnect->setEnabled(false);
        ui->leNGLeadCode->setEnabled(false);
        ui->leFtpFormalDir->setEnabled(false);
        ui->leFtpFormalVerifyDir->setEnabled(false);
        ui->leFtpEquVerifyDir->setEnabled(false);
        ui->leFtpBillInfoDir->setEnabled(false);
        ui->leFtpBillListDir->setEnabled(false);
        ui->leFtpCellInDir->setEnabled(false);
        ui->leFtpCellOutDir->setEnabled(false);
        ui->leFtpSyncTime->setEnabled(false);
        ui->leFtpServerIP->setEnabled(false);
        ui->leFtpPort->setEnabled(false);
        ui->leFtpUserName->setEnabled(false);
        ui->leFtpPassword->setEnabled(false);
        ui->leFtpNGDir->setEnabled(false);
        ui->leFtpWorkDoneDir->setEnabled(false);

        ui->btMkdir->setVisible(false);
        ui->btFtpUpload->setVisible(false);

        ui->btClearFormal->setVisible(false);//
       ui->btClearNGdata->setVisible(false);//
       ui->btClearFinishLog->setVisible(false);//
       ui->btClearScanLog->setVisible(false);//
       ui->btClearScanLog->setVisible(false);//
       ui->btCellInClear->setVisible(false);
       ui->btCellOutClear->setVisible(false);
       ui->btInterfaceClear->setVisible(false);
       ui->btClearEnterLog->setVisible(false);//
       ui->btClearExitLog->setVisible(false);//
       ui->btScanConnect->setEnabled(false);
       ui->btScanDisConnect->setEnabled(false);
        }
        else
        {
        //MES
        ui->leDevCodeW->setEnabled(true);
        ui->leProcessNumW->setEnabled(true);
        ui->leCraftAdr->setEnabled(true);
        ui->leFormulaAdr->setEnabled(true);
        ui->leDevInfoAdr->setEnabled(true);
        ui->leOrderAdr->setEnabled(true);
        ui->leOrderListAdr->setEnabled(true);
        ui->leLoadAdr->setEnabled(true);
        ui->leUnloadAdr->setEnabled(true);
        ui->leMaterialAdr->setEnabled(true);
        ui->leEnterAdr->setEnabled(true);
        ui->leNGAdr->setEnabled(true);
        ui->leExitAdr->setEnabled(true);
        ui->leFtpPath->setEnabled(true);

        ui->chbAutoLogs->setEnabled(true);
        ui->chbFtpAutoConnect->setEnabled(true);
        ui->leNGLeadCode->setEnabled(true);
        ui->leFtpFormalDir->setEnabled(true);
        ui->leFtpFormalVerifyDir->setEnabled(true);
        ui->leFtpEquVerifyDir->setEnabled(true);
        ui->leFtpBillInfoDir->setEnabled(true);
        ui->leFtpBillListDir->setEnabled(true);
        ui->leFtpCellInDir->setEnabled(true);
        ui->leFtpCellOutDir->setEnabled(true);
        ui->leFtpSyncTime->setEnabled(true);
        ui->leFtpServerIP->setEnabled(true);
        ui->leFtpPort->setEnabled(true);
        ui->leFtpUserName->setEnabled(true);
        ui->leFtpPassword->setEnabled(true);
        ui->leFtpNGDir->setEnabled(true);
        ui->leFtpWorkDoneDir->setEnabled(true);

        ui->btMkdir->setVisible(true);
        ui->btFtpUpload->setVisible(true);

         ui->btClearFormal->setVisible(true);//
        ui->btClearNGdata->setVisible(true);//
        ui->btClearFinishLog->setVisible(true);//
        ui->btClearScanLog->setVisible(true);//
        ui->btClearScanLog->setVisible(true);//

        ui->btCellInClear->setVisible(true);
        ui->btCellOutClear->setVisible(true);
        ui->btInterfaceClear->setVisible(true);
        ui->btClearEnterLog->setVisible(true);//
        ui->btClearExitLog->setVisible(true);//

        ui->btScanConnect->setEnabled(true);
        ui->btScanDisConnect->setEnabled(true);
        }

       // ui->btClearEnterLog->setVisible(false);//
       // ui->btClearExitLog->setVisible(false);//
        ui->btClearFanSpd->setVisible(false);//
        //ui->btClearFormal->setVisible(false);//
        //ui->btClearNGdata->setVisible(false);//
       // ui->btClearFinishLog->setVisible(false);//
       // ui->btClearScanLog->setVisible(false);//
        //ui->btScanConnect->setEnabled(false);
       // ui->btScanDisConnect->setEnabled(false);

       // ui->btCellInClear->setVisible(false);
       // ui->btCellOutClear->setVisible(false);
       // ui->btInterfaceClear->setVisible(false);

       // ui->btMkdir->setVisible(false);
       // ui->btFtpUpload->setVisible(false);

    }
    if(logonPrivIdx>5)
    {
        ui->tab_userreg->setEnabled(true);
        ui->tab_usermanag->setEnabled(true);
        //ui->tab_log->setEnabled(false);
        ui->tab_lnc_file->setEnabled(true);
        ui->tab_lnc_article->setEnabled(true);
        ui->leLNCPaswd->setEnabled(true);
        ui->leLncTimeOut->setEnabled(true);
        ui->leLncIP->setEnabled(true);
        ui->chbNoLncPaswd->setEnabled(true);
        ui->chbDispLncPaswd->setEnabled(true);
        ui->btConnectLncByFind->setEnabled(true);
        ui->btConnectLncByIP->setEnabled(true);
        ui->btDisconnectLnc->setEnabled(true);
        ui->cbLncFtpFolder->setEditable(true);
        ui->lePcDownloadFolder->setEnabled(true);
        ui->btBrowsPcDownloadFolder->setEnabled(true);
        ui->lePrivFilename->setEnabled(true);
        ui->btGetFileName->setEnabled(true);

        //lnc
         ui->lePrivAddr->setEnabled(true);
         ui->leTempAddr->setEnabled(true);
         ui->leFsAddr->setEnabled(true);
         ui->leLncLogoutAddr->setEnabled(true);
         ui->leLncIDorNameAddr->setEnabled(true);
         ui->leLncAlowAdminAddr->setEnabled(true);
         ui->leLncHeartRegAddr->setEnabled(true);
         ui->leHeartInterval->setEnabled(true);
         ui->leLncAlarmRegAddr->setEnabled(true);
         ui->leLncWorkStatusRegAddr->setEnabled(true);
         ui->leLncDeepRegWAddr->setEnabled(true);
         ui->leLncRidusRegWAddr->setEnabled(true);
         ui->leLncMMSpdWRegAddr->setEnabled(true);
         ui->leLncStopCtrlRegAddr->setEnabled(true);
         ui->leLncStopCtrlRegAddr2->setEnabled(true);
         ui->leLncUseParamRegAddr->setEnabled(true);

         ui->chbUseParam->setEnabled(true);
         ui->leLncNoOperateAddr->setEnabled(true);

        //Craft
        ui->leCamX->setEnabled(true);
        ui->leCamY->setEnabled(true);
        ui->leCamZ->setEnabled(true);
        ui->leHightDiff->setEnabled(true);
        ui->leProbeFixZ->setEnabled(true);
        ui->leProbeX1->setEnabled(true);
        ui->leProbeX2->setEnabled(true);
        ui->leProbeY1->setEnabled(true);
        ui->leProbeY2->setEnabled(true);
        ui->leSafetyHeitht->setEnabled(true);
        ui->leDigDeep->setEnabled(true);
        ui->leDigDiffX->setEnabled(true);
        ui->leDigDiffY->setEnabled(true);
        ui->leCircleRadius->setEnabled(true);
        ui->leTriPointDist->setEnabled(true);
        ui->lePathCutDeep->setEnabled(true);
        ui->leCutComp->setEnabled(true);
        ui->leUseHour->setEnabled(true);
        ui->leUseMinites->setEnabled(true);
        ui->leUseSeconds->setEnabled(true);
        ui->leFanSpeedR->setEnabled(true);
        ui->leMMSpeed->setEnabled(true);
        ui->lePlaneLevel->setEnabled(true);
        ui->leAirPresure->setEnabled(true);
        ui->leMarkTriger->setEnabled(true);

        ui->chbAirPressure->setEnabled(true);

        //TC
        ui->leTempAddr->setEnabled(true);
        ui->leTcComPort->setEnabled(true);
        ui->leTcBdrate->setEnabled(true);
        ui->cbMbDataBits->setEnabled(true);
        ui->cbMbCrc->setEnabled(true);
        ui->cbMbStopBits->setEnabled(true);
        ui->btConnectTc->setEnabled(true);
        ui->btDisconnectTc->setEnabled(true);
        ui->leTcTimeOut->setEnabled(true);
        ui->leTcAdaptInterval->setEnabled(true);
        ui->leTcSaveTempInterval->setEnabled(true);
        ui->leTcMbAddr->setEnabled(true);
        ui->leTempMbAddr->setEnabled(true);
        ui->leTempFactor->setEnabled(true);
        ui->chbTcConectAuto->setEnabled(true);
      //FC
        ui->leFsAddr->setEnabled(true);
        ui->leFsAdaptInterval->setEnabled(true);
        ui->leFsSaveTempInterval->setEnabled(true);
        ui->leFsMbAddr->setEnabled(true);
        ui->leFsMbSpdAddr->setEnabled(true);
        ui->leFsFactor->setEnabled(true);
       //扫码枪
        ui->leScanComPort->setEnabled(true);
        ui->leScanBdrate->setEnabled(true);
        ui->leScanLength->setEnabled(true);
        ui->leScanRules->setEnabled(true);
        ui->leScanTimeOut->setEnabled(true);
        ui->chbScAutoConnect->setEnabled(true);
        ui->chbScRuleCtrl->setEnabled(true);
        ui->cbScDataBits->setEnabled(true);
        ui->cbScStopBits->setEnabled(true);
        ui->cbScCrc->setEnabled(true);
        ui->leMesMaxDelay->setEnabled(true);
        ui->leMesMaxCnt->setEnabled(true);
        ui->leMesCraftDelay->setEnabled(true);
        ui->chbMesDelayStop->setEnabled(true);
        ui->chbMesOnlineMode->setEnabled(true);

        ui->btClearLogInLog->setEnabled(true);
        ui->btClearNewUserLog->setEnabled(true);
        ui->btClearFilesLog->setEnabled(true);
        ui->btSaveNewUserLog->setEnabled(true);
        ui->btSaveLogInLog->setEnabled(true);
        ui->btSaveFilesLog->setEnabled(true);

        ui->btClearTemp->setVisible(true);
        ui->chbConnectLncAuto->setEnabled(true);
        ui->chbTcConectAuto->setEnabled(true);
        ui->btClearSysLog->setVisible(true);
        ui->btClearCraft->setVisible(true);
        ui->btGenTableVeiw->setEnabled(true);//删除文件

        if(logonPrivIdx>=5)
        ui->btClearTemp->setEnabled(true);
        if(logonPrivIdx>=5)
        ui->btClearTemp->setVisible(true);
        ui->chbConnectLncAuto->setEnabled(true);
        ui->chbTcConectAuto->setEnabled(true);
        if(logonPrivIdx>=5)
            ui->btClearSysLog->setVisible(true);
        if(logonPrivIdx>=5)
            ui->btClearCraft->setVisible(true);
        if(logonPrivIdx>=5)
        ui->btGenTableVeiw->setEnabled(true);//删除文件
        if(logonPrivIdx>=5)
        ui->btClearEnterLog->setVisible(true);//
        if(logonPrivIdx>=4)
        ui->btClearExitLog->setVisible(true);//
        if(logonPrivIdx>=5)
        ui->btClearFanSpd->setVisible(true);//
        if(logonPrivIdx>=5)
        ui->btClearFormal->setVisible(true);//
        if(logonPrivIdx>=5)
        ui->btClearNGdata->setVisible(true);//
        if(logonPrivIdx>=5)
        ui->btClearFinishLog->setVisible(true);//
        if(logonPrivIdx>=5)
        ui->btClearScanLog->setVisible(true);//

        if(logonPrivIdx>=5)
        {
            ui->btCellInClear->setVisible(true);
            ui->btCellOutClear->setVisible(true);
            ui->btInterfaceClear->setVisible(true);
            ui->btMkdir->setVisible(true);
            ui->btFtpUpload->setVisible(true);
        }


        ui->btScanConnect->setEnabled(true);
        ui->btScanDisConnect->setEnabled(true);

        if(logonPrivIdx>5)
        {
            qDebug()<<"logonPrivIdx="<<logonPrivIdx;
            //ui->tab_file_ananlys->setVisible(true);
            ui->tab_file_ananlys->setEnabled(true);
            //ui->tab_lnc_logtab->setVisible(true);
            ui->tab_lnc_logtab->setEnabled(true);
            ui->tabWidgetLncTab->insertTab(0,ui->tab_file_ananlys,"操作日志分析");
            ui->tabWidgetLncTab->insertTab(1,ui->tab_lnc_logtab,"日志分析表");
          //  showMaximized();
        }

        ui->leDevAlarmAddr->setEnabled(true);
        ui->leToolUseCnt->setEnabled(true);

        ui->chbPlane->setEnabled(true);
        ui->chbToolUsed->setEnabled(true);

        //MES
        ui->leDevCodeW->setEnabled(true);
        ui->leProcessNumW->setEnabled(true);
        ui->leCraftAdr->setEnabled(true);
        ui->leFormulaAdr->setEnabled(true);
        ui->leDevInfoAdr->setEnabled(true);
        ui->leOrderAdr->setEnabled(true);
        ui->leOrderListAdr->setEnabled(true);
        ui->leLoadAdr->setEnabled(true);
        ui->leUnloadAdr->setEnabled(true);
        ui->leMaterialAdr->setEnabled(true);
        ui->leEnterAdr->setEnabled(true);
        ui->leNGAdr->setEnabled(true);
        ui->leExitAdr->setEnabled(true);
        ui->leFtpPath->setEnabled(true);

        ui->chbAutoLogs->setEnabled(true);
        ui->chbFtpAutoConnect->setEnabled(true);
        ui->leNGLeadCode->setEnabled(true);
        ui->leFtpFormalDir->setEnabled(true);
        ui->leFtpFormalVerifyDir->setEnabled(true);
        ui->leFtpEquVerifyDir->setEnabled(true);
        ui->leFtpBillInfoDir->setEnabled(true);
        ui->leFtpBillListDir->setEnabled(true);
        ui->leFtpCellInDir->setEnabled(true);
        ui->leFtpCellOutDir->setEnabled(true);
        ui->leFtpSyncTime->setEnabled(true);
        ui->leFtpServerIP->setEnabled(true);
        ui->leFtpPort->setEnabled(true);
        ui->leFtpUserName->setEnabled(true);
        ui->leFtpPassword->setEnabled(true);
        ui->leFtpNGDir->setEnabled(true);
        ui->leFtpWorkDoneDir->setEnabled(true);
        ui->leProbeBaseZ->setEnabled(true);
        ui->chbProbeBazeZ->setEnabled(true);


        ui->btClearEnterLog->setVisible(true);//
        ui->btClearExitLog->setVisible(true);//
        ui->btClearFanSpd->setVisible(true);//
        ui->btClearFormal->setVisible(true);//
        ui->btClearNGdata->setVisible(true);//
        ui->btClearFinishLog->setVisible(true);//
        ui->btClearScanLog->setVisible(true);//
        ui->btScanConnect->setEnabled(true);
        ui->btScanDisConnect->setEnabled(true);

        ui->btCellInClear->setVisible(true);
        ui->btCellOutClear->setVisible(true);
        ui->btInterfaceClear->setVisible(true);
        ui->btMkdir->setVisible(true);
        ui->btFtpUpload->setVisible(true);

    }
/*
    if(RegAlowAdmin)
    {
        ui->leLogInUser->setReadOnly(false);
        ui->leLogInPaswd->setReadOnly(false);
       // ui->tab_login->setEnabled(true);
    }
    else
    {
        ui->leLogInUser->setReadOnly(true);
        ui->leLogInPaswd->setReadOnly(true);
       // ui->tab_login->setEnabled(false);
    }
*/
    QFile privfile (ui->lePrivFilename->text());
    bool isOk =privfile.open(QIODevice::ReadOnly | QIODevice::WriteOnly);
    if (isOk)
             {
                 QString privnum;
                 if(logonPrivIdx>=3) privnum="1";else privnum="0";
                 QString privValue="priv="+QString::number(logonPrivIdx)+"\n"+"manual="+privnum;
                 //转化为本地平台编码
                 privfile.write(privValue.toLocal8Bit());

                 QSqlQuery query;
                 QString sqlprepare;
                 sqlprepare="UPDATE ccdused SET priv='"+QString::number(logonPrivIdx)+"',manual='"+privnum+"' WHERE id=1";
                 query.prepare(sqlprepare);
                 query.exec();
             }
             //关闭文件
            privfile.close();

  }
/*
  if(RegNameOrID != lastIDstatus)//刷卡变更，当寄存器地址不为0，可以刷卡，开启键盘监控
  {
      lastIDstatus=RegNameOrID;
      if(RegNameOrID !=0)
          startHook();
      else
          stopHook();
  }
*/

  if((RegNameOrID==1) && (RegAlowAdmin==0))//刷卡变更对应页面显示
  {
      if(ui->leLogInUser->isEnabled()==true)
          ui->leLogInUser->setEnabled(false);
      if(ui->leLogInPaswd->isEnabled()==true)
          ui->leLogInPaswd->setEnabled(false);
      if(ui->btLogIn->isEnabled()==true)
          ui->btLogIn->setEnabled(false);

  }
  else
  {
      if(ui->leLogInUser->isEnabled()==false)
          ui->leLogInUser->setEnabled(true);
      if(ui->leLogInPaswd->isEnabled()==false)
          ui->leLogInPaswd->setEnabled(true);
      if(ui->btLogIn->isEnabled()==false)
          ui->btLogIn->setEnabled(true);
  }

/*
  LncPrivAddrR=m_IniFile->value("LncPrivAddrR").toInt();
  AutoLogOutTime=m_IniFile->value("AutoLogOutTime").toLongLong();
  MesTimeOut=m_IniFile->value("MesTimeOut").toFloat();
  MesMaxCnt=m_IniFile->value("MesMaxCnt").toFloat();
  LncTempAddrR=m_IniFile->value("LncTempAddrR").toInt();
  LncFanSpdWAddrR=m_IniFile->value("LncFanSpdWAddrR").toInt();
  LncHearCtrlInterval=m_IniFile->value("LncHearCtrlInterval").toFloat();
  LncNoOperateAddrR=m_IniFile->value("LncNoOperateAddrR").toInt();
  LncHeartCtrlAddrR=m_IniFile->value("LncHeartCtrlAddrR").toInt();
  LncUseParamWAddrR=m_IniFile->value("LncUseParamWAddrR").toInt();
  LncWorkDeepAddrR=m_IniFile->value("LncWorkDeepAddrR").toInt();
  LncWorkRidusAddrR=m_IniFile->value("LncWorkRidusAddrR").toInt();
  LncMMSpdWAddrR=m_IniFile->value("LncMMSpdWAddrR").toInt();
  LncStopCtrlWAddrR=m_IniFile->value("LncStopCtrlWAddrR").toInt();
  LncStopCtrlWAddrR2=m_IniFile->value("LncStopCtrlWAddrR2").toInt();
  LncWorkStatusAddrR=m_IniFile->value("LncWorkStatusAddrR").toInt();
  LncAlowAdminAddrR=m_IniFile->value("LncAlowAdminAddrR").toInt();
  LncLogOutAddrR=m_IniFile->value("LncLogOutAddrR").toInt();
  LncIDorNameAddrR=m_IniFile->value("LncIDorNameAddrR").toInt();
  LncCamXAddrR=m_IniFile->value("LncCamXAddrR").toInt();
  LncCamYAddrR=m_IniFile->value("LncCamYAddrR").toInt();
  LncCamZAddrR=m_IniFile->value("LncCamZAddrR").toInt();
  LncProbeFixZAddrR=m_IniFile->value("LncProbeFixZAddrR").toInt();
  LncProbeX1AddrR=m_IniFile->value("LncProbeX1AddrR").toInt();
  LncProbeY1AddrR=m_IniFile->value("LncProbeY1AddrR").toInt();
  LncProbeX2AddrR=m_IniFile->value("LncProbeX2AddrR").toInt();
  LncProbeY2AddrR=m_IniFile->value("LncProbeY2AddrR").toInt();
  LncSafetyHeithtAddrR=m_IniFile->value("LncSafetyHeithtAddrR").toInt();
  LncHightDiffAddrR=m_IniFile->value("LncHightDiffAddrR").toInt();
  LncDigDeepAddrR=m_IniFile->value("LncDigDeepAddrR").toInt();
  LncDigDiffXAddrR=m_IniFile->value("LncDigDiffXAddrR").toInt();
  LncDigDiffYAddrR=m_IniFile->value("LncDigDiffYAddrR").toInt();
  LncCircleRadiusAddrR=m_IniFile->value("LncCircleRadiusAddrR").toInt();
  LncTriPointDistAddrR=m_IniFile->value("LncTriPointDistAddrR").toInt();
  LncCutCompAddrR=m_IniFile->value("LncCutCompAddrR").toInt();
  LncPathCutDeepAddrR=m_IniFile->value("LncPathCutDeepAddrR").toInt();
  LncMarkTrigerAddrR=m_IniFile->value("LncMarkTrigerAddrR").toInt();
  LncUseHourAddrR=m_IniFile->value("LncUseHourAddrR").toInt();
  LncUseMinitesAddrR=m_IniFile->value("LncUseMinitesAddrR").toInt();
  LncUseSecondsAddrR=m_IniFile->value("LncUseSecondsAddrR").toInt();
  LncFanSpeedR=m_IniFile->value("LncFanSpeedR").toInt();
  LncMMSpeedAddrR=m_IniFile->value("LncMMSpeedAddrR").toInt();
  LncAirPresureAddrR=m_IniFile->value("LncAirPresureAddrR").toInt();
  LncPlaneLevelAddrR=m_IniFile->value("LncPlaneLevelAddrR").toInt();
  LncAlarmCodeAddrR=m_IniFile->value("LncAlarmCodeAddrR").toInt();
*/
}
