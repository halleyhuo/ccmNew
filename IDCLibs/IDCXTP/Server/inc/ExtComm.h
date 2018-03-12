/*
 * ExtComm.h
 *
 *  Created on: 2017.07.29
 */

#ifndef EXT_COMM_H
#define EXT_COMM_H

typedef enum 
{
	ExtLinkManager,
	ExtLinkSimu,
	ExtLinkSize,
}ExtLinkIdx;

 
void InitExtComm();
void* ExtRcv(void* _para);
void SendExtMsg(void* data, const unsigned short _len, const ushort extType = ExtID_ObjFlow_Info, const ExtLinkIdx linkIdx = ExtLinkManager);


void RecordXTP(const char* const fmt, ...);


#endif /* EXT_COMM_H */
