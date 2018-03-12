

#ifndef IDC_SEMA_H
#define IDC_SEMA_H

#include <semaphore.h>


class CIdcSema
{
    
public:
    CIdcSema( unsigned int initValue = 0 );
   
    virtual ~CIdcSema( void );

    bool WaitFor( time_t seconds, long nanoseconds = 0 );
    bool WaitUntil( time_t seconds, long nanoseconds );
    bool TryWait( void );
    void Post( void );

    int GetValue( void );

    void Reset( void );

private:
    sem_t handle;
};


#endif /*IDC_SEMA_H*/


