#ifndef HMI_XTP_H_
#define HMI_XTP_H_ 1

#include <QThread>
#include <QObject>

#include "IDCBaseLib/IDCThread.h"
#include "IDCBaseLib/IDCSema.h"
#include "IDCBaseLib/IDCAlarm.h"
#include "xtp/inc/xtpcomm.h"
#include "xtp/client/xtpClient.h"

#include "LogTrace.h"
class recvThread : public QThread
{
public:
    void run();
};

class CHmiXtpIf : public QObject
{
    Q_OBJECT

public :
    CHmiXtpIf();
    virtual ~CHmiXtpIf();

    static CXtpClient* xtpClientIf;
    static CHmiXtpIf* m_instanse;
    static CHmiXtpIf* Sigleton(CHmiXtpIf* instanse = NULL);
    static void receive();
    static void CallBackFunc(tXtpObjHeader*,void*);
private:
    static recvThread* m_recvThread;

signals:
    void ShowUiEvent(ushort fun_id, ushort param, uchar status);
};



#endif
