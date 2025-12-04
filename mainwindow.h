#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QtSql/QSqlDatabase>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QtNetwork/QHttpMultiPart>
#include <QDebug>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QNetworkConfigurationManager>
#include <QHash>
#include <QtNetwork>
#include <QFtp>
#include<QTreeWidgetItem>

#include<myappnativeevent.h>
#include "excel/ExcelBase.h"
#include "scif2.h"
#include "scif2_define.h"

#include "analysethread.h"
#include "saveexcelthead.h"
#include "saveascsv.h"
#include "libmodbus/modbus.h"
#include "keycapturer.h"
#include "keymonitor.h"

typedef struct userdata
{
 int id;
 QString name;
 QString passwd;
 QString cardid;
 QString priv;
 QString datetime;
}userdata;//定义一个结构体类型//可以通过类型来定义变量

typedef struct tempdata
{
 int id;
 float value;
 QString datetime;
}tempdata;//定义一个结构体类型//可以通过类型来定义变量

typedef struct logmsg
{
 int id;
 QString user;
 QString msg;
 QString datetime;
}logmsg;//定义一个结构体类型//可以通过类型来定义变量
typedef struct craftmsg
{
 int id;
 QString datetime;
 float camx;
 float camy;
 float camz;
 float heightdiff;
 float probefixz;
 float probex1;
 float probey1;
 float probex2;
 float probey2;
 float safetyheight;
 float digdeep;
 float digdiffx;
 float digdiffy;
 float circleradius;
 float tripointdist;
 float cutcomp;
 float pathcutdeep;
 float planelevel;//平面度
 float mmspeed;//主轴转速
 float aripressure;//实际为风速值
 int toolusecnt;//刀具寿命
 float probebaseZ;//探测Z
 QString usetime;
}craftmsg;//定义一个结构体类型//可以通过类型来定义变量

union  floatvar{
    float fvar;
    int intvar[2];
};
enum FTPaction{
  FTP_Action_IDEL=0,
  FTP_Actoin_FIND_FILES,
  FTP_Action_DOWNLOAD_FILE
};

//进站日志：电芯条码，工单号，进站结果，结果说明，进站时间
typedef struct Entermsg
{
 int id;
 QString barcode;
 QString ordernum;
 QString result;
 QString detail;
 QString datetime;
}Entermsg;//定义一个结构体类型//可以通过类型来定义变量

//进站日志：电芯条码，工单号，进站结果，结果说明，进站时间
typedef struct ReportCellIn
{
 int id;
 QString processid;
 QString equmentid;
 QString barcode;
 QString ordernum;
 QString result;
 QString date;
}ReportCellIn;//定义一个结构体类型//可以通过类型来定义变量
//出站日志：电芯条码，工单号，进站结果，结果说明，进站时间
typedef struct ReportCellOut
{
 int id;
 QString processid;
 QString equmentid;
 QString billno;
 QString barcode;
 QString param1;
 QString param2;
 QString param3;
 QString param4;
 QString param5;
 QString param6;
 QString param7;
 QString param8;
 QString date;
}ReportCellOut;//定义一个结构体类型//可以通过类型来定义变量

//NG日志：电芯条码，工单号，出站结果，不良品名称，结果说明，出站时间
typedef struct NGmsg
{
 int id;
 QString processid;
 QString equmentid;
 QString barcode;
 QString badcode;
 QString badnum;
 QString detail;
 QString upload;
 QString datetime;
}NGmsg;//定义一个结构体类型//可以通过类型来定义变量


//出日志：电芯条码，工单号，出站结果，不良品名称，结果说明，出站时间
typedef struct Exitmsg
{
 int id;
 QString barcode;
 QString ordernum;
 QString result;
 QString ngreason;//不良名称
 QString detail;
 QString datetime;
}Exitmsg;//定义一个结构体类型//可以通过类型来定义变量
//履历需要上发的数据
typedef struct ProductMsg
{
 QString processid;
 QString equmentid;
 QString billno;
 QString creator;
 QString barcode;//扫描码
 QString num;
 QString param1;
 QString value1;
 QString param2;
 QString value2;
 QString param3;
 QString value3;
 QString param4;
 QString value4;
 QString param5;
 QString value5;
 QString param6;
 QString value6;
 QString param7;
 QString value7;
 QString param8;
 QString value8;
}ProductMsg;//定义一个结构体类型//可以通过类型来定义变量


//本地配方
typedef struct LocalFormal
{
 int id;
 QString formulano;
 QString version;
 QString productno;
 QString productname;
 QString dataparm;
 QString dateget;
 QString daterun;
 QString usestatus;
 QString opreate;
}LocalFormal;//定义一个结构体类型//可以通过类型来定义变量

//使用的配方参数明细
typedef struct FormalParam
{
 QString param_code;
 QString param_name;
 QString param_unit;
 QString param_upper;
 QString param_value;
 QString param_lower;
}FormalParam;//定义一个结构体类型//可以通过类型来定义变量

