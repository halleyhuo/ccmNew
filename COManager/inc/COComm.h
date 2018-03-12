
#define eprintf(...) printf (__VA_ARGS__)

#include "canfestival.h"
#include "IDCBase/IDCThread.h"


#define NODE_COLLIMATOR 0x20
#define NODE_DAP        0x50

typedef unsigned short CO_COBID;
const CO_COBID CO_RXPDO1 = 0x200; //Master -> Component
const CO_COBID CO_RXPDO2 = 0x300;
const CO_COBID CO_RXPDO3 = 0x400;
const CO_COBID CO_SYNC   = 0x80;
const CO_COBID CO_TXPDO1 = 0x180; //PDO Component -> Master
const CO_COBID CO_TXPDO2 = 0x280;
const CO_COBID CO_TXPDO3 = 0x380;
const CO_COBID CO_BOOT_UP = 0x700;

typedef struct
{
	uchar TaskID;
	CIDCMsgQueue* APPMsgQ;
}CO_APP_Map;

#define CO_APP_MAP_SIZE 32
extern CO_APP_Map Co_App_Map[CO_APP_MAP_SIZE];
extern uchar CO_APP_COL_IDX;
extern uchar CO_APP_DAP_IDX;
extern CO_APP_Map* GetCOAppLink(const uchar TaskID);

extern void SendPDO(const      uchar PdoIdx, const ushort COBID, const uchar len, const uchar data[] );




