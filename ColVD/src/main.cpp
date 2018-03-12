#include "ColMaster.h"
#include "ColXtp.h"
namespace IDC
{
    const int debug_level_remote=LOG_DEBUG+1; 
    const int debug_level_local=0;
}

CColMaster* pCOLMaster = NULL;

//#define TEST_XTP_
int main(int argc, char* argv[])
{
#if TEST_IDC_THREAD
    start_alarm();
    CIDCThread* myThread = new CMyThreadSrv;
    myThread->Init();
    sleep(1);

    CIDCThread* myThread1 = new CMyThreadClient;
    myThread1->Init();
#endif

#ifdef TEST_XTP_
    CIDCThread* idcTask1 = new CTestXTPTask(XTP_NODE_MASTER);

    CIDCThread* idcTask2 = new CTestXTPTask(XTP_NODE_COLLIMATOR_CONTROL);
    CIDCThread* idcTask3 = new CTestXTPTask(XTP_NODE_GENERATOR);
    idcTask1->Init();
    idcTask2->Init();
    idcTask3->Init();
#endif
    
	pCOLMaster = new CColMaster();
	pCOLMaster->Init();

	CColXtpIf* XTPIf = new CColXtpIf();
	CColXtpIf::Sigleton(XTPIf);
	XTPIf->Init();

     while(true)
     {
        sleep(5);
     }
	return 0;
}
