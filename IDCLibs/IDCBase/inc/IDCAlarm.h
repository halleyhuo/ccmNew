#ifndef IDC_ALARM_H
#define IDC_ALARM_H
#include <signal.h>
#include <time.h>
#include "IDCType.h"

typedef void (*AlarmTimeout)(void*);
#define MAX_DELAYED_MESSAGES    128

typedef struct
{
    bool            bInUse;
    ulong           tIdent;
    AlarmTimeout    handler;
    void*           param;
    timer_t         tTimerId;
    sigevent        tEvent;
    bool            bLoop;
} tinyAlarm_t;

void start_alarm();

bool create_alarm( ushort elapseMs,
	AlarmTimeout handler,void* param, ulong &newId, bool bLoop);
bool delete_alarm( ulong id );


class CIdcAlarm 
{
public:
	CIdcAlarm();
    static CIdcAlarm* GetInstance();
    static CIdcAlarm* CreateInstance();
    static CIdcAlarm* m_pSingleton;        // singleton instance
    virtual ~CIdcAlarm();

    bool CreateAlarm( ushort elapseMs,
    		AlarmTimeout handler, void*, ulong &newId, bool bLoop);
    
    
    bool DeleteAlarm( ulong id );

    ulong                m_tUniqueAlarmId;  //0 is an invalid Id

    static tinyAlarm_t   m_asDelayedMsg[MAX_DELAYED_MESSAGES];

private:
//    uint m_TaskId;

};


#endif


