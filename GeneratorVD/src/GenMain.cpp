/*forlinx serial port test*/
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "IDCTaskID.h"
#include "HeatUnitCal.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <functional>
#include "GenMain.h"
//#include "GenObj.h"
//#include <string>
//#include <map>
//#include "GenState.h"
#include "GenStateM.h"


//using namespace std;

#define Gen_Param_Len 12
#define Gen_Param_MAX 256

typedef struct
{
	int len;
	char p[Gen_Param_MAX];
}sGenCmddata;

typedef struct
{
	uchar data[Gen_Param_MAX*Gen_Param_Len];
	ushort datalen;
	sGenCmddata param[Gen_Param_Len];
	uchar paramLen;
	uchar checksum;
}sGeneratorCMD;


#define msleep(n) usleep(n*1000)

volatile	int fd;
char* dev   = NULL;
string message;
pthread_mutex_t mut;
fd_set rd;
int nread,retval;
unsigned char msg[14];
struct timeval timeout={0,100};
//std::map<std::string,sFuncMAP> cmdMap;
enum eGeneratorParse{
	CmdStart,
	CmdRevicing,
	CmdParam,
	CmdChecksum,
	CmdParse,
	CmdEnd};
eGeneratorParse GeneratorParse=CmdStart; 
sGeneratorCMD GeneratorCMD;
//cIFV* IFVparse = new cIFV();
//cVFP* VFPparse = new cVFP();

volatile	pthread_t thread[2]; 
volatile	const int READ_THREAD_ID = 0;  
volatile	const int SEND_THREAD_ID = 1;  
volatile	int COM_READ_STATU = 1;  
volatile	int COM_SEND_STATU = 1; 
CGenLogical* CGenLogical::m_instanse = NULL;

volatile	int sendnum=0,sendnum_times=0,recenum=0,recenum_times=0,cornum=0,cornum_times=0;
namespace IDC
{
	const int debug_level_remote=LOG_DEBUG+1; 
	const int debug_level_local=0;
}
static speed_t getBaudrate(int baudrate)
{
	switch(baudrate) {
	case 0: return B0;
	case 50: return B50;
	case 75: return B75;
	case 110: return B110;
	case 134: return B134;
	case 150: return B150;
	case 200: return B200;
	case 300: return B300;
	case 600: return B600;
	case 1200: return B1200;
	case 1800: return B1800;
	case 2400: return B2400;
	case 4800: return B4800;
	case 9600: return B9600;
	case 19200: return B19200;
	case 38400: return B38400;
	case 57600: return B57600;
	case 115200: return B115200;
	case 230400: return B230400;
	case 460800: return B460800;
	case 500000: return B500000;
	case 576000: return B576000;
	case 921600: return B921600;
	case 1000000: return B1000000;
	case 1152000: return B1152000;
	case 1500000: return B1500000;
	case 2000000: return B2000000;
	case 2500000: return B2500000;
	case 3000000: return B3000000;
	case 3500000: return B3500000;
	case 4000000: return B4000000;
	default: return -1;
	}
}

int str2intCheck(const char* str, int len)
{
	int i;
    for(i = 0; i < len; ++i)
    {
		if((str[i]<'0')||(str[i]>'9'))
			return -1;
	}
	return 1;
}

ushort str2int(const char* str, int len)
{
    int i;
	ushort ret = 0;
    for(i = 0; i < len; ++i)
    {
        ret = ret * 10 + (str[i] - '0');
    }
    return ret;
}

int str2ushort(const char* str, int len,ushort* revalue)
{
	int i;
	if(len>5)
		return -1;
	* revalue = 0;
    for(i = 0; i < len; ++i)
    {
		if((str[i]<'0')||(str[i]>'9'))
			return -1;
		*revalue = (*revalue) * 10 + (str[i] - '0');
    }
    return 1;
}

CGenLogical::CGenLogical():CIDCThread("GenLogical",IDC_TASK_GEN_LOGICAL,Small,Moderate,true)
{

}

CGenLogical::~CGenLogical()
{

}

CGenLogical* CGenLogical::Sigleton(CGenLogical* instanse/* = NULL*/)
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

