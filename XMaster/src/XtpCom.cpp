#include "XtpCom.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
/******************************************************************************
***************************** XTP Interface ***********************************
*******************************************************************************/

BRODCAST_LIST broadcastList[] = 
{    
	{XTP_NODE_MASTER            , 0},
	{XTP_NODE_COLLIMATOR_CONTROL, 1, {ISYSTEMINFO,IXRAY}},
	{XTP_NODE_GENERATOR,          2, {ISYSTEMINFO,SFLCMD}},
};

CXtpCom* CXtpCom::m_instanse = NULL;

CXtpCom::CXtpCom():	CIDCThread("MasterXtpIf",IDC_TASK_ID(IDC_TASK_MASTER_XTPIF),Small,Moderate,false)
{
	xtpClientIf = new CXtpClient(XTP_NODE_MASTER);
	memset(m_bladePos,0,sizeof(m_bladePos));
}

CXtpCom* CXtpCom::Sigleton(CXtpCom* instanse/* = NULL*/)
{
	if(NULL!=instanse)
	{
		m_instanse = instanse;
	}

	return m_instanse;
}


void CXtpCom::run()
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

void CXtpCom::CallBackFunc(tXtpObjHeader* msg, void* para)
{
	if(para!=NULL)
	{
		CXtpCom* Instance = (CXtpCom*)para;		
		IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got XTP msg 0x%x(%d)",
			Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
		switch(msg->telID)
		{
		case SUICMD://0x0230
			{					
				if(msg->length==(sizeof(tXtpObjHeader)+sizeof(SUiCmd)) )
				{
					SUiCmd* p = (SUiCmd*)(msg+1);
					switch(p->cmd)
					{
						case UICMD_ColHeight:
							if(p->param==UICMD_PARAM_PLS)
							{
								Instance->m_bladePos[0] += 50;
							}
							else if(p->param==UICMD_PARAM_MINUS)
							{
								if(Instance->m_bladePos[0]>=50)
								{
									Instance->m_bladePos[0] -= 50;
								}
								else
								{
									Instance->m_bladePos[0] = 0;
								}
							}
							break;

						case UICMD_ColWidth:
							if(p->param==UICMD_PARAM_PLS)
							{
								Instance->m_bladePos[1] += 50;
							}
							else if(p->param==UICMD_PARAM_MINUS)
							{
								if(Instance->m_bladePos[1]>=50)
								{
									Instance->m_bladePos[1] -= 50;
								}
								else
								{
									Instance->m_bladePos[1] = 0;
								}
							}
							break;

						case UICMD_EXP_PLUS://Plus Exposure Parameter
						case UICMD_EXP_MINUS:
						{
							onSendIntGen(FuncKVSel,(p->cmd == UICMD_EXP_PLUS) ? UICMD_PARAM_PLS:UICMD_PARAM_MINUS);
						}
							break;

						case UICMD_FPS_CHANGE:						
							onSendIntGen(FuncFlFPS,UICMD_EXP_PLUS);//Toggle						
							break;

						case UICMD_SNGL_MODE:
							onSendIntGen(FuncSingleMode, 0);
							break;

						case UICMD_FLUORO_MODE:
							onSendIntGen(FuncFluoroMode, 0);
							break;
						default:
							break;
					}
					
					if(UICMD_ColHeight==p->cmd || UICMD_ColWidth==p->cmd)
					{
						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Will call BladeSizeChange %d %d",
									Instance->m_bladePos[0],Instance->m_bladePos[1]);
						Instance->onBladeSizeChange();
					}
					
				}
			}
			break;	

		case SXRAYCONTROL:
			{
				SXRayControl * sxRayControl = (SXRayControl*)(msg + 1);
				IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"SXRAYCONTROL : hk = %d",sxRayControl->hk);
				Instance->onSendFLCmd(sxRayControl->hk);
			}
			break;

		case ICOLBLADEPOS://from Collimator VD
		{
			BladeCount* Bladecount = (BladeCount*)(msg+1);
			if( msg->length==(sizeof(tXtpObjHeader)+sizeof(IColBlade)*Bladecount->count+sizeof(Bladecount->count)) )
			{
				IColBlade* bladeInfo = (IColBlade*)(Bladecount+1);
				for(int i=0 ; i<Bladecount->count; i++)
				{
					if(bladeInfo[i].type<12)
					{
						Instance->m_bladePos[i] = bladeInfo[i].pos;
					}
				}
				Instance->onBladeSizeShow();
			}
		}
			break;

		case IGENDATA://Produce INT Telegram to Fluoro. INT_FL_DATA to unit Fluoro
		{
			iGENData* p = (iGENData*)(msg+1);
			INT_FlData_Frame InfFlDataFrame;
			InfFlDataFrame.header.tidSrc = 0;
			InfFlDataFrame.header.tidTarget = IDC_TASK_FLUORO;
			InfFlDataFrame.header.mode = ITC_SEND_MODE_DIRECT;
			InfFlDataFrame.header.flowControl = ITC_SEND_NO_FLOW;
			InfFlDataFrame.header.len = sizeof(InfFlDataFrame);
			InfFlDataFrame.header.ITCObjectID = INT_FL_DATA;
			
			InfFlDataFrame.ipcData.FlMode = p->FlMode;
			InfFlDataFrame.ipcData.kv = p->kv;
			InfFlDataFrame.ipcData.ma = p->ma;
			InfFlDataFrame.ipcData.mas		= p->mas;
			InfFlDataFrame.ipcData.Tmax		= p->Tmax;
			InfFlDataFrame.ipcData.Fps		= p->Fps;
			InfFlDataFrame.ipcData.focus		= p->focus;
			CIDCThread::ITCSend(&InfFlDataFrame.header);

		}
			break;

		case SGENERALCMD://produce Int_Gen_Status to unit Fluoro. $FluoroACQ Process  Step 1(Param != 0) & Step 5 (Param=0)
		{
			SGeneralCmd* p = (SGeneralCmd*)(msg+1);
			INT_GeneralCtrl_t_Frame IntGeneralCmdFrame;
			IntGeneralCmdFrame.header.tidSrc = 0;
			IntGeneralCmdFrame.header.tidTarget = IDC_TASK_FLUORO;
			IntGeneralCmdFrame.header.mode = ITC_SEND_MODE_DIRECT;
			IntGeneralCmdFrame.header.flowControl = ITC_SEND_NO_FLOW;
			IntGeneralCmdFrame.header.len = sizeof(IntGeneralCmdFrame);
			IntGeneralCmdFrame.header.ITCObjectID = INT_GENERAL_CONTROL;
			IntGeneralCmdFrame.ipcData.consoleType = ConsoleFootPedel;
			IntGeneralCmdFrame.ipcData.controlElementType = ON_OFF_LEVEL_BUTTON;
			IntGeneralCmdFrame.ipcData.functionCode = FuncAcqFluoro;
			IntGeneralCmdFrame.ipcData.param		= (p->param!=0)?ON_OFF_LEVEL_PRESS:ON_OFF_LEVEL_RELEASE;
			CIDCThread::ITCSend(&IntGeneralCmdFrame.header);
		}

			break;

		case IACQDATA:
		{
			iAcqData	*p = (iAcqData *)(msg + 1);
			IntGenAcqFrame		genAcqFrame;

			genAcqFrame.header.tidSrc		= 0;
			genAcqFrame.header.tidTarget	= IDC_TASK_FLUORO;
			genAcqFrame.header.mode			= ITC_SEND_MODE_DIRECT;
			genAcqFrame.header.flowControl	= ITC_SEND_NO_FLOW;
			genAcqFrame.header.len			= sizeof(IntGenAcqFrame);
			genAcqFrame.header.ITCObjectID	= INT_GEN_ACQ;

			genAcqFrame.acqData.kv			= p->kv;
			genAcqFrame.acqData.ma			= p->ma;
			genAcqFrame.acqData.ms			= p->ms;
			genAcqFrame.acqData.mas			= p->mas;
			genAcqFrame.acqData.acqIndex	= p->acqIndex;

			CIDCThread::ITCSend(&genAcqFrame.header);
		}
		break;

		case IWSDOSEINFO:
		{
			IwsDoseInfo		*p = (IwsDoseInfo *)(msg + 1);
			IntIwsDoseInfoFrame		iwsDoseInfoFrame;

			iwsDoseInfoFrame.header.tidSrc		= 0;
			iwsDoseInfoFrame.header.tidTarget	= IDC_TASK_FLUORO;
			iwsDoseInfoFrame.header.mode		= ITC_SEND_MODE_DIRECT;
			iwsDoseInfoFrame.header.flowControl	= ITC_SEND_NO_FLOW;
			iwsDoseInfoFrame.header.len			= sizeof(IntIwsDoseInfoFrame);
			iwsDoseInfoFrame.header.ITCObjectID	= INT_IWS_DOSE_INFO;

			iwsDoseInfoFrame.doseInfo.doseValue	= p->doseValue;
			iwsDoseInfoFrame.doseInfo.index		= p->index;

			CIDCThread::ITCSend(&iwsDoseInfoFrame.header);
		}
		break;

		case ITUBELOAD:
		{
			iTubeLoad		*p = (iTubeLoad *)(msg + 1);
			
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"ITUBELOAD tubeload : %d, waittime : %d",
				p->tubeload, p->waittime);
			
			Instance->onTubeLoad(p->tubeload);
		}
		break;

		case IGENREADY://produce Int_Gen_Status to unit Fluoro $FluoroACQ Process  Step 3, Fluoro Process
		{
			IGenReady*p =(IGenReady*)(msg+1);
			if(p->GeneratorReady==GeneratorReadyforAcq && p->TubeStatus==TubeStatusWork && p->DoorStatus==DoorStatusOK)
			{
				IntGenStatusFrame IntGenStatus; 
				IntGenStatus.header.tidSrc = 0;
				IntGenStatus.header.tidTarget = IDC_TASK_FLUORO;
				IntGenStatus.header.mode = ITC_SEND_MODE_DIRECT;
				IntGenStatus.header.flowControl = ITC_SEND_NO_FLOW;
				IntGenStatus.header.len = sizeof(IntGenStatus);
				IntGenStatus.header.ITCObjectID = INT_GEN_STATUS;
				memcpy(&IntGenStatus.data.IGenInfo,msg+1,sizeof(IGenReady));
				CIDCThread::ITCSend(&IntGenStatus.header);
			}
			else
			{
				IDC::Log(IDC::LOG_ERR,LOG_TAG,"FluoroAcq Abort due to %d %d %d",p->GeneratorReady,p->TubeStatus,p->DoorStatus);
			}
		}
			break;		

		default:

			break;
		}

	}

}


