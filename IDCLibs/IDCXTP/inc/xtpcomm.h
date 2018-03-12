/*
 * xtpcomm.h
 *
 *  Created on: 2017.04.04
 */

#ifndef XTP_COMM_H
#define XTP_COMM_H

#include <sys/time.h> 
#include <sys/resource.h>
#include <errno.h>
#include <mqueue.h> 
#include <unistd.h>
#include "xtpobj.h"

#include "IDCBase/inc/IDCMsgQueue.h"

#include "IDCLog/inc/LogTrace.h"


#define MAX_PORT_SUPPORT 2
#define DEFAULT_PORT_SUPPORT 0

#define xtpServerPath "/xtpServerMsgQ"

#define msgq_xtp_srv2cltpre "/xtp_reveive"


typedef enum 
{  
  XtpSysInit   =1,
  XtpSysExit   =2,
  XtpSysAddId  =3,
  XtpSysDelId  =4,
  XtpSysSend   =5,
  XtpSysReceive=6,
  XtpSysOverrun=7,
  XtpSysError  =8,
  XtpSysKill   =9,
  XtpSysStat   =10,
  XtpSysPulse  =249,
  KILL_THREAD  =250,
  XtpSys_NOCMD    =255, // not used, only a dummy
}XtpSysCommand;

/* Xtp state in answer */
enum XtpState
{
  XtpNotOk = 0,
  XtpOk = 1
};


//used in communication between Server and Client.
typedef struct
{
	int pid;               //meaningful client->Server, not care Server->Client. chid is similar
	int chid;
	unsigned char bCommand;//Always meaningful.
    unsigned char bState;  //meaningful not care Server->Client, client->Server
	tXtpObjHeader msgHeader;
    uchar         data[MAX_XTP_DATA_LENGTH];
}XTP_DEF_PACKED tXtpMessage;

//adapt to the above struct
const ushort COMM_LENGTH_SERVER_CLIENT = 10;

typedef struct
{
    //Server variables
    mqd_t           mq;
    char            mqName[30];

    //pid_t           m_pulseId;
    //struct sigevent m_event;

    int             ErrId;
    char*           pBuf;
    int             BufSize;
    int             Count;
    //_msg_info       info;
    int            NoReply;
}   msgq_info;

/* each CHANNEL support 50 ID filter in most */
const unsigned char MAX_ID_FILTER_EACH_CHN = 50;

typedef struct
{
    int pid;
    int chid;
    int bus;
    bool bus_config;
    CIDCMsgQueue* msgQReceiver;//Created by CLient, for Client Receive Msg from Server
    int   alarm;
    uchar NodeID;
    uchar idFilterNumber;
    int id_filter[MAX_ID_FILTER_EACH_CHN];
}   conn_info;

#endif /* MSGQUEUE_H_ */
