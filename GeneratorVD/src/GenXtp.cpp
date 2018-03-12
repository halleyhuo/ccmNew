#include "GenXtp.h"
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "IDCXTP/inc/xtpobj.h"
#include "GenStateM.h"
//#include "GenMain.h"

/******************************************************************************
***************************** XTP Interface **************************************
***  Receive Obj: ISIDINFO,SCOLBLADEPOS, IXRAY... ,                                    **
***  Out Obj:    ICOLBLADEPOS, ICOMPSTATUS, ICOLLSEARCHRUN                             **
*******************************************************************************/
//using namespace std;
BRODCAST_LIST broadcastList[] = 
{    
	{XTP_NODE_MASTER            , 0},
	{XTP_NODE_COLLIMATOR_CONTROL, 1, {ISYSTEMINFO,IXRAY}},
	{XTP_NODE_GENERATOR,          2, {ISYSTEMINFO,SFLCMD}},
};
CGenXtpIf* CGenXtpIf::m_instanse = NULL;
CGenXtpIf::CGenXtpIf():	CIDCThread("GenXtpIf",IDC_TASK_ID(IDC_TASK_GEN_XTP_TASK),Small,Moderate,false)
	{
		xtpClientIf = new CXtpClient(XTP_NODE_GENERATOR);
	}

CGenXtpIf* CGenXtpIf::Sigleton(CGenXtpIf* instanse/* = NULL*/)
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

