#ifndef MASTER_H_
#define MASTER_H_
#include "MasterOD.h"

void COMaster_heartbeatError(CO_Data* d, UNS8);

UNS8 COMaster_canSend(Message *);

void COMaster_initialisation(CO_Data* d);
void COMaster_preOperational(CO_Data* d);
void COMaster_operational(CO_Data* d);
void COMaster_stopped(CO_Data* d);

void COMaster_post_sync(CO_Data* d);
void COMaster_post_TPDO(CO_Data* d);
void COMaster_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
void COMaster_post_SlaveBootup(CO_Data* d, UNS8 nodeid);

void COMaster_NodeGuardError(CO_Data* d, UNS8 heartbeatID);
#endif /*MASTER_H_*/