
#include "MasterOD.h"
#include "COComm.h"

/**************************************************************************/
/* Declaration of the mapped variables                                    */
/**************************************************************************/


/**************************************************************************/
/* Declaration of the value range types                                   */
/**************************************************************************/

#define valueRange_EMC 0x9F /* Type for index 0x1003 subindex 0x00 (only set of value 0 is possible) */
UNS32 COMaster_valueRangeTest (UNS8 typeValue, void * value)
{
	switch (typeValue) {
	case valueRange_EMC:
		if (*(UNS8*)value != (UNS8)0) return OD_VALUE_RANGE_EXCEEDED;
		break;
	}
	return 0;
}

/**************************************************************************/
/* The node id                                                            */
/**************************************************************************/
/* node_id default value.*/
UNS8 COMaster_bDeviceNodeId = 0x00;

/**************************************************************************/
/* Array of message processing information */

const UNS8 COMaster_iam_a_slave = 0;

TIMER_HANDLE COMaster_heartBeatTimers[1] = {TIMER_NONE,};

/*
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

OBJECT DICTIONARY

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
*/

/* index 0x1000 :   Device Type. */
                    UNS32 COMaster_obj1000 = 0x12D;	/* 301 */
                    subindex COMaster_Index1000[] = 
                     {
                       { RO, uint32, sizeof (UNS32), (void*)&COMaster_obj1000 }
                     };

/* index 0x1001 :   Error Register. */
                    UNS8 COMaster_obj1001 = 0x0;	/* 0 */
                    subindex COMaster_Index1001[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&COMaster_obj1001 }
                     };

/* index 0x1003 :   Pre-defined Error Field */
                    UNS8 COMaster_highestSubIndex_obj1003 = 0; /* number of subindex - 1*/
                    UNS32 COMaster_obj1003[] = 
                    {
                      0x0	/* 0 */
                    };
                    ODCallback_t COMaster_Index1003_callbacks[] = 
                     {
                       NULL,
                       NULL,
                     };
                    subindex COMaster_Index1003[] = 
                     {
                       { RW, valueRange_EMC, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1003 },
                       { RO, uint32, sizeof (UNS32), (void*)&COMaster_obj1003[0] }
                     };
/* index 0x1005 :   SYNC COB ID. */
/* b29 Identifier format. This bit is clear for standard (11-bit) identifiers, and set for extended (29-bit) identifiers.*/
UNS32 COMaster_obj1005 = 0x00000080;	/* 1073741952 */
ODCallback_t COMaster_Index1005_callbacks[] = 
{
	NULL,
};
subindex COMaster_Index1005[] = 
{
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1005 }
};

/* index 0x1006 :   Communication / Cycle Period. */
UNS32 COMaster_obj1006 = 0xC350;	/* 50000 */



subindex COMaster_Index1006[] = 
{
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1006 }
};
ODCallback_t COMaster_Index1006_callbacks[] = 
{
	NULL,
};



/* index 0x100C: NodeGuarding time */
UNS16 COMaster_obj100C = 500; 

/* index 0x100D: NodeGuarding time factor*/
UNS8 COMaster_obj100D = 5; 


/* index 0x1014 :   Emergency COB ID. */
UNS32 COMaster_obj1014 = 0x80;	/* 128 */
subindex COMaster_Index1014[] = 
{
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1014 }
};

/* index 0x1016 :   Consumer Heartbeat Time. */
/* necessary by CO Master */
UNS8 COMaster_highestSubIndex_obj1016 = 0; /* number of subindex - 1*/
UNS32 COMaster_obj1016[] = 
{
	0x205DC	/* 132572 */
};
subindex COMaster_Index1016[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1016 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1016[0] }
};

/* index 0x1017 :   Producer Heartbeat Time */ 
UNS16 COMaster_obj1017 = 0x0;   /* 0 */

