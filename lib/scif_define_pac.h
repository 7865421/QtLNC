#include "mainwindow.h"
extern char             ServerIdx;
extern char             FtpProcessing;
extern QString       CurrentIP;
extern QString       CurrentFolder;  
extern QString       BottomMsg;
extern int              pTran; 
extern int              Interval;
extern unsigned char    AxisCount;
extern FTP_TRANFER_FILE TransferFile[];
extern int              LanguageIndex;
extern int Mode;
               
//===========================人機自訂的參數=============================== 
#define R_SERVO_ON                0    //1~32軸Servo On, Bit型式
#define R_MPG_HOME                1    //是否在PLC中將手輪速度填入 回Home速度比率，每個bit代表1軸
#define R_MPG_POSITION            2    //是否在PLC中將手輪速度填入 定位速度比率，每個bit代表1軸
#define R_HOME_SEQ            49000    //1~32步驟的回原點順序
#define R_HMI_AXIS_NAME       49032    //1~32軸稱文字

//===========================警告、警報===================================
//警告
#define R_ALARM_PLC            29000   //PLC Alarm 50個
#define ALARM_PLC_SIZE         50      //PLC Alarm 50個   
#define R_ALARM_HMI            80000   //HMI Alarm(100x32個)
#define ALARM_HMI_SIZE         100
#define R_ALARM_OP             80100   //OP Alarm(200x32個)
#define ALARM_OP_SIZE          200     //OP Alarm(200x32個)
#define R_ALARM_MOT            80300   //MOT Alarm(500x32個)
#define ALARM_MOT_SIZE         500     //MOT Alarm(500x32個)
#define R_ALARM_INT            80800   //INT Alarm(100x32個)
#define ALARM_INT_SIZE         100     //INT Alarm(100x32個)
#define R_ALARM_MACRO          80900   //MACRO Alarm(100x32個)
#define ALARM_MACRO_SIZE       100     //MACRO Alarm(100x32個)
//警報
#define R_WARNING_PLC          29050   //PLC Warning 50個
#define WARNING_PLC_SIZE       50      //PLC Warning 50個 
#define R_WARNING_HMI          81000   //OP Warning(200x32個)
#define WARNING_HMI_SIZE       100     //OP Warning(200x32個)
#define R_WARNING_OP           81100   //OP Warning(200x32個)
#define WARNING_OP_SIZE        100     //OP Warning(200x32個)
#define R_WARNING_MOT          81200   //MOT Warning(500x32個)
#define WARNING_MOT_SIZE       100     //MOT Warning(500x32個)
#define R_WARNING_INT          81300   //INT Warning(100x32個)
#define WARNING_INT_SIZE       100     //INT Warning(100x32個)
#define R_WARNING_MACRO        81400   //MACRO Warning(100x32個)
#define WARNING_MACRO_SIZE     100     //MACRO Warning(100x32個)  
//==============================================================================


