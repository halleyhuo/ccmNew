#ifndef XTP_COM_H_
#define XTP_COM_H_ 1

#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"

#include "IDCXTP/Client/inc/xtpClient.h"


class CXtpCom : public CIDCThread
{
    
public :
    CXtpCom();
    virtual void run();
	static CXtpCom* m_instanse;
	static CXtpCom* Sigleton(CXtpCom* instanse = NULL);
	void onSGendata(const SGENData*);
	void onIFlData(const iFlData*);
	void onFlCmd(const SFlCmd* data, bool ifBroadCast = false);
	void onAcqUiShow(const iFlData*);
	void onBladeSizeChange();
	void onTubeLoad(const ushort);
	void onSendFLCmd(uchar hk);
	 CXtpClient* xtpClientIf;
private:

    static void CallBackFunc(tXtpObjHeader*, void*);
	static void onSendIntGen(const FunctionCode funcCode, const ushort param);
	void onBladeSizeShow();
	
	
   
	ushort      m_bladePos[12];        
};



#endif