/* index 0x1018 :   Identity. */
UNS8 COMaster_highestSubIndex_obj1018 = 4; /* number of subindex - 1*/
UNS32 COMaster_obj1018_Vendor_ID = 0x0;	/* 0 */
UNS32 COMaster_obj1018_Product_Code = 0x0;	/* 0 */
UNS32 COMaster_obj1018_Revision_Number = 0x0;	/* 0 */
UNS32 COMaster_obj1018_Serial_Number = 0x0;	/* 0 */
subindex COMaster_Index1018[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1018 },
	{ RO, uint32, sizeof (UNS32), (void*)&COMaster_obj1018_Vendor_ID },
	{ RO, uint32, sizeof (UNS32), (void*)&COMaster_obj1018_Product_Code },
	{ RO, uint32, sizeof (UNS32), (void*)&COMaster_obj1018_Revision_Number },
	{ RO, uint32, sizeof (UNS32), (void*)&COMaster_obj1018_Serial_Number }
};

/* index 0x1280 :   Client SDO 1 Parameter. */
UNS8 COMaster_highestSubIndex_obj1280 = 3; /* number of subindex - 1*/
UNS32 COMaster_obj1280_COB_ID_Client_to_Server_Transmit_SDO = 0x600+NODE_COLLIMATOR;	/* 1538 */
UNS32 COMaster_obj1280_COB_ID_Server_to_Client_Receive_SDO = 0x580+NODE_COLLIMATOR;	/* 1410 */
UNS8 COMaster_obj1280_Node_ID_of_the_SDO_Server = NODE_COLLIMATOR;	/* 2 */
subindex COMaster_Index1280[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1280 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1280_COB_ID_Client_to_Server_Transmit_SDO },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1280_COB_ID_Server_to_Client_Receive_SDO },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1280_Node_ID_of_the_SDO_Server }
};

/* index 0x1281 :   Client SDO 1 Parameter. */
UNS8 COMaster_highestSubIndex_obj1281 = 3; /* number of subindex - 1*/
UNS32 COMaster_obj1281_COB_ID_Client_to_Server_Transmit_SDO = 0x602;	/* 1538 */
UNS32 COMaster_obj1281_COB_ID_Server_to_Client_Receive_SDO = 0x582;	/* 1410 */
UNS8 COMaster_obj1281_Node_ID_of_the_SDO_Server = 0x2;	/* 2 */
subindex COMaster_Index1281[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1281 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1281_COB_ID_Client_to_Server_Transmit_SDO },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1281_COB_ID_Server_to_Client_Receive_SDO },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1281_Node_ID_of_the_SDO_Server }
};



/* index 0x1400 :   Receive PDO 1 Parameter. */
UNS8 COMaster_highestSubIndex_obj1400 = 5; /* number of subindex - 1*/
UNS32 COMaster_obj1400_COB_ID_used_by_PDO = 0x200;	/* 512 */
UNS8 COMaster_obj1400_Transmission_Type = 0xFE;	/* 1 */
UNS16 COMaster_obj1400_Inhibit_Time = 0x0;	/* 0 */
UNS8 COMaster_obj1400_Compatibility_Entry = 0x0;	/* 0 */
UNS16 COMaster_obj1400_Event_Timer = 0x0;	/* 0 */
subindex COMaster_Index1400[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1400 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1400_COB_ID_used_by_PDO },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1400_Transmission_Type },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1400_Inhibit_Time },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1400_Compatibility_Entry },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1400_Event_Timer }
};

/* index 0x1401 :   Receive PDO 2 Parameter. */
UNS8 COMaster_highestSubIndex_obj1401 = 5; /* number of subindex - 1*/
UNS32 COMaster_obj1401_COB_ID_used_by_PDO = 0x300;	/* 768 */
UNS8 COMaster_obj1401_Transmission_Type = 0x0;	/* 0 */
UNS16 COMaster_obj1401_Inhibit_Time = 0x0;	/* 0 */
UNS8 COMaster_obj1401_Compatibility_Entry = 0x0;	/* 0 */
UNS16 COMaster_obj1401_Event_Timer = 0x0;	/* 0 */
subindex COMaster_Index1401[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1401 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1401_COB_ID_used_by_PDO },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1401_Transmission_Type },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1401_Inhibit_Time },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1401_Compatibility_Entry },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1401_Event_Timer }
};

