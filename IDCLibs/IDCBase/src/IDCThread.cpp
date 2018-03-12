
#ifdef DEBUGTHREAD
#include <iostream>
using namespace std;
#endif
#include <bits/local_lim.h>//get definition of PTHREAD_STACK_MIN
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include "IDCThread.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

IDC_ITC_MAP CIDCThread::itcMap[IDC_TASK_SIZE];

CIDCMsgQueue* CIDCThread::GetITCRcvMsgQ(const IDC_TASK_ID taskIdent)
{
    CIDCMsgQueue* rc = NULL;
    for(int i=0; i<IDC_TASK_SIZE; i++)
    {
        if(itcMap[i].taskIdent==taskIdent)
        {
            rc = itcMap[i].msgQRcv;
            break;
        }
    }
    return rc;
}

int CIDCThread::ITCSend(const ITCObjHeader* ITCObj)
{
    int rc = -1;
    if(ITCObj!=NULL)
    {
        CIDCMsgQueue* msgQRcv = GetITCRcvMsgQ(IDC_TASK_ID(ITCObj->tidTarget));
        if(msgQRcv!=NULL)
        {
            rc = msgQRcv->send((char*)ITCObj,ITCObj->len);
        }
        else
        {
            printf("###error, not match ITCMsgQ for thread Ident %d\n",ITCObj->tidTarget);
        }
    }
    return rc;
}


// Class CIDCThread
CIDCThread::CIDCThread(const char threadName[IDC_MAX_NAME_LENGTH]/* = NULL*/,IDC_TASK_ID threadIdent /*= IDC_TASK_INVALID*/,StackSize stackSize/* = Small*/,
                       TaskPriority priority /*= Low*/,bool _ifRcvITC/* = false*/) 
: m_ErrNo(0),taskIdent(threadIdent),ifRecvITC(_ifRcvITC)
{ 
	tID = tJoinID = 0;
	int iRetry;
	int iRetval;

	iRetry = 10;
	do 
	{
		iRetry--;
		iRetval = pthread_attr_init (&myAttr); //ignore return value
		if ( (iRetval == EINTR) || (iRetval == EAGAIN) )
		{
			IDC_DELAY(1);
		}
		else
		{
			iRetry = 0;
		}
	} while (iRetry != 0);


	iRetry = 10;
	do 
	{
		iRetry--;
		iRetval = pthread_attr_setstacksize(&myAttr, stackSize); 
		if ( (iRetval == EINTR) || (iRetval == EAGAIN) )
		{
			IDC_DELAY(1);
		}
		else
		{
			iRetry = 0;
		}
	} while (iRetry != 0);
	//must make sure that FIFO schedule policy for Linux

	int paramPolicy;   
	paramPolicy =SCHED_FIFO;

	iRetry = 10;
	do 
	{
		iRetry--;
		iRetval = pthread_attr_setschedpolicy(&myAttr, SCHED_FIFO); 
		if ( (iRetval == EINTR) || (iRetval == EAGAIN) )
		{
			IDC_DELAY(1);
		}
		else
		{
			iRetry = 0;
		}
	} while (iRetry != 0);    
	pthread_attr_getschedpolicy(&myAttr,&paramPolicy);

    setPriority(priority);


	(void) setDetachState (PTHREAD_CREATE_JOINABLE);  

    strncpy(taskName,threadName,sizeof(taskName));
    taskName[IDC_MAX_NAME_LENGTH-1] = '\0';
    if(_ifRcvITC)
    {
        m_msgQueue = new CIDCMsgQueue();
    }
    

}

CIDCThread::~CIDCThread() {
#ifdef DEBUGTHREAD
	cout << "CIDCThread::~CIDCThread() before killThread()"<< endl;
#endif

	killThread();
#ifdef DEBUGTHREAD
	cout << "CIDCThread::~CIDCThread() after killThread()"<< endl;
#endif
}