//===========================系統類資訊=========================================
#define SYS_CS_START            3000   //系統類 C, S 的開始位址
#define SYS_CS_USE              5      //系統類的使用大小
#define SYS_R_START            28000   //系統類 R 的開始位址
#define SYS_R_USE              70      //系統類的使用大小
//RESET    
//(當發生警報時，可由此功能清除警報，勿用上緣訊號觸發，否則將不會有作用)	W	P/V Mode
#define C_SYS_RESET              3000   //系統RESET訊號。0：OFF，1：ON
//警報與警告
#define S_SYS_ALARM              3000   //系統警報訊號。0：無，1：有
#define S_SYS_WARNING            3001   //系統警告訊號。0：無，1：有
//REMOTE IO異常檢測
#define S_SYS_REMOTE_1           3002   //REMOTE 1 IO狀態。0：正常，1：異常
#define S_SYS_REMOTE_2           3003   //REMOTE 2 IO狀態。0：正常，1：異常
//編輯保護功能
#define C_SYS_EDIT_PROTECT       3004   //編輯保護功能啟動(0:可編輯,1:不可編輯)
#define S_SYS_EDIT_PROTECT       3004   //編輯保護功能啟動(0:可編輯,1:不可編輯)
//時間計時功能
#define R_SYS_W_BIT_TIMER        28064   //時間計數功能 0:暫停,1:起動
#define R_SYS_W_BIT_TIMER_CLEAR  28065   //清除時間計數 0:不清除,1:清除  
#define R_SYS_R_BIT_TIMER_CLEAR  28066   //清除時間計數完成 0:未完成,1:完成
#define R_SYS_R_INT_TIMER_MS     28000   //第1~32組時間計數功能毫秒數值(ms)
#define R_SYS_R_INT_TIMER_HOUR   28032   //第1~32組時間計數功能小時數值(hour)
//==============================================================================

           
//===========================路徑類資訊=========================================
//----------------- R 範圍定義                        
#define PATH_R_START         17000  //第一路徑開始的位址
#define PATH_R_SIZE          500    //每個路徑的資源數目
#define PATH_R_USE           30     //路徑使用的 R 個數 
//----------------- C, S 範圍定義
#define PATH_CS_START       0     //第一路徑 C, S的開始位址
#define PATH_CS_SIZE        500
#define PATH_CS_USE         10
//----------------- R 值定義
#define R_PATH_W_SPEED_RATIO_MOVE   17000   //快速定位速度百分比(0.01%)
#define R_PATH_W_SPEED_RATIO_CUT    17001   //切削進給速度百分比(0.01%)
#define R_PATH_W_MODE             17002   //Path Mode Select (0:Manual,1:Auto)
#define R_PATH_R_STATUS           17003   //Path State(0:Not Ready,1:Ready,2:Cycle Start,3:Feed Hold,4:Block Stop)
#define R_PATH_W_MPG_RATIO        17015   //手輪速度倍率
#define R_PATH_R_MPG_SPEED        17016   //手輪速度回報
#define R_PATH_W_UPDATE           17020   //通知解譯資料更新訊號(0:無 1:通知更新)
#define R_PATH_R_START_LINE       17021   //加工起始行號
#define R_PATH_W_FILENAME         17022   //加工檔名,共8個暫存器*4byte=32byte,每個R值可填入4個字元,
                                          //以ASCII方式填入,最大32字元。由R17022 低byte開始讀,遇到
                                          //某一個byte為0則視為名稱結尾。 R17022~R17029

#define R_PATH_W_INT_CUT_ACC_L_TIME      60100    //第1路徑　切削進給 直線部份 前加減速時間 (ms)
#define R_PATH_W_INT_CUT_ACC_S_TIME      60106    //第1路徑 切削進給 S型部份  前加減速時間 (ms)
#define R_PATH_W_INT_CUT_SPEED_MAX       60172    //第1路徑 切削進給 最大速度　（KLU/min)


//----------------- C, S 定義
//啟動、暫停
#define C_PATH_CYCLE_START    0       //Cycle Start
#define C_PATH_FREE_HOLD      1       //Feed Hold
#define C_PATH_RESET          2       //Reset(此功能不清除路徑警報)
#define S_PATH_CYCLE_START    0       //Cycle Start
#define S_PATH_FREE_HOLD      1       //Feed Hold
#define S_PATH_RESET          2       //Reset(此功能不清除路徑警報)
//單步執行
#define C_PATH_SINGLE_BLOCK   3       //Path Single Block
#define S_PATH_SINGLE_BLOCK   3       //Path Single Block

//運作模式               
#define C_PATH_DRY_RUN        4       //Path Dry Run
#define C_PATH_MACHINE_LOCK   5       //Path Machine Lock
#define C_PATH_MST_IGONRE     6       //MST Ignore
#define C_PATH_NOT_READY      7       //Not READY
#define C_PATH_OPTIONAL_STOP  8       //Optional stop
#define S_PATH_DRY_RUN        4       //Path Dry Run
#define S_PATH_MACHINE_LOCK   5       //Path Machine Lock
#define S_PATH_MST_IGONRE     6       //MST Ignore
#define S_PATH_NOT_READY      7       //Not READY
#define S_PATH_OPTIONAL_STOP  8       //Optional stop 