/* index 0x1402 :   Receive PDO 3 Parameter. */
UNS8 COMaster_highestSubIndex_obj1402 = 5; /* number of subindex - 1*/
UNS32 COMaster_obj1402_COB_ID_used_by_PDO = 0x401;	/* 1025 */
UNS8 COMaster_obj1402_Transmission_Type = 0x0;	/* 0 */
UNS16 COMaster_obj1402_Inhibit_Time = 0x0;	/* 0 */
UNS8 COMaster_obj1402_Compatibility_Entry = 0x0;	/* 0 */
UNS16 COMaster_obj1402_Event_Timer = 0x0;	/* 0 */
subindex COMaster_Index1402[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1402 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1402_COB_ID_used_by_PDO },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1402_Transmission_Type },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1402_Inhibit_Time },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1402_Compatibility_Entry },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1402_Event_Timer }
};

/* index 0x1403 :   Receive PDO 4 Parameter. */
UNS8 COMaster_highestSubIndex_obj1403 = 5; /* number of subindex - 1*/
UNS32 COMaster_obj1403_COB_ID_used_by_PDO = 0x482;	/* 1154 */
UNS8 COMaster_obj1403_Transmission_Type = 0x0;	/* 0 */
UNS16 COMaster_obj1403_Inhibit_Time = 0x0;	/* 0 */
UNS8 COMaster_obj1403_Compatibility_Entry = 0x0;	/* 0 */
UNS16 COMaster_obj1403_Event_Timer = 0x0;	/* 0 */
subindex COMaster_Index1403[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1403 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1403_COB_ID_used_by_PDO },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1403_Transmission_Type },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1403_Inhibit_Time },
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_obj1403_Compatibility_Entry },
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_obj1403_Event_Timer }
};

/* index 0x1600 :   Receive PDO 1 Mapping. */
UNS8 COMaster_highestSubIndex_obj1600 = 2; /* number of subindex - 1*/
UNS32 COMaster_obj1600[] = 
{
 	0x60300208,	 //controlstatus (circular collimation set)
	0x60300310,  //actualpos_d (circular collimation set)
};
subindex COMaster_Index1600[] = 
{
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1600 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1600[0] },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1600[1] },
};

/* index 0x1601 :   Receive PDO 2 Mapping. */
UNS8 COMaster_highestSubIndex_obj1601 = 1; /* number of subindex - 1*/
UNS32 COMaster_obj1601[] = 
{
	0x200A0010	/* 537526288 */
};
subindex COMaster_Index1601[] = 
{
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1601 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1601[0] }
};

/* index 0x1602 :   Receive PDO 3 Mapping. */
UNS8 COMaster_highestSubIndex_obj1602 = 1; /* number of subindex - 1*/
UNS32 COMaster_obj1602[] = 
{
	0x200B0010	/* 537591824 */
};
subindex COMaster_Index1602[] = 
{
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1602 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1602[0] }
};

/* index 0x1603 :   Receive PDO 4 Mapping. */
UNS8 COMaster_highestSubIndex_obj1603 = 1; /* number of subindex - 1*/
UNS32 COMaster_obj1603[] = 
{
	0x200C0010	/* 537657360 */
};
subindex COMaster_Index1603[] = 
{
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj1603 },
	{ RW, uint32, sizeof (UNS32), (void*)&COMaster_obj1603[0] }
};

/* index 0x1800 :   Transmit PDO 1 Parameter. */                    
UNS8 ObjDict_highestSubIndex_obj1800 = 5; /* number of subindex - 1*/                    
UNS32 ObjDict_obj1800_COB_ID_used_by_PDO = 0x202;	/* 514 */                    
UNS8 ObjDict_obj1800_Transmission_Type = 0xFE;	/* 254 */                    
UNS16 ObjDict_obj1800_Inhibit_Time = 0x0;	/* 0 */                    
UNS8 ObjDict_obj1800_Compatibility_Entry = 0x0;	/* 0 */                    
UNS16 ObjDict_obj1800_Event_Timer = 0x0;	/* 0 */                    
ODCallback_t ObjDict_Index1800_callbacks[] =                      
{   
	NULL,
	NULL,                       
	NULL,                       
	NULL,                      
	NULL,     
	NULL,
};                    
subindex ObjDict_Index1800[] =                      
{                       
	{ RO, uint8, sizeof (UNS8), (void*)&ObjDict_highestSubIndex_obj1800 },                       
	{ RW, uint32, sizeof (UNS32), (void*)&ObjDict_obj1800_COB_ID_used_by_PDO },
	{ RW, uint8, sizeof (UNS8), (void*)&ObjDict_obj1800_Transmission_Type },                       
	{ RW, uint16, sizeof (UNS16), (void*)&ObjDict_obj1800_Inhibit_Time },                      
	{ RW, uint8, sizeof (UNS8), (void*)&ObjDict_obj1800_Compatibility_Entry },                     
	{ RW, uint16, sizeof (UNS16), (void*)&ObjDict_obj1800_Event_Timer }                     
};

