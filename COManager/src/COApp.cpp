#include "COApp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "canfestival.h"
#include "COComm.h"
#include "MasterOD.h"
namespace IDC
{
	const int debug_level_remote=LOG_NOTICE+1; 
	const int debug_level_local=0;
}

CCoApp::CCoApp():CIDCThread("COApp",IDC_TASK_CO_APP,Small,Moderate,true)	
{
}

CCoApp::~CCoApp()
{
}

void CCoApp::run()
{
	char buf[MAX_ITC_LENGTH];
	ITCObjHeader* const p = (ITCObjHeader*)buf;	

	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"<<<Client Got Semaphore");
	while(true)
	{        
		const int rc = m_msgQueue->recv(buf,MAX_ITC_LENGTH);
		if(rc>0)
		{			
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Recv Msg Src %d Target %d Len %d ObjID 0x%x",
				p->tidSrc,p->tidTarget,p->len,p->ITCObjectID);
			if(p->ITCObjectID==INT_TEL_CO_RCV)
			{
				CO_APP_COMM_Header* pdata = (CO_APP_COMM_Header*)(p+1);
				if(pdata->COB_ID==APP2CO_START)//Start Command From APP
				{
					if(p->tidSrc!=0)
					{
						CO_APP_Map* link = GetCOAppLink(p->tidSrc);
						if(link!=NULL)
						{	
							if(link->APPMsgQ!=NULL)
							{
								link->APPMsgQ->close();
								delete link->APPMsgQ;
								link->APPMsgQ = NULL;
							}
							link->APPMsgQ = new CIDCMsgQueue();
							if(link->APPMsgQ!=NULL)
							{
								char szMessageBoxName[IDC_MAX_NAME_LENGTH];
								memset(szMessageBoxName,0,sizeof(szMessageBoxName));
								if(snprintf(szMessageBoxName ,IDC_MAX_NAME_LENGTH-1,(RECV_MBX_NAME) ,p->tidSrc) > 0)
								{                       
									if(IDC_ERROR==link->APPMsgQ->open(szMessageBoxName, O_RDWR))
									{
										printf("###error, Open msgQ to APP %s Fail\n",szMessageBoxName);
										delete link->APPMsgQ;
										link->APPMsgQ = NULL;
									}
									else
									{
										IDC::Log(IDC::LOG_NOTICE,LOG_TAG,">>>Connection between COMaster and APP %d Created",p->tidSrc);
									}
								}
								else
								{
									printf("###error, Fill msgQ Name Fail\n");
									delete link->APPMsgQ;
									link->APPMsgQ = NULL;
								}
							}
							else
							{
								printf("###error, Malloc room for MsgQ Fail\n");
							}
						}
						else
						{
							printf("###warning, no support APP %d\n",p->tidSrc);
						}
					}
				}
				else
				{
					Message plainCANFrame;
					plainCANFrame.cob_id = pdata->COB_ID;
					plainCANFrame.len = pdata->_Len>8?8:pdata->_Len;
					plainCANFrame.rtr = 0;
					memcpy(plainCANFrame.data,(uchar*)(pdata+1),plainCANFrame.len);
					
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send out CAN Frame %xH(%d) %x %x %x %x",plainCANFrame.cob_id,plainCANFrame.len,plainCANFrame.data[0]
						,plainCANFrame.data[1],plainCANFrame.data[2],plainCANFrame.data[3]);
					canSend(COMaster_Data.canHandle,&plainCANFrame);
					
				}
				memset(p,0,sizeof(ITCObjHeader));
			}
			else
			{
				IDC::Log(IDC::LOG_ERR,LOG_TAG,"Recv Msg Fail %d err %d",rc, m_msgQueue->GetIDCErrno());
			}
		}
	}
}


