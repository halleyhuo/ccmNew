#ifndef GEN_MAIN_H
#define GEN_MAIN_H
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <signal.h>
//#include "GenMaster.h"
#include "GenXtp.h"

#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCBase/inc/IDCMsgQueue.h"

#include "IDCLog/inc/LogTrace.h"

#define msleep(n) usleep(n*1000)
using namespace std;
int initComm( );

int str2intCheck(const char* str, int len);
ushort str2int(const char* str, int len);
int str2ushort(const char* str, int len,ushort* revalue);
#define GEN_CMD_HOSTSEND	0x55//节点发送控制发生器
#define GEN_CMD_HOSTRECV	0xcc//节点从发生器接收



//extern GenRxCmdMapping GenRxCmdMappingTable[32];

class CGenLogical : public CIDCThread
{
    public:
        CGenLogical();
        virtual ~CGenLogical();

        virtual void run();

		iFlData FlData;
        iGENData iGenData;
        IGenReady iGenRadeyData;
        SFlCmd sfldata;
		static CGenLogical* m_instanse;
	    static CGenLogical* Sigleton(CGenLogical* instanse = NULL);

};


#endif