//移動單節資訊
#define S_PATH_BLOCK_STATUS          94     //路徑移動單節狀態(0：結束，1：開始)
#define S_PATH_BLOCK_BEFORE_FINISH   95     //路徑移動單節比較量前檢查完成訊號(0：未完，1：完畢)
#define S_PATH_BLOCK_AFTER_FINISH    96     //路徑移動單節比較量後檢查完成訊號(0：未完，1：完畢)


//程式再啟動
#define C_PATH_RESTART   15       //Program Restart Mode
#define S_PATH_RESTART   15       //Program Restart Mode
#define R_PATH_W_RESTART_MODE   17007   //程式再啟動方式, 0:行號, 1:序號
#define R_PATH_W_RESTART_LINE   17008   //程式再啟動行號或序號   
#define R_PATH_R_RESTART_STATUS 17009   //程式再啟動狀態(0:無,1:再啟中,2:找到指定行,-1:找不到)
         

//=========================PAC8000 define============================================

#define R_PATH_W_PAINT_PATH_1          0      //第1路徑執行繪圖(於巨集中進行設定)
#define R_PATH_W_PAINT_PATH_2          1      //第2路徑執行繪圖(於巨集中進行設定)
#define R_PATH_W_PAINT_PATH_3          2      //第3路徑執行繪圖(於巨集中進行設定)
#define R_PATH_W_PAINT_PATH_4          3      //第4路徑執行繪圖(於巨集中進行設定)
#define R_PATH_W_PAINT_PATH_5          4      //第5路徑執行繪圖(於巨集中進行設定)
#define R_PATH_W_PAINT_PATH_6          5      //第6路徑執行繪圖(於巨集中進行設定)
#define R_AXIS_R_NOT_ZEROING          7996      //各軸非在回原點中
#define R_AXIS_R_ZERO_VALID           7998      //各軸回原點有效
#define R_AXIS_R_HOME_OK              7999      //各軸回Home OK

//===========================軸類資訊===========================================


//軸一般資訊
#define R_AXIS_W_BIT_EMG               10000   //設定EMG發生時的信號值
#define R_AXIS_R_BIT_EMG               10001   //目前的EMG信號值
#define R_AXIS_W_BIT_RESET             10002   //設定Reset發生時的信號值
#define R_AXIS_R_BIT_RESET             10003   //目前的Reset信號值  
#define R_AXIS_W_BIT_MODE              10004   //軸模式切換訊號。0：位置模式，1：速度模式
#define R_AXIS_R_BIT_MODE              10005   //軸模式切換訊號完成訊號
                                                                   
#define R_AXIS_R_BIT_MOVING            10006   //軸移動狀態。0：靜止，1：轉動
#define R_AXIS_R_BIT_MOVING_CW         10021   //軸正向移動狀態。0：靜止或反轉，1：正轉
#define R_AXIS_R_BIT_MOVING_CCW        10022   //軸反向移動狀態。0：靜止或正轉，1：反轉

#define R_AXIS_R_BIT_ALARM             10011   //警報。0：無，1：有
#define R_AXIS_R_BIT_WARNING           10012   //警告。0：無，1：有

#define R_AXIS_W_BIT_HW_LIMIT_CW       10013   //設定是否觸發正向硬體極限。0：OFF，1：ON
#define R_AXIS_W_BIT_HW_LIMIT_CCW      10014   //設定是否觸發負向硬體極限。0：OFF，1：ON
#define R_AXIS_R_BIT_SW_LIMIT_CW       10015   //是否發生超過軟體極限正向極限值(0:無,1:有)
#define R_AXIS_R_BIT_SW_LIMIT_CCW      10016   //是否發生超過軟體極限負向極限值(0:無,1:有)

#define R_AXIS_R_BIT_VCMD_BREAK        10017   //是否發生Vcmd編碼器斷線(0:無,1:有)
#define R_AXIS_R_BIT_DISMATCH          10018   //軸命令與迴授超過最大誤差功能
#define R_AXIS_R_BIT_ZERO_HOME         10019   //是否軸原點位置完成且機械座標為零(0:否,1:是)
                             
