/*
 * GenStateM.cpp
 *
 *  Created on: 2017-10-07
 *      Author: 
 */
#include "GenStateM.h"
#include "GenXtp.h"
#include <stdio.h>
#include "IDCBase/inc/IDCAlarm.h"
#include <stdlib.h>

static GenRxCmdMapping GenRxCmdMappingTable[]
{
	{"IFV",GENVD_STM_EVENT_RCV_IFV,     0,{GENVD_STM_EVENT_PARAM_NONE}},//Interface Ver & Interface Type. Future use.REMARK Special Handle when receive
	{"SWV",GENVD_STM_EVENT_RCV_SWV,     1,{GENVD_STM_EVENT_PARAM_VER,GENVD_STM_EVENT_PARAM_NONE}},
	{"INI",GENVD_STM_EVENT_RCV_INI,     0,{GENVD_STM_EVENT_PARAM_NONE}},//No addtional Param
	{"SBY",GENVD_STM_EVENT_RCV_SBY,     0,{GENVD_STM_EVENT_PARAM_NONE}},//No addtional Param
	{"RDY",GENVD_STM_EVENT_RCV_RDY,     4,
		{GENVD_STM_EVENT_PARAM_RDYSTATUS,GENVD_STM_EVENT_PARAM_RDYSTATUS2,GENVD_STM_EVENT_PARAM_TEMP,GENVD_STM_EVENT_PARAM_UDC,GENVD_STM_EVENT_PARAM_NONE}},
	{"VFP",GENVD_STM_EVENT_RCV_VFP, 7,
		{GENVD_STM_EVENT_PARAM_MODE,GENVD_STM_EVENT_PARAM_KV,GENVD_STM_EVENT_PARAM_MA,GENVD_STM_EVENT_PARAM_MAS,
			GENVD_STM_EVENT_PARAM_TMAX,GENVD_STM_EVENT_PARAM_FOCUS,GENVD_STM_EVENT_PARAM_FPS,GENVD_STM_EVENT_PARAM_NONE}},
	{"EXP",GENVD_STM_EVENT_RCV_EXP,     0,{GENVD_STM_EVENT_PARAM_NONE}},//No addtional Param
	{"ERR",GENVD_STM_EVENT_RCV_ERR,     0,{GENVD_STM_EVENT_PARAM_NONE}},//No addtional Param
	{"AXD",GENVD_STM_EVENT_RCV_AXD,     6,
		{GENVD_STM_EVENT_PARAM_KV,GENVD_STM_EVENT_PARAM_MA,GENVD_STM_EVENT_PARAM_XRAY_MS,
			GENVD_STM_EVENT_PARAM_MAS,GENVD_STM_EVENT_PARAM_ENDXRAY,GENVD_STM_EVENT_PARAM_GEN_PULSE,
			GENVD_STM_EVENT_PARAM_NONE}},
	{"VFD",GENVD_STM_EVENT_RCV_VFD, 4,
		{GENVD_STM_EVENT_PARAM_KV,GENVD_STM_EVENT_PARAM_MA,GENVD_STM_EVENT_PARAM_MAS,
			GENVD_STM_EVENT_PARAM_TMAX,GENVD_STM_EVENT_PARAM_NONE}},
	{"XON",GENVD_STM_EVENT_RCV_XSTATUS, 1,{GENVD_STM_EVENT_PARAM_XON,GENVD_STM_EVENT_PARAM_NONE}},//.REMARK Special Handle when receive
	{"XOFF",GENVD_STM_EVENT_RCV_XSTATUS,1,{GENVD_STM_EVENT_PARAM_XOFF,GENVD_STM_EVENT_PARAM_NONE}},//.REMARK Special Handle when receive
	{"MSG",GENVD_STM_EVENT_RCV_MSG,3,{GENVD_STM_EVENT_PARAM_MSG_NUM,GENVD_STM_EVENT_PARAM_MSG_PRIO,
		GENVD_STM_EVENT_PARAM_MSG_TXT,GENVD_STM_EVENT_PARAM_NONE}}
};

#define ACTUAL_CMD_TABLE_CNT 13 //change follow with above Table

typedef struct 
{
	GENVD_STM_EVENT_TYPE EventType;
	char*                TxCommand;
}TxCOMMapping;

