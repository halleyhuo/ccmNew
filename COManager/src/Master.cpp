
#include "Master.h"
#include "COComm.h"

extern s_BOARD MasterBoard;
/*****************************************************************************/
void COMaster_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	eprintf("COMaster_heartbeatError %d\n", heartbeatID);
}

void COMaster_NodeGuardError(CO_Data* d, UNS8 heartbeatID)
{
	eprintf("COMaster_NodeGuardError %xh\n", heartbeatID);
    //lifeGuardStop(d);
}

void OnCollimatorCANProcess(Message * ColMsg)
{
	
	CO_APP_COMM_Frame toClientMsg;		
	
		toClientMsg.header.tidSrc = 0;
		toClientMsg.header.tidTarget = IDC_TASK_COL_MASTER;
		toClientMsg.ipcData.header._Len = ColMsg->len>8?8:ColMsg->len;
		toClientMsg.ipcData.header.COB_ID = ColMsg->cob_id;
		memcpy(toClientMsg.ipcData.VCIdata,ColMsg->data,toClientMsg.ipcData.header._Len);
		toClientMsg.header.len = sizeof(ITCObjHeader)+sizeof(CO_APP_COMM_Header)+toClientMsg.ipcData.header._Len;
		toClientMsg.header.ITCObjectID = INT_TEL_COL_MASTER_RCV;
		if(Co_App_Map[CO_APP_COL_IDX].APPMsgQ!=NULL)
		{
			if(IDC_OK!=Co_App_Map[CO_APP_COL_IDX].APPMsgQ->send((char*)&toClientMsg,toClientMsg.header.len))
			{
				
			}
		}
		else
		{
		}
}



/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup master RPDO 1 to receive TPDO 1 from id Collimator
 *  - setup master RPDO 2 to receive TPDO 2 from id Collimator
 *  - setup master RPDO 3 to receive TPDO 1 from id DAP
 *  REMARK: Configure TXPDO or RXPDO Cob-ID can also by modify the List
 *               Table in MasterOD.cpp directly, e.g. 
 *               COMaster_obj1400_COB_ID_used_by_PDO = 0x180+SlaveID
 ********************************************************/
void COMaster_initialisation(CO_Data* d)
{
	UNS32 PDO1_COBID = CO_TXPDO1+NODE_COLLIMATOR; 
	UNS32 PDO2_COBID = CO_TXPDO2+NODE_COLLIMATOR;
    UNS32 PDO3_COBID = CO_TXPDO1+NODE_DAP;
	UNS32 size = sizeof(UNS32); 
	UNS32 SINC_cicle=0;
	UNS8 data_type = 0;
	
	eprintf("COMaster_initialisation\n");

	/*****************************************
	 * Define RPDOs to match slave ID=2 TPDOs*
	 *****************************************/
	writeLocalDict( &COMaster_Data, /*CO_Data* d*/
			0x1400, /*UNS16 index*/
			0x01, /*UNS8 subind*/ 
			&PDO1_COBID, /*void * pSourceData,*/ 
			&size, /* UNS8 * pExpectedSize*/
			RW);  /* UNS8 checkAccess */
			
	writeLocalDict( &COMaster_Data, /*CO_Data* d*/
			0x1401, /*UNS16 index*/
			0x01, /*UNS8 subind*/ 
			&PDO2_COBID, /*void * pSourceData,*/ 
			&size, /* UNS8 * pExpectedSize*/
			RW);  /* UNS8 checkAccess */

    writeLocalDict( &COMaster_Data, /*CO_Data* d*/
            0x1402, /*UNS16 index*/
            0x01, /*UNS8 subind*/ 
            &PDO3_COBID, /*void * pSourceData,*/ 
            &size, /* UNS8 * pExpectedSize*/
            RW);  /* UNS8 checkAccess */

					
}

// Step counts number of times ConfigureSlaveNode is called
static int init_step = 0;

/*Froward declaration*/
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId);

/**/
static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;	
	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
	{
		eprintf("Master : Failed in initializing slave %2.2x, step %d, AbortCode :%4.4x \n", nodeId, init_step, abortCode);
	}
    else
    {
        eprintf("config slave %2.2x, step %d finished\n", nodeId, init_step);
    }

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&COMaster_Data, nodeId, SDO_CLIENT);

	ConfigureSlaveNode(d, nodeId);
}