//物料明细
typedef struct Material
{
 QString id;//虚拟ID
 QString material_code;//物料批次
 QString material_PN;//物料编码
 QString material_name;//物料名称
 QString number;//上料数量
 QString surplus_num;//剩余数量
 QString unit;//单位
 QString feeding_time;//上料时间
 QString loadopreat;//上料操作
 QString opreat;//操作
 QString bill_no;
 QString bill_num;
}Material;//定义一个结构体类型//可以通过类型来定义变量

//工单队列明细
typedef struct OrderList
{
 QString bill_no;//工单号
 QString bill_num;//工单数量
 QString unit;//单位
 QString bill_state;//工单状态
}OrderList;//定义一个结构体类型//可以通过类型来定义变量
//上下料数据结构
typedef struct LoadMaterial
{
 QString process_id;//工序号
 QString equipment_id;//设备编码
 QString material_code;//物料编码
 QString number;//数量
 QString id;
}LoadMaterial;//定义一个结构体类型//可以通过类型来定义变量

//进站接口日志表数据结构
typedef struct MesCellInLogs
{
    QString barcode;//条码
    QString reqtime;//请求时间
    QString usedtime;//消耗时间
    QString resptime;//相应时间
    QString processid;//工序号
    QString equmentid;//设备号
    QString retcode;//返回码
    QString retmsg;//返回信息
    QString billno;//工单号
    QString billnum;//工单数量
    QString special;//特殊值
    QString postjson;//发送代码
    QString respjson;//返回代码
}MesCellInLogs;

//出站接口日志表
//序号	电芯条码	请求时间	耗时ms	响应时间	工序编码	设备编码	工单号	操作员	工步(可选)	是否扣料(可选)	数量	参数1	参数2	…	参数N	返回代码	返回信息	余料信息(可选)	不良代码1	不良原因1	不良代码2	不良原因2	请求json	返回json
//进站接口日志表数据结构
typedef struct MesCellOutLogs
{
    QString barcode;//条码
    QString reqtime;//请求时间
    QString usedtime;//消耗时间
    QString resptime;//相应时间
    QString processid;//工序号
    QString equmentid;//设备号
    QString billno;//工单号
    QString opreater;//操作员
    QString steps;//工步
    QString materialdec;//是否扣料
    QString productnum;//数量
    QString paramone;//参数1 铣削深度
    QString paramtwo;//参数2 半径
    QString paramthree;//参数3 主轴转速
    QString paramfour;//参数4 风速
    QString paramfive;//参数5 刀具使用次数
    QString paramsix;//参数6 表面平整度
    QString paramseven;//参数7 探测Z值
    QString retcode;//返回码
    QString retmsg;//返回信息
    QString materialremain;//余料信息
    QString ngcodeone;//不良代码1
    QString ngdetailone;//不良原因1
    QString ngcodetwo;//不良代码2
    QString ngdetailtwo;//不良原因2
    QString postjson;//发送代码
    QString respjson;//返回代码
}MesCellOutLogs;



//其他接口日志表
//序号	工序编号	设备编码	…	返回代码	返回信息	…	请求json	返回json
typedef struct MesOtherLogs
{
    QString reqtime;//请求时间
    QString usedtime;//消耗时间
    QString resptime;//相应时间
    int type;//类型(0：所有； 1：工艺申请；2：配方校验；3：设备参数校验；4：获取工单信息；5：获取工单队列；6：不良品反馈)
    QString processid;//工序号
    QString equmentid;//设备号
    QString retcode;//返回码
    QString retmsg;//返回信息
    QString postjson;//发送代码
    QString respjson;//返回代码
}MesOtherLogs;





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    bool enter_pressed(QString id);
    void ForbidenLoginByID();
    void InsertCraftRecord(craftmsg craftData);

    bool ScanCodeReadDone(QString& sc);

    void workfinish();//加工完成