//機械鎖定
#define R_AXIS_W_BIT_MACHINE_LOCK      10023   //第1~32軸機械鎖定功能。0：不鎖定,，1：鎖定 
#define R_AXIS_R_BIT_MACHINE_LOCK      10023   //第1~32軸是否曾經啟動過機械鎖定。0：否,，1：是

//軸齒節誤差補償量的更新功能
//當該軸進行雷射量測補償間隙時，可用此功能可使其修改之補償值生效。
//軸發生警報時，此功能失效。
//更新時機只能在該軸機械座標為0時，此功能才會有效。
#define R_AXIS_W_BIT_POSTION_ADJ       10030   //齒節誤差補償量更新啟動/關閉訊號。0：OFF，1：ON
#define R_AXIS_R_BIT_POSTION_ADJ       10031   //齒節誤差補償量更新啟動/關閉完成訊號
#define R_AXIS_W_BIT_MOTOR_SERVO       10036   //馬達 Servo On/Off  0.Off 1.On

//座標
#define R_AXIS_R_INT_MACHINE_POS       11564   //第1~32軸機械座標位置(LU) 
#define R_AXIS_R_INT_ABSOLUTE_POS      12032   //第1~32軸絕對座標(LU)
#define R_AXIS_R_INT_RELATE_POS        12064   //第1~32軸相對座標(LU)
//軸編碼器數值資訊
#define R_AXIS_R_INT_ENCODER           11632   //第1~32軸編碼器數值(Pulse)
#define R_AXIS_R_INT_ENCODER_POS       11932   //第1~32軸編碼器座標(LU)
//位置資訊
#define R_AXIS_R_INT_POS_ABSOLUTE  83000   //第1~32軸絕對座標(LU)
#define R_AXIS_R_INT_POS_RELATE    83032   //第1~32軸相對座標(LU)
#define R_AXIS_R_INT_POS_MACHINE   83064   //第1~32軸機械座標(LU)
#define R_AXIS_R_INT_SPEED_POS     83100   //第1~32軸位置模式速度顯示(KLU/min)(K：位置模式速度倍率常數，由軸參數R071500~R071531設定)
#define R_AXIS_R_INT_SPEED_SPEED   83132   //第1~32軸速度模式速度顯示(KLU/min)(K：速度模式速度倍率常數，由軸參數R072232~R072263設定)
#define R_AXIS_R_INT_POS_ENCODER   83164   //第1~32軸編碼器數值(Pulse)
#define R_AXIS_R_INT_POS_PULSE_LAG 83200   //第1~32軸伺服誤差(Pulse)
#define R_AXIS_R_INT_POS_VCMD_LAG  83264   //第1~32軸Vcmd伺服誤差(Pulse) 
#define R_AXIS_R_INT_POS_LEFT_DIS  83400   //第1~32軸剩餘移動量(LU)
#define R_AXIS_W_INT_POS_OFFSET    3094032  //第1~32軸座標偏移(LU)  
#define R_AXIS_W_INT_APPLY_OFFSET  3021201  //設為2 --> 套用座標偏移


//軟體極限功能
#define R_AXIS_W_BIT_SW_LIMIT_SRC_CW   11596   //軟體極限正向I點訊號來源。0：OFF，1：ON
#define R_AXIS_W_BIT_SW_LIMIT_SRC_CCW  11597   //軟體極限負向I點訊號來源。0：OFF，1：ON
#define R_AXIS_W_INT_SW_LIMIT_CW       11500   //第1~32軸軟體極限正向極限值(LU)(原點復歸完成後生效)
#define R_AXIS_W_INT_SW_LIMIT_CCW      11532   //第1~32軸軟體極限負向極限值(LU)(原點復歸完成後生效)

//原點復歸 ---- 軸發生警報時，此功能失效。
#define R_AXIS_W_BIT_HOME              10007   //原點復歸啟動/關閉訊號。0：OFF，1：ON
#define R_AXIS_R_BIT_HOME              10008   //原點復歸啟動/關閉完成訊號
#define R_AXIS_W_BIT_HOME_SIGNAL       10009   //原點復歸擋塊訊號來源。0：OFF，1：ON

