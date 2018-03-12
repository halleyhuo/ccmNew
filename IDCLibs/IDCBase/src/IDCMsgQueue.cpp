#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "IDCMsgQueue.h"
#include "LogTrace.h"

const rlim_t MAX_MQ_SZ = 81920000;
const rlim_t MIN_MQ_SZ = 819200;

static bool ChangeMQLimit( const int newSize ){
    rlimit old_limit;
    bool rc = false;
    if( -1 == getrlimit(RLIMIT_MSGQUEUE, &old_limit) ){
        perror("getrlimit: ");
    }else{
        if( (MAX_MQ_SZ<=old_limit.rlim_max)||(MAX_MQ_SZ<=old_limit.rlim_cur) ){
#ifdef DEBUGLINUXADAPTER
             printf(">>>Warning: Cur MQ SZ is already reaches MAX\n");
#endif
            
        }else if( ((MIN_MQ_SZ>=old_limit.rlim_max)||(MIN_MQ_SZ>=old_limit.rlim_cur)) &&
                  (newSize<0) ){
#ifdef DEBUGLINUXADAPTER
             printf(">>>Warning: Cur MQ SZ is already reaches MIN\n");
#endif                  
        }else{
            rlimit new_limit; 
            new_limit.rlim_max = MAX_MQ_SZ;
            new_limit.rlim_cur = new_limit.rlim_max;
         
             if( -1==setrlimit(RLIMIT_MSGQUEUE, &new_limit) ){
                perror("setrlimit: ");
             }else{
#ifdef DEBUGLINUXADAPTER
                printf(">>>set new MQ Size Limit Scussfully, sz change from %d -> %d\n",
                        old_limit.rlim_max,MAX_MQ_SZ);
                rc = true;
#endif
             }
        }
    }
    return rc;    
}


//## class CIDCMsgQueue
CIDCMsgQueue::CIDCMsgQueue() : iMqdes(-1), pQueueName(NULL), m_ErrNo(0) 
{   
      tMqAttr.mq_flags = 0;   
    
}

CIDCMsgQueue::~CIDCMsgQueue() 
{
    //#[ operation ~CIDCMsgQueue()
    
     (void) close();
     (void) destroy(); // ignore return values
     if ( pQueueName != NULL )
     {
        delete[] pQueueName;
        pQueueName = NULL;
     }
    
    //#]
}

const CIDCMsgQueue & CIDCMsgQueue::operator=(const CIDCMsgQueue & right) 
{
    //#[ operation operator=(const CIDCMsgQueue &)
    
    
    #ifdef DEBUGMBX
    static int iI = 0;
    printf("CIDCMsgQueue::operator=: %d call(s)\n", ++iI);
    #endif
    
    if (this != &right)
    {
      iMqdes = dup (right.iMqdes);
      pQueueName = NULL;
      tMqAttr = right.tMqAttr;
      m_ErrNo = right.m_ErrNo;
    }
    
    return *this;
    
    
    //#]
}

