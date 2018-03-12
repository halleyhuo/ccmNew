#ifndef IDC_TASKID_H
#define IDC_TASKID_H
#define MAX_ITC_LENGTH 1024 /* Length of Inter Thread Communication Object <= 1024, which contain header length */
#define MAX_ITC_MSG_CNT 200
#define RECV_MBX_NAME "/idc_msgq_%02x"
#include "IDCType.h"

#include "../IDCLibs/IDCXTP/inc/xtpobj.h"
#include <sys/types.h>


typedef ENUM16 IDC_TASK_ID; 
const IDC_TASK_ID IDC_TASK_INVALID    = 0;
const IDC_TASK_ID IDC_TASK_CO_APP     = 11;

const IDC_TASK_ID IDC_TASK_COL_MASTER = 51;
const IDC_TASK_ID IDC_TASK_COL_LOGICAL= 52;
const IDC_TASK_ID IDC_TASK_COL_XTP_TASK= 53;//no receive Function


const IDC_TASK_ID IDC_TASK_DAP_MASTER = 61;
const IDC_TASK_ID IDC_TASK_HMI_XTPCLT = 71;
const IDC_TASK_ID IDC_TASK_MASTER_XTPIF = 81;
const IDC_TASK_ID IDC_TASK_FLUORO =       82;


const IDC_TASK_ID IDC_TASK_GEN_LOGICAL = 91;
const IDC_TASK_ID IDC_TASK_GEN_MASTER = 92;
const IDC_TASK_ID IDC_TASK_GEN_XTP_TASK = 93;
const IDC_TASK_ID IDC_TASK_GEN_HEAT_UNIT_CAL_TASK = 94;


const IDC_TASK_ID IDC_TASK_DEMO_Base  = 200;
const IDC_TASK_ID IDC_TASK_SIZE       = 255;

const uchar ITC_SEND_MODE_DIRECT = 1;
const uchar ITC_SEND_NO_FLOW     = 0;

typedef struct
{
    uchar tidSrc;
    uchar tidTarget;
    uchar mode;       //Reserved for future use
    uchar flowControl;//Reserved for future use
    ushort ITCObjectID;
    ushort len;       //>=8
}ITCObjHeader;

class CIDCMsgQueue;


typedef struct
{
    IDC_TASK_ID   taskIdent;
    CIDCMsgQueue* msgQRcv;
}IDC_ITC_MAP;

#define INT_TEL_BASE 0x1000

#define INT_TEL_CO_RCV (INT_TEL_BASE+10) //100AH Receive From OTHER APLICATION Sub-System
const unsigned short APP2CO_START = 0x800;
typedef struct
{
    unsigned short COB_ID;
    unsigned short _Len;       //Length of Data Followed
}XTP_DEF_PACKED CO_APP_COMM_Header;


typedef struct
{
    CO_APP_COMM_Header header;
    uchar              VCIdata[64];
}XTP_DEF_PACKED CO_APP_COMM_t;

typedef struct
{
	ITCObjHeader  header;
	CO_APP_COMM_t ipcData;
}XTP_DEF_PACKED CO_APP_COMM_Frame;

#define INT_FL_DATA            (INT_TEL_BASE+61)
typedef struct
{
	FlType FlMode; 
	ushort kv;
	ushort ma;
	ushort mas;
	ushort Tmax; //from configuration file, 34ms for siemens GEN
	ushort Fps; //5, 10, 20, 30, 50, 75, 100, 150 for 0.5, 1, 2, 3, 5, 7.5, 10, and 15 pulse(s)/s; 0 for Continue FL
	uchar  focus; // 1 for small focus; 2 for large focus
}INT_FlData_t;

typedef struct
{
	ITCObjHeader  header;
	INT_FlData_t  ipcData;
}XTP_DEF_PACKED INT_FlData_Frame;


#define INT_GENERAL_CONTROL         (INT_TEL_BASE+60)
typedef struct
{
	ConsoleType 	   consoleType;
	ControlElementType controlElementType;
	FunctionCode       functionCode;
	ushort             param;
}INT_GeneralCtrl_t;

typedef struct
{
	ITCObjHeader      header;
	INT_GeneralCtrl_t ipcData;
}INT_GeneralCtrl_t_Frame;

#define INT_GEN_STATUS         (INT_TEL_BASE+62)
typedef struct
{
	IGenReady IGenInfo;
}INT_GenStatus_t;

typedef struct
{
	ITCObjHeader     header;
	INT_GenStatus_t  data;
}IntGenStatusFrame;