#define R_AXIS_R_BIT_HOME_FINISH       10010   //原點位置是否完成。0：未完，1：完畢

#define R_AXIS_R_BIT_HOME_RUNNING      10020   //原點復歸狀態。0：等待中，1：執行中
#define R_AXIS_W_INT_HOME_SPEED_RATIO  11732   //第1~32軸原點復歸第一段速度比例(0.01%)
#define R_AXIS_W_INT_HOME_OFFSET_ADJ   11764   //第1~32軸原點復歸偏移量補值距離(LU)

#define R_AXIS_W_INT_HOME_OFFSET       77064   //第1~32軸原點復歸偏移量距離(LU)
#define R_AXIS_W_INT_HOME_PAUSE_TIME   77100   //第1~32軸原點復歸暫停時間（ms)

#define R_AXIS_W_HOME_SPPED_1          77132   //第1~32軸原點復歸第一段速度(KLU/min) 
#define R_AXIS_W_HOME_SPPED_2          77164   //第1~32軸原點復歸第一段速度(KLU/min)

//JOG功能 ---- 軸發生警報時，此功能失效。
#define R_AXIS_W_BIT_JOG              11096   //JOG啟動/關閉訊號。0：OFF，1：ON  
#define R_AXIS_R_BIT_JOG              11097   //JOG啟動/關閉完成訊號
#define R_AXIS_W_BIT_JOG_DIRECTION    11098   //JOG方向。0：+，1：-
#define R_AXIS_W_INT_JOG_SPEED_CW     11000   //第1~32軸JOG+速度(KLU/min)
#define R_AXIS_W_INT_JOG_SPEED_CCW    11032   //第1~32軸JOG-速度(KLU/min)
#define R_AXIS_W_INT_JOG_SPEED_RATIO  11064   //第1~32軸JOG速度比例(0.01%)

//定位功能 ---- 軸發生警報時，此功能失效。
#define R_AXIS_W_BIT_MOVE              11196   //定位啟動/關閉訊號。0：OFF，1：ON
#define R_AXIS_R_BIT_MOVE_FINISH       11197   //定位完成訊號。0：未完成，1：完成
#define R_AXIS_W_BIT_MOVE_DIRECTION    11198   //定位方向。0：不反向，1：反向
#define R_AXIS_W_BIT_MOVE_MODE         11199   //定位距離種類。0：增量，1：絕對
#define R_AXIS_W_INT_MOVE_DISTANCE     11100   //第1~32軸定位距離(LU)
#define R_AXIS_W_INT_MOVE_SPEED        11132   //第1~32軸定位速度(KLU/min)
#define R_AXIS_W_INT_MOVE_SPEED_RATIO  11164   //第1~32軸定位速度比例(0.01%)
//定位檢查
#define R_AXIS_W_INT_MOVE_BEFORE       11800   //第1~32軸定位比較量前檢查距離(LU)
#define R_AXIS_W_INT_MOVE_AFTER        11832   //第1~32軸定位比較量後檢查距離(LU)
#define R_AXIS_R_BIT_MOVE_BEFORE_CK    11896   //第1~32軸定位比較量前檢查完成訊號 0：未完，1：完畢
#define R_AXIS_R_BIT_MOVE_AFTER_CK     11897   //第1~32軸定位比較量後檢查完成訊號 0：未完，1：完畢


//速度功能 ---- 軸發生警報時，此功能失效。
#define R_AXIS_W_BIT_SPEED             11296   //速度啟動/關閉訊號。0：OFF，1：ON
#define R_AXIS_R_BIT_SPEED             11297   //速度啟動/關閉完成訊號
#define R_AXIS_R_BIT_SPEED_IN_ALLOW    11298   //第1~32軸速度模式命令與回授誤是否在差容許範圍內(OFF：超出範圍，ON：在範圍內)
#define R_AXIS_W_INT_SPEED_SPEED       11200   //第1~32軸速度設定(KLU/min)
#define R_AXIS_W_INT_SPEED_RATIO       11364   //第1~32軸速度比例(0.01%)