void CXtpCom::onSendFLCmd(uchar hk)
{

	SFlCmdFrame_t  buf;
	extern ushort	g_flFps;
	FLCmdType	cmd;

	if(hk == 0)
	{
		 cmd = FLCmdStop;
	}
	else if(hk == 1)
	{
		cmd = FLCmdActive;
	}
	else
	{
		return;
	}

	buf.header.sender = xtpClientIf->GetNodeID();
	buf.header.target = 0;
	buf.header.telID = SFLCMD;
	buf.header.mode  = XTP_MODE_BROADCAST; 
	buf.header.length = sizeof(SFlCmdFrame_t);
	buf.data.cmd = cmd;
	buf.data.dummy = 0;
	buf.data.FlFps = g_flFps;

	xtpClientIf->XtpSend(&buf.header);

}


void CXtpCom::onSendIntGen(const FunctionCode funcCode, const ushort param)
{	
	INT_GeneralCtrl_t_Frame IntGeneralCmdFrame;
	IntGeneralCmdFrame.header.tidSrc = 0;
	IntGeneralCmdFrame.header.tidTarget = IDC_TASK_FLUORO;
	IntGeneralCmdFrame.header.mode = ITC_SEND_MODE_DIRECT;
	IntGeneralCmdFrame.header.flowControl = ITC_SEND_NO_FLOW;
	IntGeneralCmdFrame.header.len = sizeof(IntGeneralCmdFrame);
	IntGeneralCmdFrame.header.ITCObjectID = INT_GENERAL_CONTROL;
	IntGeneralCmdFrame.ipcData.consoleType = ConsoleFootPedel;
	IntGeneralCmdFrame.ipcData.controlElementType = PLUS_MINUS_BUTTON;
	IntGeneralCmdFrame.ipcData.functionCode = funcCode;
	IntGeneralCmdFrame.ipcData.param		= param;
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"onSendIntGen funcCode %d param %d controlElementType %d",
		funcCode,param,IntGeneralCmdFrame.ipcData.controlElementType);
	CIDCThread::ITCSend(&IntGeneralCmdFrame.header);

}