void CGenLogical::run()
{
    IDC::Log(IDC::LOG_NOTICE,LOG_TAG,">>>INFO: CGenLogical Running");
	
    while(true)
    {
        char buf[MAX_ITC_LENGTH];
		const int rc = m_msgQueue->recv(buf,MAX_ITC_LENGTH);
		if(rc>0)
		{
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,">>>INFO: CGenLogical Running");
			ITCObjHeader* p = (ITCObjHeader*)buf;
		
        }
    }
}


//CGenLogical* pGenLogical = NULL;

//CGenMaster* pGenMaster = NULL;

int OpenDev(char *Dev)
{
	speed_t speed;

    int i=0;
    int fdt,c=0,num;
    struct termios oldtio,newtio;

    speed = getBaudrate(115200);
    fdt=open(Dev,O_RDWR | O_NONBLOCK| O_NOCTTY | O_NDELAY);
    if(fdt<0)
    {
        perror(Dev);
        exit(1);
    }
    //save to oldtio
    tcgetattr(fdt,&oldtio);
    //clear newtio
    bzero(&newtio,sizeof(newtio));
    //newtio.c_cflag = speed|CS8|CLOCAL|CREAD|CRTSCTS;
    newtio.c_cflag = speed|CS8|CLOCAL|CREAD|CSTOPB;
    newtio.c_iflag = IGNPAR;  
    newtio.c_oflag = 0;
    //printf("newtio.c_cflag=%x\n",newtio.c_cflag);
    tcflush(fdt,TCIFLUSH);  
    tcsetattr(fdt,TCSANOW,&newtio);  
    tcgetattr(fdt,&oldtio);
    //printf("oldtio.c_cflag=%x\n",oldtio.c_cflag);
    return fdt;
}

void ParseAndSendIntT()
{
	//printf("parse a Generator cmd--%s paramlen = %d\n",GeneratorCMD.param[0].p,GeneratorCMD.paramLen);
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"GeneraotrVD parse cmd--%s paramlen = %d\n",GeneratorCMD.param[0].p,GeneratorCMD.paramLen);
	GenRxCmdMapping* CmdTable = CheckCmdTable(GeneratorCMD.param[0].p, strnlen(GeneratorCMD.param[0].p,4));
	if(CmdTable!=NULL)
	{
		//make INT Telegram to unit GENStateMachine
		IntGenVDEventFrame IntGENVDEvent;
		IntGENVDEvent.header.tidSrc = 0;
		IntGENVDEvent.header.tidTarget = IDC_TASK_GEN_LOGICAL;
		IntGENVDEvent.header.mode = ITC_SEND_MODE_DIRECT;
		IntGENVDEvent.header.flowControl = ITC_SEND_NO_FLOW;
		//InfFlDataFrame.header.len = sizeof(InfFlDataFrame);
		IntGENVDEvent.header.ITCObjectID = INT_GENVD_STM_EVENT;
		IntGENVDEvent.data.Header.extParamCnt = 0;
		IntGENVDEvent.data.Header.Event = CmdTable->eventType;
		if(CmdTable->eventParamCnt==GeneratorCMD.paramLen )//param Length Match
		{			
			for(int paramIdx = 1; paramIdx<=GeneratorCMD.paramLen; paramIdx++)
			{	
				memset(IntGENVDEvent.data.extParam[IntGENVDEvent.data.Header.extParamCnt].param,0,GenVDParamItemSize);
				IntGENVDEvent.data.extParam[IntGENVDEvent.data.Header.extParamCnt].type = CmdTable->eventParamList[IntGENVDEvent.data.Header.extParamCnt];
				strncpy(IntGENVDEvent.data.extParam[IntGENVDEvent.data.Header.extParamCnt++].param,GeneratorCMD.param[paramIdx].p,GenVDParamItemSize-1);			
			}
			if(CmdTable->eventType==GENVD_STM_EVENT_RCV_AXD)
			{
				IntGENVDEvent.header.tidTarget = IDC_TASK_GEN_HEAT_UNIT_CAL_TASK;
				IntGENVDEvent.header.ITCObjectID = INT_GENVD_HEATCAL_EVENT;
			}
		}
		else if(CmdTable->eventType==GENVD_STM_EVENT_RCV_XSTATUS)
		{
			IntGENVDEvent.data.Header.extParamCnt = 1;
			IntGENVDEvent.data.extParam[0].type   = CmdTable->eventParamList[0];
		}
		else if(CmdTable->eventType==GENVD_STM_EVENT_RCV_IFV)
		{			
			IntGENVDEvent.data.Header.extParamCnt = 0;
		}
		else
		{
			return;
		}
		IntGENVDEvent.header.len = sizeof(IntGENVDEvent.header)+sizeof(IntGENVDEvent.data.Header)+(IntGENVDEvent.data.Header.extParamCnt*sizeof(GenVDParamPacket));
		CIDCThread::ITCSend(&IntGENVDEvent.header);		
	}
	else
	{
		IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Unknown CMD %s",GeneratorCMD.param[0].p);
	}

}

