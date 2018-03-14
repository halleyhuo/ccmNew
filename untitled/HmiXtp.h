#ifndef HMI_XTP_H_
#define HMI_XTP_H_ 1

#include <QThread>
#include <QObject>

#include "IDCBase/inc/IDCThread.h"
#include "IDCBase/inc/IDCSema.h"
#include "IDCBase/inc/IDCAlarm.h"
#include "IDCXTP/inc/xtpcomm.h"
#include "IDCXTP/Client/inc/xtpClient.h"

#include "IDCLog/inc/LogTrace.h"
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
