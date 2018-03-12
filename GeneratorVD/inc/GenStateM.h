/*
 * GenStateM.h
 *
 *  Created on: 2017-10-07
 *      Author: 
 */

#ifndef GEN_STATE_M_H
#define GEN_STATE_M_H
#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCBase/inc/IDCMsgQueue.h"

#include "IDCLog/inc/LogTrace.h"

typedef enum
{
	GEN_STATE_IDLE,
	GEN_STATE_BOOTUP,
	GEN_STATE_READY,
	GEN_STATE_EXP,
	GEN_STATE_ERR
}GEN_STATE_TYPE;

typedef struct 
{
	char* RxCmdType;
	GENVD_STM_EVENT_TYPE eventType;
	uchar                eventParamCnt;//shall <= GenVDParamCntSize
	GENVD_STM_EVENT_PARAM_TYPE eventParamList[GenVDParamCntSize];
}GenRxCmdMapping;

extern GenRxCmdMapping* CheckCmdTable(const char cmdStr[4], const uchar cmdLen = 3);

extern void* com_send(char* p, const int len);

class CGenStateM : public CIDCThread
{
public:
   CGenStateM();

    static ushort GetUSVal(const GenVDEventFrameHeader* packet, const GENVD_STM_EVENT_PARAM_TYPE type);
	static char*  GetStrVal(const GenVDEventFrameHeader* packet, const GENVD_STM_EVENT_PARAM_TYPE type);
	static bool   SetStrTx(const GenVDEventFrameHeader* packet, const GENVD_STM_EVENT_PARAM_TYPE type, char targetStr[80]);

protected:
	virtual void run();
private:
	static void AlarmTxIFV(void* param);
	void StateMachine(const GenVDEventFrameHeader* data);
    GEN_STATE_TYPE m_State;//Initial Value GEN_STATE_IDLE  
	bool m_BootUpFin;
	ulong m_alarmTid;
	iGENData m_GenData;

};
        


#endif /* GEN_STATE_M_H */