pthread_t CIDCThread::execute() {
	int iRetry = 10;

	// create the thread starting the run-method
	do
	{
		iRetry--;

		m_ErrNo = pthread_create (&tID, &myAttr, &start, this);

		if ( (m_ErrNo == EINTR) || (m_ErrNo == EAGAIN) )
		{
			IDC_DELAY(1);
		}
		else
		{
			iRetry = 0;
		}
	} while ( iRetry != 0 );

#ifdef DEBUGTHREAD
	cout << "thread ID: " << tID << endl;
#endif

	// tID lives until thread terminates or is killed. tJoinID lives until it is
	// joined.
	tJoinID = tID;

	return tID;

}

void CIDCThread::killThread() 
{
	int iRetry = 10;
	int iRetval;
	pthread_t tThreadID;

	if ( tID != 0 )
	{

		tID=0;

	}
	else
	{
		// The thread terminates by itself. It has already reset the spin lock.
		// or the thread is already terminated
	}
}

int CIDCThread::setDetachState(int iDetachState) 
{
	int iRetry;

	iRetry = 10;
	do 
	{
		iRetry--;
		m_ErrNo = pthread_attr_setdetachstate (&myAttr, iDetachState);
		if ( (m_ErrNo == EINTR) || (m_ErrNo == EAGAIN) )
		{
			IDC_DELAY(1);
		}
		else
		{
			iRetry = 0;
		}
	} while (iRetry != 0);

	if ( m_ErrNo == 0 )
		return IDC_OK;
	else
		return IDC_ERROR;
}

int CIDCThread::setPriority(int iPrio) 
 {
	int iRetValue;
	int iRetry;
	struct sched_param myParam;
	pthread_t tCallingTid ;

	iRetValue = IDC_OK ;

	if ( tID == 0 )
	{// thread is not yet running
#ifdef DEBUGTHREAD
		cout << "thread is not running" << endl;
#endif
		myParam.sched_priority = iPrio;
#ifdef DEBUGTHREAD
		cout << "prio is " << iPrio << endl;
#endif

		iRetry = 10;
		do
		{
			iRetry--;
			iRetValue = pthread_attr_setinheritsched( &myAttr, PTHREAD_EXPLICIT_SCHED );
			if ((iRetValue == EINTR) || (iRetValue == EAGAIN))
			{
				IDC_DELAY(1);
			}
			else
			{
				iRetry = 0;
			}
		} while ( iRetry != 0);

		iRetry = 10;
		do
		{
			iRetry--;
			iRetValue = pthread_attr_setschedparam( &myAttr, &myParam );
			if ((iRetValue == EINTR) || (iRetValue == EAGAIN))
			{
				IDC_DELAY(1);
			}
			else
			{
				iRetry = 0;
			}
		} while ( iRetry != 0);

#ifdef DEBUGTHREAD
		switch ( iRetValue )
		{
		case 0:
			cout << "setschedparam ist o.k." << endl;

			break;
		case EINVAL: cout << "invalid thread attribute object" << endl;
			break;
		case ENOTSUP: cout << "Invalid thread scheduling parameters attribute param" << endl;
			break;
		default: cout << "kein bekannter Fehler" << endl;
		}
#endif
		if ( iRetValue != 0 )
		{
			m_ErrNo = iRetValue;
			iRetValue = IDC_ERROR;
		}
	}
	else
	{// thread is already running
#ifdef DEBUGTHREAD
		cout << "thread is running" << endl;
#endif

		tCallingTid = pthread_self() ;
		if ( pthread_equal ( tID, tCallingTid ) != 0 )
		{
			iRetry = 10;
			do
			{
				iRetry--;
				iRetValue = setpriority( PRIO_PROCESS, 0, iPrio );
				m_ErrNo = errno;
				if ( (iRetValue == -1) && ((m_ErrNo == EINTR) || (m_ErrNo == EAGAIN)) )
				{
					IDC_DELAY(1);
				}
				else
				{
					iRetry = 0;
				}
			} while( iRetry != 0 );
			if ( iRetValue == -1 )
			{
#ifdef DEBUGTHREAD
				cout << "error setting priority" << endl ;
#endif
				iRetValue = IDC_ERROR;
			}
			else
			{
				iRetValue = IDC_OK;
			}
		}
		else
		{
#ifdef DEBUGTHREAD
			cout << "IDC: Manipulation priority from another thread not allowed" <<
				endl ;
#endif
			m_ErrNo = EPERM;
			iRetValue = IDC_ERROR;
		}
	}

	return iRetValue;
}