TxCOMMapping TxCOMList[] =
{
	{GENVD_STM_EVENT_START,   "<IFV"},
	{GENVD_STM_EVENT_RCV_INI, "<SINIT 10 15450 27000 2950 +53 1011 1185 2904 +97 995 1176"},
	{GENVD_STM_EVENT_RCV_START_EXP,"<SFL 1"},
	{GENVD_STM_EVENT_RCV_STOP_EXP,"<SFL 0"},
	{GENVD_STM_EVENT_RCV_ERR,"<ERQ"},
	{GENVD_STM_EVENT_NONE}
};

TxCOMMapping* GetCOMMTx(const GENVD_STM_EVENT_TYPE eventType)
{
	TxCOMMapping* rc = NULL;
	
	TxCOMMapping* p = &TxCOMList[0];

	while(p->EventType!=GENVD_STM_EVENT_NONE)
	{
		if(p->EventType==eventType)
		{	
			rc = p;
			break;
		}
		else 
		{
			p++;
		}
	}
	return rc;
}


GenRxCmdMapping* CheckCmdTable(const char cmdStr[4], const uchar cmdLen)
{
	GenRxCmdMapping* rc = NULL;
	for(uchar i =0; i<ACTUAL_CMD_TABLE_CNT; i++)
	{
		if( (cmdLen == strlen(GenRxCmdMappingTable[i].RxCmdType)) && 
		(strncmp(cmdStr,GenRxCmdMappingTable[i].RxCmdType,cmdLen)==0) )
		{
			rc = &GenRxCmdMappingTable[i];
			break;
		}
	}
	return rc;
}



CGenStateM::CGenStateM() 
	: CIDCThread("GenStateM",IDC_TASK_GEN_LOGICAL,Small,Moderate,true),
	m_State(GEN_STATE_IDLE),m_BootUpFin(false),
	m_alarmTid(0U)
{
}

void CGenStateM::AlarmTxIFV(void* param)
{
	if(param!=NULL)
	{
		CGenStateM* threadPointer = (CGenStateM*)param;
		char buf[MAX_ITC_LENGTH];
		ITCObjHeader* p = (ITCObjHeader*)buf;
		p->tidSrc = threadPointer->taskIdent;
		p->tidTarget = threadPointer->taskIdent;		
		p->mode = ITC_SEND_MODE_DIRECT;
		p->flowControl = ITC_SEND_NO_FLOW;		
		p->ITCObjectID = INT_GENVD_STM_EVENT;

		
		IntGenVDEventFrame IntGENVDEvent;
		IntGENVDEvent.header.tidSrc = threadPointer->taskIdent;;
		IntGENVDEvent.header.tidTarget = threadPointer->taskIdent;;
		IntGENVDEvent.header.mode = ITC_SEND_MODE_DIRECT;
		IntGENVDEvent.header.flowControl = ITC_SEND_NO_FLOW;
		//InfFlDataFrame.header.len = sizeof(InfFlDataFrame);
		IntGENVDEvent.header.ITCObjectID = INT_GENVD_STM_EVENT;
		IntGENVDEvent.data.Header.Event = GENVD_STM_EVENT_START;		
		IntGENVDEvent.data.Header.extParamCnt = 0;
		IntGENVDEvent.header.len = sizeof(IntGENVDEvent.header)+sizeof(IntGENVDEvent.data.Header)+(IntGENVDEvent.data.Header.extParamCnt*sizeof(GenVDParamPacket));
		CIDCThread::ITCSend(&IntGENVDEvent.header);	

		IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Alarm Standout %d >>> ",IntGENVDEvent.header.len);
	}
}

//0xFFFF is invalid value
ushort CGenStateM::GetUSVal(const GenVDEventFrameHeader* packet, const GENVD_STM_EVENT_PARAM_TYPE type)
{
	ushort rc = 0xFFFF;
	if(packet!=NULL)
	{
		GenVDParamPacket* ParamPacket = (GenVDParamPacket*)(packet+1);
		for(uchar i =0; i<packet->extParamCnt; i++)
		{
			if(ParamPacket[i].type==type)
			{
				rc = atoi(ParamPacket[i].param);
				break;
			}
		}
	}
	return rc;
}

