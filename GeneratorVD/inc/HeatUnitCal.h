#ifndef GEN_HEAT_UNIT_CAL_H_
#define GEN_HEAT_UNIT_CAL_H_ 

//#include <pthread.h>
#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCXTP/Client/inc/xtpClient.h"

#include "IDCLog/inc/LogTrace.h"

typedef struct
{
    ulong oldpower;
    ulong current;
    ulong colder;
    ulong HeatMax;
    ushort percentage;
    ushort waittime;
}sHeatStruc;

static sHeatStruc heatData;

class CGenHeatCal : public CIDCThread
{
    
public :
    CGenHeatCal();
    virtual void run();
	
private:
    static void HeateCalInSecond(void* param);
    ulong m_alarmTid;
    static ushort costTimeCount(ulong power);
    static ushort findColdforSecond(ulong power);
    
};


#endif
