#ifndef IDC_THREAD_H
#define IDC_THREAD_H 1

#include "IDCType.h"

#include "IDCTaskID.h"

#include "IDCMsgQueue.h"

class CIDCThread 
{
public:
    enum StackSize
    {
        /**
         * \brief      Small stack size.
         **********************************************************************/
        Small  = PTHREAD_STACK_MIN,
        /**
         * \brief      Medium stack size.
         **********************************************************************/
        Medium = STACK_SIZE_DEFAULT,
        /**
         * \brief      Large stack size.
         **********************************************************************/
        Large  = 0x100000
    };

    enum TaskPriority
    {
        /**
         * \brief      Realtime task priority.
         **********************************************************************/
        Realtime = 9,
        /**
         * \brief      Highest task priority.
         **********************************************************************/
        Highest  = 10,
        /**
         * \brief      Very high task priority.
         **********************************************************************/
        VeryHigh = 11,
        /**
         * \brief      Higher task priority.
         **********************************************************************/
        Higher   = 12,
        /**
         * \brief      High task priority.
         **********************************************************************/
        High     = 13,
        /**
         * \brief      Moderate task priority.
         **********************************************************************/
        Moderate = 14,
        /**
         * \brief      Low task priority.
         **********************************************************************/
        Low      = 15,
        /**
         * \brief      Lower task priority.
         **********************************************************************/
        Lower    = 16,
        /**
         * \brief      Very low task priority.
         **********************************************************************/
        VeryLow  = 17,
        /**
         * \brief      Lowest task priority.
         **********************************************************************/
        Lowest   = 18
    };
    
public :
    static int ITCSend(const ITCObjHeader*);
    CIDCThread(const char threadName[IDC_MAX_NAME_LENGTH] = NULL,IDC_TASK_ID threadIdent = IDC_TASK_INVALID,StackSize stackSize = Small,
                       TaskPriority priority = Low,bool _ifRcvITC = false);
    
    virtual ~CIDCThread();
    
  
    //## operation run()
    virtual void run() = 0;    
    
   
    //## operation setDetachState(int)
    int setDetachState(int iDetachState);
    
   
    //## operation setPriority(int)
    int setPriority(int iPrio);
 
    //## operation setStack(char*,int)
    int setStack(char* pStack, int iStackSize);
    
    
    //## operation waitForThreadEnd()
    void waitForThreadEnd(); 
    
   
    inline const IDCErrnoType getIDCErrno() const;
   
    
    //## operation kill()
    virtual void kill();
    
    bool Init();

protected :
     //## operation execute()
    pthread_t execute(); 
    
    //## operation start(void *)
    static void * start(void * pThread);

    static CIDCMsgQueue* GetITCRcvMsgQ(const IDC_TASK_ID);

    
    pthread_t tID;		//## attribute tID

    pthread_attr_t myAttr;		//## attribute myAttr    
 
    IDCErrnoType m_ErrNo;		//## attribute m_ErrNo
    
 
    pthread_t tJoinID;		//## attribute tJoinID

    char taskName[IDC_MAX_NAME_LENGTH];
    IDC_TASK_ID taskIdent;    
    bool ifRecvITC;
    CIDCMsgQueue* m_msgQueue;//will be instantiate  only if ifJoinIPC=true

    static IDC_ITC_MAP itcMap[IDC_TASK_SIZE];
    
 

private:
    

    void killThread();
};

// Class CIDCThread

//## Get and Set Operations for Class Attributes (inline)

inline const IDCErrnoType CIDCThread::getIDCErrno() const {
   
    return m_ErrNo;

}

#endif