char*  CGenStateM::GetStrVal(const GenVDEventFrameHeader* packet, const GENVD_STM_EVENT_PARAM_TYPE type)
{
	char* rc = NULL;
	if(packet!=NULL)
	{
		GenVDParamPacket* ParamPacket = (GenVDParamPacket*)(packet+1);
		for(uchar i =0; i<packet->extParamCnt; i++)
		{
			if(ParamPacket[i].type==type)
			{
				rc = ParamPacket[i].param;
				break;
			}
		}
	}
	return rc;
}

bool CGenStateM::SetStrTx(const GenVDEventFrameHeader* packet, const GENVD_STM_EVENT_PARAM_TYPE type, char targetStr[80])
{
	bool rc = false;
	char* strVal = GetStrVal(packet,type);
	if(strVal!=NULL)
	{
		strncat(targetStr," ",1);
		strncat(targetStr,strVal,strlen(strVal));
		rc = true;
	}
	return rc;

}



void CGenStateM::run()
{    
	//create alarm	
	create_alarm(5000,AlarmTxIFV,this,m_alarmTid,true);
    while(true)
    {
        char buf[MAX_ITC_LENGTH];
		const int rc = m_msgQueue->recv(buf,MAX_ITC_LENGTH);
		if(rc>0)
		{
			ITCObjHeader* p = (ITCObjHeader*)buf;
			if(p->tidTarget != IDC_TASK_GEN_LOGICAL)
				break;
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Recv Msg Src %d Target %d Len %d ObjID 0x%x",
				p->tidSrc,p->tidTarget,p->len,p->ITCObjectID);
			switch(p->ITCObjectID)
			{
				case INT_GENVD_STM_EVENT:
				{
					if(p->len>=(sizeof(GenVDEventFrameHeader)+sizeof(ITCObjHeader)))
					{
						GenVDEventFrameHeader* EventHeader = (GenVDEventFrameHeader*)(p+1);
						if(EventHeader->extParamCnt*sizeof(GenVDParamPacket)+sizeof(GenVDEventFrameHeader)+sizeof(ITCObjHeader)==p->len)
						{
							bool EnterintoStateM = false;
							switch(EventHeader->Event)
							{
								case GENVD_STM_EVENT_RCV_ERR:
								{
									TxCOMMapping* TxCOMM = GetCOMMTx(GENVD_STM_EVENT_RCV_ERR);
									if(NULL!=TxCOMM)
									{
										com_send(TxCOMM->TxCommand,strlen(TxCOMM->TxCommand));
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send out CMD %s in state %d",TxCOMM->TxCommand,m_State);
									}
									else
									{
										IDC::Log(IDC::LOG_WARNING,LOG_TAG,"CAN't Get COMMAND as TYPE %d",EventHeader->Event);
									}	
								}
									break;
								case GENVD_STM_EVENT_RCV_MSG:
								{
									GenVDParamPacket* ParamPacket = (GenVDParamPacket*)(EventHeader+1);

									IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"MSG num:%s prio:%s txt:%s",ParamPacket[0].param,ParamPacket[1].param,ParamPacket[2].param);

								}
									break;
								case GENVD_STM_EVENT_RCV_AXD:
								{								
									ushort kvVal = GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_KV);
									ushort masVal = GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_MAS);
									ushort maVal = GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_MA);
									ushort msVal = GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_XRAY_MS);
									IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"AXD Val KV %d mas %d ma %d ms %d in state %d",kvVal,masVal,
										maVal,msVal,m_State);
								}
									break;				
									
								case GENVD_STM_EVENT_START:
								{
									//Send IFV to generator
									TxCOMMapping* TxCOMM = GetCOMMTx(GENVD_STM_EVENT_START);
									if(NULL!=TxCOMM)
									{
										com_send(TxCOMM->TxCommand,strlen(TxCOMM->TxCommand));
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send out CMD %s in state %d",TxCOMM->TxCommand,m_State);
									}
									else
									{
										IDC::Log(IDC::LOG_WARNING,LOG_TAG,"CAN't Get COMMAND as TYPE %d",EventHeader->Event);
									}	
								}
									break;

								case GENVD_STM_EVENT_RCV_IFV:
									if(m_State==GEN_STATE_IDLE)
									{
										//delete alarm
										if(m_alarmTid!=0)
										{
											delete_alarm(m_alarmTid);
											m_alarmTid = 0;
										}
										m_State = GEN_STATE_BOOTUP;	
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"STATE Transit to %d as Event %d",m_State,EventHeader->Event);
										//EnterintoStateM = true; no need STATE Transit
									}
									else
									{
										IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Unexpect Flow RCV START in STATEn %d",m_State);
									}
									break;
									
								case GENVD_STM_EVENT_RCV_SWV:
								case GENVD_STM_EVENT_RCV_INI:
									if(m_State==GEN_STATE_BOOTUP)
									{
										EnterintoStateM = true;
									}
									break;

								case GENVD_STM_EVENT_RCV_SBY:
									IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"standy config");
									if((m_State==GEN_STATE_BOOTUP)||(m_State==GEN_STATE_EXP))
									{
										m_State = GEN_STATE_READY;
										//Enter into ReadyState, Change the component State and Report to System
										CGenXtpIf::Sigleton()->onSendICompStatus(CompStatusOperational);
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"STATE Transit to %d as Event %d",m_State,EventHeader->Event);
									}
									break;

								case GENVD_STM_EVENT_RCV_RDY:
									if(m_State==GEN_STATE_READY)
									{
										EnterintoStateM = true;//Reserved for Statemachine Handle
									}
									else if(m_State==GEN_STATE_BOOTUP)//Handle it
									{
										//TODO: If the Ready Status is not READY, then change to IDLE then rebootup GEN
										EnterintoStateM = true;
									}
									break;

								case GENVD_STM_EVENT_RCV_EXP://event make State Transfer: READY->EXP
									if(m_State==GEN_STATE_READY)
									{
										m_State = GEN_STATE_EXP;
										CGenXtpIf::Sigleton()->onSendIGenReadyData();
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"STATE Transit to %d as Event %d",m_State,EventHeader->Event);
									}
									break;

								case GENVD_STM_EVENT_RCV_XSTATUS:
								{
									GenRxCmdMapping* RxCmdMapping = CheckCmdTable("XON");
									if(RxCmdMapping!=NULL)
									{
										if(RxCmdMapping->eventParamCnt==EventHeader->extParamCnt)
										{
											GenVDParamPacket* ParamPacket = (GenVDParamPacket*)(EventHeader+1);

											if(ParamPacket[0].type==GENVD_STM_EVENT_PARAM_XOFF)
											{
												if(m_State==GEN_STATE_READY)//"XOFF" after Stop X-RAY is Expected WorkFlow.
												{
													//Todo.......
													//XTP TELEGRAM to XMaster
												}
												else if(m_State==GEN_STATE_EXP )//''XOFF" make STATE TRANSIT
												{													
													//m_State = GEN_STATE_READY;
													//Todo.......
													//XTP TELEGRAM to XMaster
												}
											}
											else if(ParamPacket[0].type==GENVD_STM_EVENT_PARAM_XON)
											{
												if(m_State==GEN_STATE_EXP )
												{
													//Todo.......
													//''XON" will Make XTP Telegram to XMaster
												}
												else//any other State 
												{
													//Serieous Error, Abort XRAY to Generator
													// may be unrecoverable or recoverable. Shall report to system
												}
											}
										}
										else
										{
											IDC::Log(IDC::LOG_ERR,LOG_TAG,"Unmatch Cnt Number %d - %d",RxCmdMapping->eventParamCnt,EventHeader->extParamCnt);
										}
									}
									else
									{
										IDC::Log(IDC::LOG_ERR,LOG_TAG,">>>Unexpect Error after receive Int Tel");
									}
								}
									break;

								case GENVD_STM_EVENT_TX_SFP://comes from SGENDATA, produce SFP to Generator
									if(m_State==GEN_STATE_READY)
									{
										bool skipSFPSetting = false;
										char VFPCmd[80];
										memset(VFPCmd,0,sizeof(VFPCmd));
										strncpy(VFPCmd,"<SFP",strlen("<SFP"));
										if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_MODE,VFPCmd))
										{
											if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_KV,VFPCmd))
											{
												if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_MA,VFPCmd))
												{
													if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_MAS,VFPCmd))
													{
														if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_TMAX,VFPCmd))
														{
															if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_FOCUS,VFPCmd))
															{
																if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_FPS,VFPCmd))
																{
																	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Set SFP %s",VFPCmd);
																	com_send(VFPCmd,strlen(VFPCmd));
																	printf("SFP Send\n");
																	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"SFP Send");
																}
															}
														}
													}
												}
											}
										}									
									}
									else if(m_State==GEN_STATE_EXP)
									{
										bool skipSFPSetting = false;
										char SFDCmd[80];
										memset(SFDCmd,0,sizeof(SFDCmd));
										strncpy(SFDCmd,"<SFD",strlen("<SFD"));
										if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_KV,SFDCmd))
										{
											if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_MA,SFDCmd))
											{
												if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_MAS,SFDCmd))
												{
													if(SetStrTx(EventHeader,GENVD_STM_EVENT_PARAM_TMAX,SFDCmd))
													{
														IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Set SFD %s",SFDCmd);
														com_send(SFDCmd,strlen(SFDCmd));
													}
												}
											}
										}
									}
							
									break;

								case GENVD_STM_EVENT_TX_SFD://new Exp Parameter Setting, produce SFD to Generator
								//TODO, Regulation or change EXP Value in ACQ 
								if(m_State==GEN_STATE_EXP)
									{
									}
									break;

								case GENVD_STM_EVENT_RCV_VFP://comes from VFP, 
									if(m_State==GEN_STATE_READY)//produce IGENDATA to XMaster
									{
										GenVDParamPacket* ParamPacket = (GenVDParamPacket*)(EventHeader+1);
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Get VFP Para Cnt %d",EventHeader->extParamCnt);
										for(int i=0; i<EventHeader->extParamCnt; i++)
										{
											IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"VFP PARA, State TYPE %d VAL %s",ParamPacket[i].type,ParamPacket[i].param);
										}
										char* mode = GetStrVal(EventHeader,GENVD_STM_EVENT_PARAM_MODE);
										if(mode!=NULL)
										{
											if(strncmp(mode,"C",1)==0)
											{
												m_GenData.FlMode = FlContinue;
											}
											else{
												if(strncmp(mode,"P",1)==0)
												{
													m_GenData.FlMode = FlPulse;
												}
												else
												{
													if(strncmp(mode,"S",1)==0)
													m_GenData.FlMode = FlSingle;
												}
											} 
										}

										char* focusVal = GetStrVal(EventHeader,GENVD_STM_EVENT_PARAM_FOCUS);
										if(focusVal!=NULL)
										{											
												if(strncmp(mode,"S",1)==0)
												{
													m_GenData.focus = 1;
												}
												else if(strncmp(mode,"B",1)==0)
												{
													m_GenData.focus = 2;
												}											
										}
										
										m_GenData.kv=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_KV);
										m_GenData.ma=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_MA);
										m_GenData.mas=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_MAS);
										m_GenData.Tmax=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_TMAX);
										ushort fps = GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_FPS);
										if(m_GenData.FlMode == FlContinue)
										{
											m_GenData.Fps=0;
										}
										else{
											m_GenData.Fps =fps;
										}
										//m_GenData.Fps = GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_FPS);
										CGenXtpIf::Sigleton()->onSendiGENData(&m_GenData);
									}
									/*else if(m_State==GEN_STATE_EXP)//comes from VFD. No next Step Handle.
									{
									}*/
									break;

								case GENVD_STM_EVENT_RCV_VFD:
									{
										GenVDParamPacket* ParamPacket = (GenVDParamPacket*)(EventHeader+1);
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Get VFD Para Cnt %d",EventHeader->extParamCnt);
										for(int i=0; i<EventHeader->extParamCnt; i++)
										{
											IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"VFD PARA, State TYPE %d VAL %s",ParamPacket[i].type,ParamPacket[i].param);
										}									
										
										m_GenData.kv=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_KV);
										m_GenData.ma=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_MA);
										m_GenData.mas=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_MAS);
										m_GenData.Tmax=GetUSVal(EventHeader,GENVD_STM_EVENT_PARAM_TMAX);
										
										CGenXtpIf::Sigleton()->onSendiGENData(&m_GenData);
									}
								
									break;

								case GENVD_STM_EVENT_RCV_START_EXP://comes from SFLCMD(ACTIVE)
									if(m_State==GEN_STATE_READY)//Send SFL(1) to GEN
									{
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Start FL @ State %d",m_State);
										TxCOMMapping* TxCOMM = GetCOMMTx(GENVD_STM_EVENT_RCV_START_EXP);
										if(TxCOMM!=NULL)
										{
											com_send(TxCOMM->TxCommand,strlen(TxCOMM->TxCommand));
											IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send out CMD %s in state %d",TxCOMM->TxCommand,m_State);
										}
										else
										{
											IDC::Log(IDC::LOG_WARNING,LOG_TAG,"CAN't Get COMMAND as TYPE %d",EventHeader->Event);
										}					
									}
									break;

								case GENVD_STM_EVENT_RCV_STOP_EXP://comes from SFLCMD(STOP)
								{
									TxCOMMapping* TxCOMM = GetCOMMTx(GENVD_STM_EVENT_RCV_STOP_EXP);
										if(TxCOMM!=NULL)
										{
											com_send(TxCOMM->TxCommand,strlen(TxCOMM->TxCommand));
											IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send out CMD %s in state %d",TxCOMM->TxCommand,m_State);
										}
										else
										{
											IDC::Log(IDC::LOG_WARNING,LOG_TAG,"CAN't Get COMMAND as TYPE %d",EventHeader->Event);
										}	
									/*if(m_State==GEN_STATE_EXP)//STATE Transit
									{
										//Send SFL(0) to GEN
										m_State = GEN_STATE_READY;
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"STATE Transit to %d as Event %d",m_State,EventHeader->Event);
									}*/
								}
									break;								
									
								default:
									break;
							}

							if(EnterintoStateM)
							{
								StateMachine(EventHeader);
							}
						}
						else
						{
							IDC::Log(IDC::LOG_WARNING,LOG_TAG,"INVALID Len %d, Item Cnt %d, calc Len %d",p->len,EventHeader->extParamCnt,
								EventHeader->extParamCnt*sizeof(GenVDParamPacket)+sizeof(GenVDEventFrameHeader)+sizeof(ITCObjHeader));
						}
					}
				}
					break;
				default:
				break;
			}
		
        }
    }
}