private slots:
    void UpdateCapslockTip(int x,int y);
    void startInit();
    void config_init();//读取配置文件，并将配置文件值写到界面上

    int lnc_init();//LNC数控系统初始化

    void modbus_init();//温控器modbus初始化

    void initScanPort();//初始化扫描枪串口函数

    void ScanSerialRead(); //扫码枪串口接收函数

    int sqlite_db_init();//数据库初始化

    void add_one_user(userdata user);
    void modify_user(userdata user);
    void delete_one_user(userdata user);
    void add_one_temperature_rec(tempdata temp);
    void add_one_fanspeed_rec(tempdata temp);//风速
    void add_one_ng_record(NGmsg nglog);//不良记录

    void add_one_cellinmeslog(MesCellInLogs cellinlog);//新增一条进站mes日志
    void add_one_celloutmeslog(MesCellOutLogs celloutlog);//新增一条出站mes日志
    void add_one_othermeslog(MesOtherLogs otherlog);//新增一条其他接口mes日志




   QString querry_temperature_by_time(QString starttime,QString endtime);

    void add_one_logmsg(logmsg lmsg);
   QString querry_logmsg_by_time(QString starttime,QString endtime);
   void add_one_craftmsg(craftmsg cramsg);
  QString querry_craftmsg_by_time(QString starttime,QString endtime);
   void add_one_finish_msg();
   void add_one_cellin(Entermsg msg);
   void add_one_cellout(Exitmsg msg);
   void table_head_init();
   bool Login(QString u, QString p);
   void LogByID(QString cardid);
   void lncpolling();
   void temppolling();
   void tempInsertPolling();
   void lncRWpolling();
   void privpolling();
   void ForbidenIDLogin();
    int analysefile(QString filename);
    void Open_Excel(QString filename);
   void analysisScanCode(QString& sc);//扫码码分析
    void slotContextMenu(QPoint pos);//配方表右键删除槽
   void postFormal();//工艺申请上传
   void postFormalTimeout();
   void postFormalVerify(QString index);//配方校验
   void postFormalVerifyTimeout(QString id);
   void postEPFormalVerify();//设备参数校验
   void postEPFormalVerifyTimeout();
   void postGetBillInfo();//获取工单
   void postGetBillInfoTimeout();
   void postGetBillInfoList();//获取工单队列
   void postGetBillInfoListTimeout();
   void postMaterialFeedingVerifity(LoadMaterial mat);//上料
   void postMaterialFeedingVerifityTimeout(LoadMaterial mat);
   void postMaterialUnloadingVerifity(LoadMaterial mat);//卸料
   void postMaterialUnloadingVerifityTimeout(LoadMaterial mat);
   void postGetEPMaterialFeeding();//获取物料
   void postGetEPMaterialFeedingTimeout();
   void postCellIntoVerifity(ReportCellIn CellIn);//电芯进站
   void postCellIntoVerifityTimeout(ReportCellIn CellIn);
   void postSaveRejectRecordList(NGmsg nglog);//不良履历
   void postSaveRejectRecordListTimeout(NGmsg nglog);
   void postProductRecordList(ReportCellOut CellOut);//出站，生产履历
   void postProductRecordListTimeout(ReportCellOut CellOut);

   void postFormalFinish(QNetworkReply *reply);//工艺申请上传完成
   void postFormalVerifyFinish(QNetworkReply *reply);//配方校验
   void postEPFormalVerifyFinish(QNetworkReply *reply);//设备参数校验
   void postGetBillInfoFinish(QNetworkReply *reply);//获取工单
   void postGetBillInfoListFinish(QNetworkReply *reply);//获取工单队列
   void postMaterialFeedingVerifityFinish(QNetworkReply *reply);//上料
   void postMaterialUnloadingVerifityFinish(QNetworkReply *reply);//卸料
   void postGetEPMaterialFeedingFinish(QNetworkReply *reply);//获取物料
   void postCellIntoVerifityFinish(QNetworkReply *reply);//电芯进站
   void postSaveRejectRecordListFinish(QNetworkReply *reply);//不良履历
   void postProductRecordListFinish(QNetworkReply *reply);//出站，生产履历

   //九个自动导出csv文件接口
   void AutoExport_Formal_CSV();
   void AutoExport_FormalVerify_CSV();
   void AutoExport_EquVerify_CSV();
   void AutoExport_BillInfo_CSV();
   void AutoExport_BillList_CSV();
   void AutoExport_CellIn_CSV();
   void AutoExport_CellOut_CSV();
   void AutoExport_NG_CSV();
   void AutoExport_Finish_CSV();

   void AutoSaveCSV();

