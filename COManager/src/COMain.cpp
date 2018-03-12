#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "canfestival.h"
#include "pdo.h"
#include "MasterOD.h"
#include "COMain.h"
#include "Master.h"
#include "COComm.h"
#include "COApp.h"

static CCoApp* coApp;

CO_APP_Map Co_App_Map[] =
{
	{IDC_TASK_COL_MASTER,NULL},
	{IDC_TASK_DAP_MASTER,NULL}
};

//change follow with above List
uchar CO_APP_COL_IDX = 0;
uchar CO_APP_DAP_IDX = 1;

CO_APP_Map* GetCOAppLink(const uchar TaskID)
{
	CO_APP_Map* rc = NULL;
	for(uchar i=0; i<CO_APP_MAP_SIZE; i++)
	{
		if(Co_App_Map[i].TaskID==TaskID)
		{
			rc = &Co_App_Map[i];
		}
	}
	return rc;
}

void SendPDO(const      uchar PdoIdx, const ushort COBID, const uchar len, const uchar data[] )
{
	
	Message pdo;
	pdo.cob_id = COBID;
	pdo.len = len<=8?len:8;
	pdo.rtr = 0;
	memcpy(pdo.data, data, pdo.len);
	sendPdo(&COMaster_Data,PdoIdx,&pdo);
}


UNS32 Index1006_callbacks(CO_Data* d, const indextable * idx, UNS8 bSubindex)
{
	//eprintf("TestMaster_Index1006_callbacks 0x%x %d %xH \n",idx->index,bSubindex,TestMaster_obj1006);
	Message pdo;
	pdo.cob_id = 0x202;
	pdo.len = 4;
	pdo.rtr = 0;
	pdo.data[0] = 0x34;
	sendPdo(d,0,&pdo);
	return 0;
}

UNS32 OnObj6030_02_StatusChange(CO_Data* d, const indextable * idx, UNS8 bSubIdx)
{
	eprintf("OnObj6030_02_StatusChange bSubIdx %d\n", bSubIdx);
	CO_APP_COMM_Frame toClientMsg;		

	toClientMsg.header.tidSrc = 0;
	toClientMsg.header.tidTarget = IDC_TASK_COL_MASTER;
	toClientMsg.ipcData.header._Len = 2;
	toClientMsg.header.len = sizeof(ITCObjHeader)+sizeof(CO_APP_COMM_Header)+toClientMsg.ipcData.header._Len;
	toClientMsg.header.ITCObjectID = INT_TEL_COL_MASTER_RCV;
	if(Co_App_Map[CO_APP_COL_IDX].APPMsgQ!=NULL)
	{
		if(IDC_OK!=Co_App_Map[CO_APP_COL_IDX].APPMsgQ->send((char*)&toClientMsg,toClientMsg.header.len))
		{
			eprintf("###Error FAIL OnObj6030_02_StatusChange bSubIdx %d\n", bSubIdx);
		}
	}
	else
	{
	}

	return 0;
}

s_BOARD MasterBoard = {"0", "125K"};

void catch_signal(int sig)
{
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
	eprintf("Got Signal %d\n",sig);
}

void help(void)
{
	printf("**************************************************************\n");
	printf("*  COManager                                                 *\n");
	printf("*                                                            *\n");
	printf("*                                                            *\n");
	printf("*   Usage:                                                   *\n");
	printf("*   ./COManager        [OPTIONS]                             *\n");
	printf("*                                                            *\n");
	printf("*   OPTIONS:                                                 *\n");
	printf("*     -l : Can library [\"libcanfestival_can_virtual.so\"]   *\n");
	printf("*                                                            *\n");
	printf("*    Master:                                                 *\n");
	printf("*     -m : bus name [\"1\"]                                  *\n");
	printf("*     -M : 1M,500K,250K,125K,100K,50K,20K,10K,none(disable)  *\n");
	printf("*                                                            *\n");
	printf("**************************************************************\n");
}

/***************************  INIT  *****************************************/
void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION MASTER *******************************/
	if(strcmp(MasterBoard.baudrate, "none")){
		RegisterSetODentryCallBack(&COMaster_Data, 0x6030, 2, &OnObj6030_02_StatusChange);
		RegisterSetODentryCallBack(&COMaster_Data, 0x1006, 0, &Index1006_callbacks);

		/* Defining the node Id */
		//setNodeId(&TestMaster_Data, 0x01);

		/* init */
		setState(&COMaster_Data, Initialisation);

	}
}

/***************************  EXIT  *****************************************/
void Exit(CO_Data* d, UNS32 id)
{
	if(strcmp(MasterBoard.baudrate, "none")){

		masterSendNMTstateChange(&COMaster_Data, 0x02, NMT_Reset_Node);    

		//Stop master
		setState(&COMaster_Data, Stopped);
	}
}

/****************************************************************************/
/***************************  MAIN  *****************************************/
/****************************************************************************/
int main(int argc,char **argv)
{

	int c;
	extern char *optarg;
	bool ifStartTimerLoop = false;
	char* LibraryPath="../../drivers/can_virtual/libcanfestival_can_virtual.so";

	while ((c = getopt(argc, argv, "-m:s:M:S:l:")) != EOF)
	{
		switch(c)
		{
		case 'm' :
			if (optarg[0] == 0)
			{
				help();
				exit(1);
			}
			MasterBoard.busname = optarg;
			break;
		case 'M' :
			if (optarg[0] == 0)
			{
				help();
				exit(1);
			}
			MasterBoard.baudrate = optarg;
			break;
		case 'l' :
			if (optarg[0] == 0)
			{
				help();
				exit(1);
			}
			LibraryPath = optarg;
			break;
		default:
			help();
			exit(1);
		}
	}

	/* install signal handler for manual break */
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
	// Moving this line into the ifndef leads to a lethal crash!
	TimerInit();

#ifndef NOT_USE_DYNAMIC_LOADING
	if (LoadCanDriver(LibraryPath) == NULL)
		printf("Unable to load library: %s\n",LibraryPath);
#endif		

	coApp = new CCoApp();
	coApp->Init();
	// Open CAN devices
	if(strcmp(MasterBoard.baudrate, "none")){
#if 0
		COMaster_Data.heartbeatError = COMaster_heartbeatError;
		COMaster_Data.initialisation = COMaster_initialisation;
		COMaster_Data.preOperational = COMaster_preOperational;
		COMaster_Data.operational = COMaster_operational;
		COMaster_Data.stopped = COMaster_stopped;
		COMaster_Data.post_sync = COMaster_post_sync;
		COMaster_Data.post_TPDO = COMaster_post_TPDO;
		COMaster_Data.post_emcy = COMaster_post_emcy;
		COMaster_Data.post_SlaveBootup=COMaster_post_SlaveBootup;

		COMaster_Data.nodeguardError = COMaster_NodeGuardError;
#endif

		if(!canOpen(&MasterBoard,&COMaster_Data))
		{
			eprintf("Cannot open Master Board (%s,%s)\n",MasterBoard.busname, MasterBoard.baudrate);
			goto fail_master;
		}
	}

	// Start timer thread
	StartTimerLoop(&InitNodes);

	// wait Ctrl-C
	pause();

	// Stop timer thread
	StopTimerLoop(&Exit);

	// Close CAN devices (and can threads)
	if(strcmp(MasterBoard.baudrate, "none")) canClose(&COMaster_Data);	
fail_master:
	if(strcmp(MasterBoard.baudrate, "none")) canClose(&COMaster_Data);
	TimerCleanup();
	return 0;
}