void CXtpCom::onBladeSizeShow()
{
	iUiShowFrame_t iUiShowFrame;
	iUiShowFrame.header.sender = xtpClientIf->GetNodeID();
	iUiShowFrame.header.target = XTP_NODE_TUI_1;
	iUiShowFrame.header.telID = IUISHOW;
	iUiShowFrame.header.mode  = XTP_MODE_DIRECT;	
	iUiShowFrame.objCnt.obj_count = 2;//REMARK: Currently Srurgery only support 2 Blade of Collimator. Value may be updated in future
	iUiShowFrame.header.length = sizeof(tXtpObjHeader)+ 
		sizeof(iUiShowFrame.objCnt.obj_count)+iUiShowFrame.objCnt.obj_count*sizeof(iUiShowObj);
	iUiShowFrame.uiShowObj[0].sel_func = UI_SEL_CollimatorHeight;	
	iUiShowFrame.uiShowObj[1].sel_func = UI_SEL_CollimatorWeight;
	for(int i=0; i<iUiShowFrame.objCnt.obj_count; i++)
	{
		iUiShowFrame.uiShowObj[i].param    = m_bladePos[i];
		iUiShowFrame.uiShowObj[i].status   = UI_Widget_ReadOnly;
	}
	xtpClientIf->XtpSend(&iUiShowFrame.header);
}

