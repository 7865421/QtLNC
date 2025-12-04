#ifndef SCIFH
#define SCIFH

#include "scif_define.h"

#ifdef __BCB_DLL
    #define IMPORT_EXPORT   __declspec(dllexport)
    #define CALL_L          __stdcall
    #define CALL_R
    #define EXTERN_C_START  extern "C" {
    #define EXTERN_C_END    };
#endif
#ifdef __BCB
    #define IMPORT_EXPORT   __declspec(dllimport)
    #define CALL_L          __stdcall
    #define CALL_R
    #define EXTERN_C_START  extern "C" {
    #define EXTERN_C_END    };
#endif
#ifdef __VC_DLL
    #define IMPORT_EXPORT   __declspec(dllexport)
    #define CALL_L          __cdecl
    #define CALL_R
    #define EXTERN_C_START  extern "C" {
    #define EXTERN_C_END    };
#endif
#ifdef __VC
    #define IMPORT_EXPORT   __declspec(dllimport)
    #define CALL_L          __cdecl
    #define CALL_R
    #define EXTERN_C_START  extern "C" {
    #define EXTERN_C_END    };
#endif
#ifdef __VC_CB
    #define IMPORT_EXPORT
    #define CALL_L          (__stdcall *
    #define CALL_R          )
    #define EXTERN_C_START
    #define EXTERN_C_END
#endif
#ifdef __GCC
    #define IMPORT_EXPORT
    #define CALL_L
    #define CALL_R
    #ifdef __cplusplus
        #define EXTERN_C_START  extern "C" {
        #define EXTERN_C_END    };
    #else
        #define EXTERN_C_START
        #define EXTERN_C_END
    #endif
#endif

EXTERN_C_START


//=================== 初始化 與 終結 共用記憶體 ======================
IMPORT_EXPORT int  CALL_L scif_Init    CALL_R(DLL_USE_SETTING *pUseSetting, int MakerID, char *pEncString);
IMPORT_EXPORT int  CALL_L scif_Init_2  CALL_R(DLL_USE_SETTING *pUseSetting, char *filename);
IMPORT_EXPORT void CALL_L scif_Destroy CALL_R();
IMPORT_EXPORT void CALL_L scif_SetTalkSoftwareType CALL_R(char SessionIdx, unsigned int SoftwareType);
IMPORT_EXPORT void CALL_L scif_SetMediaIP CALL_R(char *MediaIP);
//設定除錯等級
//level : 0不顯示   1有錯誤才顯示   2通訊內容全部顯示(除錯用，通訊速度變成每秒一次)
IMPORT_EXPORT void CALL_L scif_SetDebug CALL_R(unsigned char level);
//==============================================================================




