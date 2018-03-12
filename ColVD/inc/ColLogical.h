#ifndef COL_LOGICAL_H_
#define COL_LOGICAL_H_ 1

//#include <pthread.h>
#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCXTP/Client/inc/xtpClient.h"

#include "IDCLog/inc/LogTrace.h"




class CColLogical : public CIDCThread
{
    
public :
    CColLogical();
    
    virtual ~CColLogical();
    
  
    //## operation run()
    virtual void run();

	static ushort GetBladePosCmd(const uchar idx);
	static bool SetBladePosCmd(const uchar idx, const ushort val);
	static IColBlade* GetBladeStatus(const uchar idx);
	
	static ushort SIDCommand;

private:
    ulong m_alarmTid;
    static void _callBackAlarm(void*);  
	static ushort BladePosCommand[13];//data saved in [1]~[12]
	static IColBlade BladeStatus[13]; //data saved in [1]~[12]
	
};

#endif