/* index 0x1A00 :   Transmit PDO 1 Mapping. */                    
UNS8 ObjDict_highestSubIndex_obj1A00 = 1; /* number of subindex - 1*/                    
UNS32 ObjDict_obj1A00[] =                     
{                      
	0x21000008 /* 553648136 */                    
};                    
subindex ObjDict_Index1A00[] =                      
{                       
	{ RW, uint8, sizeof (UNS8), (void*)&ObjDict_highestSubIndex_obj1A00 },
	{ RW, uint32, sizeof (UNS32), (void*)&ObjDict_obj1A00[0] } 
};

/* SID (general), mapping index = 0x6000, subindex= 0x00, length = 0x10, SID */

UNS16 COMaster_SIDCommand = 850;	/* unit is 0.1mm */

subindex COMaster_Index6000[] = 
{
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_SIDCommand }
};
ODCallback_t COMaster_Index6000_callbacks[] = 
{
	NULL,
};

/*SFD (general), mapping index = 0x6010, subindex= 0x00, length = 0x10, SFD, between focus and edge of collimator */
UNS16 COMaster_SFDCommand = 230;	/* unit is 0.1mm */

subindex COMaster_Index6010[] = 
{
	{ RW, uint16, sizeof (UNS16), (void*)&COMaster_SFDCommand }
};
ODCallback_t COMaster_Index6010_callbacks[] = 
{
	NULL,
};

/* index 0x6030 */
UNS8 COMaster_highestSubIndex_obj6030 = 3; /* number of subindex - 1*/
typedef unsigned char ColControlCmdEnum;
const ColControlCmdEnum ColControlCmd_NOOP = 0;
const ColControlCmdEnum ColControlCmd_LOCK = 1;
const ColControlCmdEnum ColControlCmd_UNLOCK = 2;
const ColControlCmdEnum ColControlCmd_STOP = 3;
const ColControlCmdEnum ColControlCmd_RFAULT = 15;

UNS8 CoMaster_6030_01_ControlCmd = ColControlCmd_NOOP;
UNS8 CoMaster_6030_02_ControlStatus = 0;
UNS16 CoMaster_6030_03_ActualPos = 0;

subindex COMaster_Index6030[] = 
{
	{ RW, uint8, sizeof (UNS8), (void*)&COMaster_highestSubIndex_obj6030 },
	{ RW, uint8, sizeof (UNS8), (void*)&CoMaster_6030_01_ControlCmd },
	{ RO, uint8, sizeof (UNS8), (void*)&CoMaster_6030_02_ControlStatus },
	{ RO, uint16,sizeof (UNS16), (void*)&CoMaster_6030_03_ActualPos },
};

ODCallback_t COMaster_Index6030_callbacks[] = 
{
	NULL,
	NULL,
	NULL,
	NULL,
};



