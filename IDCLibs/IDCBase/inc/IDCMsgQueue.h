#ifndef IDC_MSGQUEUE_H
#define IDC_MSGQUEUE_H

#include <mqueue.h>
#include <time.h>
#include "IDCType.h"

class CIDCMsgQueue 
{
    
public :

    CIDCMsgQueue();
    virtual ~CIDCMsgQueue();
    virtual const CIDCMsgQueue & operator=(const CIDCMsgQueue & right);
    mqd_t openCreate(const char* pName, long lNrMsg, long lMsgSize, int iAccessMode = O_RDWR, bool fExclusive = true);
    int destroy();
    
    mqd_t open(const char * pName, int iAccessMode = O_RDWR);
    int close();
    
    int send(const void* pMsg, size_t tLength, unsigned int uiPrio = 0);
    int recv(void* pMsg, const size_t bufLen, long lTimeout = NO_TIMEOUT, unsigned int * pPrio = NULL);
    int getattr(mq_attr * pAttr);
    int setattr(const mq_attr * pMqstat, mq_attr * pOmqstat = NULL);
    
    inline IDCErrnoType GetIDCErrno() const;
    

protected :

    mqd_t iMqdes;		//## attribute iMqdes
    char * pQueueName;		//## attribute pQueueName
    
    // Attributes of the message queue
    // 
    mq_attr tMqAttr;		//## attribute tMqAttr
    
    // Local errno variable.
    IDCErrnoType m_ErrNo;		//## attribute m_ErrNo
};

inline IDCErrnoType CIDCMsgQueue::GetIDCErrno() const {
    //#[ operation GetIDCErrno() const
    
    return m_ErrNo;
    
    //#]
}

#endif