//手輪功能
#define R_AXIS_W_BIT_MPG               11496   //第1~32軸MPG位置啟動/關閉訊號。0：OFF，1：ON
#define R_AXIS_R_BIT_MPG               11497   //第1~32軸MPG位置啟動/關閉完成訊號
#define R_AXIS_W_INT_MPG_RATIO_POS     11400   //第1~32軸MPG位置功能倍率(0.0001)  
#define R_AXIS_W_INT_MPG_RATIO_SPEED   11432   //第1~32軸MPG速度功能倍率(0.0001)


//軸同步追隨控制功能 ---- 軸發生警報時，此功能失效
#define R_AXIS_W_BIT_FOLLOW            11696   //同步追隨控制啟動/關閉訊號。0：OFF，1：ON
#define R_AXIS_R_BIT_FOLLOW            11697   //同步追隨控制啟動/關閉完成訊號
#define R_AXIS_W_BIT_FOLLOW_DIRECTION  11698   //軸同步追隨控制方向。0：不反向，1：反向
#define R_AXIS_W_INT_FOLLOW_AXIS       11600   //同步追隨控制功能追隨軸號。0：不使用，1~32：第1~32軸
#define R_AXIS_W_INT_FOLLOW_CMR_U      11664   //同步追隨控制功能CMR分子。(設定值必需大於0，否則設定值將不被接受)
#define R_AXIS_W_INT_FOLLOW_CMR_D      11700   //同步追隨控制功能CMR分母(設定值必需大於0，否則設定值將不被接受)

//加減速時間
#define R_AXIS_W_INT_MOVE_ACC_L_TIME     71132   //第1~32軸 快速移動 直線部份 後加減速時間 (ms)
#define R_AXIS_W_INT_MOVE_ACC_S_TIME     71164   //第1~32軸 快速移動 S型部份  後加減速時間 (ms)    
#define R_AXIS_W_INT_CUT_ACC_L_TIME      71200    //第1~32軸 切削進給 直線部份 後加減速時間 (ms)
#define R_AXIS_W_INT_CUT_ACC_S_TIME      71232    //第1~32軸 切削進給 S型部份  後加減速時間 (ms)


//軸表設定
#define R_AXIS_W_INT_HARDWARE     70000   //第1~32軸 硬體編號    
#define R_AXIS_W_INT_NAME         70464   //第1~32軸 軸稱    
#define R_AXIS_W_INT_PATH         70532   //第1~32軸 路徑編號
//==============================================================================


//路徑共用全域變數
#define F_SYS_GLOBAL_START      10033000  //全域變數的開始位址
#define F_SYS_GLOBAL_SIZE       5000      //全域變數的個數


//----------------- Fix值，範圍定義
#define F_PATH_GLOBAL_START      10000000  //全域變數的開始位址
#define F_PATH_GLOBAL_SIZE       5000      //全域變數的個數
#define F_PATH_LOCAL_START       10030000  //區域變數的開始位址
#define F_PATH_LOCAL_SIZE        500       //區域變數的個數


// 解譯資訊 
#define PAC_STRING_SIZE        20
#define PAC_STRING_NUM         21
#define PAC_PATH_SIZE          5000
#define R_PATH_PAC_STRING      3021500
#define BLOCK_PATH_SIZE        2000
#define R_PATH_BLOCK_CURRENT   3006072



//============================加工程式 警報 ====================================
#define R_NC_ALARM_PATH           3021276   //警報單節所屬檔案目錄
#define R_NC_ALARM_PATH_SIZE      64        //警報單節所屬檔案目錄--資料長度
#define R_NC_ALARM_FILENAME       3021340   //警報單節所屬檔名
#define R_NC_ALARM_FILENAME_SIZE  8         //警報單節所屬檔名--資料長度
#define R_NC_ALARM_LINE           3021348   //警報單節所屬行號
#define R_NC_ALARM_STRING         3021349   //警報解譯動態字串  
#define R_NC_ALARM_STRING_SIZE    64        //警報解譯動態字串--資料長度
//==============================================================================
//==============================================================================
