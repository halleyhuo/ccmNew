#include "ColLogical.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

ushort CColLogical::BladePosCommand[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
ushort CColLogical::SIDCommand = 0;
#define INIBLADESTATUS(X) {X,moveStatusTargetRea,0,0}
IColBlade CColLogical::BladeStatus[] = 
{
	INIBLADESTATUS(ColBladeHeight),
	INIBLADESTATUS(ColBladeWidth),
};



CColLogical::CColLogical():CIDCThread("ColLogical",IDC_TASK_COL_LOGICAL,Small,Moderate,true),
	m_alarmTid(0)
{
}

CColLogical::~CColLogical()
{
}

void CColLogical::run()
{
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,">>>INFO: CColLogical Running");
	
	while(true)
	{        
		char buf[MAX_ITC_LENGTH];
		const int rc = m_msgQueue->recv(buf,MAX_ITC_LENGTH);
		if(rc>0)
		{
			ITCObjHeader* p = (ITCObjHeader*)buf;
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Recv Msg Src %d Target %d Len %d ObjID 0x%x",
				p->tidSrc,p->tidTarget,p->len,p->ITCObjectID);
			switch(p->ITCObjectID)
			{
			case IDC_TEL_DEMO_CREATE_ALARM_TEST:
				create_alarm(500,_callBackAlarm,this,m_alarmTid,false);
				break;

			case IDC_TEL_DEMO_ALARM_STANDINGOUT:
				IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"<<<Alarm Standout Event Got");
				m_alarmTid = 0;
				break;
			default:
				break;
			}               
		}
		else
		{
			IDC::Log(IDC::LOG_ERR,LOG_TAG,"Recv Msg Fail %d err %d",rc, m_msgQueue->GetIDCErrno());
		}
	}
}

ushort CColLogical::GetBladePosCmd(const uchar idx)
{
	ushort rc = 0xFFFF;
	const uchar CollimatorMAXSize = sizeof(BladePosCommand)/sizeof(BladePosCommand[0]);
	if(idx<CollimatorMAXSize)
	{
		rc = BladePosCommand[idx];
	}
	return rc;
}

bool CColLogical::SetBladePosCmd(const uchar idx, const ushort val)
{
	bool rc = false;
	const uchar CollimatorMAXSize = sizeof(BladePosCommand)/sizeof(BladePosCommand[0]);
	if(idx<CollimatorMAXSize)
	{
		BladePosCommand[idx] = val;
		rc = true;
	}
	return rc;
}

IColBlade* CColLogical::GetBladeStatus(const uchar idx)
{
	IColBlade* rc = NULL;
	const uchar CollimatorMAXSize = sizeof(BladePosCommand)/sizeof(BladePosCommand[0]);
	if(idx<CollimatorMAXSize)
	{
		rc = &BladeStatus[idx];
	}
	return rc;
	
}

void CColLogical::_callBackAlarm(void* param)
{
	if(param!=NULL)
	{
		CColLogical* threadPointer = (CColLogical*)param;
		char buf[MAX_ITC_LENGTH];
		ITCObjHeader* p = (ITCObjHeader*)buf;
		p->tidSrc = threadPointer->taskIdent;
		p->tidTarget = IDC_TASK_DEMO_Base+2;
		p->len = sizeof(ITCObjHeader);
		p->ITCObjectID = IDC_TEL_DEMO_ALARM_STANDINGOUT;
		IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Alarm Standout >>>");
		CIDCThread::ITCSend(p);
	}
}






