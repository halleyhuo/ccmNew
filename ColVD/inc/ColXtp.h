#ifndef COL_XTP_H_
#define COL_XTP_H_ 1

//#include <pthread.h>
#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCXTP/Client/inc/xtpClient.h"

#include "IDCLog/inc/LogTrace.h"


class CColXtpIf : public CIDCThread
{
    
public :
    CColXtpIf();
    virtual void run();
	static CColXtpIf* m_instanse;
	static CColXtpIf* Sigleton(CColXtpIf* instanse = NULL);
	void onSendIColBlade();
private:
	void SendInt2ColCO(ushort COB_ID, ushort Len, uchar p [8]);
    static void CallBackFunc(tXtpObjHeader*,void*);
    CXtpClient* xtpClientIf;
        
};



#endif