//FTP槽函数
   void FtpInit();
   QString _FromSpecialEncoding(const QString &InputStr);
   QString _ToSpecialEncoding(const QString &InputStr);

   void connectDisconnect();
   void uploadFile();
   void downloadFile();
   void connectToFTP();
   void disconnectFTP();

   void ftpCommandFinished(int command, bool error);
   void addToList(const QUrlInfo &urlInfo);
   void processItem(QTreeWidgetItem *item, int column);
   void goToParent();
   void enableDownload();

   void FtpUploadFileToFixPath(QString srcfile,QString desPath);

   void FtpMkdir();
 //FTP


   void autopostprg();//自动上传
   void add_one_formal(LocalFormal formal);//新增一条本地配方
   void update_one_formal(LocalFormal formal);//更新选中的配方项
   long update_usestatus_formal_before(LocalFormal formal);//更新配方之前，先更新执行状态
   void db_update_matirial(QString Mat);//更新数据库物料
   void db_update_oderlist(QString oderlist);//更新数据库工单队列

   void add_one_record_formal(FormalParam param);//配方明细新增一行
   void add_one_record_material(Material mat);//物料新增一行
   void add_one_record_orderlist(OrderList olist);//工单队列新增一行
   void update_record_material();//从数据库中更新物料表格
   void update_record_orderlist();//从数据库中更新工单表格

    void db_update_matirial_remain(Material mat);//卸料更新
    void db_update_matirial_num_ft(Material mat);//上料更新

    void update_upload_material(Material mat);//上料成功后，更新物料
    void update_unload_material(Material mat);//卸料成功后，更新物料
    void mesTxtMsg(QString msg);//MES日志明细
    void update_billInfo();//更新工单信息
    void update_mes_link_state(bool state);//更新mes链接图标指示

    void update_CellIn_Log(ReportCellIn CellIn);//更新进站数据上传情况
    void update_CellOut_Log(ReportCellOut CellOut);//更新出站数据上传情况
    void update_NG_Log(NGmsg nglog);//更新不良品上传状态

    void report_cellIn_Log();//上传进站数据

    void report_cellOut_Log();//上传出站数据

    void report_NG_Log();//上传不良品

    void append_one_Line(const QString& line);

    void add_one_record(const QString& time,const QString& msg);//分析日志新增一行

    void update_analyse(const int& percent);

    void analyse_progress_done();

    void export_syslog_progress(const int& percent);

    void export_temp_progress(const int& percent);
    void export_spd_progress(const int& percent);

    void export_analyse_progress(const int& percent);

    void export_craft_progress(const int& percent);
    void export_cellIn_progress(const int& percent);
    void export_cellOut_progress(const int& percent);
    void export_NG_progress(const int& percent);
    void export_Finish_progress(const int& percent);
    void export_Scan_progress(const int& percent);

    void keybd_ID_recive(int key);

    QString const_msg(QString msg);

    void wait_to_exit_priv();

    void show_lnc_filelist();

    void on_btUserManag_clicked();

    void on_btLNCSetting_clicked();

    void on_btTCSetting_clicked();

    void on_btLNCTable_clicked();

    void on_btTCTable_clicked();

    void on_btExit_clicked();

    void on_btAddNewUser_clicked();

    void on_btQuerryAllUsers_clicked();

    void on_btQuerryByUser_clicked();

    void on_tbvUsers_clicked(const QModelIndex &index);

    void on_btDeletUser_clicked();

    void on_btModifyUserInfo_clicked();

    void on_btLogIn_clicked();

    void on_btConnectTc_clicked();

    void on_btFindLNC_clicked();

    void on_btConnectLncByIP_clicked();

    void on_btDisconnectLnc_clicked();

    void on_btLncFilesRefresh_clicked();

    void on_btSysLogExport_clicked();

    void on_lvLncIP_clicked(const QModelIndex &index);

    void on_btConnectLncByFind_clicked();

    void on_btBrowsPcDownloadFolder_clicked();

    void on_btDisconnectTc_clicked();

    void on_btLncFileDownload_clicked();

    void on_btPcFilesRefresh_clicked();

    void on_btLogOut_clicked();

    void on_btSysLogQuerry_clicked();

    void on_btClearLogInLog_clicked();

    void on_btSaveLogInLog_clicked();

    void on_btClearNewUserLog_clicked();

    void on_btSaveNewUserLog_clicked();

    void on_btClearFilesLog_clicked();

    void on_btSaveFilesLog_clicked();

    void on_btTempQuerry_clicked();

    void on_btTempExport_clicked();

    void on_btClearTemp_clicked();

    void on_leLncIP_editingFinished();

    void on_chbDispLncPaswd_clicked();

    void on_chbNoLncPaswd_clicked();

    void on_chbConnectLncAuto_clicked();

    void on_leLncTimeOut_editingFinished();

    void on_lePcDownloadFolder_editingFinished();

    void on_lePrivAddr_editingFinished();

    void on_leTempAddr_editingFinished();

    void on_leLncLogoutAddr_editingFinished();

    void on_leTcComPort_editingFinished();

    void on_leTcBdrate_editingFinished();

    void on_cbMbDataBits_currentIndexChanged(const QString &arg1);

    void on_cbMbCrc_currentIndexChanged(const QString &arg1);

    void on_cbMbStopBits_currentIndexChanged(const QString &arg1);

    void on_chbTcConectAuto_clicked();

    void on_leTcTimeOut_editingFinished();

    void on_leTcAdaptInterval_editingFinished();

    void on_leTcSaveTempInterval_editingFinished();

    void on_leTcMbAddr_editingFinished();

    void on_leTempMbAddr_editingFinished();

    void on_leTempFactor_editingFinished();

    void on_btAnanlyseFile_clicked();

    void on_btGenTableVeiw_clicked();

    void on_btClearSysLog_clicked();

    void on_btGetFileName_clicked();


    void on_lePrivFilename_editingFinished();

    void on_chbLogOutByLnc_clicked();

    void on_leLogoutTime_editingFinished();

    void on_chbLogOutAuto_clicked();

    void on_btExportAnanlyse_clicked();

    void on_leHeartInterval_editingFinished();

    void on_leLncHeartRegAddr_editingFinished();

    void on_leLncAlowAdminAddr_editingFinished();

    void on_leLncIDorNameAddr_editingFinished();

    void on_tbvPcFiles_clicked(const QModelIndex &index);

    void on_tbvPcFiles_doubleClicked(const QModelIndex &index);

    void on_leCamX_editingFinished();

    void on_leCamY_editingFinished();

    void on_leCamZ_editingFinished();

    void on_leHightDiff_editingFinished();

    void on_leProbeFixZ_editingFinished();

    void on_leProbeX1_editingFinished();

    void on_leProbeY1_editingFinished();

    void on_leProbeX2_editingFinished();

    void on_leProbeY2_editingFinished();

    void on_leSafetyHeitht_editingFinished();

    void on_leDigDeep_editingFinished();

    void on_leDigDiffX_editingFinished();

    void on_leDigDiffY_editingFinished();

    void on_leCircleRadius_editingFinished();

    void on_leTriPointDist_editingFinished();

    void on_leCutComp_editingFinished();

    void on_lePathCutDeep_editingFinished();

    void on_leMarkTriger_editingFinished();

    void on_btCraftQuerry_clicked();

    void on_btCraftExport_clicked();

    void on_btClearCraft_clicked();

    void on_leUseHour_editingFinished();

    void on_leUseMinites_editingFinished();

    void on_leUseSeconds_editingFinished();

    void on_btMesSetting_clicked();


    void on_leFsAdaptInterval_editingFinished();

    void on_leFsSaveTempInterval_editingFinished();

    void on_leFsMbAddr_editingFinished();

    void on_leFsMbSpdAddr_editingFinished();

    void on_leFsFactor_editingFinished();

    void on_leDevCodeW_editingFinished();

    void on_leProcessNumW_editingFinished();

    void on_leCraftAdr_editingFinished();

    void on_leFormulaAdr_editingFinished();

    void on_leDevInfoAdr_editingFinished();

    void on_leOrderAdr_editingFinished();

    void on_leOrderListAdr_editingFinished();

    void on_leLoadAdr_editingFinished();

    void on_leUnloadAdr_editingFinished();

    void on_leMaterialAdr_editingFinished();

    void on_leEnterAdr_editingFinished();

    void on_leNGAdr_editingFinished();

    void on_leExitAdr_editingFinished();

    void on_leFtpPath_editingFinished();

    void on_leLncAlarmRegAddr_editingFinished();

    void on_leLncWorkStatusRegAddr_editingFinished();

    void on_leLncDeepRegWAddr_editingFinished();

    void on_leLncRidusRegWAddr_editingFinished();

    void on_leLncMMSpdWRegAddr_editingFinished();

    void on_chbScAutoConnect_clicked();

    void on_chbScRuleCtrl_clicked();

    void on_chbMesDelayStop_clicked();

    void on_chbMesOnlineMode_clicked();

    void on_leScanComPort_editingFinished();

    void on_leScanBdrate_editingFinished();

    void on_cbScDataBits_currentIndexChanged(int index);

    void on_cbScStopBits_currentIndexChanged(int index);

    void on_cbScCrc_currentIndexChanged(int index);

    void on_leScanTimeOut_editingFinished();

    void on_leScanRules_editingFinished();

    void on_leScanLength_editingFinished();

    void on_leMesMaxDelay_editingFinished();

    void on_leMesMaxCnt_editingFinished();

    void on_leMesCraftDelay_editingFinished();

    void on_btScanConnect_clicked();

    void on_leFsAddr_editingFinished();

    void on_btFormalReq_clicked();

    void on_tbvFormal_clicked(const QModelIndex &index);

    void on_btClearFormal_clicked();

    void on_btManualGetInfo_clicked();

    void on_btOrderList_clicked();

    void on_btUpateMat_clicked();

    void on_tbvMaterials_clicked(const QModelIndex &index);

    void on_leOrderNumR_textChanged(const QString &arg1);

    void on_leOrderCntR_textChanged(const QString &arg1);

    void on_btScanDisConnect_clicked();

    void on_leLncStopCtrlRegAddr_editingFinished();

    void on_leAirPresure_editingFinished();

    void on_btFanSpdQuerry_clicked();

    void on_btEnterLogExport_clicked();

    void on_btFanSpdExport_clicked();

    void on_btExitLogExport_clicked();

    void on_btNGlogsExport_clicked();

    void on_btClearFanSpd_clicked();

    void on_chbAirPressure_clicked();

    void on_leLncStopCtrlRegAddr2_editingFinished();

    void on_leFanSpeedR_editingFinished();

    void on_leLncUseParamRegAddr_editingFinished();

    void on_chbUseParam_clicked();

    void on_btScanLogExport_clicked();

    void on_btFinishLogExport_clicked();

    void on_btFinishLogQuerry_clicked();

    void on_btClearScanLog_clicked();

    void on_btClearFinishLog_clicked();

    void on_btScanLogQuerry_clicked();


    void on_btExitLogQuerry_clicked();

    void on_btEnterLogQuerry_clicked();

    void on_btClearEnterLog_clicked();

    void on_btClearExitLog_clicked();

    void on_btAddNGrecord_clicked();

    void on_btDeletNGrecord_clicked();

    void on_btNGLogQuerry_clicked();

    void on_btClearNGdata_clicked();

    void on_tbvNotGood_clicked(const QModelIndex &index);

    void on_leMMSpeed_editingFinished();

    void on_lePlaneLevel_editingFinished();

    void on_btClearMesLogs_clicked();

    void on_btSaveMesLogs_clicked();

    void on_leLncNoOperateAddr_editingFinished();

    void on_leDevAlarmAddr_editingFinished();

    void on_leToolUseCnt_editingFinished();



    void on_chbPlane_clicked();

    void on_chbToolUsed_clicked();

    void on_btCellInExport_clicked();

    void on_btInterfaceQuerry_clicked();

    void on_btInterfaceExport_clicked();

    void on_btCellOutExport_clicked();

    void on_btCellOutQuerry_clicked();

    void on_btCellInQuerry_clicked();

    void on_btCellInClear_clicked();

    void on_btCellOutClear_clicked();

    void on_btInterfaceClear_clicked();



    void on_chbAutoLogs_clicked();

    void on_leFtpFormalDir_editingFinished();

    void on_leFtpFormalVerifyDir_editingFinished();

    void on_leFtpEquVerifyDir_editingFinished();

    void on_leFtpBillInfoDir_editingFinished();

    void on_leFtpBillListDir_editingFinished();

    void on_leFtpCellInDir_editingFinished();

    void on_leFtpCellOutDir_editingFinished();

    void on_leFtpNGDir_editingFinished();

    void on_leFtpWorkDoneDir_editingFinished();

    void on_chbFtpAutoConnect_clicked();

    void on_leFtpServerIP_editingFinished();

    void on_leFtpPort_editingFinished();

    void on_leFtpUserName_editingFinished();

    void on_leFtpPassword_editingFinished();

    void on_leFtpSyncTime_editingFinished();

    void on_leNGLeadCode_editingFinished();



    void on_leProbeBaseZ_editingFinished();

    void on_chbProbeBazeZ_clicked();

