#include "ColXtp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "ColMaster.h"
#include "ColLogical.h"
/******************************************************************************
***************************** XTP Interface **************************************
***  Receive Obj: ISIDINFO,SCOLBLADEPOS, IXRAY... ,                                    **
***  Out Obj:    ICOLBLADEPOS, ICOMPSTATUS, ICOLLSEARCHRUN                             **
*******************************************************************************/

extern CColMaster* pCOLMaster;
BRODCAST_LIST broadcastList[] = 
{    
	{XTP_NODE_MASTER            , 0},
	{XTP_NODE_COLLIMATOR_CONTROL, 1, {ISYSTEMINFO,IXRAY}},
	{XTP_NODE_GENERATOR,          1, {ISYSTEMINFO}},
};

CColXtpIf* CColXtpIf::m_instanse = NULL;

CColXtpIf::CColXtpIf():	CIDCThread("ColXtpIf",IDC_TASK_ID(IDC_TASK_COL_XTP_TASK),Small,Moderate,false)
	{
		xtpClientIf = new CXtpClient(XTP_NODE_COLLIMATOR_CONTROL);
	}

CColXtpIf* CColXtpIf::Sigleton(CColXtpIf* instanse/* = NULL*/)
{
	if(NULL==instanse)
	{
		return m_instanse;
	}
	else
	{
		m_instanse = instanse;
	}
}


	void CColXtpIf::run()
	{
		while(!xtpClientIf->XtpConnect(CallBackFunc,GetBroadList(xtpClientIf->GetNodeID()),this))
		{
			sleep(2);//retry 2s later
		}
		while(true)
		{

			sleep(1);
		}

	}

void CColXtpIf::onSendIColBlade()
{
	IColBladeFrame_t IColBladeFrame;
	IColBladeFrame.header.sender = xtpClientIf->GetNodeID();
	IColBladeFrame.header.target = XTP_NODE_MASTER;
	IColBladeFrame.header.telID = ICOLBLADEPOS;
	IColBladeFrame.header.mode  = XTP_MODE_DIRECT;	
	IColBladeFrame.cnt.count = 2;//REMARK: Currently Srurgery only support 2 Blade of Collimator. Value may be updated in future
	IColBladeFrame.header.length = sizeof(tXtpObjHeader)+ 
		sizeof(IColBladeFrame.cnt)+IColBladeFrame.cnt.count*sizeof(IColBlade);
	IColBladeFrame.blade[0].pos = CColLogical::GetBladeStatus(ColBladeHeight)->pos;
	IColBladeFrame.blade[1].pos = CColLogical::GetBladeStatus(ColBladeWidth)->pos;		
			
	xtpClientIf->XtpSend(&IColBladeFrame.header);
}
	

	//Len only the data of CAN Frame
	void CColXtpIf::SendInt2ColCO(ushort COB_ID, ushort Len, uchar p [8])
	{
		//INT Telgram to ColMaster for Handle: IDC_TASK_COL_MASTER
		char buf[MAX_ITC_LENGTH];
		ITCObjHeader* IntPtr = (ITCObjHeader*)buf;
		IntPtr->tidSrc = taskIdent;
		IntPtr->tidTarget = IDC_TASK_COL_MASTER;
		IntPtr->len = sizeof(ITCObjHeader);
		IntPtr->ITCObjectID = INT_TEL_COL_SEND;
		IntPtr->mode = ITC_SEND_MODE_DIRECT;
		CO_APP_COMM_t* dataPtr = (CO_APP_COMM_t*)(IntPtr+1);
		dataPtr->header.COB_ID = COB_ID;
		dataPtr->header._Len = Len;
		memcpy(dataPtr->VCIdata,p,dataPtr->header._Len);
		IntPtr->len+= dataPtr->header._Len+sizeof(CO_APP_COMM_Header);
		IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"INT Tel to ColMaster %xH(%d), CAN data LEN %d",
							IntPtr->ITCObjectID,IntPtr->len,dataPtr->header._Len);
		CIDCThread::ITCSend(IntPtr);

	}


	void CColXtpIf::CallBackFunc(tXtpObjHeader* msg, void* para)
	{
		if(para!=NULL)
		{
			CColXtpIf* Instance = (CColXtpIf*)para;		
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got XTP msg 0x%x(%d)",
				Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
			switch(msg->telID)
			{
			case ISIDINFO://0x7A5
				{
					ISidInfo* p = (ISidInfo*)(msg+1);
					if(msg->length==(sizeof(tXtpObjHeader)+sizeof(ISidInfo)) )
					{
						//INT Telgram to ColMaster for Handle: IDC_TASK_COL_MASTER
						CColLogical::SIDCommand =  p->sid;
						uchar data[2] = {0,0};
						data[0] = (CColLogical::SIDCommand&0xFF00)>>8;//H
						data[1] = (CColLogical::SIDCommand&0x00FF);   //L
						Instance->SendInt2ColCO(0x7A5,2,data);
					}
				}
				break;

			case SCOLBLADEPOS://->0x7A0
				{
					SColBladeGrp* p = (SColBladeGrp*)(msg+1);
					if(msg->length==(sizeof(tXtpObjHeader)+sizeof(p->cnt)+p->cnt*sizeof(p->blade[0])))
					{
						for(int i = 0; i < p->cnt; i++)
						{
							IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"COL Blade Type %d Pos %04XH",p->blade[i].type,p->blade[i].pos);
							CColLogical::SetBladePosCmd(p->blade[i].type,p->blade[i].pos);							
						}
						uchar data[8] = {0,0,0,0,0,0,0,0};
						data[0] = (CColLogical::GetBladePosCmd(ColBladeHeight)&0xFF00)>>8;//H
						data[1] = (CColLogical::GetBladePosCmd(ColBladeHeight)&0x00FF);   //L
						data[2] = (CColLogical::GetBladePosCmd(ColBladeWidth)&0xFF00)>>8;//H
						data[3] = (CColLogical::GetBladePosCmd(ColBladeWidth)&0x00FF);   //L
						
						//REMARK: Currently Srurgery only support 2 Blade of Collimator. Value may be updated in future						
						Instance->SendInt2ColCO(0x7A0,8,data);
					}
					else
					{
						IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Illgeal cnt Total %d, detail %d %d",
							msg->length,p->cnt,sizeof(p->blade[0]));
					}				
				}
				break;

			case IXRAY://Impact on Serach Run
				break;

			default:

				break;
			}

		}

	}