//=============================讀取 Local data =============
IMPORT_EXPORT int          CALL_L scif_SetMirror CALL_R(char SessionIdx);
IMPORT_EXPORT char         CALL_L scif_ReadI CALL_R(unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_ReadO CALL_R(unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_ReadC CALL_R(unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_ReadS CALL_R(unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_ReadA CALL_R(unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_ReadTMR CALL_R(unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_ReadCNT CALL_R(unsigned int addr );

IMPORT_EXPORT unsigned int CALL_L scif_ReadR CALL_R( unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_ReadTMRV CALL_R(unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_ReadTMRS CALL_R(unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_ReadCNTV CALL_R(unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_ReadCNTS CALL_R(unsigned int addr );

IMPORT_EXPORT double       CALL_L scif_ReadF CALL_R(unsigned int addr );

//-------------------------------------------------------------------------
IMPORT_EXPORT char         CALL_L scif_mem_ReadI CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_mem_ReadO CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_mem_ReadC CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_mem_ReadS CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_mem_ReadA CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_mem_ReadTMR CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT char         CALL_L scif_mem_ReadCNT CALL_R(char SessionIdx, unsigned int addr );

IMPORT_EXPORT unsigned int CALL_L scif_mem_ReadR CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_mem_ReadTMRV CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_mem_ReadTMRS CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_mem_ReadCNTV CALL_R(char SessionIdx, unsigned int addr );
IMPORT_EXPORT unsigned int CALL_L scif_mem_ReadCNTS CALL_R(char SessionIdx, unsigned int addr );

IMPORT_EXPORT double       CALL_L scif_mem_ReadF CALL_R(char SessionIdx, unsigned int addr );
//==============================================================================



//===================區域網路中偵測主機功能=====================================
//自動偵測主機功能
IMPORT_EXPORT int CALL_L scif_LocalDetectControllers CALL_R();
//讀取取得的控制器資料筆數
IMPORT_EXPORT int CALL_L scif_LocalReadControllerCount CALL_R();
//讀取取得的控制器資料
IMPORT_EXPORT int CALL_L scif_LocalReadController CALL_R(unsigned short Index, LOCAL_CONTROLLER_INFO *Info);
//與取得的控制器清單中的Index值進行連線
IMPORT_EXPORT int CALL_L scif_ConnectLocalList CALL_R(char SessionIdx, unsigned short Index);
//==============================================================================


//====================跨越NAT偵測主機功能=================================
//透過控制器向媒合主機取得的 PrivateID進行連線
IMPORT_EXPORT int CALL_L scif_MediaGrabByPrivateID CALL_R(char *PrivateID);
//由媒介主機取得控制器清單
IMPORT_EXPORT int CALL_L scif_MediaGrabByMakerID CALL_R();
//檢查動作是否已完成
IMPORT_EXPORT int CALL_L scif_MediaCheckGrabDone CALL_R();
//讀取取得的控制器資料筆數
IMPORT_EXPORT int CALL_L scif_MediaReadControllerCount CALL_R();
//讀取取得的控制器資料
IMPORT_EXPORT int CALL_L scif_MediaReadController CALL_R(unsigned short Index, MEDIA_CONTROLLER_INFO *Info);
//與取得的控制器清單中的Index值進行連線
IMPORT_EXPORT int CALL_L scif_ConnectMediaList CALL_R(char SessionIdx, unsigned short Index);
//==============================================================================




//--==================================連線功能=================================
//直接輸入控制器IP進行連線
IMPORT_EXPORT int CALL_L scif_LocalConnectIP CALL_R(char SessionIdx, char *IP);
//中斷連線
IMPORT_EXPORT int CALL_L scif_Disconnect CALL_R(char SessionIdx);
//設定連線密碼
IMPORT_EXPORT int CALL_L scif_SetConnectPwd CALL_R(char SessionIdx, char *Pwd);
//==============================================================================


//--==================================檔案傳輸功能=================================
//設定 FTP 索引
IMPORT_EXPORT int CALL_L scif_FtpSetTalk CALL_R(char SessionIdx);
//上傳檔案
IMPORT_EXPORT int CALL_L scif_FtpUploadFile CALL_R(char Folder, char *Filename, char *LocalFilename);
IMPORT_EXPORT int CALL_L scif_FtpUploadFile2 CALL_R(char Folder, char *SubFolder, char *Filename, char *LocalFilename);
//下載檔案
IMPORT_EXPORT int CALL_L scif_FtpDownloadFile CALL_R(char Folder, char *Filename, char *LocalFilename);
IMPORT_EXPORT int CALL_L scif_FtpDownloadFile2 CALL_R(char Folder, char *SubFolder, char *Filename, char *LocalFilename);
//刪除檔案
IMPORT_EXPORT int CALL_L scif_FtpDeleteFile CALL_R(char Folder, char *Filename);
IMPORT_EXPORT int CALL_L scif_FtpDeleteFile2 CALL_R(char Folder, char *SubFolder, char *Filename);
//上傳檔案 -- 多個
IMPORT_EXPORT int CALL_L scif_FtpUploadFiles CALL_R(unsigned char Count, FTP_TRANFER_FILE *TransferFiles);
//下載檔案 -- 多個
IMPORT_EXPORT int CALL_L scif_FtpDownloadFiles CALL_R(unsigned char Count, FTP_TRANFER_FILE *TransferFiles);
//刪除檔案 -- 多個
IMPORT_EXPORT int CALL_L scif_FtpDeleteFiles CALL_R(unsigned char Count, FTP_TRANFER_FILE *TransferFiles);

//增加幾個函式，方便 C# 使用
IMPORT_EXPORT int CALL_L scif_FtpTransferFileReset CALL_R();
IMPORT_EXPORT int CALL_L scif_FtpTransferFileAdd CALL_R(char Folder, char *SubFolder, char *Filename, char *LocalFilename);
IMPORT_EXPORT int CALL_L scif_FtpUploadFiles2 CALL_R();
IMPORT_EXPORT int CALL_L scif_FtpDownloadFiles2 CALL_R();
IMPORT_EXPORT int CALL_L scif_FtpDeleteFiles2 CALL_R();

//----------------------------遠端檔案清單
//建立資料夾
IMPORT_EXPORT int CALL_L scif_FtpMakeDir CALL_R(char Folder, char *DirName);
//取得檔案清單
IMPORT_EXPORT int CALL_L scif_FtpGetFileList CALL_R(char Folder, char *HeadFilter, char *TailFilter);
IMPORT_EXPORT int CALL_L scif_FtpGetFileList2 CALL_R(char Folder, char *SubFolder, char *HeadFilter, char *TailFilter);
//取得執行結果
IMPORT_EXPORT int CALL_L scif_FtpCheckDone CALL_R(unsigned char *State, unsigned char *Result);
//讀取FTP檔案清單筆數
IMPORT_EXPORT int CALL_L scif_FtpReadFileCount CALL_R();
//讀取FTP檔案名稱
IMPORT_EXPORT int CALL_L scif_FtpReadFile CALL_R(unsigned short Index, FTP_FILE *file);

//----------------------------本地端檔案清單
//取得本地端檔案清單
IMPORT_EXPORT int CALL_L scif_FileGetFileList CALL_R(char *Path, char *HeadFilter, char *TailFilter);
//讀取本地檔案清單筆數
IMPORT_EXPORT int CALL_L scif_FileReadFileCount CALL_R();
//讀取本地檔案名稱
IMPORT_EXPORT int CALL_L scif_FileReadFile CALL_R(unsigned short Index, FTP_FILE *file);
//刪除本地檔案名稱
IMPORT_EXPORT int CALL_L scif_FileDeleteFile CALL_R(unsigned short Index);

//==============================================================================

//--==========================取得內部資訊 =================================
//-----取得一般的資訊
IMPORT_EXPORT unsigned int   CALL_L scif_GetCommonMsg CALL_R(char id );
//-----取得連線通訊的資訊
IMPORT_EXPORT unsigned int   CALL_L scif_GetTalkMsg CALL_R(char SessionIdx, char id);
//-----取得錯誤訊息
IMPORT_EXPORT void CALL_L scif_GetTalkError CALL_R(char SessionIdx, ERROR_MSG *Msg);
//==============================================================================


//============================= 取得通訊處理狀態 ================================
IMPORT_EXPORT unsigned char CALL_L scif_GetTranState CALL_R(int pTran);

//========= 由 Default Loop Queue 中尋找資料並讀出 =============
IMPORT_EXPORT SC_DATA* CALL_L scif_GetDefaultQueueDataPointer CALL_R(char SessionIdx, unsigned char TranIdx);
IMPORT_EXPORT SC_DATA* CALL_L scif_GetDataPointerByTranPointer CALL_R(int TranPointer);
//==============================================================================


//========= 從這裡以下的部份開始，SessionIdx設定為-1時，代表同時對全部的 連線進行設定 ============

//----------清除命令
IMPORT_EXPORT void  CALL_L scif_cmd_ClearAll CALL_R(unsigned char type, char SessionIdx);

//等待直接命令完成
IMPORT_EXPORT int CALL_L scif_WaitDirectCmdDone CALL_R(char SessionIdx, unsigned int MaxWaitTime);

//設定組合大封包的最大數
IMPORT_EXPORT int CALL_L scif_SetMaxGroupPkg CALL_R(char SessionIdx, unsigned char Count);

// ========================================================
//以下函式回傳值若為 0 ，代表指令初拒絕，若不為 0 ，代表是該筆通訊的指標
//此時，將其帶入 scif_GetTranState 的引數中，取得該筆通訊的狀態，將會是 SC_TRANSACTION_RESET
//一段時間之後再帶入 scif_GetTranState 的引數中，回傳值若為 SC_TRANSACTION_FINISH 代表該筆通訊已被處理
// ========================================================


//  ============================= Single write ============
IMPORT_EXPORT int CALL_L scif_cmd_WriteI CALL_R(char SessionIdx, unsigned int addr, char val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteO CALL_R(char SessionIdx, unsigned int addr, char val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteC CALL_R(char SessionIdx, unsigned int addr, char val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteS CALL_R(char SessionIdx, unsigned int addr, char val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteA CALL_R(char SessionIdx, unsigned int addr, char val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteTMR CALL_R(char SessionIdx, unsigned int addr, char val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteCNT CALL_R(char SessionIdx, unsigned int addr, char val);

IMPORT_EXPORT int CALL_L scif_cmd_WriteR CALL_R(char SessionIdx,  unsigned int addr, unsigned int val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteTMRV CALL_R(char SessionIdx,  unsigned int addr, unsigned int val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteTMRS CALL_R(char SessionIdx,  unsigned int addr, unsigned int val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteCNTV CALL_R(char SessionIdx,  unsigned int addr, unsigned int val);
IMPORT_EXPORT int CALL_L scif_cmd_WriteCNTS CALL_R(char SessionIdx,  unsigned int addr, unsigned int val);

IMPORT_EXPORT int CALL_L scif_cmd_WriteF CALL_R(char SessionIdx,  unsigned int addr, double val);


//  ============================= Multi write ============
// num 的最大值 MAX_BIT_NUM
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiI CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiO CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiC CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiS CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiA CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiTMR CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiCNT CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, char *data);

// num 的最大值 MAX_INT_NUM
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiR CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiTMRV CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiTMRS CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiCNTV CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiCNTS CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, unsigned int *data);

// num 的最大值 MAX_FIX_NUM
IMPORT_EXPORT int CALL_L scif_cmd_WriteMultiF CALL_R(char SessionIdx,  unsigned int addr, unsigned int num, double *data);

//  ============================= combin write ============
// num 的最大值 MAX_CB_BIT_NUM
IMPORT_EXPORT int CALL_L scif_cb_WriteI CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteO CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteC CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteS CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteA CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteTMR CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteCNT CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, char *data);

// num 的最大值 MAX_CB_INT_NUM
IMPORT_EXPORT int CALL_L scif_cb_WriteR CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteTMRV CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteTMRS CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteCNTV CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, unsigned int *data);
IMPORT_EXPORT int CALL_L scif_cb_WriteCNTS CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, unsigned int *data);

// num 的最大值 MAX_CB_FIX_NUM
IMPORT_EXPORT int CALL_L scif_cb_WriteF CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, double *data);


//  ============================= continue read ============
// num 的最大值 MAX_BIT_NUM
// 當 num 小於等於  MAX_CB_BIT_NUM/2 且 有呼叫 scif_StartComboinSet() 且 為POLLING command 時，將會被放到 combine
IMPORT_EXPORT int CALL_L scif_cmd_ReadI CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);
IMPORT_EXPORT int CALL_L scif_cmd_ReadO CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);
IMPORT_EXPORT int CALL_L scif_cmd_ReadC CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);
IMPORT_EXPORT int CALL_L scif_cmd_ReadS CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);
IMPORT_EXPORT int CALL_L scif_cmd_ReadA CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);
IMPORT_EXPORT int CALL_L scif_cmd_ReadTMR CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);
IMPORT_EXPORT int CALL_L scif_cmd_ReadCNT CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num);

// num 的最大值 MAX_INT_NUM
// 當 num 小於等於  MAX_CB_INT_NUM/2 且 有呼叫 scif_StartComboinSet() 且 為POLLING command 時，將會被放到 combine
IMPORT_EXPORT int CALL_L scif_cmd_ReadR CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );
IMPORT_EXPORT int CALL_L scif_cmd_ReadTMRV CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );
IMPORT_EXPORT int CALL_L scif_cmd_ReadTMRS CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );
IMPORT_EXPORT int CALL_L scif_cmd_ReadCNTV CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );
IMPORT_EXPORT int CALL_L scif_cmd_ReadCNTS CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );

// num 的最大值 MAX_FIX_NUM
// 當 num 小於等於  MAX_CB_FIX_NUM/2 且 有呼叫 scif_StartComboinSet() 且 為POLLING command 時，將會被放到 combine
IMPORT_EXPORT int CALL_L scif_cmd_ReadF CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );
IMPORT_EXPORT int CALL_L scif_cmd_ReadP CALL_R(char type, char SessionIdx, unsigned int addr, unsigned int num );