void CXtpCom::onBladeSizeChange()
{
	SColBladeFrame_t sColBladeFrame;
	sColBladeFrame.header.sender = xtpClientIf->GetNodeID();
	sColBladeFrame.header.target = XTP_NODE_COLLIMATOR_CONTROL;
	sColBladeFrame.header.telID = SCOLBLADEPOS;
	sColBladeFrame.header.mode  = XTP_MODE_DIRECT;	
	sColBladeFrame.data.cnt = 2;//REMARK: Currently Srurgery only support 2 Blade of Collimator. Value may be updated in future
	sColBladeFrame.header.length = sizeof(tXtpObjHeader)+ 
		sizeof(sColBladeFrame.data.cnt)+sColBladeFrame.data.cnt*sizeof(SColBlade);
	sColBladeFrame.data.blade[0].type	= ColBladeHeight;
	sColBladeFrame.data.blade[1].type	= ColBladeWidth;

	for(int i=0; i<sColBladeFrame.data.cnt; i++)
	{
		sColBladeFrame.data.blade[i].pos    = m_bladePos[i];
		sColBladeFrame.data.blade[i].rotationAng  = 0;;
	}
	xtpClientIf->XtpSend(&sColBladeFrame.header);
}

void CXtpCom::onSGendata(const SGENData* data)
{
	SGENDataFrame_t SGENDataFrame;
	SGENDataFrame.header.sender = xtpClientIf->GetNodeID();
	SGENDataFrame.header.target = XTP_NODE_GENERATOR;
	SGENDataFrame.header.telID = SGENDATA;
	SGENDataFrame.header.mode  = XTP_MODE_DIRECT;	
	SGENDataFrame.header.length = sizeof(SGENDataFrame_t);
	memcpy(&SGENDataFrame.data,data,sizeof(SGENDataFrame.data));
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"onSGendata mode %d kv %d ma %d",
		SGENDataFrame.data.FlMode,SGENDataFrame.data.kv,SGENDataFrame.data.ma);
	xtpClientIf->XtpSend(&SGENDataFrame.header);
}

