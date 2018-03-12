#include "HmiXtp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
/******************************************************************************
***************************** XTP Interface **************************************
***  Receive Obj: ISIDINFO,SCOLBLADEPOS, IXRAY... ,                                    **
***  Out Obj:    ICOLBLADEPOS, ICOMPSTATUS, ICOLLSEARCHRUN                             **
*******************************************************************************/

namespace IDC
{
    const int debug_level_remote=LOG_NOTICE+1;
    const int debug_level_local=0;
}

BRODCAST_LIST broadcastList[] =
{
    {XTP_NODE_MASTER            , 0},
    {XTP_NODE_TUI_1             , 1,{IFLDATA}},
    {XTP_NODE_COLLIMATOR_CONTROL, 1, {ISYSTEMINFO,IXRAY}},
    {XTP_NODE_GENERATOR,          1, {ISYSTEMINFO}},
};

void recvThread::run()
{
    while(!CHmiXtpIf::xtpClientIf->XtpConnect(CHmiXtpIf::CallBackFunc,GetBroadList(CHmiXtpIf::xtpClientIf->GetNodeID()),CHmiXtpIf::Sigleton()))
    {
        sleep(2);//retry 2s later
    }
    while(true)
    {
        sleep(1);
    }

};


CHmiXtpIf* CHmiXtpIf::m_instanse = NULL;

CXtpClient* CHmiXtpIf::xtpClientIf = NULL;

recvThread* CHmiXtpIf::m_recvThread = NULL;

CHmiXtpIf::CHmiXtpIf() : QObject()
{
    xtpClientIf = new CXtpClient(XTP_NODE_TUI_1);
}

CHmiXtpIf::~CHmiXtpIf()
{

}

CHmiXtpIf* CHmiXtpIf::Sigleton(CHmiXtpIf* instanse/* = NULL*/)
{
    if(NULL==instanse)
    {
        return m_instanse;
    }
    else
    {
        m_instanse = instanse;
    }
}

void CHmiXtpIf::receive()
{
    m_recvThread = new recvThread();
    m_recvThread->start();

}

void CHmiXtpIf::CallBackFunc(tXtpObjHeader* msg, void* para)
{
    if(para!=NULL)
    {
        CHmiXtpIf* Instance = (CHmiXtpIf*)para;
        IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got XTP msg 0x%x(%d)",
            Instance->xtpClientIf->GetNodeID(),msg->telID,msg->length);
        switch(msg->telID)
        {
        case IUISHOW:
            {            
                iUiShowObjCnt* uiObjCnt = (iUiShowObjCnt*)(msg+1);
                iUiShowObj* obj = (iUiShowObj*)(uiObjCnt+1);
                IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Node 0x%x Got UISHOW(%d)",
                    Instance->xtpClientIf->GetNodeID(),uiObjCnt->obj_count);
                for(int i=0; i<uiObjCnt->obj_count; i++)
                {
                    IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"emit signal {%x,%d,%d} to mainwindow",
                        UI_SEL_CollimatorHeight, obj[i].param, UI_Widget_ReadOnly);
                    emit Instance->ShowUiEvent(obj[i].sel_func, obj[i].param, obj[i].status);
                }

            }
            break;       

        case IXRAY://Impact on Serach Run
            break;

        default:

            break;
        }

    }

}