mqd_t CIDCMsgQueue::openCreate(const char* pName, long lNrMsg, long lMsgSize, int iAccessMode, bool fExclusive) 
{    
     int iRetry = 10;
     int iUnlinkRet;    
     if ( pName == NULL )
     {
        m_ErrNo = EINVAL;
        printf("###error, openCreateMSGQRcvServerFail ...1\n");
        return IDC_ERROR;
     }
     if ( strlen( pName ) > IDC_MAX_NAME_LENGTH )
     {
        m_ErrNo = ENAMETOOLONG;
        printf("###error, openCreateMSGQRcvServerFail ...2\n");
        return IDC_ERROR;
     }
     if ( pQueueName == NULL ) // save Queue name for futher use                   
     {
        pQueueName = new char [ strlen( pName ) + 1 ];
        if (pQueueName!=NULL)
           strcpy( pQueueName, pName );
        else
        {
           m_ErrNo = ENOMEM;
           printf("###error, openCreateMSGQRcvServerFail ...3\n");
           return IDC_ERROR;
        }
     }
     else
     {
        if ( ( strcmp(pQueueName,pName) == 0 ) && 
             ( tMqAttr.mq_maxmsg ==  lNrMsg ) &&
             ( tMqAttr.mq_msgsize == lMsgSize) )
        {
            printf(">>>INFO Will open Mqueue %d\n",__LINE__);
           return iMqdes;
        }
        else
        {
            printf("###error, openCreateMSGQRcvServerFail ...4\n");
           m_ErrNo = EINVAL;
           return IDC_ERROR;
        }
     }
     
     if ( fExclusive )
     {
        // unlink a possibly existing queue name
        iRetry = 10;
        do
        {
           iRetry--;
           iUnlinkRet = mq_unlink( pName ); 
           m_ErrNo = errno; 
           if ( (iUnlinkRet == -1) && ((m_ErrNo==EINTR) || (m_ErrNo==EAGAIN)) )
              (void)IDC_DELAY(1); // try again because mq_unlink was interupted by a signal
           else
              iRetry = 0;
        } while (iRetry != 0);
     }
    
     // set the attributes
     tMqAttr.mq_maxmsg =  lNrMsg;
     tMqAttr.mq_msgsize = lMsgSize;

     (void)ChangeMQLimit(lNrMsg*lMsgSize);

     iRetry = 10;
     do
     {
        iRetry--;
        iMqdes = mq_open( pQueueName,
                          iAccessMode | ( fExclusive ? O_CREAT|O_EXCL : O_CREAT ),
                          (S_IRWXU|S_IRWXG|S_IRWXO),
                          &tMqAttr
                        );
        m_ErrNo = errno;
        
        if ( (iMqdes == -1) && (m_ErrNo==EINTR) )
            (void)IDC_DELAY(1); // try again because mq_open was interupted by a signal
         else
            iRetry = 0;
     }  while (iRetry != 0);
    
     if ( iMqdes == -1 )     // error opening the msgqueue
     {
        perror( "OpenCreate: " );
        delete[] pQueueName;
        pQueueName = NULL; 
    
        tMqAttr.mq_flags = 0;
        tMqAttr.mq_maxmsg =  0;
        tMqAttr.mq_msgsize = 0;
    
        return IDC_ERROR;
     }
     (void)getattr( &tMqAttr );    // get the attributes of the queue
               
    printf(">>>INFO Will open Mqueue %d\n",iMqdes);                                   // ignore return value
     return iMqdes;
    
    //#]
}

int CIDCMsgQueue::destroy() 
{    
     int iRetval = IDC_ERROR;
     int iRetry;
     
     if( pQueueName != NULL )
     {
        (void) close(); // ignore return value
        
        iRetry = 10;
        do
        {
           iRetry--;
           iRetval = mq_unlink( pQueueName ); 
           m_ErrNo = errno; 
           if ( (iRetval == -1) && ((m_ErrNo==EINTR) || (m_ErrNo==EAGAIN)) )
              (void)IDC_DELAY(1); // try again because mq_unlink was interupted by a signal
           else
              iRetry = 0;
        } while (iRetry != 0);
    
        if ( iRetval == 0 )
        {
           delete[] pQueueName;
           pQueueName = NULL;
    
           iRetval = IDC_OK;
        }else{
            perror("###error mq_unlink: ");
        }
     }
     return iRetval;
}

mqd_t CIDCMsgQueue::open(const char * pName, int iAccessMode) 
{
    int iRetry;
    
     if(pName == NULL)
     {
        m_ErrNo = EINVAL;
        return IDC_ERROR;
     }
    
     if ( iMqdes != -1 )
     {
        return iMqdes;
     }
    
     iRetry = 10;
     do
     {
        iRetry--;
        iMqdes = mq_open( pName, iAccessMode );
        m_ErrNo = errno;
        if ( (iMqdes == -1) && (m_ErrNo==EINTR) )
            (void)IDC_DELAY(1); // try again because mq_open was interupted by a signal
         else
            iRetry = 0;
     } while (iRetry != 0);
     
     if ( iMqdes != -1 )
     {
        (void)getattr( &tMqAttr );    // get the attributes of the queue
                                      // ignore return value
     }
    
     return iMqdes;

}

int CIDCMsgQueue::close() 
{
     int iRetval = IDC_ERROR;
     int iRetry;
    
     if ( iMqdes != -1 )
     {         
        iRetry = 10;
        do
        {
           iRetry--;
           iRetval = mq_close( iMqdes );
           m_ErrNo = errno;
           if ( (iRetval == -1) && ((m_ErrNo==EINTR) || (m_ErrNo==EAGAIN)) )
              (void)IDC_DELAY(1); // try again because mq_close was interupted by a signal
           else
              iRetry = 0;
        } while (iRetry != 0);
     }
    
     if ( iRetval != -1 )
     {
        iMqdes = -1;
        tMqAttr.mq_flags = 0;
        tMqAttr.mq_maxmsg =  0;
        tMqAttr.mq_msgsize = 0;
        iRetval = IDC_OK;
     }
    
     return iRetval;
    
    
    //#]
}

