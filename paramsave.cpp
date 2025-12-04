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

void MainWindow::on_leLncIP_editingFinished()
{
     m_IniFile->setValue("LncIP",ui->leLncIP->text());
     ui->lbLncIP->setText(ui->leLncIP->text());
     QString msg="修改LNC设备IP设置";
     const_msg(msg);
}

void MainWindow::on_chbDispLncPaswd_clicked()
{
    if(ui->chbDispLncPaswd->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("DisplayLncPasswd","1");
        ui->leLNCPaswd->setEchoMode(QLineEdit::Normal);
    }
    else
    {
         m_IniFile->setValue("DisplayLncPasswd","0");
        ui->leLNCPaswd->setEchoMode(QLineEdit::Password);
    }
}

void MainWindow::on_chbNoLncPaswd_clicked()
{
    QString msg;
    if(ui->chbNoLncPaswd->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("LncNoPasswd","1");
        msg="修改LNC有密码配置";
    }
    else
    {
         m_IniFile->setValue("LncNoPasswd","0");
         msg="修改LNC无密码配置";
    }
    const_msg(msg);
}

void MainWindow::on_chbConnectLncAuto_clicked()
{
    QString msg;
    if(ui->chbConnectLncAuto->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("LncAutoConnect","1");
        msg="修改LNC链接方式为自动链接";
    }
    else
    {
         m_IniFile->setValue("LncAutoConnect","0");
         msg="修改LNC链接方式为手动链接";
    }
    const_msg(msg);
}

