#ifndef XTP_CLIENT_H_
#define XTP_CLIENT_H_

#include "IDCXTP/inc/xtpobj.h"
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCBase/inc/IDCMsgQueue.h"
#include "IDCBase/inc/IDCSema.h"

#define MAX_BROADCAST_OBJ_SUPPORT 64
typedef struct
{
	uchar  XTPNode;
	ushort count;
	ushort List[MAX_BROADCAST_OBJ_SUPPORT];
}BRODCAST_LIST;

extern BRODCAST_LIST broadcastList[32];

typedef void (*AnswerFuncType)(tXtpObjHeader*, void*);

extern BRODCAST_LIST* GetBroadList(const uchar NodeID);

class CXtpClient
{
public:
	/* Create a XTP Client Instanse */
	CXtpClient(const uchar);
	/* initiate connection to Xtp Server */
	bool XtpConnect (AnswerFuncType, BRODCAST_LIST*, void* para=NULL);   
	/* delete connection to Xtp Server   */
	void XtpDisconnect (); 
	/* send a Xtp command         */
	bool XtpSend (tXtpObjHeader *, uchar cmd = XtpSysSend, void* extralPara = NULL); 
	/* check connection to XtpServer */
	bool XtpCheckConnect ()const; 
	/* Get XTP Node ID */
	uchar inline GetNodeID(){return m_XTP_Node;}

private:
	//PIPE device use sending out
	bool m_iDstConnection;

	static void* ReceiveThread (void*); 
	static void* StateThread (void*); 
	int CreateChannelSrv2Clt();
	void ThreadDestroy();
	uchar m_XTP_Node;

	int m_iPid;//process ID
	//fifo_xtp_srv2cltpre+pid+chid(XTP node)
	char m_fifo_xtp_srv2cltPath[128];
	mqd_t m_ServerQueue;

	//msgQ used receive XTP from Server
	CIDCMsgQueue* m_own_msgQ;
	pthread_t m_RcvThreadId;
	pthread_t m_StatThreadId;
	const unsigned long dStackSize;
	char * pStack;
	int iConnectPrio;
	bool m_ExitFromServer;
	AnswerFuncType m_RcvHandler;
	void*          m_RcvHandlerPara;

	bool m_semaPReadyforRcv;
    CIdcSema waitShakeHandfromSrv;
};
#endif /* XTP_CLIENT_H_ */