void CGenStateM::StateMachine(const GenVDEventFrameHeader* data)
{
	if(data!=NULL)
	{
		while(m_State!=GEN_STATE_IDLE)
		{
			if(m_State==GEN_STATE_BOOTUP)
			{
				if(data->Event==GENVD_STM_EVENT_RCV_SWV)
				{
					//TODO.. Save the Version and report to XMaster
				}
				else if(data->Event==GENVD_STM_EVENT_RCV_INI)
				{
					//Send out SINIT
					TxCOMMapping* TxCOMM = GetCOMMTx(GENVD_STM_EVENT_RCV_INI);
					if(NULL!=TxCOMM)
					{
						com_send(TxCOMM->TxCommand,strlen(TxCOMM->TxCommand));
						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send out CMD %s in state %d",TxCOMM->TxCommand,m_State);
					}
					else
					{
						IDC::Log(IDC::LOG_WARNING,LOG_TAG,"CAN't Get COMMAND as TYPE %d",data->Event);
					}					
				}
				else if(data->Event==GENVD_STM_EVENT_RCV_RDY)
				{
					uchar GenReady = GetUSVal(data,GENVD_STM_EVENT_PARAM_RDYSTATUS);
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"READY STATUS %d",GenReady);
					if(GenReady==0)
					{
						m_State = GEN_STATE_READY;
						IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"State tran to GEN_STATE_READY %d",GenReady);
					}
				}
				break;
			}
			else if(m_State==GEN_STATE_READY)
			{
				if(data->Event==GENVD_STM_EVENT_RCV_RDY)
				{
					//TODO.. parse GEN READY INFO
					GenVDParamPacket* p = (GenVDParamPacket*)(data+1);
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Get READY STATUS From GEN Total %d, State TYPE %d %s",
						data->extParamCnt,p[0].type,p[0].param);
				}
				break;
			}
			else if(m_State==GEN_STATE_EXP)
			{
				break;
			}
			else if(m_State==GEN_STATE_ERR)
			{
				break;
			}
			else
			{
				break;
			}
		}	
	}
}