int CIDCMsgQueue::send(const void* pMsg, size_t tLength, unsigned int uiPrio/* = 0*/) 
{
     int iRetval;
     int iRetry;
     
     if ( (pMsg == NULL)  )
     {
        m_ErrNo = EINVAL;
        return IDC_ERROR;
     }
    
    #ifdef DEBUGMBX
     printf("CIDCMsgQueue::Send: before\n" );
    #endif
    
     iRetry = 10;
     do
     {
        iRetry--;
        iRetval = mq_send( iMqdes, (char*)pMsg, tLength, uiPrio );
        m_ErrNo = errno;
        if ( (iRetval == -1) && (m_ErrNo==EINTR) )
           (void)IDC_DELAY(1); // try again because mq_send was interupted by a signal
        else
           iRetry = 0;
     } while (iRetry != 0);
    
    #ifdef DEBUGMBX
     printf("CIDCMsgQueue::Send: bytes sent: %d of %d\r\n", iRetval, tLength );
    #endif
    
     if ( iRetval == -1 )
        return IDC_ERROR;
     else
        return IDC_OK;
}

int CIDCMsgQueue::recv(void* pMsg, const size_t bufLen, long lTimeout/* = NO_TIMEOUT*/, unsigned int * pPrio/* = NULL*/) 
{
     int iRetval = IDC_NO_SUFFICIENT_BUF;
     int iRetry  = 10;
     
     if (pMsg == NULL || lTimeout < 0) 
     {
        m_ErrNo = EINVAL;
        return IDC_ERROR;
     }  
    
     if (iMqdes==-1)
     {
        printf("###error, msgQ have not been created\n");
        return IDC_ERROR; // No queue
     }
      
     if ( lTimeout != NO_TIMEOUT )       // a timeout shall be installed
     {
        struct timeval tv;
        fd_set rfd;
        
        tv.tv_sec  = ((unsigned long) lTimeout)/1000;
        tv.tv_usec = (((unsigned long) lTimeout)%1000)*1000;
         
        FD_ZERO( &rfd );
        FD_SET( (unsigned int)iMqdes, &rfd ); //iMqdes is not -1 so >=0
         
        iRetry = 10;
        do
        {
           iRetry--;
           iRetval = select( FD_SETSIZE, &rfd, NULL, NULL, &tv );
           m_ErrNo = errno;
           if ( (iRetval == -1) && ((m_ErrNo==EINTR) || (m_ErrNo==EAGAIN)) )
              (void)IDC_DELAY(1); // try again because mq_send was interupted by a signal
           else
              iRetry = 0;
        } while (iRetry != 0);
        
        FD_ZERO( &rfd );
         
        if (iRetval==0)
        {
           m_ErrNo = EAGAIN;
           return IDC_TIMEOUT;
        }
        if (iRetval==-1)
        {            
           return IDC_ERROR;
        }
     }
    
     iRetry = 10;
     const size_t bufMaxLen = bufLen>tMqAttr.mq_msgsize?tMqAttr.mq_msgsize:bufLen;
     
     IDC::Log(IDC::LOG_DEBUG,LOG_TAG,"CIDCMsgQueue::Recv bufMaxLen %d",
             bufMaxLen );
     do
     {
        iRetry--;
        iRetval = mq_receive( iMqdes, (char*)pMsg, bufMaxLen, pPrio ); //not negative
        m_ErrNo = errno;
         
        if ( (iRetval == -1) && (m_ErrNo==EINTR) )
           (void)IDC_DELAY(1); // try again because mq_receive was interupted by a signal
        else
        {
            IDC::Log(IDC::LOG_DEBUG,LOG_TAG,"CIDCMsgQueue::iRetval %d",iRetval );
           iRetry = 0;
        }
     } while (iRetry != 0);
     return iRetval;    
}

int CIDCMsgQueue::getattr(mq_attr * pAttr) 
{
     int iRetry;
     int iRetval;
     
     iRetry = 10;
     do
     {
        iRetry--;
        iRetval = mq_getattr( iMqdes, pAttr );  
        m_ErrNo = errno;
        if ( (iRetval == -1) && ((m_ErrNo==EINTR) || (m_ErrNo==EAGAIN)) )
           (void)IDC_DELAY(1); // try again because mq_getattr was interupted by a signal
        else
           iRetry = 0;
     } while (iRetry != 0);
     return iRetval;  
}

int CIDCMsgQueue::setattr(const mq_attr * pMqstat, mq_attr * pOmqstat) 
{
     int iRetry;
     int iRetval;
     
     iRetry = 10;
     do
     {
        iRetry--;
        iRetval = mq_setattr( iMqdes, pMqstat, pOmqstat);  
        m_ErrNo = errno;
        if ( (iRetval == -1) && ((m_ErrNo==EINTR) || (m_ErrNo==EAGAIN)) )
           (void)IDC_DELAY(1); // try again because mq_setattr was interupted by a signal
        else
           iRetry = 0;
     } while (iRetry != 0);
     return iRetval;  
}