void CXtpCom::onIFlData(const iFlData* data)
{
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"onIFlData Broadcast");
	IFlDataFrame_t IFldataFrame;
	IFldataFrame.header.sender = xtpClientIf->GetNodeID();
	IFldataFrame.header.target = 0;
	IFldataFrame.header.telID = IFLDATA;
	IFldataFrame.header.mode  = XTP_MODE_SYSTEM;	
	IFldataFrame.header.length = sizeof(IFlDataFrame_t);
	memcpy(&IFldataFrame.data,data,sizeof(IFldataFrame.data));
	
	xtpClientIf->XtpSend(&IFldataFrame.header);
}

void CXtpCom::onFlCmd(const SFlCmd* data, bool ifBroadCast/* = false*/)
{
	const uchar tmpBufLen = sizeof(SFlCmd)+sizeof(tXtpObjHeader);
	uchar tmpData[tmpBufLen];
	((tXtpObjHeader*)tmpData)->sender = xtpClientIf->GetNodeID();
	if(ifBroadCast)
	{	
		((tXtpObjHeader*)tmpData)->target = 0xFF;
		((tXtpObjHeader*)tmpData)->mode  = XTP_MODE_BROADCAST;		
	}
	else
	{
		((tXtpObjHeader*)tmpData)->target = XTP_NODE_GENERATOR;
		((tXtpObjHeader*)tmpData)->mode  = XTP_MODE_DIRECT;
	}
	((tXtpObjHeader*)tmpData)->telID = SFLCMD;
		
	((tXtpObjHeader*)tmpData)->length = sizeof(tmpData);
	memcpy(&tmpData[sizeof(tXtpObjHeader)],data,sizeof(SFlCmd));
	
	xtpClientIf->XtpSend((tXtpObjHeader*)tmpData);
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"onFlCmd : fps = %d", data->FlFps);
	
}

void CXtpCom::onTubeLoad(const ushort tubeLoad)
{
	iUiShowFrame_t iUiShowFrame;
	iUiShowFrame.header.sender = xtpClientIf->GetNodeID();
	iUiShowFrame.header.target = XTP_NODE_TUI_1;
	iUiShowFrame.header.telID = IUISHOW;
	iUiShowFrame.header.mode  = XTP_MODE_DIRECT;

	iUiShowFrame.objCnt.obj_count = 1;
	iUiShowFrame.header.length = sizeof(tXtpObjHeader) + sizeof(iUiShowFrame.objCnt.obj_count) + sizeof(iUiShowObj);

	iUiShowFrame.uiShowObj[0].sel_func	= UI_SEL_Tubeload;
	iUiShowFrame.uiShowObj[0].param		= tubeLoad;
	iUiShowFrame.uiShowObj[0].status   = UI_Widget_ReadOnly;

	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"onTubeLoad : tubeLoad = %d", tubeLoad);

	xtpClientIf->XtpSend(&iUiShowFrame.header);
}