/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup slave TPDO 1 transmit type
 *  - setup slave TPDO 2 transmit type
 *  - switch to operational mode
 *  - send NMT to slave
 ********************************************************
 * This an example of :
 * Network Dictionary Access (SDO) with Callback 
 * Slave node state change request (NMT) 
 ********************************************************
 * This is called first by TestMaster_post_SlaveBootup
 * then it called again each time a SDO exchange is
 * finished.
 ********************************************************/
 
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId)
{
	/* Master configure heartbeat producer time at 1000 ms 
	 * for slave node-id 0x02 by DCF concise */
	 
	UNS8 Transmission_Type = 0x01;
	UNS8 res;
	eprintf("Master : ConfigureSlaveNode %2.2x\n", nodeId);

	switch(++init_step){
		case 1: /*First step : setup Slave's TPDO 1 to be transmitted on SYNC*/
			eprintf("Master : set slave %2.2x TPDO 1 transmit type\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					nodeId, /*UNS8 nodeId*/
					0x1800, /*UNS16 index*/
					0x02, /*UNS8 subindex*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Transmission_Type,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
					break;
		
		case 2:	/*Second step*/
			eprintf("Master : set slave %2.2x TPDO 2 transmit type\n", nodeId);
			writeNetworkDictCallBack (d, /*CO_Data* d*/
					nodeId, /*UNS8 nodeId*/
					0x1801, /*UNS16 index*/
					0x02, /*UNS16 index*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Transmission_Type,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
					break;
        case 3:

             readNetworkDictCallback(d,
                nodeId,
                0x1006,
                0x00,
                uint32,
                CheckSDOAndContinue,
                0);
        break;        
                
                
		case 4: 
		
		/****************************** START *******************************/
		
			/* Put the master in operational mode */
			setState(d, Operational);
		 
			/* Ask slave node to go in operational mode */
			masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
			
	}
}


void COMaster_preOperational(CO_Data* d)
{
	eprintf("COMaster_preOperational\n");
}

void COMaster_operational(CO_Data* d)
{
	eprintf("COMaster_operational\n");
}

void COMaster_stopped(CO_Data* d)
{
	eprintf("COMaster_stopped\n");
}

void COMaster_post_sync(CO_Data* d)
{
	eprintf("COMaster_post_sync\n");
}

void COMaster_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{
	eprintf("Master received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\n", nodeID, errCode, errReg);
}

char query_result = 0;
char waiting_answer = 0;

static void CheckSDO(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;	
	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
		eprintf("Master : Failed in changing Slave's transmit type AbortCode :%4.4x \n", abortCode);

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&COMaster_Data, nodeId, SDO_CLIENT);
}


static int MasterSyncCount = 0;
void COMaster_post_TPDO(CO_Data* d)
{
	eprintf("COMaster_post_TPDO MasterSyncCount = %d \n", MasterSyncCount);
	if(MasterSyncCount % 17 == 0){
		eprintf("Master : Ask RTR PDO (0x1402)\n");
		sendPDOrequest(&COMaster_Data, 0x1402 );
		sendPDOrequest(&COMaster_Data, 0x1403 );
	}
	if(MasterSyncCount % 50 == 0){
		UNS8 transmitiontype;
		eprintf("Master : Change slave's transmit type to 0xFF\n");
		transmitiontype = 0xFF;
		writeNetworkDictCallBack (&COMaster_Data, /*CO_Data* d*/
					2, /*UNS8 nodeId*/
					0x1802, /*UNS16 index*/
					0x02, /*UNS16 index*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&transmitiontype,/*void *data*/
					CheckSDO, /*SDOCallback_t Callback*/
                    0); /* use block mode */
	}   
	if(MasterSyncCount % 50 == 25){
		UNS8 transmitiontype = 0x00;
		eprintf("Master : Change slave's transmit type to 0x00\n");
		writeNetworkDictCallBack (&COMaster_Data, /*CO_Data* d*/
					2, /*UNS8 nodeId*/
					0x1802, /*UNS16 index*/
					0x02, /*UNS16 index*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&transmitiontype,/*void *data*/
					CheckSDO, /*SDOCallback_t Callback*/
                    0); /* use block mode */
	}
	MasterSyncCount++;
}

void COMaster_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
	eprintf("COMaster_post_SlaveBootup %x\n", nodeid);
	init_step = 0;
	ConfigureSlaveNode(d, nodeid);
    eprintf("COMaster_post_SlaveBootup %x Finished\n", nodeid);
}
