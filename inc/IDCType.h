#ifndef IDC_GLOBAL_H
#define IDC_GLOBAL_H

#include <unistd.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef signed char schar;
typedef signed short sshort;
typedef signed long slong;
typedef unsigned short ENUM16;
typedef unsigned char  ENUM8;


//## dependency string
#include <string.h>

#define IDC_OK 0
#define EOK              0  /* No error                                 */


#ifndef STACK_SIZE_DEFAULT
#define STACK_SIZE_DEFAULT  0x50000u //320KB
#endif

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN               0x10000//64k as application
#endif


#define XTP_DEF_PACKED __attribute__ ((packed))


#define IDC_ERROR -1
#define IDC_TIMEOUT -2
#define IDC_NO_SUFFICIENT_BUF -3

#define IDC_TRUE (bool)1

#define IDC_FALSE (bool)0

#define IDC_FLAGSIZE 4

#define IDC_FLAG_SET 1

#define IDC_FLAG_RESET 0

#define IDC_FLAG_INVALID -1

#define IDC_MAX_NAME_LENGTH 32
#define IDC_MAX_NAME_PATH_LENGTH 60


#define NO_TIMEOUT 0

#define IDC_PLANE_A 0

#define IDC_PLANE_B 1

#define IDC_PLANE_AB 2

#define IDC_PLANE_ENV 3

typedef int IDCErrnoType;

inline void IDC_DELAY(unsigned int uiDelay) 
{
    for(unsigned long i=uiDelay*10000;i>0;i--){}
}

#ifndef handle_error_en(en, msg)
#define handle_error_en(en, msg) \
             do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#endif

#define getprio(X) getpriority( PRIO_PROCESS, X)
#define setprio(X, Y) setpriority( PRIO_PROCESS, X, Y)

#define __max(a,b) (((a) > (b)) ? (a) : (b))  
#define __min(a,b) (((a) < (b)) ? (a) : (b))



#endif