void CXtpCom::onAcqUiShow(const iFlData* data)
{
	
	iUiShowFrame_t iUiShowFrame;
	iUiShowFrame.header.sender = xtpClientIf->GetNodeID();
	iUiShowFrame.header.target = XTP_NODE_TUI_1;
	iUiShowFrame.header.telID = IUISHOW;
	iUiShowFrame.header.mode  = XTP_MODE_DIRECT;
	if(data->FlMode == FlSingle)
	{
		iUiShowFrame.objCnt.obj_count = 3;
		iUiShowFrame.header.length = sizeof(tXtpObjHeader)+ 
			sizeof(iUiShowFrame.objCnt.obj_count)+iUiShowFrame.objCnt.obj_count*sizeof(iUiShowObj);
		iUiShowFrame.uiShowObj[0].sel_func	= UI_SEL_Kv;	
		iUiShowFrame.uiShowObj[1].sel_func	= UI_SEL_mAs;
		iUiShowFrame.uiShowObj[2].sel_func	= UI_SEL_FLMode;	
		
		iUiShowFrame.uiShowObj[0].param		= data->kv;
		iUiShowFrame.uiShowObj[1].param		= data->mas;
		iUiShowFrame.uiShowObj[2].param		= FlSingle;

		for(int i=0; i<iUiShowFrame.objCnt.obj_count; i++)
		{		
			iUiShowFrame.uiShowObj[i].status   = UI_Widget_ReadOnly;
		}
	}
	else if(data->FlMode==FlContinue)
	{
		iUiShowFrame.objCnt.obj_count = 3;//REMARK: Mode. Continue mode show kv ma.
		iUiShowFrame.header.length = sizeof(tXtpObjHeader)+ 
			sizeof(iUiShowFrame.objCnt.obj_count)+iUiShowFrame.objCnt.obj_count*sizeof(iUiShowObj);
		iUiShowFrame.uiShowObj[0].sel_func = UI_SEL_Kv;	
		iUiShowFrame.uiShowObj[1].sel_func = UI_SEL_mA;
		iUiShowFrame.uiShowObj[2].sel_func = UI_SEL_FLMode;	
		
		iUiShowFrame.uiShowObj[0].param    = data->kv;
		iUiShowFrame.uiShowObj[1].param    = data->ma;
		iUiShowFrame.uiShowObj[2].param    = FlContinue;
		for(int i=0; i<iUiShowFrame.objCnt.obj_count; i++)
		{		
			iUiShowFrame.uiShowObj[i].status   = UI_Widget_ReadOnly;
		}
	}
	else
	{		
		iUiShowFrame.objCnt.obj_count = 4;//REMARK: Mode. Continue mode show kv ma.
		iUiShowFrame.header.length = sizeof(tXtpObjHeader)+ 
			sizeof(iUiShowFrame.objCnt.obj_count)+iUiShowFrame.objCnt.obj_count*sizeof(iUiShowObj);
		iUiShowFrame.uiShowObj[0].sel_func = UI_SEL_Kv;	
		iUiShowFrame.uiShowObj[1].sel_func = UI_SEL_mAs;
		iUiShowFrame.uiShowObj[2].sel_func = UI_SEL_FLMode;	
		iUiShowFrame.uiShowObj[3].sel_func = UI_SEL_Fps;	
		
		iUiShowFrame.uiShowObj[0].param    = data->kv;
		iUiShowFrame.uiShowObj[1].param    = data->mas;
		iUiShowFrame.uiShowObj[2].param    = FlPulse;
		iUiShowFrame.uiShowObj[3].param    = data->Fps;
		for(int i=0; i<iUiShowFrame.objCnt.obj_count; i++)
		{		
			iUiShowFrame.uiShowObj[i].status   = UI_Widget_ReadOnly;
		}
	}
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"onAcqUiShow %d cnt %d",iUiShowFrame.header.length,iUiShowFrame.objCnt.obj_count);
	xtpClientIf->XtpSend(&iUiShowFrame.header);
}