void CGenXtpIf::onSendiGENData(const iGENData* data)
{
	if(NULL!=data)
	{
		IGENData_t IGenData;
		IGenData.header.sender = xtpClientIf->GetNodeID();
		IGenData.header.target = XTP_NODE_MASTER;
		IGenData.header.telID = IGENDATA;
		IGenData.header.mode	= XTP_MODE_DIRECT;
		IGenData.header.length = sizeof(IGENData_t);
		memcpy(&IGenData.data,data,sizeof(iGENData));
		 
		IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"SendiGENData KV %d MA %d",data->kv,data->ma);
		xtpClientIf->XtpSend(&IGenData.header);
	}
}

	void CGenXtpIf::onSendIGenReadyData()
	{
		IGenReadyFrame_t IGenReadyData;
		IGenReadyData.header.sender = xtpClientIf->GetNodeID();
		IGenReadyData.header.target = XTP_NODE_MASTER;
		IGenReadyData.header.telID = IGENREADY;
		IGenReadyData.header.mode	= XTP_MODE_DIRECT;
		IGenReadyData.header.length = sizeof(IGenReadyFrame_t);
		IGenReady tmp={1,0,1,1};
		memcpy(&IGenReadyData.data,&tmp,sizeof(IGenReady));
		//memcpy(&IGenReadyData.data,&(CGenLogical::Sigleton()->iGenRadeyData),sizeof(IGenReady));	
		xtpClientIf->XtpSend(&IGenReadyData.header);
	}

	void CGenXtpIf::onSendSFlCmdData()
	{
		SFlCmdFrame_t SFlCmdData;
		SFlCmdData.header.sender = xtpClientIf->GetNodeID();
		SFlCmdData.header.target = XTP_NODE_MASTER;
		SFlCmdData.header.telID = SFLCMD;
		SFlCmdData.header.mode	= XTP_MODE_DIRECT;
		SFlCmdData.header.length = sizeof(SFlCmdFrame_t);
		//memcpy(&SFlCmdData.data,&(CGenLogical::Sigleton()->sfldata),sizeof(SFlCmd));	
		//printf("Send message SendSFlCmdData\n");		
		xtpClientIf->XtpSend(&SFlCmdData.header);
	}

	void CGenXtpIf::onSendICompStatus(const CompStatusType status)const
	{
		ICompStatusFrame ICompStatus;
		ICompStatus.header.sender = xtpClientIf->GetNodeID();
		ICompStatus.header.target = XTP_NODE_MASTER;
		ICompStatus.header.telID = ICOMPSTATUS;
		ICompStatus.header.mode	= XTP_MODE_DIRECT;
		ICompStatus.data.compNodeID = xtpClientIf->GetNodeID();
		ICompStatus.data.status = status;
		ICompStatus.header.length = sizeof(ICompStatusFrame);
		xtpClientIf->XtpSend(&ICompStatus.header);
	}

		void CGenXtpIf::onSendTubeLoad(const iTubeLoad* data)
	{
		iTubeLoad_t ICompStatus;
		ICompStatus.header.sender = xtpClientIf->GetNodeID();
		ICompStatus.header.target = XTP_NODE_MASTER;
		ICompStatus.header.telID = ITUBELOAD;
		ICompStatus.header.mode	= XTP_MODE_DIRECT;
		memcpy(&ICompStatus.data,data,sizeof(iTubeLoad));
		ICompStatus.header.length = sizeof(iTubeLoad_t);
		xtpClientIf->XtpSend(&ICompStatus.header);
	}

	void CGenXtpIf::onSendAXD(const iAcqData* data)
	{
		IAcqData_t ICompStatus;
		ICompStatus.header.sender = xtpClientIf->GetNodeID();
		ICompStatus.header.target = XTP_NODE_MASTER;
		ICompStatus.header.telID = IACQDATA;
		ICompStatus.header.mode	= XTP_MODE_DIRECT;
		memcpy(&ICompStatus.data,data,sizeof(iAcqData));
		ICompStatus.header.length = sizeof(IAcqData_t);
		xtpClientIf->XtpSend(&ICompStatus.header);
	}

	void CGenXtpIf::run()
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

	void CGenXtpIf::CallBackFunc(tXtpObjHeader* msg, void* para)
	{
		if(para!=NULL)
		{
			CGenXtpIf* Instance = (CGenXtpIf*)para;		
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got XTP msg 0x%x(%d)",
				Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
			//printf("Get message CGenXtpIf\n");
			switch(msg->telID)
			{
				case SGENDATA://SGENDATA
				{					
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got XTP msg 0x%x(%d) SGENDATA",
						Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
					printf("Node 0x%x Got XTP msg 0x%x(%d) SGENDATA",
					Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
					if(msg->length==(sizeof(SGENDataFrame_t)))
					{
						SGENData *p = (SGENData*)(msg+1);
						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"SGENDATA Kv %d mA %d",
							p->kv,p->ma);
						//make INT Telegram to unit GENStateMachine
						GenRxCmdMapping* listFormat = CheckCmdTable("VFP");//REUSE VFP Format to fill SFP
						if(listFormat!=NULL)
						{
							IntGenVDEventFrame IntGENVDEvent;
							IntGENVDEvent.header.tidSrc = 0;
							IntGENVDEvent.header.tidTarget = IDC_TASK_GEN_LOGICAL;
							IntGENVDEvent.header.mode = ITC_SEND_MODE_DIRECT;
							IntGENVDEvent.header.flowControl = ITC_SEND_NO_FLOW;
							//InfFlDataFrame.header.len = sizeof(InfFlDataFrame);
							IntGENVDEvent.header.ITCObjectID = INT_GENVD_STM_EVENT;
							IntGENVDEvent.data.Header.Event = GENVD_STM_EVENT_TX_SFP;
							IntGENVDEvent.data.Header.extParamCnt = listFormat->eventParamCnt;						
							IntGENVDEvent.header.len = sizeof(IntGENVDEvent.header)+sizeof(IntGENVDEvent.data.Header)+(IntGENVDEvent.data.Header.extParamCnt*sizeof(GenVDParamPacket));
							bool skipNextStep = false;
							for(uchar i = 0; i<listFormat->eventParamCnt && !skipNextStep; i++)
							{
								IntGENVDEvent.data.extParam[i].type = listFormat->eventParamList[i];
								memset(IntGENVDEvent.data.extParam[i].param,0,GenVDParamItemSize);
								switch(IntGENVDEvent.data.extParam[i].type)
								{
									case GENVD_STM_EVENT_PARAM_MODE:
										//printf("mode is %d\n",IntGENVDEvent.data.extParam[i].param);
										if(p->FlMode==FlContinue)
										{
											strcpy(IntGENVDEvent.data.extParam[i].param,"C");			
										}
										else if(p->FlMode==FlPulse)
										{
											strcpy(IntGENVDEvent.data.extParam[i].param,"P");
										}
										else if(p->FlMode==FlSingle)
										{
											strcpy(IntGENVDEvent.data.extParam[i].param,"S");
										}
										else
										{
											skipNextStep = true;
											IDC::Log(IDC::LOG_WARNING,LOG_TAG,"INVALID MODE of SGENDATA %d",p->FlMode);
										}
										
										break;

									case GENVD_STM_EVENT_PARAM_KV:
										sprintf(IntGENVDEvent.data.extParam[i].param,"%d",p->kv);
										break;

									case GENVD_STM_EVENT_PARAM_MA:
										sprintf(IntGENVDEvent.data.extParam[i].param,"%d",p->ma);
										break;

									case GENVD_STM_EVENT_PARAM_MAS:
										sprintf(IntGENVDEvent.data.extParam[i].param,"%d",p->mas);
										
										break;

									case GENVD_STM_EVENT_PARAM_TMAX:
										sprintf(IntGENVDEvent.data.extParam[i].param,"%d",p->Tmax);
										
										break;

									case GENVD_STM_EVENT_PARAM_FOCUS:
										if(p->focus==1)
										{
											strcpy(IntGENVDEvent.data.extParam[i].param,"S");
										}
										else if(p->focus==2)
										{
											strcpy(IntGENVDEvent.data.extParam[i].param,"B");
										}
										else
										{
											skipNextStep = true;	
											IDC::Log(IDC::LOG_WARNING,LOG_TAG,"INVALID FOCUS of SGENDATA %d",p->focus);
										}
	
										break;

									case GENVD_STM_EVENT_PARAM_FPS:
										if(p->Fps>0)
											p->Fps = p->Fps;
										sprintf(IntGENVDEvent.data.extParam[i].param,"%d",p->Fps);
										break;

									default:
										IDC::Log(IDC::LOG_WARNING,LOG_TAG,"INVALID Param TYPE %d",IntGENVDEvent.data.extParam[i].type);
										skipNextStep = true;
									break;

									
								}
								
								
							}
							if(!skipNextStep)
							{
								CIDCThread::ITCSend(&IntGENVDEvent.header);	
							}
						}
						

					}


					//CIDCThread::ITCSend(&(p.header));
				
				}
				break;
				
			case SFLCMD:
				{					
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got XTP msg 0x%x(%d) SFLCMD",
					Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
					/*printf("Node 0x%x Got XTP msg 0x%x(%d) SFLCMD",
					Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);*/
					if(msg->length==(sizeof(SFlCmdFrame_t)))
					{
						//make INT Telegram to unit GENStateMachine
						IntGenVDEventFrame IntGENVDEvent;
						IntGENVDEvent.header.tidSrc = 0;
						IntGENVDEvent.header.tidTarget = IDC_TASK_GEN_LOGICAL;
						IntGENVDEvent.header.mode = ITC_SEND_MODE_DIRECT;
						IntGENVDEvent.header.flowControl = ITC_SEND_NO_FLOW;
						//InfFlDataFrame.header.len = sizeof(InfFlDataFrame);
						IntGENVDEvent.header.ITCObjectID = INT_GENVD_STM_EVENT;
						IntGENVDEvent.data.Header.extParamCnt = 0;						
						IntGENVDEvent.header.len = sizeof(IntGENVDEvent.header)+sizeof(IntGENVDEvent.data.Header)+(IntGENVDEvent.data.Header.extParamCnt*sizeof(GenVDParamPacket));
						
						
						SFlCmd *p  = (SFlCmd*)(msg+1);
						if(p->cmd==FLCmdActive)
						{
							IntGENVDEvent.data.Header.Event = GENVD_STM_EVENT_RCV_START_EXP;
							CIDCThread::ITCSend(&IntGENVDEvent.header);								
						}
						else if(p->cmd==FLCmdStart)//Ignor it.
						{
						}
						else
						{							
							IntGENVDEvent.data.Header.Event = GENVD_STM_EVENT_RCV_STOP_EXP;
							CIDCThread::ITCSend(&IntGENVDEvent.header);	
						}

					}

				}
				break;

			default:

				break;
			}

		}

	}