int CIDCThread::setStack(char* pStack, int iStackSize)
{
	int iRetry;
	int iRetValue;
	int iSizeOfStack;

	if ( iStackSize < PTHREAD_STACK_MIN )
		iSizeOfStack = PTHREAD_STACK_MIN;
	else
		iSizeOfStack = iStackSize;

#ifdef DEBUGTHREAD
	cout << "thread stacksize " << iStackSize << endl;
#endif

	if ( tID == 0 )
	{
		if ( pStack != NULL ) { (void)pthread_attr_setstackaddr(&myAttr,pStack); }  
		//ignore return value

		iRetry = 10;
		do 
		{
			iRetry--;
			m_ErrNo = pthread_attr_setstacksize(&myAttr,(unsigned int) iSizeOfStack); 
			if ( (m_ErrNo == EINTR) || (m_ErrNo == EAGAIN) )
			{
				IDC_DELAY(1);
			}
			else
			{
				iRetry = 0;
			}
		} while (iRetry != 0);

		//ignore return value
		iRetValue = IDC_OK;
	}
	else
	{
		m_ErrNo = EPERM;
		iRetValue = IDC_ERROR;
	}

	return iRetValue;
}

void CIDCThread::waitForThreadEnd() 
{
	int iRetry;

	iRetry = 10;
	do 
	{
		iRetry--;
		m_ErrNo = pthread_join( tJoinID, NULL );  //ignore return value
		if ( (m_ErrNo == EINTR) || (m_ErrNo == EAGAIN) )
		{
			IDC_DELAY(1);
		}
		else
		{
			iRetry = 0;
		}
	} while (iRetry != 0);

#ifdef DEBUGTHREAD
	if ( m_ErrNo != 0 )
		perror("CIDCThread");
#endif

	tJoinID = 0 ;
}

void * CIDCThread::start(void * pThread)
{
	CIDCThread *thread;
	thread = (CIDCThread*)pThread;

	usleep(1000);
	thread->run();
	return( 0 );
}

void CIDCThread::kill() 
{
	if ( tID != 0 )
	{
#ifdef DEBUGTHREAD
		printf ("CIDCThread: killing thread id %d\n", tID ) ;
#endif
		if ( pthread_detach( tID ) != 0 )
		{
#ifdef DEBUGTHREAD
			perror ("CIDCThread: Thread could not be detached") ;
#endif
		}
	}
}

bool CIDCThread::Init()
{
    if(ifRecvITC)
    {
        if(m_msgQueue!=NULL)
        {
            char szMessageBoxName[IDC_MAX_NAME_LENGTH];
            memset(szMessageBoxName,0,sizeof(szMessageBoxName));
            if(snprintf(szMessageBoxName ,IDC_MAX_NAME_LENGTH-1
                       ,(RECV_MBX_NAME) ,taskIdent) > 0)
            {
                if(m_msgQueue->openCreate(szMessageBoxName,MAX_ITC_MSG_CNT,MAX_ITC_LENGTH)!=IDC_ERROR)
                {
                    itcMap[taskIdent].taskIdent = taskIdent;
                    itcMap[taskIdent].msgQRcv   = m_msgQueue;
                }
                else
                {                    
                    printf("###error, msgQueue can't create\n");
                }
            } 
            else
            {                
                printf("###error, msgQueue can't create\n");
            }
        }
        else
        {            
            printf("###error, msgQueue still not malloc\n");
        }
    }

	return (execute()!=0);
}