void read_port(void)
{   
    FD_ZERO(&rd);
    FD_SET(fd,&rd);
   // timeout.tv_sec = 1;
   // timeout.tv_usec = 0;
    uchar oldchar;
    retval = select (fd+1,&rd,NULL,NULL,&timeout); 
    switch (retval)
    {
        case 0:;//出错
           // printf("no data input within  1s.\n");
            break;
        case -1://未准备好
            perror("select");
            break;       
        default://收到数据
		if((nread=read(fd,msg,1))>0)
         {       
         	/*printf("Com revice message %d\n",nread);
			printf("%s",msg);
			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"revice sci message:%s\n",
						msg);*/
			if((msg[0]=='>')&&(GeneratorParse==CmdStart))
			{
				GeneratorParse = CmdRevicing;
				GeneratorCMD.datalen =0; 
				//printf("CmdParam\n");
				break;
			}
			if(GeneratorParse==CmdRevicing)
			{
				if(msg[0]==0x0A)//换行符,先判断换行符，因为校验位有可能是空格从而导致问题
				{	
					if(GeneratorCMD.datalen<4)
						GeneratorParse = CmdEnd;//数据不对，抛弃
					else
						GeneratorParse=CmdParam;//进入参数解析
				}
				else
					GeneratorCMD.data[GeneratorCMD.datalen++] = msg[0]; 
			}
			if(GeneratorParse==CmdParam)
			{				
				GeneratorCMD.checksum = GeneratorCMD.data[GeneratorCMD.datalen-1];
				GeneratorCMD.data[GeneratorCMD.datalen-1] = '\0';
				GeneratorCMD.paramLen = 0;
				oldchar = '>';//初始化oldchar
				//将最后一个参数的校验位删除
				for(int i=0;i<=(GeneratorCMD.datalen-2);i++)//不包含校验位
				{
					if(GeneratorCMD.data[i]==' ')
					{
							if(oldchar!=' ')//首次碰到空格
							{
								if(GeneratorCMD.paramLen<Gen_Param_Len)
								{	GeneratorCMD.paramLen++;
									GeneratorCMD.param[GeneratorCMD.paramLen].len = 0;
								}
								else
								{
									GeneratorCMD.param[GeneratorCMD.paramLen].p[GeneratorCMD.param[GeneratorCMD.paramLen].len]= '\0';
									GeneratorParse == CmdEnd;
								}
							}

					}
					else
					{
						if(GeneratorCMD.param[GeneratorCMD.paramLen].len >=Gen_Param_MAX)
							GeneratorParse = CmdEnd;
						GeneratorCMD.param[GeneratorCMD.paramLen].p[GeneratorCMD.param[GeneratorCMD.paramLen].len++]=GeneratorCMD.data[i];
					}
					oldchar = GeneratorCMD.data[i];
				}
				//GeneratorCMD.param[GeneratorCMD.paramLen].p[GeneratorCMD.param[GeneratorCMD.paramLen].len]= '\0';//最后一个参数没有空格
				GeneratorParse = CmdChecksum;
			}
			if(GeneratorParse == CmdChecksum)
			 {
			 	//printf("Begin CheckSUm\n");
			 	//开始计算校验位
			 	uchar ch=0;
				ch += (uchar)'>';//命令起始符
				for(int i =0;i<=GeneratorCMD.paramLen;i++)
				{
					//由于len++，所以不包含最后一个len值
					for(int j=0;j<GeneratorCMD.param[i].len;j++)
					{
						ch += GeneratorCMD.param[i].p[j];
					}
					if(i!=GeneratorCMD.paramLen)
						ch += ' ';//增加空格的计算
					
				}
				if(ch<16)
					ch+= 16;				
				if(ch==GeneratorCMD.checksum)
				{
					//printf("CmdParse\n");
					GeneratorParse = CmdParse;
				}
				else
				{
					//printf("Check SUM ERROR %d %d\n",ch,GeneratorCMD.checksum);
					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"CMD Check SUM ERROR %d %d\n",ch,GeneratorCMD.checksum);
					GeneratorParse = CmdEnd;
				}
			 }
			 if(GeneratorParse == CmdParse)
			 {
			 	ParseAndSendIntT();
			 	GeneratorParse = CmdEnd;
			 }	
			 if(GeneratorParse == CmdEnd)//表示数据已解析或者数据错误
			 {
			 	
				GeneratorCMD.checksum = 0;
				GeneratorCMD.paramLen = 0;
				GeneratorCMD.datalen = 0;
				memset(GeneratorCMD.param,0,sizeof(sGenCmddata)*Gen_Param_Len);
				/*for(int i =0;i<Gen_Param_Len;i++)
				{	
					GeneratorCMD.param[i].len=0;
					memset(GeneratorCMD.param[i].p,0,256);
				}*/
				GeneratorParse = CmdStart;
			 }
				
		 }
         break;
    }
}


