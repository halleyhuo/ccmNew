#ifndef CO_APP_H
#define CO_APP_H


#include "IDCBase/IDCThread.h"
#include "IDCBase/IDCSema.h"
#include "IDCBase/IDCAlarm.h"

#include "LogTrace.h" 

class CCoApp : public CIDCThread
{
    
public :
    CCoApp();
    
    virtual ~CCoApp();
    
  
    //## operation run()
    virtual void run();
    
  
};

#endif