private:
    Ui::MainWindow *ui;
    QSettings *m_IniFile;
    QSqlDatabase db;//用于建立和数据库的连接
    QSqlQueryModel * user_model;
    QSqlQueryModel * temp_model;
    QSqlQueryModel * fanspd_model;
    QSqlQueryModel * log_model;
    QSqlQueryModel * login_model;
    QSqlQueryModel * logout_model;
    QSqlQueryModel * craft_model;//工艺参数

    QSqlQueryModel * meslocalformalmodel;//本地配方

    QSqlQueryModel * ng_model;//不良品
    QSqlQueryModel * scan_model;//扫码
    QSqlQueryModel * finish_model;//加工完成

    QSqlQueryModel * cellinmodel;//MES进站日志
    QSqlQueryModel * celloutmodel;//MES出站日志
    QSqlQueryModel * othersmodel;//其他接口日志

    QStandardItemModel *lncftpmodel;
    QStandardItemModel *localftpmodel;
    QStandardItemModel *analysemodel;

    QStandardItemModel *mesmatmodel;//物料
    QStandardItemModel *mesmatlistmodel;//物料对列
    QStandardItemModel *mesorderlistmodel;//工单队列

    QStandardItemModel *mesformaldetailmodel;//配方详细参数

    QMenu *popMenu; //菜单

    QString select_userid;//用户管理选择时的用户名
    QString logonUser;//当前登录用户
    QString logonPriv;//当前用户权限
    QString logCardID;
    int logonPrivIdx=0;//权限索引值
    QTimer *lnctimer;//lnc状态监控定时器 100ms
    QTimer *lncRWtimer;//lnc读写定时器 200ms
    QTimer *temptimer;//温控器定时插入定时器
    QTimer *tempAdaptTimer;//温控器采集定时器300ms
    QTimer *privtimer;//权限变更动作定时器，1000ms

    QTimer *autoupload;//自动上传定时器

     QTimer *AutoSaveLogs;//自动保存定时器
     QTimer *AutoFtpPut;//自动ftp上传定时器

    QString select_ip;
    SC_DATA *pointReadData=NULL;//循环读数据指针
    float CurrentTempValue=0.0;//当前温度
    float CurrentFanSpdValue=0.0;//当前风速
    int lncCtrlValue=0;//控制加工停止
    int lncCtrlValue2=0;//控制加工停止
    int lncStatusValue=0;//获取加工状态
    int lncAlarmValue=0;//获取异常状态
    int lncWorkDeepValue=0;//写入加工深度
    int lncCircleRidus=0;//写入加工圆半径
    int lncMMSpdValue=0;//写入主轴转速
    int lncUseParamR=0;

    float lncProbeBaseZValue=0;//Z轴探测值

    int lncCtrlValueR=0;//控制加工停止回读
    int lncCtrlValueR2=0;//控制加工停止回读
    int lncStatusValueR=0;//获取加工状态回读
    int lncAlarmValueR=0;//获取异常状态回读
    int lncWorkDeepValueR=0;//写入加工深度回读
    int lncCircleRidusR=0;//写入加工圆半径回读
    int lncMMSpdValueR=0;//写入主轴转速回读

    bool productDone=false;//加工完成

    //int lncWorkStatusR=0;//加工状态

    FTPaction sc_FtpAction=FTP_Action_IDEL;
    int FtpProcessing=0;



    QString ModifyUserPasswd;
    QString ModifyUserID;
    QString ModifyUserName;
    AnalyseThread *analyProgress;
    saveExcelThead *sysExcelSaveProgress;
    saveExcelThead *tempExcelSaveProgress;
    saveExcelThead *spdExcelSaveProgress;//风速导出
    saveExcelThead *analyseExcelSaveProgress;
    saveExcelThead *craftExcelSaveProgress;
    saveExcelThead *cellInExcelSaveProgress;//进站日志
    saveExcelThead *cellOutExcelSaveProgress;//出站日志
    saveExcelThead *NGExcelSaveProgress;//出站日志
    saveExcelThead *ScanExcelSaveProgress;//扫码日志
    saveExcelThead *FinishExcelSaveProgress;//加工完成日志

    saveAsCsv *saveCellInLogsThread;//保存进站日志线程
    saveAsCsv *saveCellOutLogsThread;//保存出站日志线程
    saveAsCsv *saveOthersLogsThread;//保存其他日志线程


    //FTP变量
    QHash<QString, bool> isDirectory;
    QString currentPath;
    QFtp *ftp;
    QFile *downloadfile;//下载文件
    QFile *uploadfile;//上传文件
    QNetworkSession *networkSession;
    QNetworkConfigurationManager manager;

    bool isAlreadyConnected;
    bool isProcessed;


