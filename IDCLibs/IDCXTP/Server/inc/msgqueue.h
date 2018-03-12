/*
 * msgqueue.h
 *
 *  Created on: 2017.03.29
 */

#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_

 
#include <errno.h>
#include <mqueue.h> 
#include "xtpobj.h"
#include "xtpcomm.h"


//Timeout = 150*50=7.5s. Not receive the Stat for 7.5s then remove the
const int CHNSTAT_TIMEOUT = 150; 

//Support 10 Channel connect with Driver in MAX. Not the App Process Number.
const unsigned char MAX_CONN_NUM = 32;
//extern msgq_info mq_info;
extern conn_info conn_list[MAX_CONN_NUM];


extern bool  msgq_create(int, int);
extern int	msgq_destroy();
extern int 	msgq_recv(void *message);
extern CIDCMsgQueue* conn_channel_get(const uchar NodeID);


extern bool conn_channel_add(int, int,int bus = 0);//currently not support different Eth Port
extern CIDCMsgQueue* conn_channel_del(int, int);
extern int conn_channel_del_all();
extern int conn_channel_recv(void*);
extern int conn_channel_send_l( CIDCMsgQueue* msgQSrv2Client, void *buf, int len);

//extern int conn_channel_send( CIDCMsgQueue* msgQ, void *buf, const int len );
extern int conn_channel_send( void* buf, const int len,const int pid, const int chid );
extern int conn_channel_send(void* buf, const int len);
extern int conn_channel_reply(int);

extern int conn_channel_reset_alarm(const int pid, const int chid);
extern int conn_channel_filter(int, int, unsigned short, unsigned char);
extern int conn_channel_filter_add(int, int);
extern int conn_channel_filter_del(int, int);
extern CIDCMsgQueue* conn_channel_filter_get(int, int&, unsigned char&);
extern int conn_channel_answer(int, int,unsigned char, unsigned char);
extern int conn_channel_bus_get(int pid, int chid);
#endif /* MSGQUEUE_H_ */