const indextable COMaster_objdict[] = 
{
  { (subindex*)COMaster_Index1000,sizeof(COMaster_Index1000)/sizeof(COMaster_Index1000[0]), 0x1000},
  { (subindex*)COMaster_Index1001,sizeof(COMaster_Index1001)/sizeof(COMaster_Index1001[0]), 0x1001},
	{ (subindex*)COMaster_Index1005,sizeof(COMaster_Index1005)/sizeof(COMaster_Index1005[0]), 0x1005},
	{ (subindex*)COMaster_Index1006,sizeof(COMaster_Index1006)/sizeof(COMaster_Index1006[0]), 0x1006},
	{ (subindex*)COMaster_Index1014,sizeof(COMaster_Index1014)/sizeof(COMaster_Index1014[0]), 0x1014},
	{ (subindex*)COMaster_Index1016,sizeof(COMaster_Index1016)/sizeof(COMaster_Index1016[0]), 0x1016},
	{ (subindex*)COMaster_Index1018,sizeof(COMaster_Index1018)/sizeof(COMaster_Index1018[0]), 0x1018},
	{ (subindex*)COMaster_Index1280,sizeof(COMaster_Index1280)/sizeof(COMaster_Index1280[0]), 0x1280},
	{ (subindex*)COMaster_Index1400,sizeof(COMaster_Index1400)/sizeof(COMaster_Index1400[0]), 0x1400},
	{ (subindex*)COMaster_Index1401,sizeof(COMaster_Index1401)/sizeof(COMaster_Index1401[0]), 0x1401},
	{ (subindex*)COMaster_Index1402,sizeof(COMaster_Index1402)/sizeof(COMaster_Index1402[0]), 0x1402},
	{ (subindex*)COMaster_Index1403,sizeof(COMaster_Index1403)/sizeof(COMaster_Index1403[0]), 0x1403},
	{ (subindex*)COMaster_Index1600,sizeof(COMaster_Index1600)/sizeof(COMaster_Index1600[0]), 0x1600},
	{ (subindex*)COMaster_Index1601,sizeof(COMaster_Index1601)/sizeof(COMaster_Index1601[0]), 0x1601},
	{ (subindex*)COMaster_Index1602,sizeof(COMaster_Index1602)/sizeof(COMaster_Index1602[0]), 0x1602},
	{ (subindex*)COMaster_Index1603,sizeof(COMaster_Index1603)/sizeof(COMaster_Index1603[0]), 0x1603},
	{ (subindex*)ObjDict_Index1800,sizeof(ObjDict_Index1800)/sizeof(ObjDict_Index1800[0]), 0x1800},//TxPDO1
	{ (subindex*)ObjDict_Index1A00,sizeof(ObjDict_Index1A00)/sizeof(ObjDict_Index1A00[0]), 0x1A00},
	{ (subindex*)COMaster_Index6030,sizeof(COMaster_Index6030)/sizeof(COMaster_Index6030[0]), 0x6030},
};

const indextable * COMaster_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks)
{
	int i;
	*callbacks = NULL;
	switch(wIndex){
	case 0x1000: i = 0;break;
	case 0x1001: i = 1;break;
	case 0x1005: i = 2;*callbacks = COMaster_Index1005_callbacks; break;
	case 0x1006: i = 3;*callbacks = COMaster_Index1006_callbacks; break;
	case 0x1014: i = 4;break;
	case 0x1016: i = 5;break;
	case 0x1018: i = 6;break;
	case 0x1280: i = 7;break;
	case 0x1400: i = 8;break;
	case 0x1401: i = 9;break;
	case 0x1402: i = 10;break;
	case 0x1403: i = 11;break;
	case 0x1600: i = 12;break;
	case 0x1601: i = 13;break;
	case 0x1602: i = 14;break;
	case 0x1603: i = 15;break;
	case 0x1800: i = 16;*callbacks = ObjDict_Index1800_callbacks; break;
	case 0x1A00: i = 17;break;
	case 0x6030: i = 18;*callbacks = COMaster_Index6030_callbacks;break;
	default:
		*errorCode = OD_NO_SUCH_OBJECT;
		return NULL;
	}
	*errorCode = OD_SUCCESSFUL;
	return &COMaster_objdict[i];
}

/* 
* To count at which received SYNC a PDO must be sent.
* Even if no pdoTransmit are defined, at least one entry is computed
* for compilations issues.
*/
s_PDO_status COMaster_PDO_status[1] = {s_PDO_status_Initializer};

quick_index COMaster_firstIndex = {
	0, /* SDO_SVR */
	7, /* SDO_CLT */
	8, /* PDO_RCV */
	12, /* PDO_RCV_MAP */
	16, /* PDO_TRS */
	17 /* PDO_TRS_MAP */
};

quick_index COMaster_lastIndex = {
	0, /* SDO_SVR */
	7, /* SDO_CLT */
	11, /* PDO_RCV */
	15, /* PDO_RCV_MAP */
	16, /* PDO_TRS */
	17 /* PDO_TRS_MAP */
};

UNS16 COMaster_ObjdictSize = sizeof(COMaster_objdict)/sizeof(COMaster_objdict[0]); 

CO_Data COMaster_Data = CANOPEN_NODE_DATA_INITIALIZER(COMaster);