void MainWindow::on_leLncTimeOut_editingFinished()
{
    if((ui->leLncTimeOut->text().toInt()<3000) || (ui->leLncTimeOut->text()==""))
        ui->leLncTimeOut->setText("3000");
    m_IniFile->setValue("LncTimeOut",ui->leLncTimeOut->text());
    QString msg="修改LNC超时重连接时间为"+ui->leLncTimeOut->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_lePcDownloadFolder_editingFinished()
{
    m_IniFile->setValue("LocalDownloadFolder",ui->lePcDownloadFolder->text().toUtf8());
    QString msg="修改下载目录为"+ui->lePcDownloadFolder->text();
    const_msg(msg);
}

void MainWindow::on_lePrivAddr_editingFinished()
{
    m_IniFile->setValue("LncPrivAddrR",ui->lePrivAddr->text());
    QString msg="修改LNC权限写入R地址为R"+ui->lePrivAddr->text();
    const_msg(msg);
}

void MainWindow::on_leTempAddr_editingFinished()
{
    m_IniFile->setValue("LncTempAddrR",ui->leTempAddr->text());
    QString msg="修改LNC温度写入R地址为R"+ui->leTempAddr->text();
    const_msg(msg);
}

void MainWindow::on_leFsAddr_editingFinished()
{
    m_IniFile->setValue("LncFanSpdWAddrR",ui->leFsAddr->text());
    QString msg="修改LNC风速写入R地址为R"+ui->leFsAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncLogoutAddr_editingFinished()
{
    m_IniFile->setValue("LncLogOutAddrR",ui->leLncLogoutAddr->text());
    QString msg="修改LNC登出监控R地址为R"+ui->leLncLogoutAddr->text();
    const_msg(msg);
}



void MainWindow::on_leLncAlarmRegAddr_editingFinished()
{
    m_IniFile->setValue("LncAlarmCodeAddrR",ui->leLncAlarmRegAddr->text());
    QString msg="修改LNC状态监控R地址为"+ui->leLncAlarmRegAddr->text();
    const_msg(msg);
}



void MainWindow::on_leLncWorkStatusRegAddr_editingFinished()
{
    m_IniFile->setValue("LncWorkStatusAddrR",ui->leLncWorkStatusRegAddr->text());
    QString msg="修改LNC加工完成监控R地址为"+ui->leLncWorkStatusRegAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncDeepRegWAddr_editingFinished()
{
    m_IniFile->setValue("LncWorkDeepAddrR",ui->leLncDeepRegWAddr->text());
    QString msg="修改LNC加工深度写入R地址为"+ui->leLncDeepRegWAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncRidusRegWAddr_editingFinished()
{
    m_IniFile->setValue("LncWorkRidusAddrR",ui->leLncRidusRegWAddr->text());
    QString msg="修改LNC加工半径写入R地址为"+ui->leLncRidusRegWAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncMMSpdWRegAddr_editingFinished()
{
    m_IniFile->setValue("LncMMSpdWAddrR",ui->leLncMMSpdWRegAddr->text());
    QString msg="修改LNC加工主轴速度写入R地址为"+ui->leLncMMSpdWRegAddr->text();
    const_msg(msg);
}
void MainWindow::on_leLncStopCtrlRegAddr_editingFinished()
{
    m_IniFile->setValue("LncStopCtrlWAddrR",ui->leLncStopCtrlRegAddr->text());
    QString msg="修改LNC停止加工控制写入R地址1为"+ui->leLncStopCtrlRegAddr->text();
    const_msg(msg);
}
void MainWindow::on_leLncUseParamRegAddr_editingFinished()
{
    m_IniFile->setValue("LncUseParamWAddrR",ui->leLncUseParamRegAddr->text());
    QString msg="修改LNC启用配方数据R地址1为"+ui->leLncUseParamRegAddr->text();
    const_msg(msg);
}
void MainWindow::on_leDevAlarmAddr_editingFinished()
{
    m_IniFile->setValue("DevAlarmAddrR",ui->leDevAlarmAddr->text());
    QString msg="修改外设异常状态写入R地址1为"+ui->leDevAlarmAddr->text();
    const_msg(msg);
}

void MainWindow::on_leTcComPort_editingFinished()
{
    if(ui->leTcComPort->text()=="")
        m_IniFile->setValue("TcComPort","COM1");
    else
        m_IniFile->setValue("TcComPort",ui->leTcComPort->text());
    QString msg="修改Modbus端口为"+ui->leTcComPort->text();
    const_msg(msg);
}

void MainWindow::on_leTcBdrate_editingFinished()
{
    m_IniFile->setValue("TcComBodrate",ui->leTcBdrate->text());
    QString msg="修改Modbus端口波特率为"+ui->leTcBdrate->text();
    const_msg(msg);
}

void MainWindow::on_cbMbDataBits_currentIndexChanged(const QString &arg1)
{
    m_IniFile->setValue("TcDataBits",QString::number(ui->cbMbDataBits->currentIndex()));
    QString msg="修改Modbus端口数据位为"+ui->cbMbDataBits->currentText();
    const_msg(msg);
}

void MainWindow::on_cbMbCrc_currentIndexChanged(const QString &arg1)
{
    m_IniFile->setValue("TcCrc",QString::number(ui->cbMbCrc->currentIndex()));
    QString msg="修改Modbus端口校验方式为"+ui->cbMbCrc->currentText();
    const_msg(msg);
}

void MainWindow::on_cbMbStopBits_currentIndexChanged(const QString &arg1)
{
    m_IniFile->setValue("TcStopBits",QString::number(ui->cbMbStopBits->currentIndex()));
    QString msg="修改Modbus端口停止位为"+ui->cbMbStopBits->currentText();
    const_msg(msg);
}

void MainWindow::on_chbTcConectAuto_clicked()
{
    QString msg;

    if(ui->chbTcConectAuto->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("TcAutoConnect","1");
        msg="修改Modbus链接方式为自动链接";
    }
    else
    {
         m_IniFile->setValue("TcAutoConnect","0");
         msg="修改Modbus链接方式为手动链接";
    }
    const_msg(msg);
}

void MainWindow::on_leTcTimeOut_editingFinished()
{
    if((ui->leTcTimeOut->text().toInt()<1000) || (ui->leTcTimeOut->text()==""))
        ui->leTcTimeOut->setText("1000");
    m_IniFile->setValue("TcComTimeOut",ui->leTcTimeOut->text());
    QString msg="修改温控器超时重连接时间为"+ui->leTcTimeOut->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leTcAdaptInterval_editingFinished()
{
    if((ui->leTcAdaptInterval->text().toInt()<50) || (ui->leTcAdaptInterval->text()==""))
        ui->leTcAdaptInterval->setText("50");
    m_IniFile->setValue("TcAdaptTime",ui->leTcAdaptInterval->text());
    QString msg="修改温度采集间隔时间为"+ui->leTcAdaptInterval->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leTcSaveTempInterval_editingFinished()
{
    if(ui->leTcSaveTempInterval->text().toInt()<200)
        ui->leTcSaveTempInterval->setText("200");
    m_IniFile->setValue("TcSaveTime",ui->leTcSaveTempInterval->text());
    QString msg="修改温度存储间隔时间为"+ui->leTcSaveTempInterval->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leTcMbAddr_editingFinished()
{
    if((ui->leTcMbAddr->text()=="") || (ui->leTcMbAddr->text().toInt()<1))
        m_IniFile->setValue("TcMbSlave","1");
    else
        m_IniFile->setValue("TcMbSlave",ui->leTcMbAddr->text());
    QString msg="修改温控器Modbus设备站号为"+ui->leTcMbAddr->text();
    const_msg(msg);
}

void MainWindow::on_leTempMbAddr_editingFinished()
{
    m_IniFile->setValue("TcTempAddr",ui->leTempMbAddr->text());
    QString msg="修改温度值对应的Modbus地址为"+ui->leTempMbAddr->text();
    const_msg(msg);
}

void MainWindow::on_leTempFactor_editingFinished()
{
    m_IniFile->setValue("TcTempFactor",ui->leTempFactor->text());
    QString msg="修改采集到的温度值的精度系数为"+ui->leTempFactor->text();
    const_msg(msg);
}
//选择文件系统权限文件
void MainWindow::on_btGetFileName_clicked()
{
    QString file_fullname;
    file_fullname = QFileDialog::getOpenFileName(this,tr("选择文件"), "./priv.txt", tr("txt (*.txt)"));
    if(file_fullname.isEmpty())
        m_IniFile->setValue("LocalPrivFile",QCoreApplication::applicationDirPath()+"/data/etc/priv.txt");
    else
        m_IniFile->setValue("LocalPrivFile",file_fullname);

    ui->lePrivFilename->setText(m_IniFile->value("LocalPrivFile").toString());
}



void MainWindow::on_lePrivFilename_editingFinished()
{
    if(ui->lePrivFilename->text()=="")
        m_IniFile->setValue("LocalPrivFile",QCoreApplication::applicationDirPath()+"/data/etc/priv.txt");
    else
        m_IniFile->setValue("LocalPrivFile",ui->lePrivFilename->text());
    QString msg="修改权限值文件为"+ui->lePrivFilename->text();
    const_msg(msg);
}


void MainWindow::on_chbLogOutByLnc_clicked()
{
    QString msg;

    if(ui->chbLogOutByLnc->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("LogOutByLnc","1");
        msg="设置系统退出可由LNC联动发起";
        stopHook();
        startHook();
    }
    else
    {
         m_IniFile->setValue("LogOutByLnc","0");
         msg="设置系统退出不由LNC联动发起";
         stopHook();
    }
    const_msg(msg);
}

void MainWindow::on_leLogoutTime_editingFinished()
{
    if((ui->leLogoutTime->text().toInt()<1) || (ui->leLogoutTime->text()==""))
        ui->leLogoutTime->setText("1");
    m_IniFile->setValue("AutoLogOutTime",ui->leLogoutTime->text());
    QString msg="修改系统无操作退出登录时间为"+ui->leLogoutTime->text()+"分钟";
    const_msg(msg);
}

void MainWindow::on_chbLogOutAuto_clicked()
{
    QString msg;

    if(ui->chbLogOutAuto->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("AutoLogOut","1");
        msg="设置系统退出可由软件无操作倒计时控制";
    }
    else
    {
         m_IniFile->setValue("AutoLogOut","0");
         msg="设置系统退出不由软件无操作倒计时控制";
    }
    const_msg(msg);
}


void MainWindow::on_leHeartInterval_editingFinished()
{
    if(ui->leHeartInterval->text().toFloat()<0.4)
        ui->leHeartInterval->setText("0.4");
    m_IniFile->setValue("LncHearCtrlInterval",ui->leHeartInterval->text());
    QString msg="修改LNC心跳控制周期为"+ui->leHeartInterval->text()+"秒";
    const_msg(msg);
}

void MainWindow::on_leLncHeartRegAddr_editingFinished()
{
    m_IniFile->setValue("LncHeartCtrlAddrR",ui->leLncHeartRegAddr->text());
    QString msg="修改LNC心跳控制写R地址为R"+ui->leLncHeartRegAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncNoOperateAddr_editingFinished()
{
    m_IniFile->setValue("LncNoOperateAddrR",ui->leLncNoOperateAddr->text());
    QString msg="修改LNC无操作时间写入R地址为R"+ui->leLncNoOperateAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncAlowAdminAddr_editingFinished()
{
    m_IniFile->setValue("LncAlowAdminAddrR",ui->leLncAlowAdminAddr->text());
    QString msg="修改LNC超级用户控制R地址为R"+ui->leLncAlowAdminAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncIDorNameAddr_editingFinished()
{
    m_IniFile->setValue("LncIDorNameAddrR",ui->leLncIDorNameAddr->text());
    QString msg="修改LNC刷卡或手动登录控制R地址为R"+ui->leLncIDorNameAddr->text();
    const_msg(msg);
}

void MainWindow::on_leLncStopCtrlRegAddr2_editingFinished()
{
    m_IniFile->setValue("LncStopCtrlWAddrR2",ui->leLncStopCtrlRegAddr2->text());
    QString msg="修改LNC停止加工控制写入R地址2为"+ui->leLncStopCtrlRegAddr2->text();
    const_msg(msg);
}

void MainWindow::on_leFanSpeedR_editingFinished()
{
    m_IniFile->setValue("LncFanSpeedR",ui->leFanSpeedR->text());
    QString msg="修改LNC风速仪读取R地址为"+ui->leFanSpeedR->text();
    const_msg(msg);
}


void MainWindow::on_leCamX_editingFinished()
{
    m_IniFile->setValue("LncCamXAddrR",ui->leCamX->text());
    QString msg="修改LNC拍照X坐标R地址为R"+ui->leCamX->text();
    const_msg(msg);
}

void MainWindow::on_leCamY_editingFinished()
{
    m_IniFile->setValue("LncCamYAddrR",ui->leCamY->text());
    QString msg="修改LNC拍照Y坐标R地址为R"+ui->leCamY->text();
    const_msg(msg);
}

void MainWindow::on_leCamZ_editingFinished()
{
    m_IniFile->setValue("LncCamZAddrR",ui->leCamZ->text());
    QString msg="修改LNC拍照Z坐标R地址为R"+ui->leCamZ->text();
    const_msg(msg);
}

void MainWindow::on_leHightDiff_editingFinished()
{
    m_IniFile->setValue("LncHightDiffAddrR",ui->leHightDiff->text());
    QString msg="修改LNC测高偏差值R地址为R"+ui->leHightDiff->text();
    const_msg(msg);
}

void MainWindow::on_leProbeFixZ_editingFinished()
{
    m_IniFile->setValue("LncProbeFixZAddrR",ui->leProbeFixZ->text());
    QString msg="修改LNC测高定位Z坐标R地址为R"+ui->leProbeFixZ->text();
    const_msg(msg);
}

void MainWindow::on_leProbeX1_editingFinished()
{
    m_IniFile->setValue("LncProbeX1AddrR",ui->leProbeX1->text());
    QString msg="修改LNC测高位置X1坐标R地址为R"+ui->leProbeX1->text();
    const_msg(msg);
}

void MainWindow::on_leProbeY1_editingFinished()
{
    m_IniFile->setValue("LncProbeY1AddrR",ui->leProbeY1->text());
    QString msg="修改LNC测高位置Y1坐标R地址为R"+ui->leProbeY1->text();
    const_msg(msg);
}

void MainWindow::on_leProbeX2_editingFinished()
{
    m_IniFile->setValue("LncProbeX2AddrR",ui->leProbeX2->text());
    QString msg="修改LNC测高位置X2坐标R地址为R"+ui->leProbeX2->text();
    const_msg(msg);
}

void MainWindow::on_leProbeY2_editingFinished()
{
    m_IniFile->setValue("LncProbeY2AddrR",ui->leProbeY2->text());
    QString msg="修改LNC测高位置Y2坐标R地址为R"+ui->leProbeY2->text();
    const_msg(msg);
}

void MainWindow::on_leSafetyHeitht_editingFinished()
{
    m_IniFile->setValue("LncSafetyHeithtAddrR",ui->leSafetyHeitht->text());
    QString msg="修改LNC铣削安全高度R地址为R"+ui->leSafetyHeitht->text();
    const_msg(msg);
}

void MainWindow::on_leDigDeep_editingFinished()
{
    m_IniFile->setValue("LncDigDeepAddrR",ui->leDigDeep->text());
    QString msg="修改LNC铣削深度R地址为R"+ui->leDigDeep->text();
    const_msg(msg);
}

void MainWindow::on_leDigDiffX_editingFinished()
{
    m_IniFile->setValue("LncDigDiffXAddrR",ui->leDigDiffX->text());
    QString msg="修改LNC铣削坐标偏量X值R地址为R"+ui->leDigDiffX->text();
    const_msg(msg);
}

void MainWindow::on_leDigDiffY_editingFinished()
{
    m_IniFile->setValue("LncDigDiffYAddrR",ui->leDigDiffY->text());
    QString msg="修改LNC铣削坐标偏量Y值R地址为R"+ui->leDigDiffY->text();
    const_msg(msg);
}

void MainWindow::on_leCircleRadius_editingFinished()
{
    m_IniFile->setValue("LncCircleRadiusAddrR",ui->leCircleRadius->text());
    QString msg="修改LNC加工圆半径R地址为R"+ui->leCircleRadius->text();
    const_msg(msg);
}

void MainWindow::on_leTriPointDist_editingFinished()
{
    m_IniFile->setValue("LncTriPointDistAddrR",ui->leTriPointDist->text());
    QString msg="修改LNC三点测量间距R地址为R"+ui->leTriPointDist->text();
    const_msg(msg);
}

void MainWindow::on_leCutComp_editingFinished()
{
    m_IniFile->setValue("LncCutCompAddrR",ui->leCutComp->text());
    QString msg="修改LNC刀具直径补偿R地址为R"+ui->leCutComp->text();
    const_msg(msg);
}

void MainWindow::on_lePathCutDeep_editingFinished()
{
    m_IniFile->setValue("LncPathCutDeepAddrR",ui->lePathCutDeep->text());
    QString msg="修改LNC铣削单圈Z轴下刀量R地址为R"+ui->lePathCutDeep->text();
    const_msg(msg);
}

void MainWindow::on_leMarkTriger_editingFinished()
{
    m_IniFile->setValue("LncMarkTrigerAddrR",ui->leMarkTriger->text());
    QString msg="修改LNC工艺表单记录触发器R地址为R"+ui->leMarkTriger->text();
    const_msg(msg);
}

void MainWindow::on_leUseHour_editingFinished()
{
    m_IniFile->setValue("LncUseHourAddrR",ui->leUseHour->text());
    QString msg="修改LNC工艺表单加工时间小时数R地址为R"+ui->leUseHour->text();
    const_msg(msg);
}

void MainWindow::on_leUseMinites_editingFinished()
{
    m_IniFile->setValue("LncUseMinitesAddrR",ui->leUseMinites->text());
    QString msg="修改LNC工艺表单记加工时间毫秒值R地址为R"+ui->leUseMinites->text();
    const_msg(msg);
}

void MainWindow::on_leUseSeconds_editingFinished()
{
    m_IniFile->setValue("LncUseSecondsAddrR",ui->leUseSeconds->text());
    QString msg="修改LNC工艺表单加工时间秒钟数R地址为R"+ui->leUseSeconds->text();
    const_msg(msg);
}
void MainWindow::on_leMMSpeed_editingFinished()
{
    m_IniFile->setValue("LncMMSpeedAddrR",ui->leMMSpeed->text());
    QString msg="修改LNC工艺表单主轴转速采集R地址为R"+ui->leMMSpeed->text();
    const_msg(msg);
}

void MainWindow::on_lePlaneLevel_editingFinished()
{
    m_IniFile->setValue("LncPlaneLevelAddrR",ui->lePlaneLevel->text());
    QString msg="修改LNC工艺表单表面平整度采集R地址为R"+ui->lePlaneLevel->text();
    const_msg(msg);
}

void MainWindow::on_leAirPresure_editingFinished()
{
    m_IniFile->setValue("LncAirPresureAddrR",ui->leAirPresure->text());
    QString msg="修改LNC工艺表单风速采集R地址为R"+ui->leAirPresure->text();
    const_msg(msg);
}
void MainWindow::on_leToolUseCnt_editingFinished()
{
    m_IniFile->setValue("LncToolUseCntAddrR",ui->leToolUseCnt->text());
    QString msg="修改LNC工艺表单风速采集R地址为R"+ui->leToolUseCnt->text();
    const_msg(msg);
}
void MainWindow::on_chbAirPressure_clicked()
{
    QString msg;

    if(ui->chbAirPressure->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("MesUpLoadAirPressure","1");
        msg="设置为风压为生产履历上传参数";
    }
    else
    {
         m_IniFile->setValue("MesUpLoadAirPressure","0");
         msg="取消风压为生产履历上传参数";
    }
    const_msg(msg);
}


//表面平整度是否上传
void MainWindow::on_chbPlane_clicked()
{
    QString msg;

    if(ui->chbPlane->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("MesUpLoadPlane","1");
        msg="设置表面平整度为上传参数";
    }
    else
    {
         m_IniFile->setValue("MesUpLoadPlane","0");
         msg="取消表面平整度为上传参数";
    }
    const_msg(msg);
}


//刀具使用次数是否上传
void MainWindow::on_chbToolUsed_clicked()
{
    QString msg;

    if(ui->chbToolUsed->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("MesUpLoadToolUsed","1");
        msg="设置刀具使用次数为上传参数";
    }
    else
    {
         m_IniFile->setValue("MesUpLoadToolUsed","0");
         msg="取消刀具使用次数为上传参数";
    }
    const_msg(msg);
}

void MainWindow::on_leProbeBaseZ_editingFinished()
{
    m_IniFile->setValue("LncProbeBaseZAddrR",ui->leProbeBaseZ->text());
    QString msg="修改LNC铣削Z探测值采集R地址为R"+ui->leProbeBaseZ->text();
    const_msg(msg);
}
void MainWindow::on_chbProbeBazeZ_clicked()
{
    QString msg;

    if(ui->chbProbeBazeZ->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("MesUpLoadProbeZ","1");
        msg="设置铣削Z探测值为上传参数";
    }
    else
    {
         m_IniFile->setValue("MesUpLoadProbeZ","0");
         msg="取消铣削Z探测值为上传参数";
    }
    const_msg(msg);
}

void MainWindow::on_leFsAdaptInterval_editingFinished()
{
    m_IniFile->setValue("FanAdaptTime",ui->leFsAdaptInterval->text());
    //ui->leFsAdaptInterval->setText(ui->leFsAdaptInterval->text());
    QString msg="修改风速仪采集时间为"+ui->leFsAdaptInterval->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leFsSaveTempInterval_editingFinished()
{
    m_IniFile->setValue("FanSaveTime",ui->leFsSaveTempInterval->text());
    QString msg="修改风速仪记录风速周期为"+ui->leFsSaveTempInterval->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leFsMbAddr_editingFinished()
{
    m_IniFile->setValue("FanMbSlave",ui->leFsMbAddr->text());
    QString msg="修改风速仪Modbus设备地址为"+ui->leFsMbAddr->text();
    const_msg(msg);
}

void MainWindow::on_leFsMbSpdAddr_editingFinished()
{
    m_IniFile->setValue("FanSpeedAddr",ui->leFsMbSpdAddr->text());
    QString msg="修改风速仪风速采集地址为"+ui->leFsMbSpdAddr->text();
    const_msg(msg);
}

void MainWindow::on_leFsFactor_editingFinished()
{
    m_IniFile->setValue("FanSpeedFactor",ui->leFsFactor->text());
    QString msg="修改风速仪风速精度因子为"+ui->leFsFactor->text();
    const_msg(msg);
}

void MainWindow::on_leDevCodeW_editingFinished()
{
    m_IniFile->setValue("LncDevCode",ui->leDevCodeW->text());
    ui->leDevCodeR->setText(ui->leDevCodeW->text());
    ui->lbLncDevCode->setText(ui->leDevCodeW->text());
    QString msg="修改MES设备编号为"+ui->leDevCodeW->text();
    const_msg(msg);
    postEPFormalVerifyOK=false;
    EPFormalVerifyCnt=m_IniFile->value("MesMaxCnt").toInt();
    postEPFormalVerify();
}

void MainWindow::on_leProcessNumW_editingFinished()
{
    m_IniFile->setValue("lncProcessNum",ui->leProcessNumW->text());
    ui->leProcessNumR->setText(ui->leProcessNumW->text());
    QString msg="修改MES工序编号为"+ui->leProcessNumW->text();
    const_msg(msg);
    postEPFormalVerifyOK=false;
    EPFormalVerifyCnt=m_IniFile->value("MesMaxCnt").toInt();
    postEPFormalVerify();
}

void MainWindow::on_leCraftAdr_editingFinished()
{
    m_IniFile->setValue("mesCraftUrl",ui->leCraftAdr->text());
    QString msg="修改MES工艺申请链接为"+ui->leCraftAdr->text();
    const_msg(msg);
}

void MainWindow::on_leFormulaAdr_editingFinished()
{
    m_IniFile->setValue("mesFormulaUrl",ui->leFormulaAdr->text());
    QString msg="修改MES配方校验链接为"+ui->leFormulaAdr->text();
    const_msg(msg);
}



void MainWindow::on_leDevInfoAdr_editingFinished()
{
    m_IniFile->setValue("mesDevInfoUrl",ui->leDevInfoAdr->text());
    QString msg="修改MES设备参数校验链接为"+ui->leDevInfoAdr->text();
    const_msg(msg);
}

void MainWindow::on_leOrderAdr_editingFinished()
{
    m_IniFile->setValue("mesOrderUrl",ui->leOrderAdr->text());
    QString msg="修改MES获取工单链接为"+ui->leOrderAdr->text();
    const_msg(msg);
}


void MainWindow::on_leOrderListAdr_editingFinished()
{
    m_IniFile->setValue("mesOrderListUrl",ui->leOrderListAdr->text());
    QString msg="修改MES获取工单队列链接为"+ui->leOrderListAdr->text();
    const_msg(msg);
}

void MainWindow::on_leLoadAdr_editingFinished()
{
    m_IniFile->setValue("mesLoadUrl",ui->leLoadAdr->text());
    QString msg="修改MES物料上料链接为"+ui->leLoadAdr->text();
    const_msg(msg);
}

void MainWindow::on_leUnloadAdr_editingFinished()
{
    m_IniFile->setValue("mesUnLoadUrl",ui->leUnloadAdr->text());
    QString msg="修改MES物料卸料链接为"+ui->leUnloadAdr->text();
    const_msg(msg);
}

void MainWindow::on_leMaterialAdr_editingFinished()
{
    m_IniFile->setValue("mesMaterialUrl",ui->leMaterialAdr->text());
    QString msg="修改MES获取物料设备数据链接为"+ui->leMaterialAdr->text();
    const_msg(msg);
}

void MainWindow::on_leEnterAdr_editingFinished()
{
    m_IniFile->setValue("mesEntertUrl",ui->leEnterAdr->text());
    QString msg="修改MES电芯进站校验链接为"+ui->leEnterAdr->text();
    const_msg(msg);
}


void MainWindow::on_leNGAdr_editingFinished()
{
    m_IniFile->setValue("mesNotGoodUrl",ui->leNGAdr->text());
    QString msg="修改MES不良品反馈链接为"+ui->leNGAdr->text();
    const_msg(msg);
}

void MainWindow::on_leExitAdr_editingFinished()
{
    m_IniFile->setValue("mesExitUrl",ui->leExitAdr->text());
    QString msg="修改MES生产履历记录链接为"+ui->leExitAdr->text();
    const_msg(msg);
}

void MainWindow::on_leFtpPath_editingFinished()
{
    m_IniFile->setValue("mesFtpDir",ui->leFtpPath->text());
    QString msg="修改MES的FTP文件路径为"+ui->leFtpPath->text();
    const_msg(msg);
}

void MainWindow::on_leOrderNumR_textChanged(const QString &arg1)
{
    m_IniFile->setValue("mesBillCode",arg1);
}

void MainWindow::on_leOrderCntR_textChanged(const QString &arg1)
{
    m_IniFile->setValue("mesBillNum",arg1);
}

void MainWindow::on_chbUseParam_clicked()
{
    QString msg;

    if(ui->chbUseParam->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("LncUseParam","1");
        msg="设置LNC启用MES配方数据";
    }
    else
    {
         m_IniFile->setValue("LncUseParam","0");
         msg="LNC取消MES配方数据";
    }
    const_msg(msg);
}

void MainWindow::on_chbScAutoConnect_clicked()
{
    QString msg;

    if(ui->chbScAutoConnect->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("ScanAutoConnect","1");
        msg="设置扫码枪链接为自动链接";
    }
    else
    {
         m_IniFile->setValue("ScanAutoConnect","0");
         msg="取消扫码枪自动链接";
    }
    const_msg(msg);
}

void MainWindow::on_chbScRuleCtrl_clicked()
{
    QString msg;

    if(ui->chbScRuleCtrl->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("ScanActiveRules","1");
        msg="设置扫码枪匹配规则生效";
    }
    else
    {
         m_IniFile->setValue("ScanActiveRules","0");
         msg="取消扫码枪匹配规则生效";
    }
    const_msg(msg);
}

void MainWindow::on_chbMesDelayStop_clicked()
{
    QString msg;

    if(ui->chbMesDelayStop->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("MesStopCtrl","1");
        msg="设置检测到MES断线则停止加工";
    }
    else
    {
         m_IniFile->setValue("MesStopCtrl","0");
         msg="取消设测到MES断线停止加工";
    }
    const_msg(msg);
}

void MainWindow::on_chbMesOnlineMode_clicked()
{
    QString msg;

    if(ui->chbMesOnlineMode->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("MesOnLineMode","1");
         ui->lbMesOnLineMode->setStyleSheet("QLabel{background-color: rgb(85, 255, 0);}");
        msg="设置为MES在线模式";
        mesConOK=true;
        postEPFormalVerifyOK=false;
        EPFormalVerifyCnt=m_IniFile->value("MesMaxCnt").toInt();
        postEPFormalVerify();
    }
    else
    {
        ui->lbMesOnLineMode->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
         m_IniFile->setValue("MesOnLineMode","0");
         msg="取消扫MES在线模式";
    }
    const_msg(msg);

}


void MainWindow::on_leScanComPort_editingFinished()
{
    if(ui->leScanComPort->text()=="")
        m_IniFile->setValue("ScanComPort","COM2");
    else
        m_IniFile->setValue("ScanComPort",ui->leScanComPort->text());
    QString msg="修改扫码枪端口为"+ui->leScanComPort->text();
    const_msg(msg);
}

void MainWindow::on_leScanBdrate_editingFinished()
{
    m_IniFile->setValue("ScanComBodrate",ui->leScanBdrate->text());
    QString msg="修改扫码枪端口波特率为"+ui->leScanBdrate->text();
    const_msg(msg);
}

void MainWindow::on_cbScDataBits_currentIndexChanged(int index)
{
    m_IniFile->setValue("ScanDataBits",QString::number(ui->cbScDataBits->currentIndex()));
    QString msg="修改扫码枪端口数据位为"+ui->cbScDataBits->currentText();
    const_msg(msg);
}

void MainWindow::on_cbScStopBits_currentIndexChanged(int index)
{
    m_IniFile->setValue("ScanStopBits",QString::number(ui->cbScStopBits->currentIndex()));
    QString msg="修改扫码枪端口停止位为"+ui->cbScStopBits->currentText();
    const_msg(msg);
}

void MainWindow::on_cbScCrc_currentIndexChanged(int index)
{
    m_IniFile->setValue("ScanCrc",QString::number(ui->cbScCrc->currentIndex()));
    QString msg="修改扫码枪端口校验方式为"+ui->cbScCrc->currentText();
    const_msg(msg);
}

void MainWindow::on_leScanTimeOut_editingFinished()
{
    if((ui->leScanTimeOut->text().toInt()<1000) || (ui->leScanTimeOut->text()==""))
        ui->leScanTimeOut->setText("1000");
    m_IniFile->setValue("ScanComTimeOut",ui->leScanTimeOut->text());
    QString msg="修改扫码枪超时重连接时间为"+ui->leScanTimeOut->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leScanRules_editingFinished()
{
    m_IniFile->setValue("ScanRules",ui->leScanRules->text());
    QString msg="修改扫码枪开始匹配规则位"+ui->leScanRules->text();
    const_msg(msg);
}

void MainWindow::on_leScanLength_editingFinished()
{
    if((ui->leScanLength->text().toInt()<8) || (ui->leScanLength->text()==""))
        ui->leScanLength->setText("8");
    m_IniFile->setValue("ScanCodeLength",ui->leScanLength->text());
    QString msg="修改扫码枪扫秒数据为位"+ui->leScanLength->text();
    const_msg(msg);
}

void MainWindow::on_leMesMaxDelay_editingFinished()
{
    if((ui->leMesMaxDelay->text().toInt()<1000) || (ui->leMesMaxDelay->text()==""))
        ui->leMesMaxDelay->setText("1000");
    m_IniFile->setValue("MesTimeOut",ui->leMesMaxDelay->text());
    QString msg="修改MES链接超时时间为"+ui->leMesMaxDelay->text()+"ms";
    const_msg(msg);
}

void MainWindow::on_leMesMaxCnt_editingFinished()
{
    if((ui->leMesMaxCnt->text().toInt()<1) || (ui->leMesMaxCnt->text()==""))
        ui->leMesMaxCnt->setText("0");
    m_IniFile->setValue("MesMaxCnt",ui->leMesMaxCnt->text());
    QString msg="修改MES最多重连次数为"+ui->leMesMaxCnt->text();
    const_msg(msg);
}

void MainWindow::on_leMesCraftDelay_editingFinished()
{
    if((ui->leMesCraftDelay->text().toInt()<1) || (ui->leMesCraftDelay->text()==""))
        ui->leMesCraftDelay->setText("1");
    m_IniFile->setValue("MesCraftTimeOut",ui->leMesCraftDelay->text());
    QString msg="修改MES工艺定时校验时长为"+ui->leMesCraftDelay->text()+"分钟";
    const_msg(msg);
}

void MainWindow::on_leNGLeadCode_editingFinished()
{
    m_IniFile->setValue("mesNGLeadCode",ui->leNGLeadCode->text());
    QString msg="修改MES不良反馈不良原因代码前缀为"+ui->leNGLeadCode->text();
    const_msg(msg);
}

//FTP服务器上传路径设置
void MainWindow::on_leFtpFormalDir_editingFinished()
{
    m_IniFile->setValue("mesFtpFormalHostDir",ui->leFtpFormalDir->text());
    QString msg="修改MES服务器的工艺参数申请FTP文件路径为"+ui->leFtpFormalDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpFormalVerifyDir_editingFinished()
{
    m_IniFile->setValue("mesFtpFormalVerifyHostDir",ui->leFtpFormalVerifyDir->text());
    QString msg="修改MES服务器的配方验证FTP文件路径为"+ui->leFtpFormalVerifyDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpEquVerifyDir_editingFinished()
{
    m_IniFile->setValue("mesFtpEquVerifyHostDir",ui->leFtpEquVerifyDir->text());
    QString msg="修改MES服务器的设备参数校验FTP文件路径为"+ui->leFtpEquVerifyDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpBillInfoDir_editingFinished()
{
    m_IniFile->setValue("mesFtpBillInfoHostDir",ui->leFtpBillInfoDir->text());
    QString msg="修改MES服务器的获取工单信息FTP文件路径为"+ui->leFtpBillInfoDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpBillListDir_editingFinished()
{
    m_IniFile->setValue("mesFtpBillListHostDir",ui->leFtpBillListDir->text());
    QString msg="修改MES服务器的获取工单队列FTP文件路径为"+ui->leFtpBillListDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpCellInDir_editingFinished()
{
    m_IniFile->setValue("mesFtpCellInHostDir",ui->leFtpCellInDir->text());
    QString msg="修改MES服务器的进站校验FTP文件路径为"+ui->leFtpCellInDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpCellOutDir_editingFinished()
{
    m_IniFile->setValue("mesFtpCellOutHostDir",ui->leFtpCellOutDir->text());
    QString msg="修改MES服务器的生产履历记录FTP文件路径为"+ui->leFtpCellOutDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpNGDir_editingFinished()
{
    m_IniFile->setValue("mesFtpNGHostDir",ui->leFtpNGDir->text());
    QString msg="修改MES服务器的不良履历记录FTP文件路径为"+ui->leFtpNGDir->text();
    const_msg(msg);
}

void MainWindow::on_leFtpWorkDoneDir_editingFinished()
{
    m_IniFile->setValue("mesFtpWorkDoneHostDir",ui->leFtpWorkDoneDir->text());
    QString msg="修改MES服务器的生产记录FTP文件路径为"+ui->leFtpWorkDoneDir->text();
    const_msg(msg);
}

//自动保存Logs日志
void MainWindow::on_chbAutoLogs_clicked()
{
    QString msg;

    if(ui->chbAutoLogs->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("AtuoGenLogs","1");
        msg="设置自动产生接口日志等日志CSV文件";
    }
    else
    {
         m_IniFile->setValue("AtuoGenLogs","0");
         msg="取消自动产生接口日志等日志CSV文件";
    }
    const_msg(msg);
}

//自动链接FTP服务器
void MainWindow::on_chbFtpAutoConnect_clicked()
{
    QString msg;

    if(ui->chbFtpAutoConnect->checkState()==Qt::Checked)
    {
        m_IniFile->setValue("AtuoFtpConnect","1");
        msg="设置自动链接FTP服务器";
    }
    else
    {
         m_IniFile->setValue("AtuoFtpConnect","0");
         msg="取消自动链接FTP服务器";
    }
    const_msg(msg);
}

//FTP服务器IP设置
void MainWindow::on_leFtpServerIP_editingFinished()
{
    m_IniFile->setValue("mesFtpHostIP",ui->leFtpServerIP->text());
    QString msg="修改MES服务器的IP地址为"+ui->leFtpServerIP->text();
    const_msg(msg);
}
//FTP服务器端口设置
void MainWindow::on_leFtpPort_editingFinished()
{
    m_IniFile->setValue("mesFtpHostPort",ui->leFtpPort->text());
    QString msg="修改MES服务器的端口地址为"+ui->leFtpPort->text();
    const_msg(msg);
}
//FTP服务器用户名设置
void MainWindow::on_leFtpUserName_editingFinished()
{
    m_IniFile->setValue("mesFtpHostUserName",ui->leFtpUserName->text());
    QString msg="修改MES服务器的登录用户名为"+ui->leFtpUserName->text();
    const_msg(msg);
}
//FTP服务器密码设置
void MainWindow::on_leFtpPassword_editingFinished()
{
    m_IniFile->setValue("mesFtpHostPasswd",ui->leFtpPassword->text());
    QString msg="修改MES服务器的登录用户密码为******";
    const_msg(msg);
}
//FTP服务器自动同步时间设置
void MainWindow::on_leFtpSyncTime_editingFinished()
{
    if(ui->leFtpSyncTime->text().toFloat()<5.0)
        ui->leFtpSyncTime->setText("5");
    m_IniFile->setValue("mesFtpHostSyncTime",ui->leFtpSyncTime->text());
    csvSaveInterval=(int)(m_IniFile->value("mesFtpHostSyncTime").toFloat()*60.0/6.0);
    qDebug()<<"自动保存日志查询日志时间间隔为"<<csvSaveInterval<<"秒";
    QString msg="修改MES服务器的登录用户密码为"+ui->leFtpSyncTime->text();
    const_msg(msg);
}

