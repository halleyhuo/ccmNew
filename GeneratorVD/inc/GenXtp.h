#ifndef GEN_XTP_H_
#define GEN_XTP_H_ 

#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCBase/inc/IDCMsgQueue.h"

#include "IDCLog/inc/LogTrace.h"


//#include <pthread.h>
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCXTP/Client/inc/xtpClient.h"



class CGenXtpIf : public CIDCThread
{
    
public :
    CGenXtpIf();
    virtual void run();
	static CGenXtpIf* m_instanse;
	static CGenXtpIf* Sigleton(CGenXtpIf* instanse = NULL);
	void onSendiGENData(const iGENData* data);
	void onSendIGenReadyData();
	void onSendSFlCmdData();
	void onSendICompStatus(const CompStatusType status)const;
	void onSendAXD(const iAcqData* data);
	void onSendTubeLoad(const iTubeLoad* data);
	CXtpClient* xtpClientIf;
private:
    static void CallBackFunc(tXtpObjHeader*,void*);      
};


#endif