void* com_read(void* pstatu)  
{
	int o;
	while(COM_READ_STATU)
	{
		pthread_mutex_lock(&mut);
    	read_port();
    	pthread_mutex_unlock(&mut);
	}
    pthread_exit(NULL);
}

void* com_send(char* p, const int len)  
{
	int ii;
	if(COM_SEND_STATU)
	{
		int CheckSum = 0;
		uchar FrameBuf[128];
		memset(FrameBuf,0,sizeof(FrameBuf));
		memcpy(FrameBuf,p,len);
		
		//memcpy(&FrameBuf[len],)
		for(int i=0; i<len; i++)
		{
			CheckSum += p[i];			
		}
		FrameBuf[len] = (uchar)(CheckSum&0xFF);
		if(FrameBuf[len] < 16)
		{
			FrameBuf[len] += 16;
		}
		
		FrameBuf[len+1] = 0xA;
		
		write(fd, FrameBuf, len+2);
		//write(fd, FrameBuf, len);
		printf("CCM Send(%d) CMD %s ",len+2,FrameBuf);
		printf("Checksum = 0x%x, frameBuf[x] = 0x%x\n", CheckSum, FrameBuf[len]);
	}
	return NULL;
	
}
  
int start_thread_func(void*(*func)(void*), pthread_t* pthread, void* par, int* COM_STATU)  
{  
        *COM_STATU = 1;  
        memset(pthread, 0, sizeof(pthread_t));  
        int temp;  
            /*creat thread*/  
	if((temp = pthread_create(pthread, NULL, func, par)) != 0)  
	{
		printf("creat thread failer!\n");
	}  
	else  
        {  
            int id = pthread_self();  
            printf("%s,creat thread %lu sucess\n",dev, *pthread);  
        }  
        return temp;  
} 

int initComm()
{

	//char *dev ="/dev/ttymxc1"; 
	//signal(SIGINT,SignHandler);
	fd = OpenDev("/dev/ttymxc2");

	if (fd>0)
	{}
	else
	{

		printf("Can't Open Serial Port %s \n",dev);
		exit(0);

	}
	

   	pthread_mutex_init(&mut,NULL);  
 
	 if(start_thread_func(com_read, (pthread_t*)&thread[READ_THREAD_ID],  (int *)&COM_READ_STATU, (int *)&COM_READ_STATU) != 0)  
     {  
        printf("error to leave\n");  
        return -1;  
     }  

}
int main(int argc,char* argv[])
{
	IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"INFO:Geneartor is Running.");
	//pGenLogical = new CGenLogical();
	//CGenLogical::Sigleton(pGenLogical);
	//pGenLogical->Init();

	//pGenMaster = new CGenMaster();
	//pGenMaster->Init();

	initComm();
	start_alarm();
	CGenXtpIf* pGenXtpIf = new CGenXtpIf();
	CGenXtpIf::Sigleton(pGenXtpIf);
	pGenXtpIf->Init();

	CGenHeatCal* pGenHeatCal = new CGenHeatCal();
	pGenHeatCal->Init();
	
	CGenStateM* GenStateM = new CGenStateM();
	GenStateM->Init();
	//cmdMap = create_map();
	
	
	while(true)
	{
		sleep(5);
	}
}