public:
   // int lncMakerID=706995;
   // char lncEncode[49]="C4B6F4BBD567825AE16BB5E10F44B1426EDEF7F4B13D9FC2";
  //   int lncMakerID=813361;
    // char lncEncode[49]="C9D9F532F533EBAEBF15601E3C7BD293D18ED446A4AB9BCF";
    //int lncMakerID=813361;
    //char lncEncode[49]="C9D9F532F533EBAEBF15601E3C7BD293D18ED446A4AB9BCF";
    SC2 sc2;
    char ServerIdx=0;
    DLL_USE_SETTING DllSetting;
   // modbus_t *ctx;
    QScopedPointer<ExcelBase> m_xls;

    QSerialPort *ScanSerial;//串口实例

    QByteArray ScanCodeBytes;//扫描码
    QString ScanCodeGlb;//扫码全局
    bool lncConOK=false;//LNC链接标志
    bool tcConOK=false;//温控器链接标志
    bool fanConOK=false;//风速仪链接标志
    bool mesConOK=false;//mes链接标志,默认链接成功
    bool scConOK=false;//扫码枪链接状态标志
    long long LogInKeepTime=0;
    int LncConnStatus=0;//数控系统链接状态值
    int LncHeartTime=0;//心跳计数器
    int RegNameOrID=0;//LNC刷卡控制寄存器值
    int RegAlowAdmin=0;//LNC运行超级用户登寄存器值
    int LncDevAlarm=0;//外设异常值
    //18个工艺值
    float CamX=0.0;
    float CamY=0.0;
    float CamZ=0.0;
    float HightDiff=0.0;
    float ProbeFixZ=0.0;
    float ProbeX1=0.0;
    float ProbeY1=0.0;
    float ProbeX2=0.0;
    float ProbeY2=0.0;
    float SafetyHeitht=0.0;
    float DigDeep=0.0;
    float DigDiffX=0.0;
    float DigDiffY=0.0;
    float CircleRadius=0.0;
    float TriPointDist=0.0;
    float CutComp=0.0;
    float PathCutDeep=0.0;
    int MarkTriger=0;
    int LncUseHour=0;
    int LncUseMinites=0;
    int LncUseSeconds=0;
    float FanSpeedR=0.0;
    float PlaneBlance=0.0;//平整度
    float MMspeed=0.0;//主轴转速
    float AirPressure=0.0;//风刀气压
    int toolUseCnt=0;
    int LncPrivRead=0;

    //自动保存周期分成6各间隔

    int csvSaveInterval=40;


    bool postFormalOK;//工艺申请上传成功e
    bool postFormalSucceed;//工艺上传成功
    bool postFormalVerifyOK;//配方校验完成
    bool postFormalVerifySucceed=false;//配方校验成功
    bool postEPFormalVerifyOK;//设备参数校验完成
    bool postEPFormalVerifySucceed;//设备参数校验成功
    bool postGetBillInfoOK;//获取工单完成
    bool postGetBillInfoListOK;//获取工单队列完成
    bool postMaterialFeedingVerifityOK;//上料完成
    bool postMaterialUnloadingVerifityOK;//卸料完成
    bool postGetEPMaterialFeedingOK;//获取物料完成
    bool postCellIntoVerifityOK;//电芯进站完成
    bool CellIntoVerifySucceed=false;//电芯进站校验成功
    bool postSaveRejectRecordListOK;//不良履历完成
    bool postProductRecordListOK;//出站，生产履历完成

    int FormalCnt;//配方申请重连次数
    int FormalVerifyCnt;//配方验证重连次数
    int EPFormalVerifyCnt;//设备校验重连次数
    int GetBillInfoCnt;//工单获取重连次数
    int GetBillInfoListCnt;//工单队列重连次数
    int MaterialFeedingCnt;//上料验证重连次数
    int MaterialUnloadCnt;//卸料重连次数
    int GetEPMaterialFeedCnt;//物料申请重连次数
    int CellIntolCnt;//进站日志重连次数
    int RejectRecordCnt;//不良品上传重连次数
    int ProductRecordCnt;//出站日志重连次数

    ReportCellIn CellInUsed;
    ReportCellOut CellOutUsed;
    NGmsg nglogUsed;
    NGmsg ngDelet;

    int LncPrivAddrR=-1;
    int AutoLogOutTime=-1;
    float MesTimeOut=-1.0;
    float MesMaxCnt=-1.0;
    int LncTempAddrR=-1;
    int LncFanSpdWAddrR=-1;
    float LncHearCtrlInterval=-1;
    int LncHeartCtrlAddrR=-1;
    int LncNoOperateAddrR=-1;
    int LncUseParamWAddrR=-1;
    int LncWorkDeepAddrR=-1;
    int LncWorkRidusAddrR=-1;
    int LncMMSpdWAddrR=-1;
    int LncStopCtrlWAddrR=-1;
    int LncStopCtrlWAddrR2=-1;
    int LncWorkStatusAddrR=-1;
    int LncAlowAdminAddrR=-1;
    int LncLogOutAddrR=-1;
    int LncIDorNameAddrR=-1;
    int LncCamXAddrR=-1;
    int LncCamYAddrR=-1;
    int LncCamZAddrR=-1;
    int LncProbeFixZAddrR=-1;
    int LncProbeX1AddrR=-1;
    int LncProbeY1AddrR=-1;
    int LncProbeX2AddrR=-1;
    int LncProbeY2AddrR=-1;
    int LncSafetyHeithtAddrR=-1;
    int LncHightDiffAddrR=-1;
    int LncDigDeepAddrR=-1;
    int LncDigDiffXAddrR=-1;
    int LncDigDiffYAddrR=-1;
    int LncCircleRadiusAddrR=-1;
    int LncTriPointDistAddrR=-1;
    int LncCutCompAddrR=-1;
    int LncPathCutDeepAddrR=-1;
    int LncMarkTrigerAddrR=-1;
    int LncUseHourAddrR=-1;
    int LncUseMinitesAddrR=-1;
    int LncUseSecondsAddrR=-1;
    int LncFanSpeedR=-1;
    int LncMMSpeedAddrR=-1;
    int LncAirPresureAddrR=-1;
    int LncPlaneLevelAddrR=-1;
    int LncAlarmCodeAddrR=-1;
    int DevAlarmAddrR=-1;
    int LncToolUseCntR=-1;
    int LncProbeBaseZR=-1;

MesOtherLogs LocalFormalLogs;//工艺申请日志数据
MesOtherLogs FormalVerifyLogs;//配方验证日志数据
MesOtherLogs EqumentVerifyLogs;//设备参数校验
MesOtherLogs BillInfoLogs;//获取工单信息日志
MesOtherLogs BillListLogs;//获取工单队列日志
MesOtherLogs NGPLogs;//产品不良品日志
MesCellInLogs CellInLogs;//进站日志
MesCellOutLogs CellOutLogs;//出站日志

//接口消息循环
//QEventLoop FormalVerifyLoop;


private:

//bool eventFilter(QObject *obj, QEvent *e);
void mouseMoveEvent(QMouseEvent *event);
void closeEvent (QCloseEvent *event);

};
#endif // MAINWINDOW_H
