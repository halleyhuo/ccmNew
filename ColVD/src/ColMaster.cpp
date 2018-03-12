#include "ColMaster.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "ColLogical.h"
#include "ColXtp.h"

CColMaster::CColMaster():CIDCThread("ColMaster",IDC_TASK_ID(IDC_TASK_COL_MASTER),Small,Moderate,true)
{
	m_COMan = new CIDCMsgQueue();
	m_2CO.header.tidSrc = taskIdent;
	m_2CO.header.tidTarget = IDC_TASK_CO_APP;
	m_2CO.header.mode      = ITC_SEND_MODE_DIRECT;
	m_2CO.header.flowControl = ITC_SEND_NO_FLOW;
	m_2CO.header.ITCObjectID = INT_TEL_CO_RCV;
	
}

CColMaster::~CColMaster()
{
}

void CColMaster::run()
{	
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"INFO: COLMasterRunning");
	if(m_COMan!=NULL)
	{
		char szMessageBoxName[IDC_MAX_NAME_LENGTH];
		memset(szMessageBoxName,0,sizeof(szMessageBoxName));
		if(snprintf(szMessageBoxName ,IDC_MAX_NAME_LENGTH-1,(RECV_MBX_NAME) ,IDC_TASK_CO_APP) > 0)
		{
			while(IDC_ERROR==m_COMan->open(szMessageBoxName, O_RDWR))
			{
				printf("###error, Open msgQ to APP %s Fail\n",szMessageBoxName);
				sleep(5);//Try re-connect 5s later
			}
			if(snprintf((char*)&m_2CO.ipcData.VCIdata[0] ,IDC_MAX_NAME_LENGTH-1,(RECV_MBX_NAME) ,taskIdent) > 0)
			{			
				Send2CO(APP2CO_START,0,NULL);
			}
			else
			{
				IDC::Log(IDC::LOG_ERR,LOG_TAG,"###Error, Build command to CO Fail");
			}
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
						case INT_TEL_COL_MASTER_RCV:
						{
							CO_APP_COMM_t* pData = (CO_APP_COMM_t*)(p+1);
							switch(pData->header.COB_ID)
							{
								case 0x7F0://Blade Position
								{
									IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Recv COBID 0x%xH(%d),%04X,%04X",
										pData->header.COB_ID,pData->header._Len,*((ushort*)&pData->VCIdata[0]),
										*((ushort*)&pData->VCIdata[2]));
									for(int i=0; i<4 && i<pData->header._Len; i++)
									{
										if(i%2==0)//H
										{
											CColLogical::GetBladeStatus(i/2+1)->pos = (pData->VCIdata[i]<<8);
										}
										else      //L
										{
											CColLogical::GetBladeStatus(i/2+1)->pos |= (pData->VCIdata[i]<<0);
										}
									}
									IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Collimator Blade POS %04X,%04X",
										CColLogical::GetBladeStatus(1)->pos,CColLogical::GetBladeStatus(2)->pos);
									if(CColXtpIf::Sigleton()!=NULL)
									{
										CColXtpIf::Sigleton()->onSendIColBlade();
									}
								}
								
									break;

								case 0x7F1://Filter Status. Reserved for future use
								break;							

								case 0x7F3://Miscellaneous Collimator Data. Motor Movement, Alarm.
								break;
								default:
								IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Recv UNKNOWN COBID 0x%xH(%d)",
									pData->header.COB_ID,pData->header._Len);
									break;
							}
							//Make Telegram to COMan
							//ulong tmpData = 0x12345678;
							//Send2CO(0x220,4,(uchar*)&tmpData);
						}
						
						break;

						case INT_TEL_COL_SEND:
						{
							CO_APP_COMM_t* pData = (CO_APP_COMM_t*)(p+1);
							Send2CO(pData->header.COB_ID,pData->header._Len,pData->VCIdata);
						}
							break;
						
					default:
						IDC::Log(IDC::LOG_WARNING,LOG_TAG,"ILLEGAL TEL %04xH RCV from TaskID %d",p->ITCObjectID,p->tidSrc);
						break;
					}               
				}
				else
				{
					IDC::Log(IDC::LOG_ERR,LOG_TAG,"Recv Msg Fail %d err %d",rc, m_msgQueue->GetIDCErrno());
				}
			}
		}
	}
	else
	{
		IDC::Log(IDC::LOG_ERR,LOG_TAG,"###Error Software Inner Error");
	}

}

void CColMaster::Send2CO(const ushort COB_ID, const uchar _Len, uchar VciData[64])
{
	m_2CO.ipcData.header.COB_ID = COB_ID;
		m_2CO.ipcData.header._Len = _Len;
		if(_Len>0 && _Len<64 && VciData!=NULL)
		{
			memcpy(&m_2CO.ipcData.VCIdata[0],VciData,m_2CO.ipcData.header._Len);
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"COBID %xH(%d) %04x %04x", 
				COB_ID,_Len,*((ushort*)(&m_2CO.ipcData.VCIdata[0])),*((ushort*)(&m_2CO.ipcData.VCIdata[2])));
		}
		m_2CO.header.len = sizeof(ITCObjHeader)+sizeof(CO_APP_COMM_Header)+sizeof(m_2CO.ipcData.VCIdata[0])*m_2CO.ipcData.header._Len;
		if(IDC_OK!=m_COMan->send(&m_2CO,m_2CO.header.len))
		{
			IDC::Log(IDC::LOG_ERR,LOG_TAG,"###Error, Send START command to CO Fail");
		}		
}







