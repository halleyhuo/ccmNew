#ifndef FLUORO_H_
#define FLUORO_H_ 1

#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCThread.h"
#include "IDCLog/inc/LogTrace.h"
#include "XtpCom.h"
#include "CurvePoints.h"

#define ACQ_DATA_BUFFER_NUM			3

typedef enum 
{
	FluoroSupportContinue,
	FluoroSupportPulse,
	FluoroSupportSub,
	FluoroSupportRoadmap,
	FluoroSupportCnt
}FluoroSupportMode;

typedef struct
{
	uchar  curveNo;
	ushort LowValue;
	ushort HighValue;
	ushort ActValue;
}FluoroCurveKvEPTable;

#define MAX_NUM_BASE_POINTS		4
#define MAX_NUM_CURVES			5

typedef struct _VasContext
{
	ulong	kv;
	ulong	ma;
	ulong	ms;
} VasContext;

typedef struct _CurveInfo
{
	ushort		curveId;
	ushort		numBasePoints;
	VasContext	baseVasContext[MAX_NUM_BASE_POINTS];
} CurveInfo;

typedef struct
{
	ushort		numCurves;
	CurveInfo	curveInfo[MAX_NUM_CURVES];
} CurveSet;

class CFluoro : public CIDCThread
{
    
public :
    CFluoro();
    
    virtual ~CFluoro();
    
  
    //## operation run()
    virtual void run();
	static ushort GetLinerVal(const ushort kv, const ushort FlFps);
private:
	SFlData               m_FlPara[FluoroSupportCnt];
	FluoroCurveKvEPTable  m_KvEpVal[FluoroSupportCnt];//change combine with m_FlPara EP of KV, in 1/4 BP
	FluoroSupportMode     m_ActualFluoroMode;
	bool                  m_FluoroModeActived;
	uchar                 m_FpsIdx;//Index of Table FpsList, default value is 0
	ushort                m_selParamIdx[FluoroSupportCnt];

private:
	ushort				m_fpsIndex;
	ushort				m_epIndex;

	INT_FlData_t		m_genVdData;

	INT_GenAcq_t		m_genAcqData[ACQ_DATA_BUFFER_NUM];
	ushort				m_lastestAcqDataIndex;

	ushort				m_targetDose;

	CurvePoints			*m_curvePoints;
	CurveSet			m_curveSet;
	
	void SyncDataIndex(void);
};

#endif
