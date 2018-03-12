#ifndef COL_MASTER_H
#define COL_MASTER_H 1

#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCXTP/Client/inc/xtpClient.h"

#include "IDCLog/inc/LogTrace.h"


class CColMaster : public CIDCThread
{
    
public :
    CColMaster();
    
    virtual ~CColMaster();
    
	
    //## operation run()
    virtual void run();

private:
	
    CIDCMsgQueue* m_COMan;
	CO_APP_COMM_Frame m_2CO;
	void Send2CO(const ushort COB_ID, const uchar _Len, uchar VciData[64]);
};


#endif