//  ============================= discrate read ============
// num 的最大值 MAX_CB_BIT_NUM
IMPORT_EXPORT int CALL_L scif_cb_ReadI CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadO CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadC CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadS CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadA CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadTMR CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadCNT CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);

// num 的最大值 MAX_CB_INT_NUM
IMPORT_EXPORT int CALL_L scif_cb_ReadR CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadTMRV CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadTMRS CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadCNTV CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadCNTS CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);

// num 的最大值 MAX_CB_FIX_NUM
IMPORT_EXPORT int CALL_L scif_cb_ReadF CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr);
IMPORT_EXPORT int CALL_L scif_cb_ReadP CALL_R(char type, char SessionIdx, unsigned int num, unsigned int *addr );




//===============字串存取功能==============
//由鏡射記憶體中讀取字串
IMPORT_EXPORT unsigned int CALL_L scif_ReadRString CALL_R(unsigned int addr, unsigned int BufSize, char *Buf );
IMPORT_EXPORT unsigned int CALL_L scif_mem_ReadRString CALL_R(char SessionIdx, unsigned int addr, unsigned int BufSize, char *Buf );
//寫入字串到 R
IMPORT_EXPORT int           CALL_L scif_cmd_WriteRString CALL_R(char SessionIdx, unsigned int addr, unsigned int BufSize, char *Buf);
//寫入 R bit
IMPORT_EXPORT int CALL_L scif_cmd_WriteRBit CALL_R(char SessionIdx, unsigned int addr, unsigned char BitIdx, unsigned char BitValue);
//寫入 多個 R bit
IMPORT_EXPORT int CALL_L scif_cb_WriteRBit CALL_R(char SessionIdx, unsigned int num, unsigned int *addr, unsigned char *BitIdx, unsigned char *BitValue);
//=========================================


//===============開始與結束 Combin Queue==================
//在呼叫 scif_StartComboin 之後，未呼叫scif_FinishComboin()之前，
//若 scif_cmd_Readxxx() 時傳入的 num 太小，該命令的內容將會被放到一暫存的Buf
//等到 呼叫 scif_FinishComboin 之時，才會將 Buf 的內容重新整理過，自動填入
//scif_cb_Readxxx 的命令中
//設定自動組合旗標
IMPORT_EXPORT void CALL_L scif_StartCombineSet CALL_R(char SessionIdx);
//完成自動組合設定並開始產生組合封包
IMPORT_EXPORT void CALL_L scif_FinishCombineSet CALL_R(char SessionIdx);
//完成自動組合設定並開始產生組合封包---傳回最後一個組合封包的指標
IMPORT_EXPORT int CALL_L scif_FinishCombineSet2 CALL_R(char SessionIdx);


//設定自動組合旗標
IMPORT_EXPORT void CALL_L scif_Write_Start CALL_R(char SessionIdx);
//完成自動組合設定並開始產生組合封包
IMPORT_EXPORT void CALL_L scif_Write_Finish CALL_R(char SessionIdx);


EXTERN_C_END

#endif