#define INT_GEN_ACQ			(INT_TEL_BASE + 63)
typedef struct 
{
	ushort		kv;
	ushort		ma;
	ushort		ms;
	ushort		mas;
	ulong		acqIndex;
	ushort		epValue;
} INT_GenAcq_t;

typedef struct 
{
	ITCObjHeader	header;
	INT_GenAcq_t	acqData;
}IntGenAcqFrame;

#define INT_IWS_DOSE_INFO	(INT_TEL_BASE + 64)
typedef struct
{
	ushort		doseValue;
	ulong		index;
} INT_IwsDoseInfo_t;

typedef struct
{
	ITCObjHeader		header;
	INT_IwsDoseInfo_t	doseInfo;
} IntIwsDoseInfoFrame;

#define INT_GENVD_STM_EVENT (INT_TEL_BASE+80)
#define INT_GENVD_HEATCAL_EVENT (INT_TEL_BASE+81)

typedef enum 
{
	GENVD_STM_EVENT_NONE   = 0x0,
	GENVD_STM_EVENT_START  = 0x01,
	GENVD_STM_EVENT_RCV_IFV= 0x10,
	GENVD_STM_EVENT_RCV_SWV,
	GENVD_STM_EVENT_RCV_INI,
	GENVD_STM_EVENT_RCV_SBY,
	GENVD_STM_EVENT_RCV_RDY,
	GENVD_STM_EVENT_RCV_EXP,
	GENVD_STM_EVENT_RCV_ERR,
	GENVD_STM_EVENT_RCV_XSTATUS,	
	GENVD_STM_EVENT_RCV_VFP,
	GENVD_STM_EVENT_RCV_VFD,
	GENVD_STM_EVENT_RCV_AXD,
	GENVD_STM_EVENT_RCV_START_EXP,
	GENVD_STM_EVENT_RCV_STOP_EXP,
	GENVD_STM_EVENT_TX_SFP      =0x40,
	GENVD_STM_EVENT_TX_SFD,
	GENVD_STM_EVENT_RCV_MSG
}GENVD_STM_EVENT_TYPE;

typedef enum 
{
	GENVD_STM_EVENT_PARAM_NONE = 0,
	GENVD_STM_EVENT_PARAM_VER,//GenVD Version
	GENVD_STM_EVENT_PARAM_RDYSTATUS,
	GENVD_STM_EVENT_PARAM_RDYSTATUS2,
	GENVD_STM_EVENT_PARAM_TEMP,
	GENVD_STM_EVENT_PARAM_UDC,
	GENVD_STM_EVENT_PARAM_XON,
	GENVD_STM_EVENT_PARAM_XOFF,
	GENVD_STM_EVENT_PARAM_MODE,
	GENVD_STM_EVENT_PARAM_KV,
	GENVD_STM_EVENT_PARAM_MA,
	GENVD_STM_EVENT_PARAM_MAS,
	GENVD_STM_EVENT_PARAM_TMAX,
	GENVD_STM_EVENT_PARAM_FOCUS,
	GENVD_STM_EVENT_PARAM_FPS,
	GENVD_STM_EVENT_PARAM_XRAY_MS,
	GENVD_STM_EVENT_PARAM_ENDXRAY,
	GENVD_STM_EVENT_PARAM_GEN_PULSE,
	GENVD_STM_EVENT_PARAM_MSG_NUM,
	GENVD_STM_EVENT_PARAM_MSG_PRIO,
	GENVD_STM_EVENT_PARAM_MSG_TXT
}GENVD_STM_EVENT_PARAM_TYPE;

const size_t GenVDParamItemSize = 16;
const size_t GenVDParamCntSize  = 12;

typedef struct 
{
	GENVD_STM_EVENT_PARAM_TYPE type;
	char                       param[GenVDParamItemSize];
}GenVDParamPacket;

typedef struct
{
	GENVD_STM_EVENT_TYPE Event;
	ushort extParamCnt;
}GenVDEventFrameHeader;

typedef struct
{
	GenVDEventFrameHeader Header;	
	GenVDParamPacket      extParam[GenVDParamCntSize];
}GenVDEventFrame;

typedef struct
{
	ITCObjHeader     header;
	GenVDEventFrame  data;
}IntGenVDEventFrame;


#define INT_TEL_COL_MASTER_RCV (INT_TEL_BASE+50) //task COL_Master Receive From COMan. Struct also use CO_APP_COMM_t
#define INT_TEL_COL_SEND       (INT_TEL_BASE+51)




#define IDC_TEL_DEMO_BASE 0x5000
#define IDC_TEL_DEMO_CREATE_ALARM_TEST 0x5010
#define IDC_TEL_DEMO_ALARM_STANDINGOUT 0x5060


#endif /*IDC_TASKID_H*/
